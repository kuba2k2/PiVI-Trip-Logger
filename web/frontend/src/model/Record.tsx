/*
 * Copyright (c) Kuba Szczodrzyński 2025-1-26.
 */

import moment, { Moment } from "moment"

export type Record = {
	startTime: Moment
	endTime: Moment
	startMileage: number
	endMileage: number
	dist: number // kilometers
	fuel: number // liters
	engineSpeed: number
	engineSpeedMax: number
	vehicleSpeedMin: number
	vehicleSpeedMax: number
	coolantTemp: number
	outsideTemp: number
	oilTemp: number
	oilLevel: number
	fuelLevel: number
	fuelRange: number
	fuelConsMin: number
	fuelConsMax: number
	tripId: number | null
}

export function mapToRecord(record: any): Record {
	return {
		startTime: moment(record.start_time),
		endTime: moment(record.end_time),
		startMileage: record.start_mileage,
		endMileage: record.end_mileage,
		dist: record.dist / 100.0 / 1000.0,
		fuel: record.fuel / 1000.0 / 1000.0,
		engineSpeed: record.engine_speed,
		engineSpeedMax: record.engine_speed_max,
		vehicleSpeedMin: record.vehicle_speed_min,
		vehicleSpeedMax: record.vehicle_speed_max,
		coolantTemp: record.coolant_temp,
		outsideTemp: record.outside_temp,
		oilTemp: record.oil_temp,
		oilLevel: record.oil_level,
		fuelLevel: record.fuel_level,
		fuelRange: record.fuel_range,
		fuelConsMin: record.fuel_cons_min,
		fuelConsMax: record.fuel_cons_max,
		tripId: record.trip_id,
	}
}
