#pragma once

#include <stdint.h>

/******* geomerty *******/

// side length of an equilateral triangle (in mm)
constexpr double R = 133.8;

// base length of an equilateral triangle (in mm) in start position
constexpr double StartL = 44.2;

// Maximul len (in mm)
constexpr double MaxL = 180.0;


/******* rod parameters *******/

// rod step in mm
constexpr double RodStep = 0.8;


/******* stepper motor parameters *******/

// steps per rotation (200 for 1.8°, 400 for 0.9°)
constexpr double MotorSteps = 200.0;

// microsteps on motos driver
constexpr int32_t MotorMicroSteps = 16;


/******* periodical random moving (dithering) *******/

// Maximum dither angle in °
constexpr double MoveMaxAngle = 0.3;

// Maximum dither period in minutes
constexpr int32_t MovePeriod = 10;
