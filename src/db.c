// Copyright (c) Kuba Szczodrzyński 2025-1-26.

#include "db.h"

static sqlite3 *db				= NULL;
static pthread_mutex_t db_mutex = PTHREAD_MUTEX_INITIALIZER;

static void db_save_record_thread(record_t *record);
static void db_save_trip_thread(trip_t *trip);
static void db_process_trips_thread(void *arg);

sqlite3 *db_connect(const char *filename) {
	if (db != NULL)
		return db;
	if (sqlite3_open(filename, &db) != SQLITE_OK)
		SQLITE3_ERROR("sqlite3_open()", return NULL);

	pthread_mutex_init(&db_mutex, NULL);

	LT_I("Database: opened %s", filename);

	const char *sql = (
		// record
		"CREATE TABLE IF NOT EXISTS record ("
		"start_time INTEGER NOT NULL, "
		"end_time INTEGER NOT NULL, "
		"start_mileage REAL NOT NULL, "
		"end_mileage REAL NOT NULL, "
		"dist INTEGER NOT NULL, "
		"fuel INTEGER NOT NULL, "
		"engine_speed REAL NOT NULL, "
		"engine_speed_max REAL NOT NULL, "
		"vehicle_speed_min REAL NOT NULL, "
		"vehicle_speed_max REAL NOT NULL, "
		"coolant_temp REAL NOT NULL, "
		"outside_temp REAL NOT NULL, "
		"oil_temp REAL NOT NULL, "
		"oil_level REAL NOT NULL, "
		"fuel_level REAL NOT NULL, "
		"fuel_range REAL NOT NULL, "
		"fuel_cons_min REAL NOT NULL, "
		"fuel_cons_max REAL NOT NULL, "
		"trip_id INTEGER DEFAULT NULL, "
		"PRIMARY KEY(start_time, end_time)"
		");"
	);
	if (sqlite3_exec(db, sql, NULL, NULL, NULL) != SQLITE_OK)
		SQLITE3_ERROR("sqlite3_exec(CREATE TABLE)", return NULL);

	sql = (
		// record
		"CREATE TABLE IF NOT EXISTS trip ("
		"trip_id INTEGER NOT NULL PRIMARY KEY, "
		"time INTEGER NOT NULL, "
		"dist INTEGER NOT NULL, "
		"fuel INTEGER NOT NULL, "
		"start_time INTEGER NOT NULL, "
		"end_time INTEGER NOT NULL, "
		"start_mileage REAL NOT NULL, "
		"end_mileage REAL NOT NULL, "
		"engine_speed_max REAL NOT NULL, "
		"vehicle_speed_max REAL NOT NULL, "
		"coolant_temp_avg REAL NOT NULL, "
		"coolant_temp_min REAL NOT NULL, "
		"coolant_temp_max REAL NOT NULL, "
		"outside_temp_avg REAL NOT NULL, "
		"outside_temp_min REAL NOT NULL, "
		"outside_temp_max REAL NOT NULL, "
		"oil_temp_avg REAL NOT NULL, "
		"oil_temp_min REAL NOT NULL, "
		"oil_temp_max REAL NOT NULL, "
		"oil_level_min REAL NOT NULL, "
		"oil_level_max REAL NOT NULL, "
		"fuel_level_min REAL NOT NULL, "
		"fuel_level_max REAL NOT NULL, "
		"fuel_range_min REAL NOT NULL, "
		"fuel_range_max REAL NOT NULL, "
		"fuel_cons_min REAL NOT NULL, "
		"fuel_cons_max REAL NOT NULL"
		");"
	);
	if (sqlite3_exec(db, sql, NULL, NULL, NULL) != SQLITE_OK)
		SQLITE3_ERROR("sqlite3_exec(CREATE TABLE)", return NULL);

	return db;
}

void db_close() {
	pthread_mutex_lock(&db_mutex);
	pthread_mutex_unlock(&db_mutex);
	pthread_mutex_destroy(&db_mutex);
	sqlite3_close(db);
	db = NULL;
}

void db_save_record(record_t *record) {
	if (record->start.time == record->end.time || record->dist == 0) {
		// nothing to save (but process trips anyway)
		db_process_trips();
		return;
	}
	record_t *record_copy;
	MALLOC(record_copy, sizeof(*record_copy), goto error);
	memcpy(record_copy, record, sizeof(*record));

	pthread_t thread;
	if (pthread_create(&thread, NULL, (void *(*)(void *))db_save_record_thread, record_copy) != 0)
		LT_ERR(E, goto error, "Database: cannot create record save thread");

	return;

error:
	free(record_copy);
}

void db_save_trip(trip_t *trip) {
	if (trip->start_time == trip->end_time || trip->dist == 0)
		// nothing to save
		return;
	trip_t *trip_copy;
	MALLOC(trip_copy, sizeof(*trip_copy), goto error);
	memcpy(trip_copy, trip, sizeof(*trip));

	pthread_t thread;
	if (pthread_create(&thread, NULL, (void *(*)(void *))db_save_trip_thread, trip_copy) != 0)
		LT_ERR(E, goto error, "Database: cannot create record save thread");

	return;

error:
	free(trip_copy);
}

