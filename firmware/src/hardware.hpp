#pragma once

#include <stdint.h>

#include "config.hpp"

constexpr unsigned SysClockFreq = 72'000'000;
constexpr unsigned APB1Freq = 36'000'000;

constexpr double TurnsOnStep = 1.0 / (MotorSteps * MotorMicroSteps); // rod rotations per one motor step

constexpr unsigned TimeTimerFreq = 1'000; // Hz

void init_hardware();

void set_rotations_per_seconds(float value);

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