# WorldQual - Schnellreferenz

**Navigation:** [Übersicht](../index.md) | [Detaillierte Anleitung](anleitung.md) | [Index](../index.md) | [Struktur](projektstruktur.md)

Kurze Übersicht für die tägliche Nutzung.

## Inhaltsverzeichnis

- [Schnellstart](#schnellstart)
- [Wichtige Nummern](#wichtige-nummern)
- [Datenbank-Abfragen](#datenbank-abfragen)
- [OPTIONS.DAT - Wichtige Einstellungen](#optionsdat---wichtige-einstellungen)
- [Häufige Befehle](#häufige-befehle)
- [Datenbank-Backup](#datenbank-backup)
- [Ergebnisse exportieren](#ergebnisse-exportieren)
- [R-Visualisierung](#r-visualisierung)
- [Fehlerbehebung - Schnell](#fehlerbehebung---schnell)
- [Modul-Verzeichnisse](#modul-verzeichnisse)
- [Workflow-Checkliste](#workflow-checkliste)
- [Dauer-Schätzungen](#dauer-schätzungen)

## Schnellstart

```bash
# 1. Einträge berechnen
cd fill_worldqual_load
./fill_worldqual_load START_JAHR END_JAHR

# 2. Wassertemperatur (optional)
cd ../water_temperature
./water_temperature START_JAHR END_JAHR

# 3. Hauptsimulation
cd ../worldqual
./worldqual IDRUN START_JAHR END_JAHR

# 4. Statistik
cd ../wq_stat_stations
./wq_stat_stations IDRUN START_JAHR END_JAHR
```

## Wichtige Nummern

### Parameter-IDs
- **0** = BOD (Biochemischer Sauerstoffbedarf)
- **1** = Salz (TDS)
- **2** = FC (Fäkalcoliforme Bakterien)
- **3** = TN (Gesamtstickstoff)
- **4** = TP (Gesamtphosphor)

### Region-IDs
- **1** = Europa (eu)
- **2** = Afrika (af)
- **3** = Asien (as)
- **4** = Australien (au)
- **5** = Nordamerika (na)
- **6** = Südamerika (sa)

### WaterGAP Versionen
- **2** = WaterGAP2
- **3** = WaterGAP3 (empfohlen)

## Datenbank-Abfragen

### Projekt-ID finden
```sql
SELECT project_id, Project_Name FROM wq_general._project;
```

### Szenario-ID finden
```sql
SELECT IDScen, ScenName FROM wq_general._szenario;
```

### IDrun finden
```sql
SELECT IDrun, runName FROM wq_general._runlist 
WHERE parameter_id=2 AND IDScen=91;
```

### Parameter-Liste
```sql
SELECT parameter_id, ParameterName FROM wq_general._parameter;
```

### Ergebnisse anzeigen
```sql
SELECT * FROM wwqa_worldqual_af.concentration 
WHERE IDrun=403100091 AND year=2010 
LIMIT 10;
```

### Durchschnittliche Konzentration
```sql
SELECT month, AVG(concentration) as avg_conc
FROM wwqa_worldqual_af.concentration 
WHERE IDrun=403100091 AND year=2010
GROUP BY month;
```

## OPTIONS.DAT - Wichtige Einstellungen

```
project_id = 2          # Projekt-ID (aus Datenbank)
IDScen = 91            # Szenario-ID (aus Datenbank)
IDVersion = 3           # WaterGAP Version (2 oder 3)
IDReg = 2              # Region (1-6)
MyHost = localhost      # MySQL-Server
MyUser = worldqual      # MySQL-User
MyPassword = passwort   # MySQL-Passwort
InputType = 1          # 0=Datenbank, 1=UNF-Dateien
input_dir = /pfad/zu/daten  # Pfad zu UNF-Dateien
```

## Häufige Befehle

### MySQL-Verbindung testen
```bash
mysql -u worldqual -p -h localhost
```

### MySQL-Status prüfen
```bash
systemctl status mysql
```

### Dateien prüfen
```bash
ls -lh /pfad/zu/daten/*.UNF0
```

### RAM prüfen
```bash
free -h
```

### Logging aktivieren
```bash
./worldqual 403100091 2010 2010 > log.txt 2>&1
```

### Fortschritt prüfen (während Simulation)
```sql
SELECT DISTINCT year FROM wwqa_worldqual_af.concentration 
WHERE IDrun=403100091 
ORDER BY year;
```

## Datenbank-Backup

```bash
# Backup erstellen
mysqldump -u username -p wwqa_worldqual_af > backup_$(date +%Y%m%d).sql

# Backup wiederherstellen
mysql -u username -p wwqa_worldqual_af < backup_20260202.sql
```

## Ergebnisse exportieren

### Als CSV
```sql
SELECT * FROM wwqa_worldqual_af.concentration 
WHERE IDrun=403100091 AND year=2010
INTO OUTFILE '/tmp/ergebnisse.csv'
FIELDS TERMINATED BY ',' 
ENCLOSED BY '"'
LINES TERMINATED BY '\n';
```

## R-Visualisierung

```r
# In R
setwd("R-scripte")

# start.R anpassen:
input.path <- "/pfad/zu/statistik"
input.filename <- ""
unit <- "cfu/100ml"
log_scale <- "y"

source("start.R")
source("station.R")
```

## Fehlerbehebung - Schnell

| Problem | Lösung |
|---------|--------|
| Datenbankverbindung fehlgeschlagen | MySQL läuft? `systemctl status mysql` |
| Datei nicht gefunden | Pfad prüfen: `ls /pfad/zu/datei` |
| Falsche Parameter-ID | `SELECT * FROM wq_general._parameter;` |
| Falsche IDrun | `SELECT * FROM wq_general._runlist WHERE IDrun=...;` |
| Speicherprobleme | Kleinere Region/Zeitraum testen |

## Modul-Verzeichnisse

```
fill_worldqual_load/     # Einträge berechnen
water_temperature/        # Wassertemperatur
worldqual/               # Hauptsimulation
wq_stat_stations/        # Statistik
wq_stat_scen/           # Szenarienvergleich
MapRiverQuality/         # Kartierung
create_scenarios/        # Szenarien erstellen
R-scripte/              # Visualisierung
```

## Workflow-Checkliste

- [ ] Projekt und Szenario in Datenbank vorhanden?
- [ ] OPTIONS.DAT angepasst?
- [ ] Input-Daten vorhanden?
- [ ] Module kompiliert?
- [ ] MySQL läuft?
- [ ] IDrun bekannt?

## Dauer-Schätzungen

| Aufgabe | Dauer (ca.) |
|---------|-------------|
| Einträge (1 Jahr, kleine Region) | 5-15 Min |
| Einträge (mehrere Jahre) | Stunden |
| Hauptsimulation (1 Jahr, kleine Region) | 1-4 Std |
| Hauptsimulation (mehrere Jahre) | Tage |
| Statistik | Minuten bis Stunden |

## Wichtige Dateien

- `OPTIONS.DAT` - Konfiguration (in jedem Modul)
- `STAT.DAT` - Statistik-Konfiguration
- `makefile` - Kompilierung
- `start.R` - R-Konfiguration

## Weitere Hilfe

- [README.md](../index.md) - Übersicht und Einführung
- [DOCUMENTATION.md](anleitung.md) - Detaillierte Anleitung
- [STRUCTURE.md](projektstruktur.md) - Projektstruktur
- [INDEX.md](../index.md) - Dokumentations-Übersicht

---

**Tipp:** Lesezeichen setzen für schnellen Zugriff! Auf GitHub können Sie alle Links durch Klick nutzen.
