/*
 * Copyright (c) Kuba Szczodrzyński 2025-1-26.
 */

import moment, { duration, Duration, Moment } from "moment"

export type Trip = {
	tripId: number | null
	time: Duration
	dist: number // kilometers
	fuel: number // liters
	startTime: Moment
	endTime: Moment
	startMileage: number
	endMileage: number
	engineSpeedMax: number
	vehicleSpeedMax: number
	coolantTempAvg: number
	coolantTempMin: number
	coolantTempMax: number
	outsideTempAvg: number
	outsideTempMin: number
	outsideTempMax: number
	oilTempAvg: number
	oilTempMin: number
	oilTempMax: number
	oilLevelMin: number
	oilLevelMax: number
	fuelLevelMin: number
	fuelLevelMax: number
	fuelRangeMin: number
	fuelRangeMax: number
	fuelConsMin: number
	fuelConsMax: number
}

export function mapToTrip(trip: any): Trip {
	return {
		tripId: trip.trip_id,
		time: duration(trip.time),
		dist: trip.dist / 100.0 / 1000.0,
		fuel: trip.fuel / 1000.0 / 1000.0,
		startTime: moment(trip.start_time),
		endTime: moment(trip.end_time),
		startMileage: trip.start_mileage,
		endMileage: trip.end_mileage,
		engineSpeedMax: trip.engine_speed_max,
		vehicleSpeedMax: trip.vehicle_speed_max,
		coolantTempAvg: trip.coolant_temp_avg,
		coolantTempMin: trip.coolant_temp_min,
		coolantTempMax: trip.coolant_temp_max,
		outsideTempAvg: trip.outside_temp_avg,
		outsideTempMin: trip.outside_temp_min,
		outsideTempMax: trip.outside_temp_max,
		oilTempAvg: trip.oil_temp_avg,
		oilTempMin: trip.oil_temp_min,
		oilTempMax: trip.oil_temp_max,
		oilLevelMin: trip.oil_level_min,
		oilLevelMax: trip.oil_level_max,
		fuelLevelMin: trip.fuel_level_min,
		fuelLevelMax: trip.fuel_level_max,
		fuelRangeMin: trip.fuel_range_min,
		fuelRangeMax: trip.fuel_range_max,
		fuelConsMin: trip.fuel_cons_min,
		fuelConsMax: trip.fuel_cons_max,
	}
}
