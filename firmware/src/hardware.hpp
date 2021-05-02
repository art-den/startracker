#pragma once

#include <stdint.h>

constexpr unsigned SysClockFreq = 72'000'000;
constexpr unsigned APB1Freq = 36'000'000;
constexpr unsigned TimerClock = 1'000'000;
constexpr unsigned TimeTimerFreq = 1'000; // Hz

/* stepper motor parameters */

constexpr double MotorSteps = 200.0; // steps per rotation
constexpr int32_t MotorMicroSteps = 16; // microsteps

void init_hardware();

void enable_step_timer(bool enable);
bool is_step_timer_enabled();
void set_step_timer_period(uint16_t value);
void set_forward_direction();
void set_revert_direction();

void send_debug_uart_char(char chr);

bool is_revert_btn_pressed();
bool is_dither_time_btn_pressed();
bool is_dither_angle_btn_pressed();

unsigned get_time_counter();

void delay_ms(unsigned delay_in_ms);

void led_on();

void led_off();

int32_t get_steps_counter();

uint32_t get_value_for_srand();

void init_display();

bool is_display_ok();