#include <stdint.h>
#include <string.h>
#include <math.h>

#include "hardware.hpp"
#include "debug_printf.hpp"
#include "gfx.hpp"

constexpr double Pi = 3.141592653589793;
constexpr double TurnPeriod = 23.0 /*H*/ * 3600.0 + 56.0 /*M*/ * 60.0 + 4.0 /*S*/;
constexpr double RotSpeed = 2 * Pi / TurnPeriod;

template <uint32_t Freq>
class PeriodicalTimer
{
public:
	void reset(uint32_t cur_time)
	{
		prev_value_ = cur_time;
	}

	bool is_signaled(uint32_t cur_time, uint32_t period)
	{
		if ((cur_time - prev_value_) >= period)
		{
			prev_value_ = cur_time;
			return true;
		}

		return false;
	}

	uint32_t get_seconds_to_tick(uint32_t cur_time, uint32_t period)
	{
		if (cur_time < prev_value_) return 0;

		auto from_time = cur_time - prev_value_;
		if (from_time >= period) return 0;

		return (period - from_time) / Freq;
	}

private:
	uint32_t prev_value_ = 0;
};

static double start_angle = 0;
static PeriodicalTimer<TimeTimerFreq> dither_timer;
static unsigned start_time_counter = 0;
static unsigned dither_period = MovePeriod; // in minutes
static double dither_angle = MoveMaxAngle;

/*****************************************************************************/

static double calc_angle(double l)
{
	return 2 * asin(l / (2 * R));
}

static double calc_l(double angle)
{
	return 2 * R * sin(angle / 2);
}

static double calc_ideal_angle()
{
	double time_in_sec = (double)(get_time_counter() - start_time_counter) / (double)TimeTimerFreq;
	return start_angle + time_in_sec * RotSpeed;
}

static double get_l()
{
	return get_steps_counter() * RodStep * TurnsOnStep + StartL;
}

static void recalc_speed(bool store_angle)
{
	double l = get_l();
	if (l > MaxL)
	{
		set_rotations_per_seconds(0);
		return;
	}

	double angle = calc_angle(l);

	constexpr double d_angle = 1e-8;

	double l1 = calc_l(angle + d_angle);
	double l2 = calc_l(angle - d_angle);
	double dl = l1 - l2;
	constexpr double dt = 2 * d_angle / RotSpeed;
	double v = dl / dt; // mm in second

	double rod_rotataions_v = v / RodStep; // rod rotations in second

	set_rotations_per_seconds((float)rod_rotataions_v);

	if (store_angle)
	{
		start_time_counter = get_time_counter();
		dither_timer.reset(start_time_counter);
		start_angle = angle;
		debug_printf("Angle stored {:.5}\n", 180.0*start_angle/Pi);
	}

	debug_printf(
		"angle = {:.5}, rot_per_secs = {:.5} angle_diff = {:+.5}\n",
		180.0*angle/Pi,
		rod_rotataions_v,
		180.0*(angle - calc_ideal_angle())/Pi
	);
}

static void make_random_move()
{
	set_rotations_per_seconds(0);

	if (get_l() >= MaxL) return;

	double MoveMaxAngleRad = Pi * dither_angle / 180.0;

	double angle_diff = MoveMaxAngleRad * (double)(rand() - RAND_MAX/2) / (double)RAND_MAX;
	debug_printf("Move. angle_diff={:.5}\n", 180.0*angle_diff/Pi);

	auto make_move = [&] (const float speed)
	{
		double prev_diff_abs = NAN;
		bool prev_decrease = false;
		bool first_time = true;
		for (;;)
		{
			double new_angle = calc_ideal_angle() + angle_diff;
			double l = get_l();
			double cur_angle = calc_angle(l);
			double diff = new_angle - cur_angle;
			if ((diff < 0) && (l < (StartL + 1.0f))) break;
			if ((diff > 0) && (l > MaxL)) break;
			double diff_abs = fabs(diff);
			bool cur_dir = (diff > 0);

			if (!isnan(prev_diff_abs))
			{
				bool diff_decrease = (diff_abs < prev_diff_abs);
				if (!diff_decrease && prev_decrease) break;
				prev_decrease = diff_decrease;
			}

			set_rotations_per_seconds(cur_dir ? speed : -speed);
			prev_diff_abs = diff_abs;
			if (first_time) delay_ms(20);
		}
	};

	make_move(1.00f);
	make_move(0.05f);
}

