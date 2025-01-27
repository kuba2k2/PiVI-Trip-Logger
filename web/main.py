#  Copyright (c) Kuba Szczodrzyński 2025-1-26.

from typing import Annotated

from fastapi import Depends, FastAPI, HTTPException, Query
from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles
from sqlmodel import Session, create_engine, select, func
from starlette.exceptions import HTTPException as StarletteHTTPException

from .model.record import Record
from .model.trip import Trip, TripNoId

sqlite_file_name = "canlogger.db"
sqlite_url = f"sqlite:///{sqlite_file_name}"

connect_args = dict(check_same_thread=False)
engine = create_engine(sqlite_url, connect_args=connect_args)


def get_session():
    with Session(engine) as session:
        yield session


SessionDep = Annotated[Session, Depends(get_session)]
app = FastAPI()
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


@app.get("/api/records", response_model=list[Record])
async def get_record_list(
    session: SessionDep,
    after: int = None,
    before: int = None,
    trip_id: int = None,
    limit: Annotated[int, Query(le=100)] = 20,
):
    stmt = select(Record)
    order_by = Record.start_time.desc()
    if after is not None:
        stmt = stmt.where(Record.start_time > after)
        order_by = Record.start_time
    if before is not None:
        stmt = stmt.where(Record.end_time < before)
    stmt = stmt.order_by(order_by)
    if trip_id is not None:
        stmt = stmt.where(Record.trip_id == trip_id)
    return session.exec(stmt.limit(limit)).all()


@app.get("/api/trips", response_model=list[TripNoId])
async def get_trip_list(
    session: SessionDep,
    after: int = None,
    before: int = None,
    limit: Annotated[int, Query(le=100)] = 20,
):
    current_trip = []
    if after is None and before is None:
        # noinspection PyArgumentList
        stmt = (
            select(
                Record.trip_id,
                func.sum(Record.end_time - Record.start_time).label("time"),
                func.sum(Record.dist).label("dist"),
                func.sum(Record.fuel).label("fuel"),
                func.min(Record.start_time).label("start_time"),
                func.max(Record.end_time).label("end_time"),
                func.min(Record.start_mileage).label("start_mileage"),
                func.max(Record.end_mileage).label("end_mileage"),
                func.max(Record.engine_speed_max).label("engine_speed_max"),
                func.max(Record.vehicle_speed_max).label("vehicle_speed_max"),
                (func.sum(Record.coolant_temp) / func.count(Record.coolant_temp)).label(
                    "coolant_temp_avg"
                ),
                func.min(Record.coolant_temp).label("coolant_temp_min"),
                func.max(Record.coolant_temp).label("coolant_temp_max"),
                (func.sum(Record.outside_temp) / func.count(Record.outside_temp)).label(
                    "outside_temp_avg"
                ),
                func.min(Record.outside_temp).label("outside_temp_min"),
                func.max(Record.outside_temp).label("outside_temp_max"),
                (func.sum(Record.oil_temp) / func.count(Record.oil_temp)).label(
                    "oil_temp_avg"
                ),
                func.min(Record.oil_temp).label("oil_temp_min"),
                func.max(Record.oil_temp).label("oil_temp_max"),
                func.min(Record.oil_level).label("oil_level_min"),
                func.max(Record.oil_level).label("oil_level_max"),
                func.min(Record.fuel_level).label("fuel_level_min"),
                func.max(Record.fuel_level).label("fuel_level_max"),
                func.min(Record.fuel_range).label("fuel_range_min"),
                func.max(Record.fuel_range).label("fuel_range_max"),
                func.min(Record.fuel_cons_min).label("fuel_cons_min"),
                func.max(Record.fuel_cons_max).label("fuel_cons_max"),
            )
            .where(Record.trip_id.is_(None))
            .group_by(Record.trip_id)
        )
        trip = session.exec(stmt.limit(1)).first()
        if trip:
            current_trip = [trip]
            limit -= 1
    stmt = select(Trip)
    order_by = Trip.start_time.desc()
    if after is not None:
        stmt = stmt.where(Trip.start_time > after)
        order_by = Trip.start_time
    if before is not None:
        stmt = stmt.where(Trip.end_time < before)
    stmt = stmt.order_by(order_by)
    return current_trip + session.exec(stmt.limit(limit)).all()


@app.get("/api/trips/{trip_id}", response_model=Trip)
async def get_trip_single(
    session: SessionDep,
    trip_id: int,
):
    trip = session.get(Trip, trip_id)
    if not trip:
        raise HTTPException(status_code=404, detail="Trip not found")
    return trip


class SPAStaticFiles(StaticFiles):
    async def get_response(self, path: str, scope):
        try:
            return await super().get_response(path, scope)
        except (HTTPException, StarletteHTTPException) as e:
            if e.status_code == 404:
                return await super().get_response("index.html", scope)
            raise e


app.mount(
    "/",
    SPAStaticFiles(directory="web/frontend/build", html=True),
    name="frontend",
)
