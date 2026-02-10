-- Tabulka fireport_device_data
CREATE TABLE fireport_device_data (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    device_id text NOT NULL,
    type integer NOT NULL,
    device_address text NOT NULL,
    FOREIGN KEY (device_id) REFERENCES devices(name) ON DELETE CASCADE
);