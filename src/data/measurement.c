// Copyright (c) Kuba Szczodrzyński 2025-1-26.

#include "include.h"

void measurement_append(measurement_t *meas, double value) {
	meas->count++;
	meas->avg += (value - meas->avg) / (double)meas->count;
	if (!meas->is_init) {
		meas->min	  = value;
		meas->max	  = value;
		meas->is_init = true;
	} else {
		if (value < meas->min)
			meas->min = value;
		if (value > meas->max)
			meas->max = value;
	}
}
