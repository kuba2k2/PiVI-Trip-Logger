/*
 * Copyright (c) Kuba Szczodrzyński 2025-1-26.
 */

import React from "react"
import { BrowserRouter, Route, Switch } from "react-router-dom"
import { LinkContainer } from "react-router-bootstrap"

import "./App.css"
import TripsPage from "./pages/TripsPage"
import {
	Container,
	Nav,
	Navbar,
	NavbarBrand,
	NavItem,
	NavLink,
} from "react-bootstrap"
import NavbarToggle from "react-bootstrap/NavbarToggle"
import NavbarCollapse from "react-bootstrap/NavbarCollapse"

const App: React.FC = () => (
	<BrowserRouter>
		<Navbar variant="dark" bg="dark">
			<Container>
				<NavbarBrand>Trip Logger</NavbarBrand>
				<NavbarToggle />
				<NavbarCollapse>
					<Nav>
						<NavItem>
							<LinkContainer to="/trips">
								<NavLink>Trasy</NavLink>
							</LinkContainer>
						</NavItem>
						<NavItem>
							<LinkContainer to="/stats">
								<NavLink>Statystyki</NavLink>
							</LinkContainer>
						</NavItem>
					</Nav>
				</NavbarCollapse>
			</Container>
		</Navbar>
		<Container>
			<Switch>
				<Route path="/trips">
					<TripsPage />
				</Route>
				<Route path="/">{/*<Home />*/}</Route>
			</Switch>
		</Container>
	</BrowserRouter>
)

export default App
