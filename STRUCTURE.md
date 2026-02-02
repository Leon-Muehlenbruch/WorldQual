# WorldQual - Projektstruktur

> 📚 **Navigation:** [Übersicht](README.md) | [Schnellreferenz](QUICK_REFERENCE.md) | [Detaillierte Anleitung](DOCUMENTATION.md) | [Index](INDEX.md)

Übersicht über die Verzeichnisstruktur und wichtige Dateien.

## Inhaltsverzeichnis

- [Verzeichnisstruktur](#verzeichnisstruktur)
- [Wichtige Dateitypen](#wichtige-dateitypen)
- [Abhängigkeiten zwischen Modulen](#abhängigkeiten-zwischen-modulen)
- [Kompilierungsreihenfolge](#kompilierungsreihenfolge)
- [Konfigurationsdateien-Struktur](#konfigurationsdateien-struktur)
- [Datenbankstruktur (Übersicht)](#datenbankstruktur-übersicht)
- [Input-Dateien (UNF-Format)](#input-dateien-unf-format)
- [Output-Dateien](#output-dateien)
- [R-Skripte Struktur](#r-skripte-struktur)
- [Entwicklungshinweise](#entwicklungshinweise)

## Verzeichnisstruktur

```
WorldQual-1.0/
│
├── README.md                    # Hauptdokumentation
├── DOCUMENTATION.md             # Detaillierte Dokumentation
├── STRUCTURE.md                # Diese Datei
│
├── _corr_factor/               # R-Skript für Korrekturfaktoren
│   └── corr_factor.R
│
├── create_scenarios/            # Szenarienerstellung
│   ├── create_scenarios.cpp
│   ├── makefile
│   └── OPTIONS.DAT (Beispiel)
│
├── fill_worldqual_load/        # Eintragsdaten-Berechnung
│   ├── fill_worldqual_load.cpp
│   ├── options_fill_wq_load.cpp
│   ├── options_fill_wq_load.h
│   ├── makefile
│   └── OPTIONS.DAT
│
├── find_river_cells/            # Flusszellen-Identifikation
│   ├── find_river_cells.cpp
│   ├── optionsfindRiverCells.cpp
│   ├── optionsfindRiverCells.h
│   ├── makefile
│   ├── OPTIONS.DAT
│   └── FC_AF_SENS_ANLYS.DAT
│
├── general_function/            # Gemeinsame Funktionen
│   ├── general_functions.cpp
│   ├── general_functions.h
│   ├── country.cpp
│   ├── country.h
│   ├── timestring.cpp
│   ├── timestring.h
│   └── makefile
│
├── MapRiverQuality/             # Kartierung der Wasserqualität
│   ├── mapRiverQuality.cpp
│   ├── options_mapRiverQuality.cpp
│   ├── options_mapRiverQuality.h
│   ├── makefile
│   └── OPTIONS.DAT
│
├── newton/                      # Numerische Kurvenanpassung
│   ├── Newton.cpp
│   ├── Newton.h
│   ├── countryinfo.cpp
│   ├── countryinfo.h
│   ├── DATA.DIR
│   └── Readme_Newton.txt
│
├── options/                     # Konfigurationsverwaltung
│   ├── options.cpp
│   ├── options.h
│   ├── options_scen.cpp
│   ├── options_scen.h
│   └── OPTIONS.DAT
│
├── pop_dens_input/              # Bevölkerungsdichte-Input
│   ├── pop_dens_input.cpp
│   ├── pop_dens_input.h
│   └── makefile
│
├── R-scripte/                   # R-Skripte für Visualisierung
│   ├── start.R                 # Hauptkonfigurationsskript
│   ├── station.R               # Stationen-Analyse
│   ├── station_years.R          # Zeitreihen-Analyse
│   ├── stations_scatterplot.R   # Streudiagramme
│   ├── RiverSection.R           # Flussabschnitt-Analyse
│   ├── RiverSection_scen.R     # Szenarien-Vergleich
│   ├── RiverSectionComparison.R
│   ├── start_single_stations_rout_gr_3000.R
│   ├── diagramm_functions.R     # Diagrammfunktionen
│   ├── ld_for_charts/          # Lastverteilungs-Diagramme
│   │   ├── ld_for_charts_ellen.R
│   │   ├── load_pie_chart.R
│   │   └── grid_arrange_shared_legend.R
│   └── ld_for_charts_var2/     # Variante 2
│       ├── ld_for_charts_ellen.R
│       ├── ld_for_charts_ellen_1990_2010.R
│       ├── load_pie_chart2.R
│       └── ReadMe.txt
│
├── rtf_input/                   # Return-to-Flow Input
│   ├── rtf_input.cpp
│   └── makefile
│
├── rtf_input_corr_factor/       # RTF mit Korrekturfaktoren
│   ├── rtf_input_corr_factor.cpp
│   ├── options_rtf_input.cpp
│   ├── options_rtf_input.h
│   ├── makefile
│   └── OPTIONS.DAT
│
├── water_temperature/           # Wassertemperatur-Berechnung
│   ├── water_temperature.cpp
│   ├── options_waterTemp.cpp
│   ├── options_waterTemp.h
│   ├── makefile
│   └── OPTIONS.DAT
│
├── wq_instream_daily/           # Tägliche In-Stream Berechnung
│   ├── wq_instream_daily.cpp
│   └── makefile
│
├── wq_stat_riverdef/            # Flussabschnitt-Definition
│   ├── river_start_end.cpp
│   ├── river_start_end.h
│   └── makefile
│
├── wq_stat_scen/                # Szenarien-Statistik
│   ├── wq_stat_scen.cpp
│   ├── options_wqstat_scen.cpp
│   ├── options_wqstat_scen.h
│   ├── makefile
│   ├── OPTIONS.DAT
│   └── STAT.DAT
│
├── wq_stat_stations/            # Stations-Statistik
│   ├── wq_stat_stations.cpp
│   ├── statistic_stations.h
│   ├── makefile
│   └── STAT.DAT
│
└── .gitignore                   # Git-Ignore-Regeln
```

## Wichtige Dateitypen

### C++ Quellcode
- `*.cpp`: C++ Implementierungsdateien
- `*.h`: C++ Header-Dateien

### Konfiguration
- `OPTIONS.DAT`: Hauptkonfigurationsdatei für jedes Modul
- `STAT.DAT`: Statistik-Konfiguration
- `*.DAT`: Weitere Konfigurationsdateien

### Build-Dateien
- `makefile`: Makefile für Kompilierung (jedes Modul hat sein eigenes)

### R-Skripte
- `*.R`: R-Skripte für Analyse und Visualisierung

### Dokumentation
- `README.md`: Hauptdokumentation
- `DOCUMENTATION.md`: Detaillierte Dokumentation
- `STRUCTURE.md`: Diese Datei
- `Readme_*.txt`: Modulspezifische READMEs

## Abhängigkeiten zwischen Modulen

```
general_function/  ──┐
                     ├──> Alle Module nutzen gemeinsame Funktionen
options/          ──┘

newton/            ──> Wird von water_temperature verwendet

fill_worldqual_load ──> Muss vor worldqual ausgeführt werden
water_temperature  ──> Optional, wird von worldqual verwendet
worldqual          ──> Hauptsimulation
wq_stat_*         ──> Analysieren Ergebnisse von worldqual

R-scripte/         ──> Analysieren Output von wq_stat_*
```

## Kompilierungsreihenfolge

1. **Zuerst gemeinsame Module:**
   ```bash
   cd general_function
   make all
   
   cd ../options
   make all
   ```

2. **Dann Utility-Module:**
   ```bash
   cd ../newton
   make all
   ```

3. **Dann spezifische Module** (in beliebiger Reihenfolge):
   ```bash
   cd ../water_temperature
   make all
   
   cd ../fill_worldqual_load
   make all
   
   # etc.
   ```

## Konfigurationsdateien-Struktur

### OPTIONS.DAT Format

Jede `OPTIONS.DAT` Datei folgt einem ähnlichen Format:

```
Runtime options [modulname]

1. [Option 1 Name]
   [Optionen-Beschreibung]
Value: [Wert]

2. [Option 2 Name]
   [Optionen-Beschreibung]
Value: [Wert]

...
```

### Gemeinsame Optionen

Die meisten Module teilen diese Optionen:
- `project_id`: Projekt-ID
- `IDScen`: Szenario-ID
- `IDVersion`: WaterGAP Version
- `IDReg`: Region
- `MyHost`, `MyUser`, `MyPassword`: MySQL-Verbindung
- `InputType`: 0=Datenbank, 1=UNF-Dateien
- `input_dir`: Pfad zu Input-Dateien

## Datenbankstruktur (Übersicht)

### Hauptdatenbanken

```
wq_general/                    # Allgemeine Konfiguration
├── _project                  # Projekte
├── _szenario                # Szenarien
├── _runlist                 # Run-Konfigurationen
├── _parameter               # Parameter-Definitionen
├── _water_temperature_list  # Wassertemperatur-Konfigurationen
└── watergap_region         # Region-Konfigurationen

wwqa_worldqual_[region]/     # Simulationsergebnisse
├── concentration           # Konzentrationen
├── cell_input             # Zell-Inputs
└── ...

wwqa_wq_load_[region]/      # Eintragsdaten
├── calc_cell_month_load_*  # Berechnete Einträge
├── cell_input             # Zell-Inputs
└── country_parameter_input  # Länder-Parameter
```

## Input-Dateien (UNF-Format)

### Typische Dateinamen-Konventionen

```
G_[PARAMETER]_[year].UNF0           # Jährliche Daten
G_[PARAMETER]_[year].12.UNF0        # Monatliche Daten (12 Monate)

Beispiele:
G_Q_out_m3_[year].12.UNF0           # Monatlicher Abfluss
G_RUNOFF_TOTAL_mm_[year].12.UNF0    # Monatlicher Runoff
G_MAN_WW_m3_[year].UNF0             # Jährliches Abwasservolumen
G_IRRIG_RTF_SW_m3_[year].12.UNF0    # RTF Bewässerung
G_CORR_FACT_RTF_[year].12.UNF0     # RTF Korrekturfaktoren
```

## Output-Dateien

### Datenbank-Output
- Ergebnisse werden in regionsspezifische Datenbanken geschrieben
- Tabellen: `concentration`, `cell_input`, etc.

### UNF-Output
- `.12.UNF0` Dateien für monatliche Grids
- `.UNF0` Dateien für jährliche Grids

### Statistik-Output
- Textdateien mit statistischen Kennzahlen
- Werden von R-Skripten weiterverarbeitet

## R-Skripte Struktur

### Hauptskripte
- `start.R`: Konfiguration und Startpunkt
- `station.R`: Einzelne Stationen
- `RiverSection.R`: Flussabschnitte
- `stations_scatterplot.R`: Validierungs-Diagramme

### Hilfsskripte
- `diagramm_functions.R`: Allgemeine Diagrammfunktionen
- `ld_for_charts/*.R`: Lastverteilungs-Diagramme

### Konfiguration in start.R

Wichtige Variablen:
- `r_script_dir`: Verzeichnis mit R-Skripten
- `current_dir`: Ausgabe-Verzeichnis für Grafiken
- `input.path`: Pfad zu Input-Dateien
- `input.filename`: Dateiname (oder leer für alle)
- `log_scale`: Logarithmische Skalierung
- `unit`: Einheit für Konzentration

## Entwicklungshinweise

### Neue Module hinzufügen

1. Verzeichnis erstellen
2. `makefile` erstellen (basierend auf bestehenden)
3. `OPTIONS.DAT` erstellen
4. Abhängigkeiten zu `general_function` und `options` einrichten
5. In Dokumentation aufnehmen

### Code-Standards

- C++11 Standard
- MySQL++ für Datenbankzugriff
- Konsistente Fehlerbehandlung
- Status-Ausgaben auf `stdout`
- Fehler auf `stderr`

### Debugging

- `#define DEBUG` für allgemeine Debug-Ausgaben
- `#define DEBUG_queries` für SQL-Query-Ausgaben
- Logging über `stdout`/`stderr` umleiten

---

**Weitere Hilfe:**
- 📖 [README.md](README.md) - Übersicht und Einführung
- 📚 [DOCUMENTATION.md](DOCUMENTATION.md) - Detaillierte Anleitung
- ⚡ [QUICK_REFERENCE.md](QUICK_REFERENCE.md) - Schnellreferenz
- 🎯 [INDEX.md](INDEX.md) - Dokumentations-Übersicht

**Hinweis:** Diese Strukturübersicht dient als Referenz. Für detaillierte Informationen siehe [DOCUMENTATION.md](DOCUMENTATION.md).
