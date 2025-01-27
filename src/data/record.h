// Copyright (c) Kuba Szczodrzyński 2025-1-26.

#pragma once

#include "include.h"

#include "measurement.h"

typedef struct frame_t frame_t;

typedef struct record_stat_t {
	unsigned long long time; //!< Time of the measurements

	double mileage; //!< Total mileage (km)

	bool is_init;					 //!< Whether trip data is populated
	unsigned int trip_time;			 //!< Trip time (min)
	unsigned int trip_dist;			 //!< Trip distance (km)
	unsigned int trip_avg_speed;	 //!< Trip average speed (km/h)
	unsigned int trip_avg_fuel_cons; //!< Trip average fuel consumption (0.1 * l/100 km)
} record_stat_t;

typedef struct record_t {
	record_stat_t start; //!< Statistics at the start of the record
	record_stat_t end;	 //!< Statistics at the end of the record

	measurement_t engine_speed;	 //!< Engine speed (RPM)
	measurement_t vehicle_speed; //!< Vehicle speed (km/h)

	bool is_init;			//!< Whether distance/fuel data is initialized
	unsigned int dist_last; //!< Last distance value
	unsigned int fuel_last; //!< Last fuel value
	unsigned int dist;		//!< Distance (cm)
	unsigned int fuel;		//!< Fuel (mm³)

	measurement_t coolant_temp; //!< Coolant temperature (°C)
	measurement_t outside_temp; //!< Outside temperature (°C)
	measurement_t oil_temp;		//!< Oil temperature (°C)
	measurement_t oil_level;	//!< Oil level (%)
	measurement_t fuel_level;	//!< Fuel level (%)

	measurement_t fuel_cons;  //!< Instant fuel consumption (l/100 km)
	measurement_t fuel_range; //!< Approximate remaining range (km)
} record_t;

void record_reset(record_t *record);
void record_append(record_t *record, frame_t *frame);
void record_print(record_t *record);
