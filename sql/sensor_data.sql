-- Aktivace rozšíření pro UUID (pokud už není)
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- Smazání staré tabulky
DROP TABLE IF EXISTS sensor_data;

-- Vytvoření nové tabulky s UUID ID a senzory
CREATE TABLE sensor_data (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    battery INTEGER NOT NULL,
    temperature DOUBLE PRECISION NOT NULL,
    humidity DOUBLE PRECISION NOT NULL,
    pressure DOUBLE PRECISION NOT NULL,
    measured_at TIMESTAMP WITHOUT TIME ZONE NOT NULL,
    created_at TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT NOW()
);