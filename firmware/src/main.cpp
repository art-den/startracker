#include <stdint.h>
#include <string.h>
#include <math.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/cm3/nvic.h>

#include "micro_format/micro_format.hpp"

/*****************************************************************************/

// geomerty
constexpr double R = 133.8; // side length of an equilateral triangle (in mm)
constexpr double StartL = 44.2; // base length of an equilateral triangle (in mm) in start position
constexpr double MaxL = 180.0; // (in mm)

// stepper motor parameters
constexpr double RodStep = 0.8; // rod step in mm
constexpr double MotorSteps = 200.0; // steps per rotation
constexpr int32_t MotorMicroSteps = 16; // microsteps

// periodical random moving
constexpr double MoveMaxAngle = 0.3;
constexpr int32_t MovePeriod = 8; // in minutes

/*****************************************************************************/

constexpr unsigned SysClockFreq = 72'000'000;
constexpr unsigned TimerClock = 1'000'000;

/* peripheral configuration */

#define LED_PIN_PORT GPIOC
#define LED_PIN_IDX GPIO13

#define DIR_PIN_PORT GPIOA
#define DIR_PIN_IDX GPIO0

#define STEP_PIN_PORT GPIOA
#define STEP_PIN_IDX GPIO1

#define STEP_TIMER TIM2
#define STEP_TIMER_RCC RCC_TIM2
#define STEP_TIMER_RST RST_TIM2
#define STEP_TIMER_IRQ NVIC_TIM2_IRQ
#define STEP_TIMER_CHAN TIM_OC2
#define STEP_TIMER_ISR TIM2_IRQHandler

#define MICROSTEPCTRL_PIN_PORT GPIOA
#define MICROSTEPCTRL_PIN_IDX GPIO2

#define TIME_TIMER TIM3
#define TIME_TIMER_RCC RCC_TIM3
#define TIME_TIMER_RST RST_TIM3
#define TIME_TIMER_IRQ NVIC_TIM3_IRQ
#define TIME_TIMER_ISR TIM3_IRQHandler

#define PRINT_USART USART1
#define PRINT_USART_RCC RCC_USART1
#define PRINT_PIN_PORT GPIOA
#define PRINT_PIN_IDX GPIO9
#define PRINTGND_PIN_PORT GPIOA
#define PRINTGND_PIN_IDX GPIO8

#define REVERT_PIN_PORT GPIOA
#define REVERT_PIN_IDX GPIO3
#define REVERTGND_PIN_PORT GPIOA
#define REVERTGND_PIN_IDX GPIO4

/*****************************************************************************/

constexpr unsigned TimeTimerFreq = 1000; // Hz
constexpr double Pi = 3.141592653589793;
constexpr double TurnPeriod = 23.0 /*H*/ * 3600.0 + 56.0 /*M*/ * 60.0 + 4.0 /*S*/;
constexpr double RotSpeed = 2 * Pi / TurnPeriod;
constexpr double TurnsOnStep = 1.0 / (MotorSteps * MotorMicroSteps); // rod rotations per one motor step

static volatile unsigned time_counter = 0;
static double start_angle = 0;
static double last_move_time_counter = 0;
static unsigned start_time_counter = 0;
static volatile int32_t steps_counter = 0;

/*****************************************************************************/

/* debug_printf ans UART stuff */

template <typename ... Args>
size_t debug_printf(const char* format, const Args& ... args)
{
	auto uart_format_callback = [](auto, char character)
	{
		usart_send_blocking(USART1, (unsigned char)character);
        return true;
	};
	return mf::format(uart_format_callback, nullptr, format, args...);
}

/*****************************************************************************/

static void delay_ms(unsigned delay_in_ms)
{
	auto start_cnt = time_counter;
	auto max_diff = delay_in_ms * 1000U / TimeTimerFreq;
	while ((time_counter - start_cnt) < max_diff) {}
}

static void led_on()
{
	gpio_clear(LED_PIN_PORT, LED_PIN_IDX);
}

static void led_off()
{
	gpio_set(LED_PIN_PORT, LED_PIN_IDX);
}

static bool is_revert_btn_pressed()
{
	return !gpio_get(REVERT_PIN_PORT, REVERT_PIN_IDX);
}

