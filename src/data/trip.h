// Copyright (c) Kuba Szczodrzyński 2025-1-26.

#pragma once

#include "include.h"

typedef struct record_t record_t;

typedef struct trip_t {
	unsigned int time; //!< Total trip run time (ms)
	unsigned int dist; //!< Total trip distance (cm)
	unsigned int fuel; //!< Total trip fuel usage (mm³)

	unsigned long long start_time; //!< Start time of first record
	unsigned long long end_time;   //!< End time of last record
	double start_mileage;		   //!< Start mileage of first record
	double end_mileage;			   //!< End mileage of last record

	measurement_t engine_speed;	 //!< Engine speed - max only (RPM)
	measurement_t vehicle_speed; //!< Vehicle speed - max only (km/h)
	measurement_t coolant_temp;	 //!< Coolant temperature (°C)
	measurement_t outside_temp;	 //!< Outside temperature (°C)
	measurement_t oil_temp;		 //!< Oil temperature (°C)
	measurement_t oil_level;	 //!< Oil level - min/max only (%)
	measurement_t fuel_level;	 //!< Fuel level - min/max only (%)
	measurement_t fuel_range;	 //!< Approximate remaining range - min/max only (km)
	measurement_t fuel_cons;	 //!< Instant fuel consumption - min/max only (l/100 km)
} trip_t;

void trip_reset(trip_t *trip);
void trip_append(trip_t *trip, record_t *record);
void trip_print(trip_t *trip);
