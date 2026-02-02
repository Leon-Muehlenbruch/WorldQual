# WorldQual - Datenbank-Schema

Detaillierte Dokumentation der **Datenbankstruktur** für Entwickler.

## Inhaltsverzeichnis

- [Übersicht](#übersicht)
- [Hauptdatenbanken](#hauptdatenbanken)
- [Tabellen-Details](#tabellen-details)
- [Beziehungen](#beziehungen)
- [Indizes](#indizes)
- [Wichtige Queries](#wichtige-queries)

## Übersicht

WorldQual verwendet **mehrere MySQL-Datenbanken**:

1. **wq_general** - Zentrale Konfiguration (Projekte, Szenarien, Runs)
2. **wwqa_worldqual_[region]** - Simulationsergebnisse pro Region
3. **wwqa_wq_load_[region]** - Eintragsdaten pro Region

**Region-Abkürzungen:**
- `eu` - Europa
- `af` - Afrika
- `as` - Asien
- `au` - Australien
- `na` - Nordamerika
- `sa` - Südamerika

## Hauptdatenbanken

### wq_general - Konfigurationsdatenbank

**Zweck:** Zentrale Verwaltung von Projekten, Szenarien, Runs und Parametern.

**Wichtige Tabellen:**

#### _project

**Zweck:** Verwaltet Projekte (CESR, WWQA, etc.)

**Struktur (Beispiel):**
```sql
CREATE TABLE _project (
    project_id INT PRIMARY KEY,
    Project_Name VARCHAR(255),
    database_name VARCHAR(255),  -- Optional: Projekt-spezifische DB
    description TEXT
);
```

**Typische Daten:**
```sql
INSERT INTO _project VALUES 
(1, 'CESR', '', 'Climate and Environment System Research'),
(2, 'WWQA', '', 'World Water Quality Assessment'),
(3, 'CESR Sensitivity Analysis', '', 'Sensitivity Analysis'),
(4, 'test', '', 'Test-Projekt');
```

#### _szenario

**Zweck:** Verwaltet Szenarien (Baseline, verschiedene Zukunftsszenarien)

**Struktur (Beispiel):**
```sql
CREATE TABLE _szenario (
    IDScen INT PRIMARY KEY,
    ScenName VARCHAR(255),
    description TEXT,
    project_id INT,
    FOREIGN KEY (project_id) REFERENCES _project(project_id)
);
```

**Typische Daten:**
```sql
INSERT INTO _szenario VALUES 
(91, 'Baseline 2010', 'Baseline-Szenario für 2010', 2),
(92, 'Szenario A', 'Beschreibung...', 2);
```

#### _parameter

**Zweck:** Verwaltet Wasserqualitätsparameter

**Struktur (Beispiel):**
```sql
CREATE TABLE _parameter (
    parameter_id INT PRIMARY KEY,
    ParameterName VARCHAR(255),
    description TEXT,
    unit VARCHAR(50)
);
```

**Typische Daten:**
```sql
INSERT INTO _parameter VALUES 
(0, 'BOD', 'Biochemischer Sauerstoffbedarf', 'mg/l'),
(1, 'TDS', 'Total Dissolved Solids (Salz)', 'mg/l'),
(2, 'FC', 'Fäkalcoliforme Bakterien', 'cfu/100ml'),
(3, 'TN', 'Gesamtstickstoff', 'mg/l'),
(4, 'TP', 'Gesamtphosphor', 'mg/l');
```

#### _runlist

**Zweck:** Verwaltet Run-Konfigurationen (Verbindet Parameter, Szenario, etc.)

**Struktur (Beispiel):**
```sql
CREATE TABLE _runlist (
    IDrun INT PRIMARY KEY,
    runName VARCHAR(255),
    parameter_id INT,
    IDScen INT,
    IDScen_wq_load INT,  -- Szenario für Einträge
    IDTemp INT,           -- Wassertemperatur-Konfiguration
    project_id INT,
    Q_low_limit DOUBLE,  -- Untere Abfluss-Grenze
    Q_low_limit_type INT, -- Typ der Grenze
    conservative INT,     -- 1=konservativer Stoff, 0=nicht-konservativ
    lake INT,            -- 1=mit See-Abbau, 0=ohne
    UseWaterTemp INT,    -- 1=Wassertemperatur verwenden
    FOREIGN KEY (parameter_id) REFERENCES _parameter(parameter_id),
    FOREIGN KEY (IDScen) REFERENCES _szenario(IDScen),
    FOREIGN KEY (project_id) REFERENCES _project(project_id)
);
```

**IDrun-Struktur:**
Die IDrun setzt sich zusammen aus:
- Parameter-ID (1-2 Stellen)
- Szenario-ID (2-3 Stellen)
- Weitere Konfigurationen

**Beispiel:** `403100091`
- `4` = Parameter (kann variieren)
- `03` = WaterGAP Version 3
- `100` = Weitere Konfiguration
- `091` = Szenario 91

#### _water_temperature_list

**Zweck:** Verwaltet Wassertemperatur-Konfigurationen

**Struktur (Beispiel):**
```sql
CREATE TABLE _water_temperature_list (
    IDTemp INT PRIMARY KEY,
    Name VARCHAR(255),
    climate VARCHAR(255),  -- Klima-Datenquelle
    comment TEXT,
    C0 DOUBLE,  -- Konstante für Kurve
    C1 DOUBLE,
    C2 DOUBLE
);
```

#### watergap_region

**Zweck:** Verwaltet Region-Konfigurationen

**Struktur (Beispiel):**
```sql
CREATE TABLE watergap_region (
    IDVersion INT,
    IDRegion INT,
    hydro_input VARCHAR(255),  -- Pfad zu WaterGAP-Daten
    PRIMARY KEY (IDVersion, IDRegion)
);
```

### wwqa_worldqual_[region] - Ergebnisse

**Zweck:** Speichert Simulationsergebnisse (Konzentrationen)

**Wichtige Tabellen:**

#### concentration

**Zweck:** Speichert monatliche Konzentrationen pro Grid-Zelle

**Struktur (Beispiel):**
```sql
CREATE TABLE concentration (
    IDrun INT,
    cell INT,
    year INT,
    month INT,  -- 1-12
    concentration DOUBLE,
    PRIMARY KEY (IDrun, cell, year, month),
    INDEX idx_cell (cell),
    INDEX idx_year (year),
    INDEX idx_idrun (IDrun)
);
```

**Datenformat:**
- `IDrun`: Run-ID
- `cell`: Grid-Zellen-ID (ArcID)
- `year`: Jahr
- `month`: Monat (1=Januar, ..., 12=Dezember)
- `concentration`: Konzentration [Einheit je nach Parameter]

### wwqa_wq_load_[region] - Eintragsdaten

**Zweck:** Speichert berechnete Eintragsdaten

**Wichtige Tabellen:**

#### calc_cell_month_load_*

**Zweck:** Berechnete monatliche Einträge nach Quelle

**Tabellen-Namen:**
- `calc_cell_month_load_man_ww` - Industrieabwasser
- `calc_cell_month_load_dom_ww` - Haushaltsabwasser
- `calc_cell_month_load_rtf_irr` - Bewässerungs-Rückfluss
- `calc_cell_month_load_ls` - Viehhaltung
- `calc_cell_month_load_crop` - Landwirtschaft
- etc.

**Struktur (Beispiel):**
```sql
CREATE TABLE calc_cell_month_load_man_ww (
    cell INT,
    year INT,
    month INT,  -- 1-12
    load DOUBLE,  -- Eintrag [t/month] oder [cfu/month]
    PRIMARY KEY (cell, year, month),
    INDEX idx_cell (cell),
    INDEX idx_year (year)
);
```

#### cell_input

**Zweck:** Zell-spezifische Input-Daten

**Struktur (Beispiel):**
```sql
CREATE TABLE cell_input (
    cell INT,
    IDScen INT,
    year INT,
    -- Weitere Spalten je nach Parameter
    PRIMARY KEY (cell, IDScen, year)
);
```

#### country_parameter_input

**Zweck:** Länder-spezifische Parameter

**Struktur (Beispiel):**
```sql
CREATE TABLE country_parameter_input (
    country_id INT,
    IDScen INT,
    year INT,
    parameter_id INT,
    -- Weitere Spalten je nach Parameter
    PRIMARY KEY (country_id, IDScen, year, parameter_id)
);
```

## Beziehungen

### Entity-Relationship (vereinfacht)

```
_project (1) ──< (N) _szenario
_szenario (1) ──< (N) _runlist
_parameter (1) ──< (N) _runlist
_runlist (1) ──< (N) concentration
_runlist (1) ──< (N) calc_cell_month_load_*
```

### Wichtige Foreign Keys

- `_szenario.project_id` → `_project.project_id`
- `_runlist.parameter_id` → `_parameter.parameter_id`
- `_runlist.IDScen` → `_szenario.IDScen`
- `concentration.IDrun` → `_runlist.IDrun`

## Indizes

### Performance-kritische Indizes

**concentration Tabelle:**
```sql
CREATE INDEX idx_idrun ON concentration(IDrun);
CREATE INDEX idx_cell ON concentration(cell);
CREATE INDEX idx_year ON concentration(year);
CREATE INDEX idx_idrun_year ON concentration(IDrun, year);
```

**calc_cell_month_load_* Tabellen:**
```sql
CREATE INDEX idx_cell ON calc_cell_month_load_man_ww(cell);
CREATE INDEX idx_year ON calc_cell_month_load_man_ww(year);
CREATE INDEX idx_cell_year ON calc_cell_month_load_man_ww(cell, year);
```

## Wichtige Queries

### Run-Informationen holen

```sql
SELECT 
    r.IDrun,
    r.runName,
    p.ParameterName,
    s.ScenName,
    r.IDTemp,
    r.conservative,
    r.lake
FROM wq_general._runlist r
JOIN wq_general._parameter p ON r.parameter_id = p.parameter_id
JOIN wq_general._szenario s ON r.IDScen = s.IDScen
WHERE r.IDrun = 403100091;
```

### Konzentrationen abfragen

```sql
-- Alle Konzentrationen für einen Run
SELECT cell, year, month, concentration
FROM wwqa_worldqual_af.concentration
WHERE IDrun = 403100091
ORDER BY cell, year, month;

-- Durchschnittliche Konzentration pro Monat
SELECT month, AVG(concentration) as avg_conc
FROM wwqa_worldqual_af.concentration
WHERE IDrun = 403100091 AND year = 2010
GROUP BY month;

-- Konzentrationen für bestimmte Zellen
SELECT cell, year, month, concentration
FROM wwqa_worldqual_af.concentration
WHERE IDrun = 403100091 
  AND cell IN (12345, 12346, 12347)
ORDER BY year, month;
```

### Einträge abfragen

```sql
-- Einträge für eine Zelle
SELECT year, month, load
FROM wwqa_wq_load_af.calc_cell_month_load_man_ww
WHERE cell = 12345
ORDER BY year, month;

-- Summe aller Einträge pro Jahr
SELECT year, SUM(load) as total_load
FROM wwqa_wq_load_af.calc_cell_month_load_man_ww
WHERE year = 2010
GROUP BY year;
```

### Datenbank-Statistiken

```sql
-- Anzahl Zellen mit Daten
SELECT COUNT(DISTINCT cell) as num_cells
FROM wwqa_worldqual_af.concentration
WHERE IDrun = 403100091;

-- Jahre mit Daten
SELECT DISTINCT year
FROM wwqa_worldqual_af.concentration
WHERE IDrun = 403100091
ORDER BY year;

-- Größe der Tabellen
SELECT 
    table_name,
    ROUND(((data_length + index_length) / 1024 / 1024), 2) AS size_mb
FROM information_schema.TABLES
WHERE table_schema = 'wwqa_worldqual_af'
ORDER BY size_mb DESC;
```

## Schema-Erstellung

### Vollständiges Schema erstellen

**Wichtig:** Das vollständige Schema ist komplex und sollte von einem bestehenden System exportiert werden.

**Export:**
```bash
mysqldump -u username -p --no-data wq_general > schema_wq_general.sql
mysqldump -u username -p --no-data wwqa_worldqual_af > schema_worldqual_af.sql
mysqldump -u username -p --no-data wwqa_wq_load_af > schema_wq_load_af.sql
```

**Import:**
```bash
mysql -u username -p < schema_wq_general.sql
mysql -u username -p < schema_worldqual_af.sql
mysql -u username -p < schema_wq_load_af.sql
```

### Tabellenstruktur prüfen

```sql
-- Alle Tabellen anzeigen
SHOW TABLES;

-- Struktur einer Tabelle
DESCRIBE concentration;
-- oder
SHOW CREATE TABLE concentration;

-- Alle Spalten einer Tabelle
SELECT COLUMN_NAME, DATA_TYPE, IS_NULLABLE
FROM INFORMATION_SCHEMA.COLUMNS
WHERE TABLE_SCHEMA = 'wwqa_worldqual_af' 
  AND TABLE_NAME = 'concentration';
```

## Migrationen

### Schema-Änderungen

**Vorgehen:**
1. Backup erstellen
2. Migration-Skript erstellen
3. Testen auf Test-Datenbank
4. Auf Produktion anwenden

**Beispiel-Migration:**
```sql
-- Neue Spalte hinzufügen
ALTER TABLE concentration 
ADD COLUMN quality_class INT AFTER concentration;

-- Index hinzufügen
CREATE INDEX idx_quality ON concentration(quality_class);
```

---

**Weitere Informationen:**
- [API-Referenz](API.md) - Funktionen die Datenbank nutzen
- [Architektur](ARCHITECTURE.md) - Wie Datenbank genutzt wird
