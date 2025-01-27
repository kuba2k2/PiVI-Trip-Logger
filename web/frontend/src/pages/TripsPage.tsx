/*
 * Copyright (c) Kuba Szczodrzyński 2025-1-26.
 */

import React from "react"
import { Table } from "react-bootstrap"
import { mapToTrip, Trip } from "../model/Trip"
import moment from "moment"
import "moment/dist/locale/pl"
import "moment/locale/pl"

type TripsPageState = {
	trips?: Trip[]
	error?: string
}

export default class TripsPage extends React.Component<any, TripsPageState> {
	constructor(props: any) {
		super(props)
		this.state = {}
	}

	componentDidMount() {
		this.loadData()
	}

	render() {
		if (!this.state.trips)
			return (
				<div>
					<h2 className="my-3">Ostatnie trasy</h2>
					{this.state.error && <p>Error: {this.state.error}</p>}
					{!this.state.error && <p>Loading...</p>}
				</div>
			)

		moment.locale("pl")

		const hideOnMedium = "d-none d-lg-table-cell"
		const hideOnSmall = "d-none d-md-table-cell"
		const showOnMedium = "d-xs-none d-md-table-cell d-lg-none"
		return (
			<div>
				<h2 className="my-3">Ostatnie trasy</h2>
				<Table responsive={true} striped={true} variant="sm">
					<thead>
						<tr>
							<th>Data</th>
							<th>Godzina</th>
							<th>Dystans</th>
							<th className={hideOnMedium}>Czas jazdy</th>
							<th className={hideOnMedium}>Paliwo</th>
							<th className={hideOnSmall}>Śr. prędkość</th>
							<th className={hideOnSmall}>Śr. spalanie</th>
							<th className={hideOnMedium}>Max. prędkość</th>
							<th className={hideOnMedium}>Temperatura</th>
						</tr>
					</thead>
					<tbody>
						{this.state.trips.map((trip) => (
							<tr>
								<td className={showOnMedium}>
									{trip.startTime.format("LL")}
								</td>
								<td className={hideOnMedium}>
									{trip.startTime.format("dddd, LL")}
								</td>
								<td>
									{trip.startTime.format("HH:mm")} -{" "}
									{trip.endTime.format("HH:mm")}
								</td>
								<td>{trip.dist.toFixed(2)} km</td>
								<td className={hideOnMedium}>
									{(trip.time.hours() &&
										`${trip.time.hours()} h`) ||
										""}{" "}
									{(trip.time.minutes() &&
										`${trip.time.minutes()} min`) ||
										""}
								</td>
								<td className={hideOnMedium}>
									{trip.fuel.toFixed(2)} l
								</td>
								<td className={hideOnSmall}>
									{(trip.dist / trip.time.asHours()).toFixed(
										1
									)}{" "}
									km/h
								</td>
								<td className={hideOnSmall}>
									{((trip.fuel / trip.dist) * 100.0).toFixed(
										1
									)}{" "}
									l/100 km
								</td>
								<td className={hideOnMedium}>
									{trip.vehicleSpeedMax.toFixed(1)} km/h
								</td>
								<td className={hideOnMedium}>
									{trip.outsideTempAvg.toFixed(1)}°C
								</td>
							</tr>
						))}
					</tbody>
				</Table>
			</div>
		)
	}

	async loadData() {
		const response = await fetch("/api/trips")
		const tripList: any[] = await response.json()
		const trips: Trip[] = tripList.map(mapToTrip)
		this.setState({ trips })
	}
}