static void init_hardware()
{
	// Main clocks
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);

	// GPIO ports
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_reset_pulse(RST_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_reset_pulse(RST_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_reset_pulse(RST_GPIOC);

	// USART
	gpio_set_mode(PRINT_PIN_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, PRINT_PIN_IDX);
	gpio_set_mode(PRINTGND_PIN_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, PRINTGND_PIN_IDX);
	rcc_periph_clock_enable(PRINT_USART_RCC);
	usart_set_baudrate(PRINT_USART, 115200);
	usart_set_databits(PRINT_USART, 8);
	usart_set_stopbits(PRINT_USART, USART_STOPBITS_1);
	usart_set_mode(PRINT_USART, USART_MODE_TX);
	usart_set_parity(PRINT_USART, USART_PARITY_NONE);
	usart_set_flow_control(PRINT_USART, USART_FLOWCONTROL_NONE);
	usart_enable(PRINT_USART);

	// led pin
	gpio_set_mode(LED_PIN_PORT, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, LED_PIN_IDX);

	// stepper motor pins
	gpio_set_mode(DIR_PIN_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, DIR_PIN_IDX);
	gpio_set_mode(STEP_PIN_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, STEP_PIN_IDX);
	gpio_set_mode(MICROSTEPCTRL_PIN_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, MICROSTEPCTRL_PIN_IDX);

	// stepper motor step timer
	rcc_periph_clock_enable(STEP_TIMER_RCC);
	rcc_periph_reset_pulse(STEP_TIMER_RST);
	nvic_enable_irq(STEP_TIMER_IRQ);
	timer_set_mode(STEP_TIMER, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_set_prescaler(STEP_TIMER, (rcc_apb1_frequency*2)/TimerClock - 1);
	timer_enable_preload(STEP_TIMER);
	timer_set_period(STEP_TIMER, 1000);
	timer_set_oc_mode(STEP_TIMER, STEP_TIMER_CHAN, TIM_OCM_PWM1);
	timer_set_oc_polarity_high(STEP_TIMER, STEP_TIMER_CHAN);
	timer_set_oc_value(STEP_TIMER, STEP_TIMER_CHAN, (TimerClock / (MotorSteps*MotorMicroSteps*10)) /2 - 1); // 10 rotations per second maximum
	timer_enable_oc_output(STEP_TIMER, STEP_TIMER_CHAN);
	timer_generate_event(STEP_TIMER, TIM_EGR_UG);
	timer_clear_flag(STEP_TIMER, TIM_SR_UIF);
	timer_enable_irq(STEP_TIMER, TIM_DIER_UIE);
	//timer_enable_counter(STEP_TIMER);

	// time timer
	constexpr unsigned TimeTimerRealClock = 10000;
	rcc_periph_clock_enable(TIME_TIMER_RCC);
	rcc_periph_reset_pulse(TIME_TIMER_RST);
	nvic_enable_irq(TIME_TIMER_IRQ);
	timer_set_prescaler(TIME_TIMER, (rcc_apb1_frequency*2)/TimeTimerRealClock - 1);
	timer_set_period(TIME_TIMER, TimeTimerRealClock / TimeTimerFreq - 1);
	timer_enable_irq(TIME_TIMER, TIM_DIER_UIE);
	timer_generate_event(TIME_TIMER, TIM_EGR_UG);
	timer_clear_flag(TIME_TIMER, TIM_SR_UIF);
	timer_enable_counter(TIME_TIMER);

	// revert button
	gpio_set_mode(REVERT_PIN_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, REVERT_PIN_IDX);
	gpio_set(REVERT_PIN_PORT, REVERT_PIN_IDX);
	gpio_set_mode(REVERTGND_PIN_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, REVERTGND_PIN_IDX);
}

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
	double time_in_sec = (double)(time_counter - start_time_counter) / (double)TimeTimerFreq;
	return start_angle + time_in_sec * RotSpeed;
}

static double get_l()
{
	return steps_counter * RodStep * TurnsOnStep + StartL;
}

volatile static bool step_timer_enabled = false;

static void enable_step_timer(bool enable)
{
	step_timer_enabled = enable;
	if (enable)
		timer_enable_counter(STEP_TIMER);
	else
		timer_disable_counter(STEP_TIMER);
}

static void recalc_speed(bool store_angle)
{
	double l = get_l();
	if (l > MaxL)
	{
		enable_step_timer(false);
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

	double steps_in_second = rod_rotataions_v / TurnsOnStep;

	uint32_t reload_value = (uint32_t)(TimerClock / steps_in_second + 0.5) - 1;

	timer_set_period(STEP_TIMER, reload_value);

	if (store_angle)
	{
		start_time_counter = time_counter;
		last_move_time_counter = start_time_counter;
		start_angle = angle;
		debug_printf("Angle stored {:.5}\n", 180.0*start_angle/Pi);
	}

	double ideal_angle = calc_ideal_angle();

	debug_printf(
		"angle = {:.5}, freq = {:.3}, timer = {}, angle_diff = {:.5}\n",
		180.0*angle/Pi,
		steps_in_second,
		reload_value,
		180.0*(ideal_angle - angle)/Pi
	);
}

static void make_move()
{
	if (!step_timer_enabled) return;

	double angle_diff = Pi*MoveMaxAngle/180.0 * (double)(rand() - RAND_MAX/2) / (double)RAND_MAX;
	debug_printf("Move. angle_diff={:.5}\n", 180.0*angle_diff/Pi);
	double new_angle = calc_ideal_angle() - angle_diff;
	double cur_angle = calc_angle(get_l());
	bool cur_sign = (cur_angle > new_angle);

	enable_step_timer(false);

	if (!cur_sign)
		gpio_set(DIR_PIN_PORT, DIR_PIN_IDX);
	else
		gpio_clear(DIR_PIN_PORT, DIR_PIN_IDX);

	timer_set_period(STEP_TIMER, TimerClock/(MotorSteps * MotorMicroSteps/8) - 1);

	delay_ms(1);

	enable_step_timer(true);

	for (;;)
	{
		bool sign = (calc_angle(get_l()) > new_angle);
		if (sign != cur_sign) break;
	}

	enable_step_timer(false);
}

static void revert()
{
	enable_step_timer(false);
	delay_ms(10);

	gpio_clear(DIR_PIN_PORT, DIR_PIN_IDX);
	delay_ms(10);

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
			enable_step_timer(false);
			delay_ms(10);

			if (steps_per_sec < 0)
				gpio_set(DIR_PIN_PORT, DIR_PIN_IDX);
			else
				gpio_clear(DIR_PIN_PORT, DIR_PIN_IDX);
		}

		if (steps_per_sec > max_steps_per_sec)
			steps_per_sec = max_steps_per_sec;
		if (steps_per_sec < -max_steps_per_sec)
			steps_per_sec = -max_steps_per_sec;

		if (steps_per_sec != 0)
			timer_set_period(STEP_TIMER, TimerClock / abs(steps_per_sec) - 1);

		if (!step_timer_enabled)
			enable_step_timer(true);
	}

	enable_step_timer(false);
	delay_ms(10);
}


