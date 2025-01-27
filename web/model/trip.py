#  Copyright (c) Kuba Szczodrzyński 2025-1-26.

from sqlmodel import Field, SQLModel


class TripBase(SQLModel):
    trip_id: int = Field(primary_key=True)
    time: int
    dist: int
    fuel: int
    start_time: int
    end_time: int
    start_mileage: float
    end_mileage: float
    engine_speed_max: float
    vehicle_speed_max: float
    coolant_temp_avg: float
    coolant_temp_min: float
    coolant_temp_max: float
    outside_temp_avg: float
    outside_temp_min: float
    outside_temp_max: float
    oil_temp_avg: float
    oil_temp_min: float
    oil_temp_max: float
    oil_level_min: float
    oil_level_max: float
    fuel_level_min: float
    fuel_level_max: float
    fuel_range_min: float
    fuel_range_max: float
    fuel_cons_min: float
    fuel_cons_max: float


class Trip(TripBase, table=True):
    pass
