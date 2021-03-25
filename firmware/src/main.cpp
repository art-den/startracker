// Astrotracker firmware for STM32 "Blue Pill" board

#include <math.h>

#include "stm32_hl/hl_misc.hpp"
#include "stm32_hl/hl_gpio.hpp"
#include "stm32_hl/hl_tim.hpp"
#include "stm32_hl/hl_dwt.hpp"

#include "debug_printf.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////

// geomerty
constexpr double R = 135.5; // side length of an equilateral triangle (in mm)
constexpr double StartL = 44.2; // base length of an equilateral triangle (in mm) in start position

// stepper motor parameters
constexpr double RodStep = 0.8; // rod step in mm
constexpr double MotorSteps = 200.0; // steps per rotation
constexpr int32_t MotorMicroSteps = 16; // microsteps

////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace hl;

constexpr unsigned SysClockFreq = 72'000'000;
constexpr unsigned TimerClock = 1'500'000;

// Step motor control
using DirPin = PA0;
using StepPin = PA1;
using MicroStepCtrlPin = PA2;
using StepTimer = Timer2;
constexpr unsigned StepTimerChan = 2;
static volatile int32_t steps_counter = 0;

// "Revert" button
using RevertPin = PA3;
using RevertGndPin = PA4;

using LedPin = PC13;


static void delay_ms(unsigned delay_in_ms)
{
	Delay<SysClockFreq>::exec_ms(delay_in_ms);
}

static bool is_revert_btn_pressed()
{
	return !RevertPin::get_in();
}

static void init_sysclock()
{
	dwt_enable();

	misc_f1_conf_sysclock_hse_high(
		PLLMult::_9,
		APB2Prescaler::_1,
		APB1Prescaler::_1,
		AHBPrescaler::_1,
		FlashLatency::_2
	);
}

static void init_hardware()
{
	PA::clock_on();
	PA::reset();
	PB::clock_on();
	PB::reset();
	PC::clock_on();
	PC::reset();

	StepPin::conf_alt_push_pull();
	DirPin::conf_out_push_pull();
	MicroStepCtrlPin::conf_out_push_pull();
	RevertPin::conf_in_pull_up();
	RevertGndPin::conf_out_push_pull();

	StepTimer::clock_on();
	StepTimer::reset();
	StepTimer::set_prescaler(SysClockFreq/TimerClock - 1);
	StepTimer::enable_auto_reload_preload();
	StepTimer::conf_pwm<StepTimerChan>(PWMMode::_1, PWMPolarity::High, PWMPreload::Enable);

	StepTimer::set_pwm_value<StepTimerChan>((TimerClock / (MotorSteps*MotorMicroSteps*10)) /2 - 1); // 10 rotations per second maximum

	StepTimer::enable_update_interrupt();

	NVIC_EnableIRQ(StepTimer::IRQn);

	LedPin::conf_out_open_drain();
}

static void led_on()
{
	LedPin::off();
}

static void led_off()
{
	LedPin::on();
}

static void recalc_speed()
{
	constexpr double Pi = 3.141592653589793;
	constexpr double TurnPeriod = 23.0 /*H*/ * 3600.0 + 56.0 /*M*/ * 60.0 + 4.0 /*S*/;
	constexpr double RotSpeed = 2 * Pi / TurnPeriod;
	constexpr double TurnsOnStep = 1.0 / (MotorSteps * MotorMicroSteps); // rod rotations per one motor step

	double l = steps_counter * RodStep * TurnsOnStep + StartL;

	double angle = 2 * asin(l / (2 * R));

	constexpr double d_angle = 1e-8;

	auto l_fun = [](double a) { return 2 * R * sin(a / 2); };
	double l1 = l_fun(angle + d_angle);
	double l2 = l_fun(angle - d_angle);
	double dl = l1 - l2;
	constexpr double dt = 2 * d_angle / RotSpeed;
	double v = dl / dt; // mm in second

	double rod_rotataions_v = v / RodStep; // rod rotations in second

	double steps_in_second = rod_rotataions_v / TurnsOnStep;

	uint32_t reload_value = (uint32_t)(TimerClock / steps_in_second + 0.5) - 1;

	StepTimer::set_auto_reload_value(reload_value);

	debug_printf(
		"l={:.3} mm, angle = {:.5}, freq = {:.3}, timer = {}\n",
		l,
		180.0*angle/Pi,
		steps_in_second,
		reload_value
	);
}

static void start_work()
{
	StepTimer::stop();
	delay_ms(10);

	recalc_speed();

	DirPin::on();
	MicroStepCtrlPin::on();

	delay_ms(200);
	StepTimer::start();
}

static void revert()
{
	StepTimer::stop();
	delay_ms(10);

	DirPin::off();
	delay_ms(200);

	int steps_per_sec = 100;
	int v = MotorSteps * MotorMicroSteps / 5;

	constexpr int max_steps_per_sec = 8*MotorSteps * MotorMicroSteps;
	bool prev_revert_btn_pressed = is_revert_btn_pressed();

	for (;;)
	{
		bool revert_btn_pressed = is_revert_btn_pressed();
		if ((revert_btn_pressed != prev_revert_btn_pressed) && !revert_btn_pressed) v = -v;
		prev_revert_btn_pressed = revert_btn_pressed;

		delay_ms(10);

		int prev_steps_per_sec = steps_per_sec;
		steps_per_sec += v;

		bool sign_changed =
			((prev_steps_per_sec >= 0) && (steps_per_sec < 0)) ||
			((prev_steps_per_sec <= 0) && (steps_per_sec > 0));

		if (sign_changed)
		{
			if (!revert_btn_pressed) break;
			StepTimer::stop();
			delay_ms(10);

			DirPin::set_out(steps_per_sec < 0);
		}

		if (steps_per_sec > max_steps_per_sec)
			steps_per_sec = max_steps_per_sec;
		if (steps_per_sec < -max_steps_per_sec)
			steps_per_sec = -max_steps_per_sec;

		if (steps_per_sec != 0)
			StepTimer::set_auto_reload_value(TimerClock / abs(steps_per_sec) - 1);

		if (!StepTimer::is_started())
			StepTimer::start();
	}

	StepTimer::stop();
	delay_ms(10);
}

extern "C" void TIM2_IRQHandler()
{
	if (StepTimer::get_update_interrupt_flag())
	{
		StepTimer::clear_update_interrupt_flag();

		int32_t inc = MicroStepCtrlPin::get_out() ? 1 : MotorMicroSteps;
		if (DirPin::get_out()) steps_counter += inc;
		else steps_counter -= inc;

		if (steps_counter < 0)
		{
			steps_counter = 0;
			led_on();
		}
		else
		{
			led_off();
		}
	}
}

int main()
{
	init_sysclock();
	init_hardware();
	init_debug_printf(SysClockFreq);

	delay_ms(200);

	start_work();

	unsigned recalc_speed_counter = 0;
	unsigned revert_btn_pressed_cnt = 0;

	for (;;)
	{
		delay_ms(10);

		if (is_revert_btn_pressed())
			revert_btn_pressed_cnt++;

		if (revert_btn_pressed_cnt > 10)
		{
			revert_btn_pressed_cnt = 0;
			revert();
			start_work();
		}

		recalc_speed_counter++;
		if (recalc_speed_counter >= 100)
		{
			recalc_speed_counter = 0;
			recalc_speed();
		}
	}
}