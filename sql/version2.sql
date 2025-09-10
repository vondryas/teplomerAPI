ALTER DATABASE defaultdb SET TIMEZONE = 'Europe/Prague';

ALTER TABLE weather_station_data
ALTER COLUMN created_at TYPE TIMESTAMPTZ
USING created_at AT TIME ZONE 'Europe/Prague';

ALTER TABLE weather_station_data
ALTER COLUMN measure_at TYPE TIMESTAMPTZ
USING measure_at AT TIME ZONE 'Europe/Prague';