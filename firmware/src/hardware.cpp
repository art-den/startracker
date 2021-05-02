#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/cm3/nvic.h>

#include "hardware.hpp"
#include "debug_printf.hpp"
#include "mgfxpp/displays/mono_sh1106.hpp"
#include "mgfxpp/connectors/libopencm3_display_i2c_v1_conn.hpp"
#include "mgfxpp/mgfxpp_display.hpp"

/* Helpers for GPIO port declarations like
   #define LED GPIOC, GPIO13 */

#define FIRST__(A1, A2) A1
#define SECOND__(A1, A2) A2
#define GPIO_PORT(...) FIRST__(__VA_ARGS__)
#define GPIO_PIN(...) SECOND__(__VA_ARGS__)

/* peripheral configuration */

// GPIO pins

#define DIR_PIN            GPIOA, GPIO0
#define STEP_PIN           GPIOA, GPIO1
#define MICROSTEPCTRL_PIN  GPIOA, GPIO2

#define REVERT_BTN_PIN     GPIOA, GPIO3
#define REVERT_GND_PIN     GPIOA, GPIO4

#define DITH_TIME_BTN_PIN  GPIOA, GPIO5
#define DITH_TIME_GND_PIN  GPIOA, GPIO6

#define DITH_ANGL_BTN_PIN  GPIOA, GPIO7
#define DITH_ANGL_GND_PIN  GPIOB, GPIO0

#define PRINT_PIN          GPIOA, GPIO9
#define PRINTGND_PIN       GPIOA, GPIO10

#define DISP_SCL_PIN       GPIOB, GPIO6
#define DISP_SDA_PIN       GPIOB, GPIO7

#define LED_PIN            GPIOC, GPIO13

// Timer to generate step signal for stepper motot

#define STEP_TIMER TIM2
#define STEP_TIMER_RCC RCC_TIM2
#define STEP_TIMER_RST RST_TIM2
#define STEP_TIMER_IRQ NVIC_TIM2_IRQ
#define STEP_TIMER_CHAN TIM_OC2
#define STEP_TIMER_ISR tim2_isr

// Timer for time intervals

#define TIME_TIMER TIM3
#define TIME_TIMER_RCC RCC_TIM3
#define TIME_TIMER_RST RST_TIM3
#define TIME_TIMER_IRQ NVIC_TIM3_IRQ
#define TIME_TIMER_ISR tim3_isr

// UART for debug logging

#define PRINT_USART USART1
#define PRINT_USART_RCC RCC_USART1

// i2c for display

#define DISP_I2C I2C1
#define DISP_I2C_RCC RCC_I2C1

class Button
{
public:
	void tick(bool pressed)
	{
		if (pressed && (pressed_cnt_ < 2 * PressBtnTimeMs * TimeTimerFreq / 1000))
			pressed_cnt_++;
		else if (!pressed && (pressed_cnt_ > 0))
			pressed_cnt_--;
	}

	bool is_pressed_filtered() const
	{
		return pressed_cnt_ > (PressBtnTimeMs * TimeTimerFreq / 1000);
	}

private:
	static constexpr uint16_t PressBtnTimeMs = 20;
	volatile uint16_t pressed_cnt_ = 0;
};

static bool step_timer_enabled = false;
static volatile unsigned time_counter = 0;
static Button revert_btn;
static Button dither_time_btn;
static Button dither_angle_btn;

static volatile int32_t steps_counter = 0;


