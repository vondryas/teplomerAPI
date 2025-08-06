-- Rozšíøení pro UUID, pokud není aktivní
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- Tabulka users
CREATE TABLE users (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    username TEXT NOT NULL UNIQUE,
    email TEXT NOT NULL UNIQUE,
    passhash TEXT NOT NULL,
    role Integer NOT NULL default 0
);

-- Tabulka devices
CREATE TABLE devices (
    user_id UUID NOT NULL,
    name TEXT PRIMARY KEY NOT NULL,
    type integer NOT NULL,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
);

-- Tabulka weather_station_data
CREATE TABLE weather_station_data (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    device_id text NOT NULL,
    battery INT NOT NULL CHECK (battery >= 0 AND battery <= 100),
    temperature DOUBLE PRECISION NOT NULL,
    humidity DOUBLE PRECISION NOT NULL,
    pressure DOUBLE PRECISION NOT NULL,
    measure_at TIMESTAMP WITHOUT TIME ZONE NOT NULL,
    created_at TIMESTAMP WITHOUT TIME ZONE DEFAULT now(),
    FOREIGN KEY (device_id) REFERENCES devices(name) ON DELETE CASCADE
);
