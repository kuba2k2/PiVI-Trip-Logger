/*
 * Copyright (c) Kuba Szczodrzyński 2025-1-27.
 */

import React from "react"
import { mapToTrip, Trip } from "../model/Trip"
import { mapToRecord, Record } from "../model/Record"
import moment, { duration } from "moment"
import "moment/dist/locale/pl"
import "moment/locale/pl"
import { Button } from "react-bootstrap"
import { LinkContainer } from "react-router-bootstrap"
import StatCard from "../components/StatCard"
import { Line } from "react-chartjs-2"
import { Chart, ChartData, ChartOptions } from "chart.js"

// @ts-ignore
import zoomPlugin from "chartjs-plugin-zoom"

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

Chart.register(zoomPlugin)

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
		this.state.records.sort((a, b) => a.startTime.diff(b.startTime))

		const chartOptions: ChartOptions<"line"> = {
			responsive: true,
			scales: {
				x: {
					type: "linear",
					position: "bottom",
					offset: false,
					title: {
						display: true,
						text: "Czas",
					},
					ticks: {
						callback: function (value: string | number) {
							return moment(value).format("HH:mm")
						},
					},
					grid: {
						drawOnChartArea: false,
					},
				},
				ySpeed: {
					type: "linear",
					position: "left",
					title: {
						display: true,
						text: "Prędkość (km/h)",
					},
					ticks: {
						callback: function (value: number | string) {
							return value + " km/h"
						},
					},
					grid: {
						drawOnChartArea: false,
					},
				},
				yFuel: {
					type: "linear",
					position: "right",
					title: {
						display: true,
						text: "Spalanie (l/100 km)",
					},
					ticks: {
						callback: function (value: number | string) {
							return value + " l/100 km"
						},
					},
					grid: {
						drawOnChartArea: false,
					},
				},
			},
			plugins: {
				zoom: {
					zoom: {
						wheel: {
							enabled: true,
						},
						drag: {
							enabled: false,
						},
						pinch: {
							enabled: true,
						},
						mode: "x",
					},
					pan: {
						enabled: true,
						mode: "x",
					},
				},
			},
		}

		const labels = this.state.records.map((record) => record.startTime)

		const chartData: ChartData<"line"> = {
			labels,
			datasets: [
				{
					label: "Prędkość",
					data: this.state.records.map(
						(record) =>
							record.dist /
							duration(
								record.endTime.diff(record.startTime)
							).asHours()
					),
					borderColor: "rgb(255, 99, 132)",
					backgroundColor: "transparent",
					yAxisID: "ySpeed",
				},
				{
					label: "Spalanie",
					data: this.state.records.map(
						(record) => (record.fuel / record.dist) * 100.0
					),
					borderColor: "rgb(53, 162, 235)",
					backgroundColor: "transparent",
					yAxisID: "yFuel",
				},
			],
		}

		const chart = <Line options={chartOptions} data={chartData} />

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

				{chart}
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
		let url = `/api/records?trip_id=${this.props.tripId}&limit=100`
		if (this.state.before) url += `&before=${this.state.before}`
		const response = await fetch(url)
		const recordList: any[] = await response.json()
		const records: Record[] = recordList.map(mapToRecord)
		this.setState({ records })
	}
}
