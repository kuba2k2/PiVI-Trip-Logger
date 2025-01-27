// Copyright (c) Kuba Szczodrzyński 2025-1-26.

#pragma once

#include "include.h"

typedef struct measurement_t {
	bool is_init;
	double min;
	double max;
	double avg;
	unsigned int count;
} measurement_t;

void measurement_append(measurement_t *meas, double value);
