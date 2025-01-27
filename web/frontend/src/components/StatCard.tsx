/*
 * Copyright (c) Kuba Szczodrzyński 2025-1-27.
 */

import React from "react"
import { Card } from "react-bootstrap"

type StatCardProps = {
	title: string
	value: string
	min?: string
	max?: string
}

export default class StatCard extends React.Component<StatCardProps> {
	render() {
		return (
			<Card
				className="mr-2 mt-2"
				style={{
					width: this.props.min || this.props.max ? "12rem" : "10rem",
					display: "inline-block",
				}}
			>
				<div className="card-body p-2 px-3 mb-1">
					<p className="card-text m-0">{this.props.title}</p>
					<h5 className="card-title m-0">{this.props.value}</h5>
					{this.props.min || this.props.max ? (
						<small className="text-muted">
							{this.props.min ? "min: " + this.props.min : ""}
							{this.props.min && this.props.max ? " / " : ""}
							{this.props.max ? "max: " + this.props.max : ""}
						</small>
					) : (
						""
					)}
				</div>
			</Card>
		)
	}
}
