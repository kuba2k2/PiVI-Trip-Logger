// Copyright (c) Kuba Szczodrzyński 2025-1-26.

#include "trip.h"

void trip_reset(trip_t *trip) {
	memset(trip, 0, sizeof(*trip));
}

void trip_append(trip_t *trip, record_t *record) {
	trip->time += (record->end.time - record->start.time);
	trip->dist += record->dist;
	trip->fuel += record->fuel;

	if (trip->start_time == 0)
		trip->start_time = record->start.time;
	else
		trip->start_time = min(trip->start_time, record->start.time);
	trip->end_time = max(trip->end_time, record->end.time);

	if (trip->start_mileage == 0.0)
		trip->start_mileage = record->start.mileage;
	else
		trip->start_mileage = min(trip->start_mileage, record->start.mileage);
	trip->end_mileage = max(trip->end_mileage, record->end.mileage);

	// max only
	measurement_append(&trip->engine_speed, record->engine_speed.max);
	measurement_append(&trip->vehicle_speed, record->vehicle_speed.max);
	// min/max/avg
	measurement_append(&trip->coolant_temp, record->coolant_temp.avg);
	measurement_append(&trip->outside_temp, record->outside_temp.avg);
	measurement_append(&trip->oil_temp, record->oil_temp.avg);
	// min/max only
	measurement_append(&trip->oil_level, record->oil_temp.avg);
	measurement_append(&trip->fuel_level, record->fuel_level.avg);
	measurement_append(&trip->fuel_range, record->fuel_range.avg);
	measurement_append(&trip->fuel_cons, record->fuel_cons.min);
	measurement_append(&trip->fuel_cons, record->fuel_cons.max);
}

void trip_print(trip_t *trip) {
	LT_I(
		"Trip: %llu-%llu, time: %u ms, dist: %u cm, fuel: %u mm³",
		trip->start_time,
		trip->end_time,
		trip->time,
		trip->dist,
		trip->fuel
	);
	LT_I("Trip: %.1f km - %.1f km", trip->start_mileage, trip->end_mileage);
	LT_I(
		"Trip: engine speed (max): %u RPM, vehicle speed (max): %.2f km/h",
		(int)trip->engine_speed.max,
		trip->vehicle_speed.max
	);
	LT_I("Trip: fuel level (min): %u%%, fuel level (max): %u%%", (int)trip->fuel_level.min, (int)trip->fuel_level.max);
	LT_I(
		"Trip: fuel cons. (min): %.1f l/100 km, fuel cons. (max): %.1f l/100 km",
		trip->fuel_cons.min,
		trip->fuel_cons.max
	);
	if (trip->dist && trip->time)
		LT_I(
			"Trip: avg speed: %.1f km/h, avg cons.: %.1f l/100 km",
			trip->dist / (double)trip->time * 36.0,
			trip->fuel * 10.0 / trip->dist
		);
}
