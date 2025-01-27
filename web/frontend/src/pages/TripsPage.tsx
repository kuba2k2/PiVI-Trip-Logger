/*
 * Copyright (c) Kuba Szczodrzyński 2025-1-26.
 */

import React from "react"
import { Card, PageItem, Pagination, Table } from "react-bootstrap"
import { mapToTrip, Trip } from "../model/Trip"
import moment, { duration, Duration } from "moment"
import "moment/dist/locale/pl"
import "moment/locale/pl"
import StatCard from "../components/StatCard"
import { LinkContainer } from "react-router-bootstrap"
import { Link } from "react-router-dom"

type TripsPageState = {
	trips?: Trip[]
	error?: string
	before?: number
	prevBefore: (number | undefined)[]
}

export default class TripsPage extends React.Component<any, TripsPageState> {
	constructor(props: any) {
		super(props)
		this.state = { prevBefore: [] }
	}

	render() {
		if (!this.state.trips) {
			if (!this.state.error) this.loadData()
			return (
				<div>
					{this.state.error && <p>Błąd: {this.state.error}</p>}
					{!this.state.error && <p>Ładowanie...</p>}
				</div>
			)
		}

		moment.locale("pl")

		this.state.trips.sort((a, b) => b.startTime.diff(a.startTime))

		const largeUp = "d-none d-lg-table-cell"
		const mediumUp = "d-none d-md-table-cell"
		const xlargeUp = "d-none d-xl-table-cell"
		const largeDown = "d-xl-none"
		return (
			<div>
				<h2 className="mt-3 mb-0">Podsumowanie</h2>
				<small className="m-0">
					{this.state.trips[
						this.state.trips.length - 1
					].startTime.format("LL")}
					{" - "}
					{this.state.trips[0].startTime.format("LL")}
				</small>
				<div className="mt-3">
					<StatCard
						title="Dystans"
						value={
							this.state.trips
								.reduce((sum, trip) => sum + trip.dist, 0)
								.toFixed(2) + " km"
						}
					/>
					<StatCard
						title="Czas jazdy"
						value={this.state.trips
							.map((trip) => trip.time)
							.reduce((sum, value) => sum.add(value), duration(0))
							.humanize()}
					/>
					<StatCard
						title="Paliwo"
						value={
							this.state.trips
								.reduce((sum, trip) => sum + trip.fuel, 0)
								.toFixed(2) + " l"
						}
					/>
					<StatCard
						title="Śr. prędkość"
						value={
							(
								this.state.trips.reduce(
									(sum, trip) =>
										sum + trip.dist / trip.time.asHours(),
									0
								) / this.state.trips.length
							).toFixed(1) + " km/h"
						}
					/>
					<StatCard
						title="Śr. spalanie"
						value={
							(
								this.state.trips.reduce(
									(sum, trip) =>
										sum + (trip.fuel / trip.dist) * 100.0,
									0
								) / this.state.trips.length
							).toFixed(2) + " l/100 km"
						}
					/>
				</div>
				<h2 className="my-3">Ostatnie trasy</h2>
				<Table responsive={true} striped={true} variant="sm">
					<thead>
						<tr>
							<th>Data</th>
							<th>Godzina</th>
							<th>Dystans</th>
							<th className={largeUp}>Czas jazdy</th>
							<th className={largeUp}>Paliwo</th>
							<th className={mediumUp}>Śr. prędkość</th>
							<th className={mediumUp}>Śr. spalanie</th>
							<th className={largeUp}>Max. prędkość</th>
							<th className={largeUp}>Temperatura</th>
						</tr>
					</thead>
					<tbody>
						{this.state.trips.map((trip) => (
							<tr>
								<td className={largeDown}>
									{trip.tripId ? (
										<Link to={`/trips/${trip.tripId}`}>
											{trip.startTime.format("LL")}
										</Link>
									) : (
										<span>
											{trip.startTime.format("LL")}
										</span>
									)}
								</td>
								<td className={xlargeUp}>
									{trip.tripId ? (
										<Link to={`/trips/${trip.tripId}`}>
											{trip.startTime.format("dddd, LL")}
										</Link>
									) : (
										<span>
											{trip.startTime.format("dddd, LL")}
										</span>
									)}
								</td>
								<td>
									{trip.tripId ? (
										<Link to={`/trips/${trip.tripId}`}>
											{trip.startTime.format("HH:mm")}
											{" - "}
											{trip.tripId
												? trip.endTime.format("HH:mm")
												: "(teraz)"}
										</Link>
									) : (
										<span>
											{trip.startTime.format("HH:mm")}
											{" - "}
											{trip.tripId
												? trip.endTime.format("HH:mm")
												: "(teraz)"}
										</span>
									)}
								</td>
								<td>{trip.dist.toFixed(2)} km</td>
								<td className={largeUp}>
									{trip.time.hours()
										? `${trip.time.hours()} h`
										: ""}{" "}
									{trip.time.minutes()
										? `${trip.time.minutes()} min`
										: ""}
								</td>
								<td className={largeUp}>
									{trip.fuel.toFixed(2)} l
								</td>
								<td className={mediumUp}>
									{(trip.dist / trip.time.asHours()).toFixed(
										1
									)}{" "}
									km/h
								</td>
								<td className={mediumUp}>
									{((trip.fuel / trip.dist) * 100.0).toFixed(
										2
									)}{" "}
									l/100 km
								</td>
								<td className={largeUp}>
									{trip.vehicleSpeedMax.toFixed(1)} km/h
								</td>
								<td className={largeUp}>
									{trip.outsideTempAvg.toFixed(1)}°C
								</td>
							</tr>
						))}
					</tbody>
				</Table>
				<Pagination>
					<PageItem
						disabled={this.state.prevBefore.length == 0}
						onClick={this.onPagePrevClick.bind(this)}
					>
						&laquo; Następne
					</PageItem>
					<PageItem
						disabled={this.state.trips.length != 20}
						onClick={this.onPageNextClick.bind(this)}
					>
						Poprzednie &raquo;
					</PageItem>
				</Pagination>
			</div>
		)
	}

	onPagePrevClick() {
		if (!this.state.trips) return
		const newBefore = this.state.prevBefore.pop()
		this.setState({
			trips: undefined,
			before: newBefore,
			prevBefore: Array.of(...this.state.prevBefore),
		})
	}

	onPageNextClick() {
		if (!this.state.trips?.length) return
		const newBefore = Math.min(
			...this.state.trips.map((trip) =>
				parseInt(trip.startTime.format("x"))
			)
		)
		this.setState({
			trips: undefined,
			before: newBefore,
			prevBefore: Array.of(...this.state.prevBefore, this.state.before),
		})
	}

	async loadData() {
		let url = "/api/trips"
		if (this.state.before) url += `?before=${this.state.before}`
		const response = await fetch(url)
		const tripList: any[] = await response.json()
		const trips: Trip[] = tripList.map(mapToTrip)
		this.setState({ trips })
	}
}
