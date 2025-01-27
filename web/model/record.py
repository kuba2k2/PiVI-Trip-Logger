#  Copyright (c) Kuba Szczodrzyński 2025-1-26.

from sqlmodel import Field, SQLModel


class RecordBase(SQLModel):
    start_time: int = Field(primary_key=True)
    end_time: int = Field(primary_key=True)
    start_mileage: float
    end_mileage: float
    dist: int
    fuel: int
    engine_speed: float
    engine_speed_max: float
    vehicle_speed_min: float
    vehicle_speed_max: float
    coolant_temp: float
    outside_temp: float
    oil_temp: float
    oil_level: float
    fuel_level: float
    fuel_range: float
    fuel_cons_min: float
    fuel_cons_max: float
    trip_id: int | None = Field(default=None)


class Record(RecordBase, table=True):
    pass