static void revert()
{
	int rot_per_sec = -5;

	for (;;)
	{
		set_rotations_per_seconds(rot_per_sec);
		for (;;)
		{
			bool revert_btn_pressed = is_revert_btn_pressed();
			if (!revert_btn_pressed) break;
			delay_ms(10);
		}

		set_rotations_per_seconds(0);

		delay_ms(300);

		if (!is_revert_btn_pressed()) break;

		rot_per_sec = -rot_per_sec;
	}
}


static void start_work(bool store_angle)
{
	recalc_speed(store_angle);
}

static void show_info_data()
{
	double angle = calc_angle(get_l());
	double min_angle = calc_angle(StartL);

	unsigned time_to_dithrering =
		dither_period
		? dither_timer.get_seconds_to_tick(get_time_counter(), TimeTimerFreq * dither_period * 60)
		: 0;

	show_cur_info(
		180.0 * (angle - min_angle) / Pi,
		time_to_dithrering,
		dither_angle
	);
}

static void change_dithering_time()
{
	if (dither_period) --dither_period;
	else dither_period = MovePeriod;
	dither_timer.reset(get_time_counter());
}

static void change_dithering_angle()
{
	constexpr size_t count = 5;

	const double angle_values[count] = { 2.0, 1.0, 0.5, 0.3, 0.2 };

	for (size_t i = 0; i < count; ++i)
	{
		if (fabs(dither_angle - angle_values[i]) < 0.01)
		{
			dither_angle = (i == (count-1)) ? angle_values[0] : angle_values[i+1];
			break;
		}
	}
}

int main()
{
	init_hardware();
	led_off();
	delay_ms(100);

	init_display();
	show_wellcome_screen();
	delay_ms(1000);

	auto srand_value = get_value_for_srand();
	debug_printf("value_for_srand={} (0x{:x})\n", srand_value, srand_value);
	srand(srand_value);

	start_work(true);

	PeriodicalTimer<TimeTimerFreq> recalc_speed_timer;

	dither_timer.reset(get_time_counter());
	recalc_speed_timer.reset(get_time_counter());

	bool prev_dither_time_btn_pressed = false;
	bool prev_dither_angle_btn_pressed = false;
	for (;;)
	{
		delay_ms(10);
		auto tm_cnt = get_time_counter();

		bool show_info = false;

		if (is_revert_btn_pressed())
		{
			revert();
			start_work(true);
			dither_timer.reset(tm_cnt);
			show_info = true;
		}

		bool dither_time_btn_pressed = is_dither_time_btn_pressed();
		if (dither_time_btn_pressed && !prev_dither_time_btn_pressed)
		{
			change_dithering_time();
			show_info = true;
		}
		prev_dither_time_btn_pressed = dither_time_btn_pressed;

		bool dither_angle_btn_pressed = is_dither_angle_btn_pressed();
		if (dither_angle_btn_pressed && !prev_dither_angle_btn_pressed)
		{
			change_dithering_angle();
			show_info = true;
		}
		prev_dither_angle_btn_pressed = dither_angle_btn_pressed;

		if (recalc_speed_timer.is_signaled(tm_cnt, TimeTimerFreq / 2))
		{
			recalc_speed(false);
			show_info = true;
		}

		if (dither_period && dither_timer.is_signaled(tm_cnt, TimeTimerFreq * dither_period * 60))
		{
			make_random_move();
			start_work(false);
			show_info = true;
		}

		if (show_info) show_info_data();
	}
}