static void start_work(bool store_angle)
{
	enable_step_timer(false);
	delay_ms(10);

	recalc_speed(store_angle);

	gpio_set(DIR_PIN_PORT, DIR_PIN_IDX);
	gpio_set(MICROSTEPCTRL_PIN_PORT, MICROSTEPCTRL_PIN_IDX);

	delay_ms(10);

	enable_step_timer(true);
}


static uint32_t get_value_for_srand()
{
	rcc_periph_clock_enable(RCC_ADC1);
	rcc_periph_reset_pulse(RST_ADC1);
	adc_power_off(ADC1);

	adc_disable_scan_mode(ADC1);
	adc_set_single_conversion_mode(ADC1);
	adc_disable_external_trigger_regular(ADC1);
	adc_set_right_aligned(ADC1);
	adc_enable_temperature_sensor();
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_1DOT5CYC);

	adc_power_on(ADC1);

	delay_ms(1);

	adc_reset_calibration(ADC1);
	adc_calibrate(ADC1);

	uint8_t chan = 16;
	adc_set_regular_sequence(ADC1, 1, &chan);

	uint32_t result = ~0U;

	for (unsigned i = 0; i < 19999; i++)
	{
		adc_start_conversion_direct(ADC1);
		while (!(adc_eoc(ADC1))) {}
		result += adc_read_regular(ADC1);
		bool high = result & 0x8000;
		result <<= 1;
		if (high) result |= 1;
	}

	adc_power_off(ADC1);
	rcc_periph_clock_disable(RCC_ADC1);

	return result;
}


extern "C" void STEP_TIMER_ISR()
{
	if (timer_get_flag(STEP_TIMER, TIM_SR_UIF))
	{
		timer_clear_flag(STEP_TIMER, TIM_SR_UIF);

		int32_t inc = 1;
		steps_counter += gpio_get(DIR_PIN_PORT, DIR_PIN_IDX) ? inc : -inc;
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


extern "C" void TIME_TIMER_ISR()
{
	if (timer_get_flag(TIME_TIMER, TIM_SR_UIF))
	{
		timer_clear_flag(TIME_TIMER, TIM_SR_UIF);
		++time_counter;
	}
}


int main()
{
	init_hardware();
	led_off();

	delay_ms(100);

	auto srand_value = get_value_for_srand();
	debug_printf("value_for_srand={} (0x{:x})\n", srand_value, srand_value);
	srand(srand_value);

	start_work(true);

	uint8_t recalc_speed_counter = 0;
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
			start_work(true);
		}

		recalc_speed_counter++;
		if (recalc_speed_counter >= 100)
		{
			recalc_speed_counter = 0;
			recalc_speed(false);
		}

		auto tm_cnt = time_counter;
		if ((tm_cnt - last_move_time_counter) / TimeTimerFreq > MovePeriod * 60)
		{
			last_move_time_counter = tm_cnt;
			make_move();
			start_work(false);
		}
	}
}
