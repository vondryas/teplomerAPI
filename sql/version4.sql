-- version4.sql

-- Odstranìní sloupce type
ALTER TABLE fireport_device_data 
DROP COLUMN type;

-- Pøejmenování sloupce device_address na password
ALTER TABLE fireport_device_data 
RENAME COLUMN device_address TO password;