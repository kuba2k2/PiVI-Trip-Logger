// Copyright (c) Kuba Szczodrzyński 2025-1-26.

#pragma once

#include "include.h"

typedef struct record_t record_t;
typedef struct trip_t trip_t;

sqlite3 *db_connect(const char *filename);
void db_close();
void db_save_record(record_t *record);
void db_save_trip(trip_t *trip);
void db_process_trips();
