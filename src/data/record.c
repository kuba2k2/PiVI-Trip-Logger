// Copyright (c) Kuba Szczodrzyński 2025-1-26.

#include "record.h"

void record_reset(record_t *record) {
	bool is_init		   = record->is_init;
	unsigned int dist_last = record->dist_last;
	unsigned int fuel_last = record->fuel_last;

	memset(record, 0, sizeof(*record));

	if (is_init) {
		record->is_init	  = true;
		record->dist_last = dist_last;
		record->fuel_last = fuel_last;
	}
}

void record_append(record_t *record, frame_t *frame) {
	unsigned long long now = millis();
	if (record->start.time == 0)
		record->start.time = now;
	record->end.time = now;

	switch (frame->type) {
		case FRAME_BSI_COMMAND:
			break;

		case FRAME_BSI_FAST:
			measurement_append(&record->engine_speed, frame->bsi_fast.engine_speed * 0.125);
			measurement_append(&record->vehicle_speed, frame->bsi_fast.vehicle_speed * 0.01);
			unsigned int dist_raw = frame->bsi_fast.dist * 10;
			unsigned int fuel_raw = frame->bsi_fast.fuel * 80;
			if (!record->is_init) {
				record->is_init = true;
			} else {
				unsigned int dist = dist_raw;
				unsigned int fuel = fuel_raw;
				if (dist < record->dist_last)
					dist += 65535 * 10;
				if (fuel < record->fuel_last)
					fuel += 255 * 80;
				record->dist += dist - record->dist_last;
				record->fuel += fuel - record->fuel_last;
			}
			record->dist_last = dist_raw;
			record->fuel_last = fuel_raw;
			break;

		case FRAME_BSI_SLOW:
			measurement_append(&record->coolant_temp, frame->bsi_slow.coolant_temp);
			measurement_append(&record->outside_temp, frame->bsi_slow.outside_temp * 0.5);
			if (record->start.mileage == 0.0)
				record->start.mileage = frame->bsi_slow.total_mileage * 0.1;
			record->end.mileage = frame->bsi_slow.total_mileage * 0.1;
			break;

		case FRAME_TEMP_LEVEL:
			measurement_append(&record->oil_temp, frame->temp_level.oil_temp);
			measurement_append(&record->oil_level, frame->temp_level.oil_level);
			measurement_append(&record->fuel_level, frame->temp_level.fuel_level);
			break;

		case FRAME_TRIP_GENERAL:
			if (!frame->trip_general.invalid_cons)
				measurement_append(&record->fuel_cons, frame->trip_general.fuel_cons * 0.1);
			if (!frame->trip_general.invalid_range)
				measurement_append(&record->fuel_range, frame->trip_general.fuel_range);
			break;

		case FRAME_TRIP_DATA_1:
			if (!record->start.is_init) {
				record->start.trip_time			 = frame->trip_data_1.total_time;
				record->start.trip_dist			 = frame->trip_data_1.total_dist;
				record->start.trip_avg_speed	 = frame->trip_data_1.speed;
				record->start.trip_avg_fuel_cons = frame->trip_data_1.fuel_cons;
				record->start.is_init			 = true;
			}
			record->end.trip_time		   = frame->trip_data_1.total_time;
			record->end.trip_dist		   = frame->trip_data_1.total_dist;
			record->end.trip_avg_speed	   = frame->trip_data_1.speed;
			record->end.trip_avg_fuel_cons = frame->trip_data_1.fuel_cons;
			record->end.is_init			   = true;
			break;

		case FRAME_TRIP_DATA_2:
			break;
	}
}

void record_print(record_t *record) {
	if (record->start.time == record->end.time)
		return;
	LT_I(
		"Time: %lld sec/%u sec - "
		"Distance: %.3f km/%u km - "
		"Fuel: %.3f l (%u%%) - "
		"Engine speed (avg): %.0f RPM - "
		"Vehicle speed (avg/calc): %.2f km/h / %.2f km/h - "
		"Fuel consumption (avg/calc): %.1f l/100 km / %.1f l/100 km",
		(record->end.time - record->start.time) / 1000,
		(record->end.trip_time - record->start.trip_time) * 60,
		record->dist * 0.00001,
		record->end.trip_dist - record->start.trip_dist,
		record->fuel * 0.000001,
		(unsigned int)record->fuel_level.avg,
		record->engine_speed.avg,
		record->vehicle_speed.avg,
		record->dist / (double)(record->end.time - record->start.time) * 36.0,
		record->fuel_cons.avg,
		record->fuel * 10.0 / max(record->dist, 1)
	);
}
