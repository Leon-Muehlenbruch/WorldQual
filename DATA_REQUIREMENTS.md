# WorldQual Daten-Anforderungen

## Überblick

WorldQual benötigt umfangreiche Input-Daten aus drei Hauptquellen:
1. **MySQL-Datenbank** - Routing, Szenarien, Konfiguration
2. **UNF-Dateien** - Hydrologische und klimatische Zeitreihen (WaterGAP)
3. **CSV/Text-Dateien** - Zusätzliche Parameter

---

## 1. MySQL-Datenbank Struktur

### Benötigte Datenbanken:

#### a) `wq_general` (Core Database)
**Zweck:** Zentrale Konfigurations- und Verwaltungsdatenbank

**Tabellen:**
- `_runlist` - Liste aller Simulationsläufe mit Parametern
- `_szenario` - Szenario-Definitionen
- `_parameter` - Liste der simulierten Parameter (N, P, BOD, etc.)

**Felder in _runlist:**
```sql
CREATE TABLE _runlist (
    IDrun INT PRIMARY KEY,
    IDScen INT,
    parameter_id INT,
    IDTemp INT,
    IDVersion INT,
    IDReg INT,
    start_year INT,
    end_year INT,
    description TEXT
);
```

#### b) `worldqual_{continent}` (z.B. worldqual_af, worldqual_eu)
**Zweck:** Kontinentspezifische Routing- und Flussdaten

**Tabellen:**
- `routing` - Fluss-Routing Informationen
  - cell (Zellen-ID)
  - inflow_count (Anzahl Zuflüsse)
  - inflow_done (Routing-Status)
  
- `flow_velocity` - Fließgeschwindigkeiten
  - cell
  - velocity (m/s)
  - river_length (km)
  - rout_order (Routing-Reihenfolge)

- `load` oder `load_{IDScen}_{parameter}` - Nährstoffeinträge
  - cell
  - date (YYYYMM Format)
  - load (kg/Monat)

#### c) `wq_out_{continent}` (z.B. wq_out_af, wq_out_eu)
**Zweck:** Ausgabedatenbank für Simulationsergebnisse

**Tabellen:**
- `concentration_{IDrun}` - Berechnete Konzentrationen
  - date (YYYYMM)
  - cell (Zellen-ID)
  - con (Konzentration)
  - con2 (Sekundäre Konzentration)

#### d) `watergap_unf`
**Zweck:** WaterGAP hydrologische Basisdaten

**Tabellen:**
- `gr` - Grid Row (Zeilen-Index)
- `gc` - Grid Column (Spalten-Index)
- `g_inflc_arc_id` - Zufluss-Informationen
  - cell
  - SW, S, SE, W, E, NW, N, NE (Nachbarzellen)
- `mother_{continent}` - Einzugsgebiete
  - arcid
  - bas0_id
  - rout_area
- `country_continent` - Länder-Kontinent Zuordnung
  - isonum
  - idreg

#### e) `wq_load_{continent}` (z.B. wq_load_af)
**Zweck:** Berechnete Nährstoffeinträge

**Tabellen:**
- `calc_cell_month_load_{type}_{IDScen}_{parameter}`
  - cell
  - month
  - load
  - (verschiedene Quellen: dom, man, ind, atm, etc.)

---

## 2. UNF-Dateien (Binary Grid Files)

UNF = Unformatted Binary Files (WaterGAP-Format)

### Format:
- Binäre Rasterdateien
- 0.5° oder 5' Auflösung
- Zeitreihen (monatlich)

### Benötigte UNF-Dateien:

#### A. Hydrologische Daten (path_watergap_output)
**Quelle:** WaterGAP Hydrologiemodell

| Datei | Beschreibung | Einheit |
|-------|--------------|---------|
| `G_RIVER_VELOCITY_{year}.12.UNF0` | Fließgeschwindigkeit | m/s |
| `G_GLO_LAKE_STOR_MEAN_{year}.12.UNF0` | Seespeicher | km³ |
| `G_RES_STOR_MEAN_{year}.12.UNF0` | Reservoir-Speicher | km³ |

**Format:** 12 monatliche Werte pro Jahr

#### B. Klimadaten (path_climate)
**Quelle:** Klimamodell (CRU/WATCH)

