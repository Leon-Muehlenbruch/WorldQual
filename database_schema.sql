-- WorldQual MySQL Database Schema
-- Minimales Schema für Test-Setup
-- Version: 1.0
-- Date: 2026-01-22

-- ============================================================================
-- 1. CORE DATABASE: wq_general
-- ============================================================================

CREATE DATABASE IF NOT EXISTS wq_general;
USE wq_general;

-- Run-Liste: Konfiguration aller Simulationsläufe
CREATE TABLE IF NOT EXISTS _runlist (
    IDrun INT PRIMARY KEY COMMENT 'Unique run identifier',
    IDScen INT COMMENT 'Scenario ID',
    parameter_id INT COMMENT 'Parameter ID (N, P, BOD, etc.)',
    IDTemp INT COMMENT 'Temperature calculation method ID',
    IDVersion INT COMMENT 'WaterGAP Version (2 or 3)',
    IDReg INT COMMENT 'Region ID',
    start_year INT COMMENT 'Simulation start year',
    end_year INT COMMENT 'Simulation end year',
    description TEXT COMMENT 'Run description'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='Simulation run configuration';

-- Szenario-Definitionen
CREATE TABLE IF NOT EXISTS _szenario (
    IDScen INT PRIMARY KEY COMMENT 'Scenario ID',
    ScenName VARCHAR(100) COMMENT 'Scenario name',
    description TEXT COMMENT 'Scenario description'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='Scenario definitions';

-- Parameter-Liste
CREATE TABLE IF NOT EXISTS _parameter (
    parameter_id INT PRIMARY KEY COMMENT 'Parameter ID',
    parameter_name VARCHAR(50) COMMENT 'Parameter name (N, P, BOD, etc.)',
    unit VARCHAR(20) COMMENT 'Unit (mg/l, kg/month, etc.)',
    description TEXT COMMENT 'Parameter description'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='Available parameters';

-- Beispiel-Daten einfügen
INSERT INTO _parameter (parameter_id, parameter_name, unit, description) VALUES
(1, 'TN', 'mg/l', 'Total Nitrogen'),
(2, 'TP', 'mg/l', 'Total Phosphorus'),
(3, 'BOD', 'mg/l', 'Biochemical Oxygen Demand'),
(4, 'DOC', 'mg/l', 'Dissolved Organic Carbon')
ON DUPLICATE KEY UPDATE parameter_name=VALUES(parameter_name);

INSERT INTO _szenario (IDScen, ScenName, description) VALUES
(1, 'BASELINE', 'Baseline scenario - current conditions'),
(2, 'SSP2', 'Shared Socioeconomic Pathway 2')
ON DUPLICATE KEY UPDATE ScenName=VALUES(ScenName);


-- ============================================================================
-- 2. CONTINENT-SPECIFIC DATABASE: worldqual_{continent}
-- ============================================================================
-- Beispiel für Afrika (af), gleiche Struktur für: eu, as, au, na, sa

CREATE DATABASE IF NOT EXISTS worldqual_af;
USE worldqual_af;

-- Routing-Tabelle
CREATE TABLE IF NOT EXISTS routing (
    cell INT PRIMARY KEY COMMENT 'Cell ID (Arc ID)',
    inflow_count INT DEFAULT 0 COMMENT 'Number of inflowing cells',
    inflow_done INT DEFAULT 0 COMMENT 'Routing status counter',
    INDEX idx_inflow (inflow_done)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='River routing information';

-- Fließgeschwindigkeiten
CREATE TABLE IF NOT EXISTS flow_velocity (
    cell INT PRIMARY KEY COMMENT 'Cell ID',
    velocity FLOAT COMMENT 'Flow velocity in m/s',
    river_length FLOAT COMMENT 'River length in km',
    rout_order INT COMMENT 'Routing order',
    INDEX idx_order (rout_order)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='Flow velocities and river properties';

-- Nährstoff-Einträge (Beispiel für Basis-Tabelle)
CREATE TABLE IF NOT EXISTS load (
    cell INT COMMENT 'Cell ID',
    IDScen INT COMMENT 'Scenario ID',
    parameter_id INT COMMENT 'Parameter ID',
    date INT COMMENT 'Date in YYYYMM format',
    load FLOAT COMMENT 'Load in kg/month',
    PRIMARY KEY (cell, IDScen, parameter_id, date),
    INDEX idx_scen (IDScen),
    INDEX idx_param (parameter_id),
    INDEX idx_date (date)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='Nutrient loads';


-- ============================================================================
-- 3. OUTPUT DATABASE: wq_out_{continent}
-- ============================================================================

CREATE DATABASE IF NOT EXISTS wq_out_af;
USE wq_out_af;

-- Konzentrations-Ergebnisse (Tabelle wird dynamisch erstellt)
-- Beispiel-Schema: concentration_{IDrun}
-- CREATE TABLE concentration_1 (
--     date INT COMMENT 'Date in YYYYMM format',
--     cell INT COMMENT 'Cell ID',
--     con FLOAT COMMENT 'Concentration',
--     con2 FLOAT COMMENT 'Secondary concentration',
--     PRIMARY KEY (date, cell)
-- );


-- ============================================================================
-- 4. WATERGAP BASE DATA: watergap_unf
-- ============================================================================

CREATE DATABASE IF NOT EXISTS watergap_unf;
USE watergap_unf;

-- Grid Row (Zeilen-Indizes)
CREATE TABLE IF NOT EXISTS gr (
    cell INT PRIMARY KEY COMMENT 'Cell ID',
    row INT COMMENT 'Row index in grid'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='Grid row indices';

-- Grid Column (Spalten-Indizes)
CREATE TABLE IF NOT EXISTS gc (
    cell INT PRIMARY KEY COMMENT 'Cell ID',
    column INT COMMENT 'Column index in grid'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='Grid column indices';

-- Zufluss-Informationen (8 Nachbarn)
CREATE TABLE IF NOT EXISTS g_inflc_arc_id (
    cell INT PRIMARY KEY COMMENT 'Cell ID',
    SW INT COMMENT 'Southwest neighbor cell',
    S INT COMMENT 'South neighbor cell',
    SE INT COMMENT 'Southeast neighbor cell',
    W INT COMMENT 'West neighbor cell',
    E INT COMMENT 'East neighbor cell',
    NW INT COMMENT 'Northwest neighbor cell',
    N INT COMMENT 'North neighbor cell',
    NE INT COMMENT 'Northeast neighbor cell'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='Inflow connections';

-- Einzugsgebiete (pro Kontinent)
CREATE TABLE IF NOT EXISTS mother_af (
    arcid INT PRIMARY KEY COMMENT 'Arc ID (Cell ID)',
    bas0_id INT COMMENT 'Basin ID',
    rout_area FLOAT COMMENT 'Routing area in km²'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='Catchment information for Africa';

-- Länder-Kontinent Zuordnung
CREATE TABLE IF NOT EXISTS country_continent (
    isonum INT PRIMARY KEY COMMENT 'ISO country number',
    idreg INT COMMENT 'Region ID',
    country_name VARCHAR(100) COMMENT 'Country name',
    continent VARCHAR(2) COMMENT 'Continent code (af, eu, as, au, na, sa)'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='Country to continent mapping';


-- ============================================================================
-- 5. LOAD CALCULATION DATABASE: wq_load_{continent}
-- ============================================================================

CREATE DATABASE IF NOT EXISTS wq_load_af;
USE wq_load_af;

-- Berechnete monatliche Einträge (dynamisch erstellt)
-- Beispiel-Schema: calc_cell_month_load_dom_1_1
-- CREATE TABLE calc_cell_month_load_dom_1_1 (
--     cell INT,
--     month INT,
--     load FLOAT,
--     PRIMARY KEY (cell, month)
-- );


-- ============================================================================
-- 6. HELPER FUNCTIONS
-- ============================================================================

-- Erstelle alle kontinentspezifischen Datenbanken
DELIMITER //

CREATE PROCEDURE IF NOT EXISTS create_continent_dbs()
BEGIN
    DECLARE continents VARCHAR(100) DEFAULT 'af,as,au,eu,na,sa';
    DECLARE cont VARCHAR(2);
    DECLARE pos INT;
    DECLARE db_name VARCHAR(50);
    
    WHILE LENGTH(continents) > 0 DO
        SET pos = LOCATE(',', continents);
        IF pos = 0 THEN
            SET cont = continents;
            SET continents = '';
        ELSE
            SET cont = LEFT(continents, pos - 1);
            SET continents = SUBSTRING(continents, pos + 1);
        END IF;
        
        -- worldqual_*
        SET @sql = CONCAT('CREATE DATABASE IF NOT EXISTS worldqual_', cont);
        PREPARE stmt FROM @sql;
        EXECUTE stmt;
        DEALLOCATE PREPARE stmt;
        
        -- wq_out_*
        SET @sql = CONCAT('CREATE DATABASE IF NOT EXISTS wq_out_', cont);
        PREPARE stmt FROM @sql;
        EXECUTE stmt;
        DEALLOCATE PREPARE stmt;
        
        -- wq_load_*
        SET @sql = CONCAT('CREATE DATABASE IF NOT EXISTS wq_load_', cont);
        PREPARE stmt FROM @sql;
        EXECUTE stmt;
        DEALLOCATE PREPARE stmt;
    END WHILE;
END//

DELIMITER ;

-- Aufruf: CALL create_continent_dbs();


-- ============================================================================
-- 7. TEST DATA
-- ============================================================================

USE wq_general;

-- Test-Run einfügen
INSERT INTO _runlist (IDrun, IDScen, parameter_id, IDTemp, IDVersion, IDReg, start_year, end_year, description) VALUES
(1, 1, 1, 1, 3, 2, 2000, 2001, 'Test run - Africa TN')
ON DUPLICATE KEY UPDATE description=VALUES(description);


-- ============================================================================
-- FERTIG!
-- ============================================================================

-- Um dieses Schema zu verwenden:
-- mysql -u root -p < worldqual_schema.sql

-- Um alle Datenbanken zu erstellen:
-- mysql -u root -p -e "CALL wq_general.create_continent_dbs();"
