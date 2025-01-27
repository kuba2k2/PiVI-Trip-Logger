// Copyright (c) Kuba Szczodrzyński 2025-1-25.

#include "frames.h"

bool frame_parse(struct can_frame *can_frame, frame_t *frame) {
	uint8_t *data = can_frame->data;
	switch (can_frame->can_id) {
		case FRAME_BSI_COMMAND:
			frame->bsi_command.economy_mode	 = data[2] & 0x80;
			frame->bsi_command.power_level	 = data[2] & 0b1111;
			frame->bsi_command.network_state = data[4] & 0b111;
			break;

		case FRAME_BSI_FAST:
			frame->bsi_fast.engine_speed  = (data[0] << 8) | (data[1] << 0);
			frame->bsi_fast.vehicle_speed = (data[2] << 8) | (data[3] << 0);
			frame->bsi_fast.dist		  = (data[4] << 8) | (data[5] << 0);
			frame->bsi_fast.fuel		  = data[6];
			break;

		case FRAME_BSI_SLOW:
			frame->bsi_slow.state_sev	  = (data[0] >> 3) & 0b11;
			frame->bsi_slow.state_gen	  = (data[0] >> 2) & 0b1;
			frame->bsi_slow.state_gmp	  = (data[0] >> 0) & 0b11;
			frame->bsi_slow.coolant_temp  = data[1] - 40;
			frame->bsi_slow.total_mileage = (data[2] << 16) | (data[3] << 8) | (data[4] << 0);
			frame->bsi_slow.outside_temp  = data[6] - 80;
			break;

		case FRAME_TEMP_LEVEL:
			frame->temp_level.oil_temp	 = data[2] - 40;
			frame->temp_level.fuel_level = data[3];
			if (data[6] < 0xFB)
				frame->temp_level.oil_level = data[6];
			else
				frame->temp_level.oil_level = 0xFF;
			break;

		case FRAME_TRIP_GENERAL:
			frame->trip_general.invalid_cons  = data[0] & 0x80;
			frame->trip_general.invalid_range = data[0] & 0x40;
			frame->trip_general.fuel_cons	  = (data[1] << 8) | (data[2] << 0);
			frame->trip_general.fuel_range	  = (data[3] << 8) | (data[4] << 0);
			frame->trip_general.route_dist	  = (data[5] << 8) | (data[6] << 0);
			if (frame->trip_general.fuel_cons == 0xFFFF)
				frame->trip_general.invalid_cons = true;
			break;

		case FRAME_TRIP_DATA_1:
			frame->trip_data_1.speed	  = data[0];
			frame->trip_data_1.total_dist = (data[1] << 8) | (data[2] << 0);
			frame->trip_data_1.fuel_cons  = (data[3] << 8) | (data[4] << 0);
			frame->trip_data_1.total_time = (data[5] << 8) | (data[6] << 0);
			break;

		case FRAME_TRIP_DATA_2:
			frame->trip_data_2.speed	  = data[0];
			frame->trip_data_2.total_dist = (data[1] << 8) | (data[2] << 0);
			frame->trip_data_2.fuel_cons  = (data[3] << 8) | (data[4] << 0);
			frame->trip_data_2.total_time = (data[5] << 8) | (data[6] << 0);
			break;

		default:
			return false;
	}
	frame->type = can_frame->can_id;
	return true;
}

void frame_print(frame_t *frame) {
	char buf1[16];
	char buf2[16];

	switch (frame->type) {
		case FRAME_BSI_COMMAND:
			LT_I(
				"BSI_COMMAND - Economy mode: %s, Power level: %u, Network state: %u",
				frame->bsi_command.economy_mode ? "true" : "false",
				frame->bsi_command.power_level,
				frame->bsi_command.network_state
			);
			break;

		case FRAME_BSI_FAST:
			LT_I(
				"BSI_FAST - Engine: %.3f RPM, Speed: %.2f km/h, Distance: %.1f m, Fuel: %u mm³",
				frame->bsi_fast.engine_speed * 0.125f,
				frame->bsi_fast.vehicle_speed * 0.01f,
				frame->bsi_fast.dist * 0.1f,
				frame->bsi_fast.fuel * 80
			);
			break;

		case FRAME_BSI_SLOW:
			LT_I(
				"BSI_SLOW - Coolant temp.: %d°C, Mileage: %.1f km, Outside temp.: %.1f°C",
				frame->bsi_slow.coolant_temp,
				frame->bsi_slow.total_mileage * 0.1f,
				frame->bsi_slow.outside_temp * 0.5f
			);
			break;

		case FRAME_TEMP_LEVEL:
			LT_I(
				"TEMP_LEVEL - Oil temp.: %d°C, Fuel level: %u%%, Oil level: %u%%",
				frame->temp_level.oil_temp,
				frame->temp_level.fuel_level,
				frame->temp_level.oil_level
			);
			break;

		case FRAME_TRIP_GENERAL:
			if (frame->trip_general.invalid_cons)
				strcpy(buf1, "(invalid)");
			else
				sprintf(buf1, "%.1f l/100 km", (float)frame->trip_general.fuel_cons * 0.1f);
			if (frame->trip_general.invalid_range)
				strcpy(buf2, "(invalid)");
			else
				sprintf(buf2, "%u km", frame->trip_general.fuel_range);
			LT_I(
				"TRIP_GENERAL - Fuel cons.: %s, Remaining distance: %s, Route distance: %.1f",
				buf1,
				buf2,
				frame->trip_general.route_dist * 0.1f
			);
			break;

		case FRAME_TRIP_DATA_1:
			LT_I(
				"TRIP_DATA_1 - Avg. speed: %u km/h, Distance: %u km, Avg. fuel cons.: %.1f l/100 km, Time: %u min",
				frame->trip_data_1.speed,
				frame->trip_data_1.total_dist,
				frame->trip_data_1.fuel_cons * 0.1f,
				frame->trip_data_1.total_time
			);
			break;

		case FRAME_TRIP_DATA_2:
			LT_I(
				"TRIP_DATA_2 - Avg. speed: %u km/h, Distance: %u km, Avg. fuel cons.: %.1f l/100 km, Time: %u min",
				frame->trip_data_2.speed,
				frame->trip_data_2.total_dist,
				frame->trip_data_2.fuel_cons * 0.1f,
				frame->trip_data_2.total_time
			);
			break;
	}
}
