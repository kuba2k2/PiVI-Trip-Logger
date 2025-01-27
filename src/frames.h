// Copyright (c) Kuba Szczodrzyński 2025-1-25.

#pragma once

#include "include.h"

typedef enum {
	FRAME_BSI_COMMAND  = 0x036,
	FRAME_BSI_FAST	   = 0x0B6,
	FRAME_BSI_SLOW	   = 0x0F6,
	FRAME_TEMP_LEVEL   = 0x161,
	FRAME_TRIP_GENERAL = 0x221,
	FRAME_TRIP_DATA_1  = 0x2A1,
	FRAME_TRIP_DATA_2  = 0x261,
} frame_type_t;

typedef enum {
	NETWORK_STATE_WAKING  = 0b000,
	NETWORK_STATE_NORMAL  = 0b001,
	NETWORK_STATE_STANDBY = 0b010,
	NETWORK_STATE_WAKING2 = 0b011,
	NETWORK_STATE_COM_OFF = 0b100,
} network_state_t;

typedef struct frame_bsi_command_t {
	bool economy_mode;
	unsigned int power_level;
	network_state_t network_state;
} frame_bsi_command_t;

typedef struct frame_bsi_fast_t {
	unsigned int engine_speed;	//!< Resolution: 0.125 RPM
	unsigned int vehicle_speed; //!< Resolution: 0.01 km/h
	unsigned int dist;			//!< Resolution: 0.1 m
	unsigned int fuel;			//!< Resolution: 80 mm³
} frame_bsi_fast_t;

typedef struct frame_bsi_slow_t {
	unsigned int state_sev;
	unsigned int state_gen;
	unsigned int state_gmp;
	int coolant_temp;			//!< Resolution: 1°C
	unsigned int total_mileage; //!< Resolution: 0.1 km
	int outside_temp;			//!< Resolution: 0.5°C
} frame_bsi_slow_t;

typedef struct frame_temp_level_t {
	int oil_temp;			 //!< Resolution: 1°C
	unsigned int fuel_level; //!< Resolution: 1%
	unsigned int oil_level;	 //!< Resolution: 1%
} frame_temp_level_t;

typedef struct frame_trip_general_t {
	bool invalid_cons;
	bool invalid_range;
	unsigned int fuel_cons;	 //!< Resolution: 0.1 l/100 km
	unsigned int fuel_range; //!< Resolution: 1 km
	unsigned int route_dist; //!< Resolution: 0.1 km
} frame_trip_general_t;

typedef struct frame_trip_data_t {
	unsigned int speed;		 //!< Resolution: 1 km/h
	unsigned int total_dist; //!< Resolution: 1 km
	unsigned int fuel_cons;	 //!< Resolution: 0.1 l/100 km
	unsigned int total_time; //!< Resolution: 1 min
} frame_trip_data_t;

typedef struct frame_t {
	frame_type_t type;

	union {
		frame_bsi_command_t bsi_command;
		frame_bsi_fast_t bsi_fast;
		frame_bsi_slow_t bsi_slow;
		frame_temp_level_t temp_level;
		frame_trip_general_t trip_general;
		frame_trip_data_t trip_data_1;
		frame_trip_data_t trip_data_2;
	};
} frame_t;

bool frame_parse(struct can_frame *can_frame, frame_t *frame);
void frame_print(frame_t *frame);