void db_process_trips() {
	pthread_t thread;
	if (pthread_create(&thread, NULL, (void *(*)(void *))db_process_trips_thread, NULL) != 0)
		LT_ERR(E, , "Database: cannot create record save thread");
}

static void db_save_record_thread(record_t *record) {
	pthread_mutex_lock(&db_mutex);

	const char *sql = (
		// record
		"INSERT INTO record ("
		"start_time, end_time, start_mileage, end_mileage, "
		"dist, fuel, engine_speed, engine_speed_max, "
		"vehicle_speed_min, vehicle_speed_max, "
		"coolant_temp, outside_temp, oil_temp, oil_level, "
		"fuel_level, fuel_range, fuel_cons_min, fuel_cons_max"
		") VALUES ("
		"?, ?, ?, ?, "
		"?, ?, ?, ?, "
		"?, ?, "
		"?, ?, ?, ?, "
		"?, ?, ?, ?"
		");"
	);

	sqlite3_stmt *stmt = NULL;
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
		SQLITE3_ERROR("sqlite3_prepare_v2()", goto cleanup);

	sqlite3_bind_int64(stmt, 1, (long long)record->start.time);
	sqlite3_bind_int64(stmt, 2, (long long)record->end.time);
	sqlite3_bind_double(stmt, 3, record->start.mileage);
	sqlite3_bind_double(stmt, 4, record->end.mileage);
	sqlite3_bind_int(stmt, 5, (int)record->dist);
	sqlite3_bind_int(stmt, 6, (int)record->fuel);
	sqlite3_bind_double(stmt, 7, round(record->engine_speed.avg * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 8, round(record->engine_speed.max * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 9, round(record->vehicle_speed.min * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 10, round(record->vehicle_speed.max * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 11, round(record->coolant_temp.avg * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 12, round(record->outside_temp.avg * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 13, round(record->oil_temp.avg * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 14, round(record->oil_level.avg * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 15, round(record->fuel_level.avg * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 16, round(record->fuel_range.avg * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 17, round(record->fuel_cons.min * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 18, round(record->fuel_cons.max * 1000.0) / 1000.0);

	if (sqlite3_step(stmt) != SQLITE_DONE)
		SQLITE3_ERROR("sqlite3_step()", goto cleanup);
	else
		LT_I("Database: record saved, end time = %llu", record->end.time);

cleanup:
	sqlite3_finalize(stmt);
	free(record);
	pthread_mutex_unlock(&db_mutex);

	// call the trip processing function *after* saving the record
	db_process_trips();
}

static void db_save_trip_thread(trip_t *trip) {
	pthread_mutex_lock(&db_mutex);

	const char *sql = (
		// trip
		"INSERT INTO trip ("
		"time, dist, fuel, "
		"start_time, end_time, start_mileage, end_mileage, "
		"engine_speed_max, vehicle_speed_max, "
		"coolant_temp_avg, coolant_temp_min, coolant_temp_max, "
		"outside_temp_avg, outside_temp_min, outside_temp_max, "
		"oil_temp_avg, oil_temp_min, oil_temp_max, "
		"oil_level_min, oil_level_max, fuel_level_min, fuel_level_max, "
		"fuel_range_min, fuel_range_max, fuel_cons_min, fuel_cons_max"
		") VALUES ("
		"?, ?, ?, "
		"?, ?, ?, ?, "
		"?, ?, "
		"?, ?, ?, "
		"?, ?, ?, "
		"?, ?, ?, "
		"?, ?, ?, ?, "
		"?, ?, ?, ?"
		");"
	);
	sqlite3_stmt *stmt = NULL;
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
		SQLITE3_ERROR("sqlite3_prepare_v2()", goto cleanup);

	sqlite3_bind_int(stmt, 1, (int)trip->time);
	sqlite3_bind_int(stmt, 2, (int)trip->dist);
	sqlite3_bind_int(stmt, 3, (int)trip->fuel);
	sqlite3_bind_int64(stmt, 4, (long long)trip->start_time);
	sqlite3_bind_int64(stmt, 5, (long long)trip->end_time);
	sqlite3_bind_double(stmt, 6, trip->start_mileage);
	sqlite3_bind_double(stmt, 7, trip->end_mileage);
	sqlite3_bind_double(stmt, 8, round(trip->engine_speed.max * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 9, round(trip->vehicle_speed.max * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 10, round(trip->coolant_temp.avg * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 11, round(trip->coolant_temp.min * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 12, round(trip->coolant_temp.max * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 13, round(trip->outside_temp.avg * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 14, round(trip->outside_temp.min * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 15, round(trip->outside_temp.max * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 16, round(trip->oil_temp.avg * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 17, round(trip->oil_temp.min * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 18, round(trip->oil_temp.max * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 19, round(trip->oil_level.min * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 20, round(trip->oil_level.max * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 21, round(trip->fuel_level.min * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 22, round(trip->fuel_level.max * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 23, round(trip->fuel_range.min * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 24, round(trip->fuel_range.max * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 25, round(trip->fuel_cons.min * 1000.0) / 1000.0);
	sqlite3_bind_double(stmt, 26, round(trip->fuel_cons.max * 1000.0) / 1000.0);

	if (sqlite3_step(stmt) != SQLITE_DONE)
		SQLITE3_ERROR("sqlite3_step()", goto cleanup);

	long long trip_id = sqlite3_last_insert_rowid(db);
	LT_I("Database: trip saved, trip ID = %lld", trip_id);

	sqlite3_finalize(stmt);
	sql = (
		// record
		"UPDATE record "
		"SET trip_id = ? "
		"WHERE start_time >= ? AND start_time < ? "
		"AND end_time > ? AND end_time <= ?;"
	);
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
		SQLITE3_ERROR("sqlite3_prepare_v2()", goto cleanup);

	sqlite3_bind_int64(stmt, 1, trip_id);
	sqlite3_bind_int64(stmt, 2, (long long)trip->start_time);
	sqlite3_bind_int64(stmt, 3, (long long)trip->end_time);
	sqlite3_bind_int64(stmt, 4, (long long)trip->start_time);
	sqlite3_bind_int64(stmt, 5, (long long)trip->end_time);

	if (sqlite3_step(stmt) != SQLITE_DONE)
		SQLITE3_ERROR("sqlite3_step()", goto cleanup);

cleanup:
	sqlite3_finalize(stmt);
	free(trip);
	pthread_mutex_unlock(&db_mutex);
}

static void db_process_trips_thread(void *arg) {
	pthread_mutex_lock(&db_mutex);

	const char *sql = (
		// record
		"SELECT "
		"start_time, end_time, start_mileage, end_mileage, "
		"dist, fuel, engine_speed, engine_speed_max, "
		"vehicle_speed_min, vehicle_speed_max, "
		"coolant_temp, outside_temp, oil_temp, oil_level, "
		"fuel_level, fuel_range, fuel_cons_min, fuel_cons_max "
		"FROM record "
		"WHERE trip_id IS NULL "
		"ORDER BY start_time;"
	);

	sqlite3_stmt *stmt = NULL;
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
		SQLITE3_ERROR("sqlite3_prepare_v2()", goto cleanup);

	trip_t trip		= {0};
	record_t record = {0};
	trip_reset(&trip);
	record_reset(&record);

	while (1) {
		int ret = sqlite3_step(stmt);
		if (ret == SQLITE_BUSY) {
			sleep(1);
			continue;
		}
		if (ret == SQLITE_DONE)
			break;
		if (ret != SQLITE_ROW)
			SQLITE3_ERROR("sqlite3_step()", goto cleanup);

		record.start.time		 = sqlite3_column_int64(stmt, 0);
		record.end.time			 = sqlite3_column_int64(stmt, 1);
		record.start.mileage	 = sqlite3_column_double(stmt, 2);
		record.end.mileage		 = sqlite3_column_double(stmt, 3);
		record.dist				 = sqlite3_column_int(stmt, 4);
		record.fuel				 = sqlite3_column_int(stmt, 5);
		record.engine_speed.avg	 = sqlite3_column_double(stmt, 6);
		record.engine_speed.max	 = sqlite3_column_double(stmt, 7);
		record.vehicle_speed.min = sqlite3_column_double(stmt, 8);
		record.vehicle_speed.max = sqlite3_column_double(stmt, 9);
		record.coolant_temp.avg	 = sqlite3_column_double(stmt, 10);
		record.outside_temp.avg	 = sqlite3_column_double(stmt, 11);
		record.oil_temp.avg		 = sqlite3_column_double(stmt, 12);
		record.oil_level.avg	 = sqlite3_column_double(stmt, 13);
		record.fuel_level.avg	 = sqlite3_column_double(stmt, 14);
		record.fuel_range.avg	 = sqlite3_column_double(stmt, 15);
		record.fuel_cons.min	 = sqlite3_column_double(stmt, 16);
		record.fuel_cons.max	 = sqlite3_column_double(stmt, 17);

		if (trip.end_time != 0 && (record.end.time - trip.end_time) > 5 * 60 * 1000) {
			// start a new trip if there was no record for 5 min
			db_save_trip(&trip);
			trip_print(&trip);
			trip_reset(&trip);
		}

		// add this record to the trip
		trip_append(&trip, &record);
	}

	if (trip.end_time != 0 && (millis() - trip.end_time) > 5 * 60 * 1000) {
		// save the last records if they are older than 5 min
		db_save_trip(&trip);
		trip_print(&trip);
	}

cleanup:
	sqlite3_finalize(stmt);
	pthread_mutex_unlock(&db_mutex);
}
