# WorldQual - Architektur und Code-Struktur

Diese Dokumentation erklärt die **technische Architektur** von WorldQual für Entwickler.

## Inhaltsverzeichnis

- [Übersicht](#übersicht)
- [Modulare Architektur](#modulare-architektur)
- [Kern-Komponenten](#kern-komponenten)
- [Datenfluss](#datenfluss)
- [Klassen-Hierarchie](#klassen-hierarchie)
- [Datenstrukturen](#datenstrukturen)
- [Abhängigkeiten](#abhängigkeiten)

## Übersicht

WorldQual ist ein **modulares C++-System** zur Simulation der Wasserqualität in Flüssen. Die Architektur folgt dem Prinzip:

- **Modulare Programme**: Jedes Modul ist ein eigenständiges ausführbares Programm
- **Gemeinsame Bibliotheken**: `general_function` und `options` werden von allen Modulen genutzt
- **Datenbank-zentriert**: MySQL als zentrale Datenspeicherung
- **Grid-basiert**: Berechnungen auf 0.5° x 0.5° Grid-Zellen

## Modulare Architektur

### Architektur-Prinzipien

1. **Separation of Concerns**: Jedes Modul hat eine spezifische Aufgabe
2. **Shared Libraries**: Gemeinsame Funktionalität in `general_function` und `options`
3. **Datenbank als Interface**: Module kommunizieren über die Datenbank
4. **Konfiguration über Dateien**: Jedes Modul hat eine `OPTIONS.DAT`

### Modul-Typen

#### 1. Core-Module (Kern-Funktionalität)

**worldqual** (`src/worldqual/`)
- **Zweck**: Hauptsimulation der Wasserqualität
- **Input**: Einträge (aus wq_load), hydrologische Daten
- **Output**: Konzentrationen in Datenbank
- **Hauptdateien**: `WorldQual.cpp`, `options_wq.h/cpp`

**wq_load** (`src/wq_load/`)
- **Zweck**: Berechnung von Schadstoffeinträgen
- **Input**: Bevölkerungsdaten, Landnutzung, etc.
- **Output**: Eintragsdaten in Datenbank
- **Hauptdateien**: `wq_load.cpp`, `ClassWQload.h/cpp`

#### 2. Input-Module (Datenvorbereitung)

**fill_worldqual_load** (`src/fill_worldqual_load/`)
- Befüllt Datenbanktabellen mit Eintragsdaten
- Nutzt `ClassWQ_load` aus `wq_load`

**water_temperature** (`src/water_temperature/`)
- Berechnet Wassertemperatur aus Lufttemperatur
- Nutzt Newton-Verfahren für Kurvenanpassung

**rtf_input**, **rtf_input_corr_factor** (`src/rtf_input*/`)
- Verarbeitet Return-to-Flow Daten für Bewässerung

#### 3. Analyse-Module (Auswertung)

**wq_stat**, **wq_stat_stations**, **wq_stat_scen** (`src/wq_stat*/`)
- Statistische Analysen und Validierung
- Vergleich mit Messungen

**MapRiverQuality** (`src/MapRiverQuality/`)
- Kartierung und Klassifizierung der Ergebnisse

#### 4. Utility-Module (Hilfsprogramme)

**create_scenarios** (`src/create_scenarios/`)
- Erstellt neue Szenarien basierend auf Referenzszenarien

**find_river_cells** (`src/find_river_cells/`)
- Identifiziert Flusszellen

**copy_run**, **copy_scen** (`src/copy_*/`)
- Verwaltung von Runs und Szenarien

#### 5. Shared Libraries (Gemeinsame Funktionen)

**general_function** (`src/general_function/`)
- **Zweck**: Zentrale Funktionen für alle Module
- **Inhalt**:
  - Datenbankzugriff
  - Datei-I/O (UNF-Format)
  - Hydrologische Berechnungen
  - Grid-Management
- **Hauptdateien**: `general_functions.h/cpp`, `country.h/cpp`, `timestring.h/cpp`

**options** (`src/options/`)
- **Zweck**: Konfigurationsverwaltung
- **Inhalt**:
  - Parsing von `OPTIONS.DAT`
  - Command-Line-Optionen
  - Basis-Klasse `optionsClass`
- **Hauptdateien**: `options.h/cpp`, `options_scen.h/cpp`

**newton** (`src/newton/`)
- **Zweck**: Numerische Kurvenanpassung
- **Verwendung**: Von `water_temperature` genutzt
- **Hauptdateien**: `Newton.h/cpp`

## Kern-Komponenten

### 1. optionsClass - Konfigurations-Basisklasse

**Lage**: `src/options/options.h`

**Zweck**: Basisklasse für alle Options-Klassen

**Wichtige Member:**
```cpp
class optionsClass {
    int IDVersion;          // WaterGAP Version (2 oder 3)
    int IDReg;              // Region (1-6)
    char continent_abb[10]; // Kontinent-Abkürzung
    char* MyHost;           // MySQL Host
    char* MyUser;           // MySQL User
    char* MyPassword;       // MySQL Passwort
    short InputType;        // 0=Datenbank, 1=UNF-Dateien
    char* input_dir;        // Pfad zu Input-Dateien
};
```

**Verwendung**: Alle Module haben eine spezialisierte Options-Klasse:
- `options_wqClass` (worldqual)
- `options_wq_loadClass` (wq_load)
- `options_waterTempClass` (water_temperature)
- etc.

### 2. general_functions - Zentrale Funktionen

**Lage**: `src/general_function/general_functions.h`

**Kategorien von Funktionen:**

#### Datenbankzugriff
- `getProject()` - Projekt-Informationen laden
- `checkParameter()` - Parameter prüfen
- `checkIDRun()` - Run-Konfiguration prüfen

#### Hydrologische Funktionen
- `get_outflc()` - Abflusszellen lesen
- `get_inflow()` - Zuflüsse lesen
- `get_Q()` - Abfluss lesen
- `get_cell_runoff()` - Oberflächenabfluss lesen
- `get_flowVelocity()` - Fließgeschwindigkeit berechnen

#### Wasserqualitäts-Funktionen
- `get_factor()` - Faktoren für Berechnungen
- `get_s_cell()` - Schadstoff-Einträge pro Zelle
- `calculateAttenuation()` - Abschwächung berechnen
- `getDecayRates()` - Abbau-Raten

#### Datei-I/O
- `readInputFile()` - UNF-Dateien lesen
- `readUnf()` - Template-Funktion für UNF-Format
- `readArea()` - Flächen-Daten lesen

#### Grid-Management
- `getCELLS()` - Grid-Informationen laden
- `getGCRC()` - Grid-Zell-IDs laden
- `getArcID()` - Arc-ID Konvertierung

### 3. ClassWQ_load - Belastungsberechnung

**Lage**: `src/wq_load/ClassWQload.h`

**Zweck**: Berechnet Schadstoffeinträge aus verschiedenen Quellen

**Wichtige Methoden:**
```cpp
class ClassWQ_load {
    // Konstruktor berechnet Einträge für ein Jahr
    ClassWQ_load(options_wq_loadClass* options_ptr, int year);
    
    // Berechnet verschiedene Eintragstypen
    void calculatePointSources();      // Punktuelle Quellen
    void calculateDiffuseSources();   // Diffuse Quellen
    void calculateGeogenic();         // Geogene Quellen
    void calculateAtmospheric();      // Atmosphärische Deposition
};
```

**Hilfsklassen:**
- `COUNTRY_CLASS` - Länderinformationen
- `CROP_CLASS` - Kulturpflanzen-Daten
- `LS_CLASS` - Landnutzung
- `GB_CLASS` - Viehhaltung

### 4. continentStruct - Grid-Informationen

**Lage**: `src/general_function/general_functions.h`

**Zweck**: Speichert Grid-Dimensionen und -Informationen

```cpp
struct continentStruct {
    int ng;              // Anzahl aller Zellen
    int ng_land;         // Anzahl Landzellen
    int nrows;           // Anzahl Zeilen
    int ncols;           // Anzahl Spalten
    int cellFraction;    // Zellfraktion
    char hydro_input[maxcharlength]; // Pfad zu WaterGAP-Daten
};
```

**Verwendung**: Wird an fast alle Funktionen übergeben, um Grid-Dimensionen zu kennen.

## Datenfluss

### Typischer Workflow

```
1. Input-Daten (WaterGAP, Bevölkerungsdaten, etc.)
   ↓
2. fill_worldqual_load
   → Berechnet Einträge
   → Speichert in Datenbank: wwqa_wq_load_[region].calc_cell_month_load_*
   ↓
3. water_temperature (optional)
   → Berechnet Wassertemperatur
   → Speichert in Datenbank
   ↓
4. worldqual
   → Liest Einträge aus Datenbank
   → Liest hydrologische Daten (Q_out, Runoff)
   → Simuliert Transport und Abbau
   → Speichert Konzentrationen: wwqa_worldqual_[region].concentration
   ↓
5. wq_stat_stations
   → Liest Konzentrationen
   → Vergleicht mit Messungen
   → Erstellt Statistik-Dateien
   ↓
6. R-Skripte
   → Lesen Statistik-Dateien
   → Erstellen Diagramme
```

### Datenbank als Kommunikationsschnittstelle

Module kommunizieren **nicht direkt**, sondern über die Datenbank:

- **wq_load** schreibt → **worldqual** liest
- **worldqual** schreibt → **wq_stat** liest
- **wq_stat** schreibt → **R-Skripte** lesen

**Vorteile:**
- Lose Kopplung zwischen Modulen
- Module können unabhängig entwickelt werden
- Ergebnisse bleiben persistent

**Nachteile:**
- Datenbank muss immer verfügbar sein
- Potenzielle Performance-Probleme bei großen Datenmengen

## Klassen-Hierarchie

### Options-Klassen

```
optionsClass (Basisklasse)
├── options_wqClass (worldqual)
├── options_wq_loadClass (wq_load)
├── options_waterTempClass (water_temperature)
├── options_wqstatClass (wq_stat)
├── options_mapRiverQualityClass (MapRiverQuality)
└── ... (weitere spezialisierte Klassen)
```

### Load-Klassen

```
ClassWQ_load (Hauptklasse)
├── COUNTRY_CLASS (Länderinformationen)
├── CROP_CLASS (Kulturpflanzen)
├── LS_CLASS (Landnutzung)
└── GB_CLASS (Viehhaltung)
```

## Datenstrukturen

### Wichtige Strukturen

#### continentStruct
Siehe oben - Grid-Informationen

#### loadStruct
```cpp
struct loadStruct {
    bool loadType[loadTypes];           // Welche Load-Typen werden benötigt?
    char loadTab[loadTypes][maxcharlength]; // Tabellennamen
    char loadTabAlias[loadTypes][loadTypes]; // SQL-Aliase
    char loadTabLongName[loadTypes][maxcharlength]; // Beschreibungen
};
```

**Zweck**: Verwaltet welche Eintragstabellen für einen Parameter benötigt werden.

#### Substance
```cpp
struct Substance {
    int IDrun;
    int date;
    int cell;
    double Value;
};
```

**Zweck**: Speichert eine einzelne Konzentration (für Routing-Berechnungen).

## Abhängigkeiten

### Kompilierungs-Abhängigkeiten

```
general_function (muss zuerst kompiliert werden)
  ↓
options (muss zuerst kompiliert werden)
  ↓
newton (wird von water_temperature benötigt)
  ↓
Alle anderen Module (können parallel kompiliert werden)
```

### Laufzeit-Abhängigkeiten

```
fill_worldqual_load → worldqual
  (muss vorher ausgeführt werden)

water_temperature → worldqual
  (optional, aber empfohlen)

worldqual → wq_stat_*
  (muss vorher ausgeführt werden)

wq_stat_* → R-Skripte
  (muss vorher ausgeführt werden)
```

### Bibliotheks-Abhängigkeiten

**MySQL++** (`libmysqlpp`)
- Für alle Module
- Datenbankzugriff

**MySQL Client** (`libmysqlclient`)
- Für alle Module
- Low-level MySQL-Zugriff

**Standard C++ Library**
- `<vector>`, `<string>`, `<iostream>`, etc.

**Math Library** (`libm`)
- Mathematische Funktionen

## Design-Entscheidungen

### Warum modulare Programme statt einer großen Anwendung?

1. **Flexibilität**: Module können unabhängig aktualisiert werden
2. **Wartbarkeit**: Klare Trennung der Verantwortlichkeiten
3. **Testbarkeit**: Module können einzeln getestet werden
4. **Parallele Entwicklung**: Verschiedene Entwickler können an verschiedenen Modulen arbeiten

### Warum Datenbank statt direkter Kommunikation?

1. **Persistenz**: Ergebnisse bleiben erhalten
2. **Flexibilität**: Module können in beliebiger Reihenfolge ausgeführt werden
3. **Debugging**: Daten können direkt in der Datenbank geprüft werden
4. **Skalierbarkeit**: Module können auf verschiedenen Rechnern laufen

### Warum Makefiles statt CMake/autotools?

1. **Einfachheit**: Jedes Modul hat sein eigenes Makefile
2. **Kontrolle**: Entwickler haben volle Kontrolle über Kompilierung
3. **Tradition**: Projekt ist alt, Makefiles waren Standard

**Nachteil**: Pfade müssen manuell angepasst werden.

## Erweiterungspunkte

### Wo kann man neue Funktionalität hinzufügen?

1. **Neue Module**: Erstellen Sie ein neues Verzeichnis mit eigenem Makefile
2. **Neue Funktionen in general_function**: Fügen Sie Funktionen zu `general_functions.h/cpp` hinzu
3. **Neue Options-Klassen**: Erben Sie von `optionsClass`
4. **Neue Load-Typen**: Erweitern Sie `loadStruct` und `fillLoadStruct()`

### Wichtige Dateien für Erweiterungen

- `src/general_function/general_functions.h` - Neue gemeinsame Funktionen
- `src/options/options.h` - Basis-Options-Klasse
- `src/wq_load/ClassWQload.h` - Belastungsberechnung erweitern

---

**Weitere Informationen:**
- [Entwicklungsumgebung](DEVELOPMENT.md) - Wie entwickelt man?
- [Code-Standards](CODING_STANDARDS.md) - Wie codet man?
- [API-Referenz](API.md) - Wichtige Funktionen
