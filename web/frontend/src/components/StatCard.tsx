/*
 * Copyright (c) Kuba Szczodrzyński 2025-1-27.
 */

import React from "react"
import { Card } from "react-bootstrap"

type StatCardProps = {
	title: string
	value: string
}

export default class StatCard extends React.Component<StatCardProps> {
	render() {
		return (
			<Card
				className="mr-2 mt-2"
				style={{ width: "10rem", display: "inline-block" }}
			>
				<div className="card-body p-2 px-3 mb-1">
					<p className="card-text m-0">{this.props.title}</p>
					<h5 className="card-title m-0">{this.props.value}</h5>
				</div>
			</Card>
		)
	}
}