| Datei | Beschreibung | Einheit |
|-------|--------------|---------|
| `GSHORTWAVE_{year}.12.UNF2` | Kurzwellige Strahlung | W/m² |
| `GTEMP_{year}.12.UNF2` | Lufttemperatur | °C |

**Verwendet für:** Wassertemperatur-Berechnung

#### C. Hydro-Input (path_hydro_input)
**Quelle:** WaterGAP Basisdaten

| Datei | Beschreibung | Einheit |
|-------|--------------|---------|
| `G_WG3_WG2WITH5MIN.UNF4` | Grid-Mapping WG3→WG2 | - |
| `G_WG3_WATCH.UNF4` | Grid-Mapping WG3→WATCH | - |
| `G_MEANDERING_RATIO.UNF0` | Flussmäander-Verhältnis | - |
| `G_ELEV_MEAN_CLM.UNF0` | Mittlere Höhe (CLM) | m |
| `G_ELEV_MEAN_WATCH.UNF0` | Mittlere Höhe (WATCH) | m |
| `G_LAND_AREA.UNF1` | Landfläche | km² |
| `G_SURFACE_RUNOFF_MEAN.UNF0` | Oberflächenabfluss | mm/a |
| `G_RIVERSLOPE.UNF0` | Flussgefälle | m/m |
| `G_LANDSLOPE.UNF0` | Landgefälle | m/m |
| `GAREA.UNF0` | Zellenfläche | km² |
| `GBUILTUP.UNF0` | Versiegelungsgrad | - |
| `GFREQ.UNF1` | Überschwemmungsfrequenz | - |
| `G_PWEATHERING.UNF0` | P-Verwitterung | kg/ha/a |
| `G_SOILEROS.UNF0` | Bodenerosion | t/ha/a |

#### D. Routing (path_hydro_routing)
**Quelle:** WaterGAP Routing

| Datei | Beschreibung |
|-------|--------------|
| `GCELLDIST.9.UNF0` | Zelldistanzen für Routing |

---

## 3. Zusätzliche Input-Dateien

### Nährstoff-Einträge (wq_load Modul)
- Bevölkerungsdaten
- Viehbestandsdaten
- Industriedaten
- Atmosphärische Deposition
- Düngemittel-Anwendung

---

## 4. Daten-Organisation

### Verzeichnisstruktur (Beispiel):
```
/data/
├── watergap_output/           # Hydrologische Zeitreihen
│   ├── G_RIVER_VELOCITY_2000.12.UNF0
│   ├── G_RIVER_VELOCITY_2001.12.UNF0
│   ├── G_GLO_LAKE_STOR_MEAN_2000.12.UNF0
│   └── ...
│
├── climate/                   # Klimadaten
│   ├── GSHORTWAVE_2000.12.UNF2
│   ├── GSHORTWAVE_2001.12.UNF2
│   ├── GTEMP_2000.12.UNF2
│   └── ...
│
├── hydro_input/              # Statische hydrologische Daten
│   ├── G_MEANDERING_RATIO.UNF0
│   ├── G_ELEV_MEAN_CLM.UNF0
│   ├── G_LAND_AREA.UNF1
│   └── ...
│
└── routing/                  # Routing-Daten
    └── GCELLDIST.9.UNF0
```

### MySQL Datenbank:
```
WorldQual_DB/
├── wq_general                # Global
├── worldqual_af              # Afrika
├── worldqual_as              # Asien
├── worldqual_au              # Australien
├── worldqual_eu              # Europa
├── worldqual_na              # Nordamerika
├── worldqual_sa              # Südamerika
├── wq_out_af                 # Outputs Afrika
├── wq_out_as                 # etc.
└── watergap_unf              # WaterGAP Basis
```

---

## 5. Datengröße und -umfang

### Geschätzte Größen:

**UNF-Dateien:**
- Pro Jahr hydrologische Daten: ~500 MB
- 10 Jahre Simulation: ~5 GB
- Klimadaten: ~200 MB/Jahr
- Statische Daten: ~1 GB

**MySQL-Datenbank:**
- wq_general: ~10 MB
- worldqual_{continent}: ~100 MB pro Kontinent
- watergap_unf: ~500 MB
- wq_out_{continent}: ~50 MB pro Run