void init_hardware()
{
	// Main clocks

	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	rcc_periph_clock_enable(RCC_AFIO);

	// GPIO ports

	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_reset_pulse(RST_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_reset_pulse(RST_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_reset_pulse(RST_GPIOC);

	// USART

	gpio_set_mode(GPIO_PORT(PRINT_PIN), GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_PIN(PRINT_PIN));
	gpio_set_mode(GPIO_PORT(PRINTGND_PIN), GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, GPIO_PIN(PRINTGND_PIN));
	rcc_periph_clock_enable(PRINT_USART_RCC);
	usart_set_baudrate(PRINT_USART, 115200);
	usart_set_databits(PRINT_USART, 8);
	usart_set_stopbits(PRINT_USART, USART_STOPBITS_1);
	usart_set_mode(PRINT_USART, USART_MODE_TX);
	usart_set_parity(PRINT_USART, USART_PARITY_NONE);
	usart_set_flow_control(PRINT_USART, USART_FLOWCONTROL_NONE);
	usart_enable(PRINT_USART);

	// led pin

	gpio_set_mode(GPIO_PORT(LED_PIN), GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO_PIN(LED_PIN));

	// stepper motor pins

	gpio_set_mode(GPIO_PORT(DIR_PIN), GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO_PIN(DIR_PIN));
	gpio_set_mode(GPIO_PORT(STEP_PIN), GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_PIN(STEP_PIN));
	gpio_set_mode(GPIO_PORT(MICROSTEPCTRL_PIN), GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO_PIN(MICROSTEPCTRL_PIN));
	gpio_set(MICROSTEPCTRL_PIN);

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

	gpio_set_mode(GPIO_PORT(REVERT_BTN_PIN), GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO_PIN(REVERT_BTN_PIN));
	gpio_set(REVERT_BTN_PIN);
	gpio_set_mode(GPIO_PORT(REVERT_GND_PIN), GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, GPIO_PIN(REVERT_GND_PIN));
	gpio_clear(REVERT_GND_PIN);

	// dither time button

	gpio_set_mode(GPIO_PORT(DITH_TIME_BTN_PIN), GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO_PIN(DITH_TIME_BTN_PIN));
	gpio_set(DITH_TIME_BTN_PIN);
	gpio_set_mode(GPIO_PORT(DITH_TIME_GND_PIN), GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, GPIO_PIN(DITH_TIME_GND_PIN));
	gpio_clear(DITH_TIME_GND_PIN);

	// dither angle button

	gpio_set_mode(GPIO_PORT(DITH_ANGL_BTN_PIN), GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO_PIN(DITH_ANGL_BTN_PIN));
	gpio_set(DITH_ANGL_BTN_PIN);
	gpio_set_mode(GPIO_PORT(DITH_ANGL_GND_PIN), GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, GPIO_PIN(DITH_ANGL_GND_PIN));
	gpio_clear(DITH_ANGL_GND_PIN);

	// i2c display connection

	gpio_set_mode(GPIO_PORT(DISP_SDA_PIN), GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN, GPIO_PIN(DISP_SDA_PIN));
	gpio_set_mode(GPIO_PORT(DISP_SCL_PIN), GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN, GPIO_PIN(DISP_SCL_PIN));

	rcc_periph_clock_enable(DISP_I2C_RCC);

	i2c_reset(DISP_I2C);
	i2c_peripheral_disable(DISP_I2C);
	i2c_set_speed(DISP_I2C, i2c_speed_fm_400k, rcc_apb1_frequency / 1'000'000);
	i2c_peripheral_enable(DISP_I2C);

	debug_printf("Hardware initialized\n");
	debug_printf("rcc_apb1_frequency = {} Mhz\n", rcc_apb1_frequency / 1'000'000);
	debug_printf("rcc_apb2_frequency = {} Mhz\n", rcc_apb2_frequency / 1'000'000);
	debug_printf("rcc_ahb_frequency = {} Mhz\n", rcc_ahb_frequency / 1'000'000);
}

void enable_step_timer(bool enable)
{
	step_timer_enabled = enable;
	if (enable)
		timer_enable_counter(STEP_TIMER);
	else
		timer_disable_counter(STEP_TIMER);
}

bool is_step_timer_enabled()
{
	return step_timer_enabled;
}

void set_step_timer_period(uint16_t value)
{
	timer_set_period(STEP_TIMER, value - 1);
}

void set_forward_direction()
{
	gpio_set(DIR_PIN);
}

void set_revert_direction()
{
	gpio_clear(DIR_PIN);
}

void send_debug_uart_char(char chr)
{
	usart_send_blocking(PRINT_USART, (unsigned char)chr);
}

bool is_revert_btn_pressed()
{
	return revert_btn.is_pressed_filtered();
}

bool is_dither_time_btn_pressed()
{
	return dither_time_btn.is_pressed_filtered();
}

bool is_dither_angle_btn_pressed()
{
	return dither_angle_btn.is_pressed_filtered();
}

unsigned get_time_counter()
{
	return time_counter;
}

void delay_ms(unsigned delay_in_ms)
{
	auto start_cnt = time_counter;
	auto max_diff = delay_in_ms * 1000U / TimeTimerFreq;
	while ((time_counter - start_cnt) < max_diff) {}
}

void led_on()
{
	gpio_clear(LED_PIN);
}

void led_off()
{
	gpio_set(LED_PIN);
}

int32_t get_steps_counter()
{
	return steps_counter;
}

uint32_t get_value_for_srand()
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


using DisplayConn = mgfxpp::LibOpenCM3_Display_I2C_V1_Conn<DISP_I2C, 0x78, 1'000'000>;
using Display = mgfxpp::sh1106_display<DisplayConn>;
using BufferedDisplay = mgfxpp::MonoBufferedDisplay<Display>;

MGFXPP_DISPLAY_BUFFERED_IMPL(BufferedDisplay)

static bool display_is_initialized = false;

void init_display()
{
	debug_printf("Initializing display... ");

	display_is_initialized = Display::init(false);

	debug_printf("{}\n", display_is_initialized ? "OK" : "FAILED!");

	if (!display_is_initialized) return;

	Display::set_brightness(0x40);

	mgfxpp::display_draw([]
	{
		mgfxpp::display_fill_rect(mgfxpp::display_get_clip_rect(), mgfxpp::Color::PixelOff);
	});
}

bool is_display_ok()
{
	return display_is_initialized;
}

extern "C" void TIME_TIMER_ISR()
{
	if (timer_get_flag(TIME_TIMER, TIM_SR_UIF))
	{
		timer_clear_flag(TIME_TIMER, TIM_SR_UIF);
		++time_counter;

		revert_btn.tick(!gpio_get(REVERT_BTN_PIN));
		dither_time_btn.tick(!gpio_get(DITH_TIME_BTN_PIN));
		dither_angle_btn.tick(!gpio_get(DITH_ANGL_BTN_PIN));
	}
}

extern "C" void STEP_TIMER_ISR()
{
	if (timer_get_flag(STEP_TIMER, TIM_SR_UIF))
	{
		timer_clear_flag(STEP_TIMER, TIM_SR_UIF);

		int32_t inc = 1;
		steps_counter += gpio_get(DIR_PIN) ? inc : -inc;
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
