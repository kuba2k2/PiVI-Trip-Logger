/*
 * Copyright (c) Kuba Szczodrzyński 2025-1-27.
 */

import React from "react"
import { mapToTrip, Trip } from "../model/Trip"
import { mapToRecord, Record } from "../model/Record"
import moment, { duration, Duration } from "moment"
import "moment/dist/locale/pl"
import "moment/locale/pl"
import { Button } from "react-bootstrap"
import { LinkContainer } from "react-router-bootstrap"
import StatCard from "../components/StatCard"

type TripPageProps = {
	tripId: number
}

type TripPageState = {
	trip?: Trip
	records?: Record[]
	error?: string
	before?: number
	prevBefore: (number | undefined)[]
}

export default class TripPage extends React.Component<
	TripPageProps,
	TripPageState
> {
	constructor(props: any) {
		super(props)
		this.state = { prevBefore: [] }
	}

	render() {
		if (!this.state.trip) {
			if (!this.state.error) this.loadTrip()
			return (
				<div>
					{this.state.error && <p>Błąd: {this.state.error}</p>}
					{!this.state.error && <p>Ładowanie...</p>}
				</div>
			)
		}
		if (!this.state.records) {
			if (!this.state.error) this.loadRecords()
			return (
				<div>
					{this.state.error && <p>Błąd: {this.state.error}</p>}
					{!this.state.error && <p>Ładowanie...</p>}
				</div>
			)
		}

		moment.locale("pl")

		const trip = this.state.trip
		this.state.records.sort((a, b) => b.startTime.diff(a.startTime))

		return (
			<div>
				<LinkContainer to="/trips">
					<Button size="sm" variant="outline-dark" className="mt-3">
						&laquo; Wróć
					</Button>
				</LinkContainer>
				<h2 className="mt-2">Szczegóły trasy</h2>
				<h4>{trip.startTime.calendar()}</h4>
				<div className="mt-3">
					<StatCard
						title="Dystans"
						value={trip.dist.toFixed(2) + " km"}
					/>
					<StatCard
						title="Czas jazdy"
						value={
							(trip.time.hours()
								? `${trip.time.hours()} h`
								: "") +
							" " +
							(trip.time.minutes()
								? `${trip.time.minutes()} min`
								: "")
						}
					/>
					<StatCard
						title="Paliwo"
						value={trip.fuel.toFixed(2) + " l"}
					/>
					<StatCard
						title="Śr. prędkość"
						value={
							(trip.dist / trip.time.asHours()).toFixed(1) +
							" km/h"
						}
					/>
					<StatCard
						title="Śr. spalanie"
						value={
							((trip.fuel / trip.dist) * 100.0).toFixed(2) +
							" l/100 km"
						}
					/>
				</div>
			</div>
		)
	}

	onPagePrevClick() {
		if (!this.state.records) return
		const newBefore = this.state.prevBefore.pop()
		this.setState({
			records: undefined,
			before: newBefore,
			prevBefore: Array.of(...this.state.prevBefore),
		})
	}

	onPageNextClick() {
		if (!this.state.records?.length) return
		const newBefore = Math.min(
			...this.state.records.map((trip) =>
				parseInt(trip.startTime.format("x"))
			)
		)
		this.setState({
			records: undefined,
			before: newBefore,
			prevBefore: Array.of(...this.state.prevBefore, this.state.before),
		})
	}

	async loadTrip() {
		const url = `/api/trips/${this.props.tripId}`
		const response = await fetch(url)
		const tripObject: any = await response.json()
		const trip: Trip = mapToTrip(tripObject)
		this.setState({ trip })
	}

	async loadRecords() {
		let url = `/api/records?trip_id=${this.props.tripId}`
		if (this.state.before) url += `?before=${this.state.before}`
		const response = await fetch(url)
		const recordList: any[] = await response.json()
		const records: Record[] = recordList.map(mapToRecord)
		this.setState({ records })
	}
}