**Gesamt für globale 10-Jahres-Simulation: ~50-100 GB**

---

## 6. Datenanforderungen nach Modul

### worldqual (Hauptprogramm)
✓ MySQL: wq_general, worldqual_{continent}, watergap_unf
✓ UNF: Hydrologische Zeitreihen, Klimadaten
✓ Ausgabe: wq_out_{continent}

### wq_load (Nährstoffeinträge)
✓ MySQL: worldqual_{continent}, watergap_unf
✓ UNF: Statische hydrologische Daten
✓ CSV: Bevölkerung, Viehbestand, Düngemittel
✓ Ausgabe: wq_load_{continent}

### wq_stat (Statistik)
✓ MySQL: wq_out_{continent}
✓ Eingabe: Simulationsergebnisse
✓ Ausgabe: Statistische Auswertungen

### MapRiverQuality (Visualisierung)
✓ MySQL: wq_out_{continent}, watergap_unf
✓ Ausgabe: Karten, Grafiken

---

## 7. Minimal-Setup für Tests

Für einen minimalen Test-Lauf benötigen Sie:

### Option A: Minimale Datenbank
```sql
-- Erstellen
CREATE DATABASE wq_general;
CREATE DATABASE worldqual_test;
CREATE DATABASE wq_out_test;
CREATE DATABASE watergap_unf;

-- Beispiel-Daten einfügen
INSERT INTO wq_general._runlist VALUES (1, 1, 1, 1, 3, 1, 2000, 2001, 'Test');
```

### Option B: Demo-Daten
- Kleine Region (z.B. ein Einzugsgebiet)
- 1 Jahr Simulation
- Vereinfachte UNF-Dateien

---

## 8. Datenquellen

### Wo kommen die Daten her?

1. **WaterGAP Output**
   - Quelle: WaterGAP Hydrologiemodell
   - Institution: Universität Kassel / Goethe Uni Frankfurt
   - Format: UNF binary files

2. **Klimadaten**
   - CRU (Climate Research Unit)
   - WATCH (Water and Global Change)

3. **Nährstoffdaten**
   - NEWS2 (Nutrient Export from WaterSheds)
   - FAO (Food and Agriculture Organization)
   - Verschiedene nationale Statistiken

---

## 9. Empfohlene Vorgehensweise

### Für produktiven Einsatz:
1. Kontakt mit WaterGAP-Gruppe aufnehmen
2. Datenzugriff für benötigte Region/Zeitraum anfragen
3. MySQL-Datenbank mit Schema aufsetzen
4. UNF-Dateien organisieren
5. OPTIONS.DAT konfigurieren
6. Test-Lauf mit kleiner Region

### Für Entwicklung/Tests:
1. Synthetische Testdaten erstellen
2. Minimale Datenbank aufsetzen
3. Mock-UNF-Dateien generieren
4. Validierung mit echten Daten

---

## 10. Nächste Schritte

**Was möchten Sie tun?**

A. **Synthetische Testdaten erstellen**
   → Skript zum Generieren minimaler Test-Datenbank
   → Mock-UNF-Dateien für einen Test-Lauf

B. **Datenbank-Schema dokumentieren**
   → SQL-Schema-Dateien erstellen
   → ER-Diagramm generieren

C. **Daten-Akquise-Guide erstellen**
   → Kontakte und Quellen auflisten
   → Datenformat-Konverter entwickeln

D. **Demo-Setup**
   → Voll funktionsfähiges Mini-Beispiel
   → Mit einem kleinen Einzugsgebiet

---

## Zusammenfassung

WorldQual benötigt:
- ✅ **3 Datenquellen:** MySQL + UNF-Dateien + zusätzliche Inputs
- ✅ **5-6 Datenbanken:** wq_general, worldqual_*, wq_out_*, watergap_unf, wq_load_*
- ✅ **~50-100 GB Daten** für globale Simulation
- ✅ **WaterGAP-Output** als Hauptquelle
- ✅ **Komplexe Datenstruktur** erfordert sorgfältige Organisation

**Ohne diese Daten läuft WorldQual nicht produktiv, aber wir können:**
1. Testdaten generieren
2. Datenbank-Schema dokumentieren
3. Daten-Akquise vorbereiten
