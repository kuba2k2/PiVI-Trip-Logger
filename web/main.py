#  Copyright (c) Kuba Szczodrzyński 2025-1-26.

from typing import Annotated

from fastapi import Depends, FastAPI, HTTPException, Query
from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles
from sqlmodel import Session, create_engine, select
from starlette.exceptions import HTTPException as StarletteHTTPException

from .model.record import Record
from .model.trip import Trip

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


@app.get("/api/trips", response_model=list[Trip])
async def get_trip_list(
    session: SessionDep,
    after: int = None,
    before: int = None,
    limit: Annotated[int, Query(le=100)] = 20,
):
    stmt = select(Trip)
    order_by = Trip.start_time.desc()
    if after is not None:
        stmt = stmt.where(Trip.start_time > after)
        order_by = Trip.start_time
    if before is not None:
        stmt = stmt.where(Trip.end_time < before)
    stmt = stmt.order_by(order_by)
    return session.exec(stmt.limit(limit)).all()


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
