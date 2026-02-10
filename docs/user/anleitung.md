# WorldQual - Detaillierte Anleitung

**Navigation:** [Übersicht](../index.md) | [Schnellreferenz](schnellreferenz.md) | [Index](../index.md) | [Struktur](projektstruktur.md)

Diese Dokumentation erklärt **genau**, wie Sie WorldQual Schritt für Schritt nutzen.

## Inhaltsverzeichnis

1. [Erste Schritte - Kompletter Workflow](#erste-schritte---kompletter-workflow)
   - [Vorbereitung: Was brauchen Sie?](#vorbereitung-was-brauchen-sie)
   - [Schritt-für-Schritt: Ihre erste Simulation](#schritt-für-schritt-ihre-erste-simulation)
2. [Module im Detail](#module-im-detail)
   - [fill_worldqual_load - Einträge berechnen](#fill_worldqual_load---einträge-berechnen)
   - [worldqual - Hauptsimulation](#worldqual---hauptsimulation)
   - [wq_stat_stations - Statistische Analyse](#wq_stat_stations---statistische-analyse)
   - [water_temperature - Wassertemperatur](#water_temperature---wassertemperatur)
3. [Konfiguration verstehen](#konfiguration-verstehen)
   - [OPTIONS.DAT - Die wichtigste Datei](#optionsdat---die-wichtigste-datei)
   - [Wichtige Optionen erklärt](#wichtige-optionen-erklärt)
4. [Datenbank verstehen](#datenbank-verstehen)
   - [Übersicht über die Datenbanken](#übersicht-über-die-datenbanken)
   - [wq_general - Konfigurationsdatenbank](#wq_general---konfigurationsdatenbank)
5. [Datenbankstruktur komplett anlegen](#datenbankstruktur-komplett-anlegen)
   - [Voraussetzungen](#voraussetzungen)
   - [Schritt 1: Basis-Datenbank erstellen](#schritt-1-basis-datenbank-erstellen)
   - [Schritt 2: wq_general Tabellen erstellen](#schritt-2-wq_general-tabellen-erstellen)
   - [Schritt 3-7: Weitere Tabellen](#schritt-3-regionsspezifische-datenbanken---concentration-tabelle)
6. [WaterGAP-Daten genau beschaffen](#watergap-daten-genau-beschaffen)
   - [Was sind WaterGAP-Daten?](#was-sind-watergap-daten)
   - [Welche Daten werden benötigt?](#welche-daten-werden-benötigt)
   - [Wo bekommt man WaterGAP-Daten?](#wo-bekommt-man-watergap-daten)
   - [Datenformat verstehen](#datenformat-verstehen)
   - [Benötigte Dateien - Komplette Liste](#benötigte-dateien---komplette-liste)
7. [Alle Module kompilieren - Komplette Liste](#alle-module-kompilieren---komplette-liste)
   - [Kompilierungsreihenfolge (wichtig!)](#kompilierungsreihenfolge-wichtig)
   - [Automatisiertes Build-Skript](#automatisiertes-build-skript)
   - [Modul-Übersicht](#modul-übersicht)
   - [Häufige Kompilierungsfehler](#häufige-kompilierungsfehler)
8. [Fehlermeldungen verstehen - Kompletter Guide](#fehlermeldungen-verstehen---kompletter-guide)
   - [Datenbank-Fehler](#datenbank-fehler)
   - [Datei-Fehler](#datei-fehler)
   - [Kompilierungsfehler](#kompilierungsfehler)
   - [Laufzeit-Fehler](#laufzeit-fehler)
   - [Fehlerbehebungs-Workflow](#fehlerbehebungs-workflow)
9. [Praktische Beispiele](#praktische-beispiele)
   - [Beispiel 1: Einfache Simulation](#beispiel-1-einfache-simulation-ein-jahr-ein-parameter)
   - [Beispiel 2: Mehrere Jahre](#beispiel-2-mehrere-jahre)
   - [Beispiel 3: Szenarienvergleich](#beispiel-3-szenarienvergleich)
10. [Fehlerbehebung](#fehlerbehebung)
    - [Problem 1: Datenbankverbindung schlägt fehl](#problem-1-datenbankverbindung-schlägt-fehl)
    - [Problem 2: Datei nicht gefunden](#problem-2-datei-nicht-gefunden)
    - [Problem 3: Falsche Parameter-ID](#problem-3-falsche-parameter-id)
    - [Problem 4: Speicherprobleme](#problem-4-speicherprobleme)
    - [Problem 5: Falsche IDrun](#problem-5-falsche-idrun)
11. [Tipps und Tricks](#tipps-und-tricks)

---

## Erste Schritte - Kompletter Workflow

### Vorbereitung: Was brauchen Sie?

Bevor Sie starten, brauchen Sie:

1. ✅ **MySQL-Datenbank** läuft und ist eingerichtet
2. ✅ **Module kompiliert** (siehe Installation)
3. ✅ **Input-Daten** vorhanden (WaterGAP-Daten, etc.)
4. ✅ **Projekt in Datenbank** angelegt
5. ✅ **Szenario in Datenbank** angelegt

### Schritt-für-Schritt: Ihre erste Simulation

#### Schritt 1: Projekt und Szenario prüfen

**Was ist das?** Jede Simulation gehört zu einem Projekt (z.B. "WWQA") und einem Szenario (z.B. "Baseline 2010").

**Wie prüfen Sie das?**

```sql
-- In MySQL einloggen
mysql -u username -p

-- Projekte anzeigen
USE wq_general;
SELECT * FROM _project;
-- Merken Sie sich die project_id (z.B. 2 für WWQA)

-- Szenarien anzeigen
SELECT * FROM _szenario;
-- Merken Sie sich die IDScen (z.B. 91)
```

**Was tun wenn Projekt oder Szenario fehlen?**

Falls die Abfrage keine Ergebnisse liefert, müssen Sie Projekt/Szenario anlegen:

**Projekt anlegen:**
```sql
USE wq_general;
INSERT INTO _project (project_id, Project_Name, description) 
VALUES (2, 'WWQA', 'World Water Quality Assessment');
-- Prüfen
SELECT * FROM _project WHERE project_id=2;
```

**Szenario anlegen:**
```sql
USE wq_general;
INSERT INTO _szenario (IDScen, ScenName, description, project_id) 
VALUES (91, 'Baseline 2010', 'Baseline-Szenario für 2010', 2);
-- Prüfen
SELECT * FROM _szenario WHERE IDScen=91;
```

**Wichtig:** Die genauen Spaltennamen können variieren. Prüfen Sie zuerst die Tabellenstruktur:
```sql
DESCRIBE _project;
DESCRIBE _szenario;
```

#### Schritt 2: Parameter wählen

**Was ist das?** Welchen Schadstoff wollen Sie simulieren?

**Parameter-IDs:**
- 0 = BOD (Biochemischer Sauerstoffbedarf)
- 1 = Salz (TDS)
- 2 = FC (Fäkalcoliforme Bakterien)
- 3 = TN (Gesamtstickstoff)
- 4 = TP (Gesamtphosphor)

**Beispiel:** Sie wollen Fäkalcoliforme simulieren → Parameter-ID = 2

#### Schritt 3: Einträge berechnen

**Was passiert hier?** Das Programm berechnet, wie viele Schadstoffe in die Flüsse gelangen.

**Konkret:**

```bash
# 1. In das Modul-Verzeichnis wechseln
cd fill_worldqual_load

# 2. OPTIONS.DAT öffnen und anpassen

**Wie öffnet man die Datei?**

**Option A: Mit Texteditor (empfohlen für Anfänger)**
```bash
# Mit nano (einfacher Editor)
nano OPTIONS.DAT

# Navigation:
# - Pfeiltasten: Bewegen
# - Strg+O: Speichern (dann Enter)
# - Strg+X: Beenden
```

**Option B: Mit vim (für Erfahrene)**
```bash
vim OPTIONS.DAT
# Drücken Sie 'i' zum Einfügen
# Ändern Sie die Werte
# Drücken Sie ESC, dann ':wq' zum Speichern und Beenden
```

**Option C: Mit grafischem Editor**
```bash
# macOS
open -a TextEdit OPTIONS.DAT

# Linux (GNOME)
gedit OPTIONS.DAT

# Oder jeder andere Texteditor
```

**Was genau ändern?**

Die OPTIONS.DAT sieht etwa so aus:
```
Runtime options fill_worldqual_load

1. project_id, Tabelle wq_general._project
  1: CESR
  2: WWQA
  3: CESR Sensitivity Analysis
  4: test
Value: 2

2. IDScen, wq_general._szenario
Value: 91

3. IDVersion
  2: WaterGAP2
  3: WaterGAP3
Value: 3

4. IDReg
  1 WaterGAP3 eu
  2 WaterGAP3 af
  ...
Value: 2

5. MyHost
Value: localhost

6. MyUser
Value: worldqual

7. MyPassword
Value: ihr_passwort_hier

8. InputType
  0: Daten aus der Datenbank einlesen
  1: Daten aus UNF-Dateien einlesen
Value: 1

9. input_dir
Value: /pfad/zu/ihren/daten
```

**Ändern Sie die Werte nach "Value:":**

- `Value: 2` → Ihre Projekt-ID (z.B. 2 für WWQA)
- `Value: 91` → Ihre Szenario-ID (z.B. 91)
- `Value: 3` → WaterGAP Version (2 oder 3)
- `Value: 2` → Region (1=EU, 2=AF, 3=AS, 4=AU, 5=NA, 6=SA)
- `Value: localhost` → MySQL-Server (localhost oder IP-Adresse)
- `Value: worldqual` → Ihr MySQL-Benutzername
- `Value: ihr_passwort_hier` → Ihr MySQL-Passwort
- `Value: 1` → InputType (0=Datenbank, 1=UNF-Dateien)
- `Value: /pfad/zu/ihren/daten` → Vollständiger Pfad zu WaterGAP-Daten

**Wichtig:** 
- Ändern Sie NUR die Werte nach "Value:"
- Lassen Sie den Rest der Datei unverändert
- Speichern Sie die Datei nach den Änderungen

# 3. Programm starten
./fill_worldqual_load 2010 2010
# Berechnet Einträge für Jahr 2010
```

**Was sehen Sie?**

```
Program fill_worldqual_load started at: 2026-02-02 10:00:00

project_id = 2 (WWQA)
Database: wwqa_wq_load_af connection ok...

Berechne Einträge für Parameter 2 (FC)...
Jahr 2010...
  Monat 1... 2... 3... ... 12
Fertig!

Program fill_worldqual_load ended at: 2026-02-02 10:15:00
```

**Wie lange dauert das?**
- Ein Jahr, kleine Region: 5-15 Minuten
- Mehrere Jahre, große Region: Stunden

**Was ist passiert?**
- Das Programm hat für jede Grid-Zelle (ca. 0.5° x 0.5°) berechnet:
  - Wie viele Menschen leben dort? → Abwassermengen
  - Wie viel Abwasser wird behandelt? → Reduktion
  - Landnutzung? → Diffuse Einträge
  - Geologie? → Natürliche Quellen
- Ergebnisse wurden in Datenbank gespeichert: `wwqa_wq_load_af.calc_cell_month_load_*`

#### Schritt 4: Wassertemperatur berechnen (optional)

**Wann brauchen Sie das?** Wenn Temperatur für Abbauprozesse wichtig ist (z.B. bei Bakterien).

**Konkret:**

```bash
cd ../water_temperature

# OPTIONS.DAT anpassen (ähnlich wie Schritt 3)
# Zusätzlich: IDTemp = 100 (aus Datenbank holen)

./water_temperature 2010 2010
```

**Was passiert?** Das Programm berechnet aus Lufttemperatur die Wassertemperatur (mit nichtlinearer Kurvenanpassung).

#### Schritt 5: Hauptsimulation

**Was passiert hier?** Das Programm simuliert, wie sich die Schadstoffe im Fluss verhalten.

**Konkret:**

```bash
cd ../worldqual

# 1. IDrun aus Datenbank holen

**Was ist eine IDrun?**
Die IDrun ist eine eindeutige Nummer für eine Simulation. Sie identifiziert:
- Welchen Parameter (z.B. FC = 2)
- Welches Szenario (z.B. 91)
- Weitere Konfigurationen (Temperatur, etc.)

**Wie finden Sie die IDrun?**

```sql
# In MySQL einloggen
mysql -u worldqual -p
# Passwort eingeben

# Datenbank wählen
USE wq_general;

# IDrun suchen
SELECT IDrun, runName, parameter_id, IDScen FROM _runlist 
WHERE parameter_id=2 AND IDScen=91;

# Sollte Ergebnis zeigen, z.B.:
# +-----------+------------------+-------------+--------+
# | IDrun     | runName          | parameter_id| IDScen |
# +-----------+------------------+-------------+--------+
# | 403100091 | FC Baseline 2010 |           2 |     91 |
# +-----------+------------------+-------------+--------+
```

**Was wenn keine IDrun existiert?**

Falls die Abfrage keine Ergebnisse liefert, müssen Sie eine IDrun anlegen. Dies ist komplexer und erfordert mehrere Schritte:

**Schritt 1: Prüfen welche Spalten die Tabelle hat**
```sql
DESCRIBE _runlist;
-- Zeigt alle Spalten und deren Typen
```

**Schritt 2: IDrun anlegen (Beispiel)**
```sql
INSERT INTO _runlist (
    IDrun, 
    runName, 
    parameter_id, 
    IDScen, 
    IDTemp,
    project_id,
    -- weitere benötigte Spalten je nach Schema
) VALUES (
    403100091,
    'FC Baseline 2010',
    2,      -- Parameter-ID für FC
    91,     -- Szenario-ID
    -9999,  -- IDTemp (oder konkrete Nummer)
    2       -- project_id
);
```

**Wichtig:** Die genaue Struktur hängt von Ihrer Datenbank-Version ab. Fragen Sie einen Kollegen oder prüfen Sie die Tabellenstruktur genau!

**Alternative:** Nutzen Sie `create_scenarios` Modul, um Runs automatisch zu erstellen (siehe Modul-Dokumentation).

# 2. OPTIONS.DAT anpassen (wie Schritt 3)

# 3. Simulation starten
./worldqual 403100091 2010 2010
```

**Was sehen Sie?**

```
Program worldqual started at: 2026-02-02 10:30:00

project_id = 2 (WWQA)
IDrun = 403100091
Parameter: FC (Fäkalcoliforme)
Region: Afrika
Database: wwqa_worldqual_af connection ok...

Lese Einträge...
Lese hydrologische Daten...
Simuliere Jahr 2010...
  Monat 1... 2... 3... ... 12
Fertig!

Program worldqual ended at: 2026-02-02 14:30:00
```

**Wie lange dauert das?**
- Ein Jahr, kleine Region: 1-4 Stunden
- Mehrere Jahre, große Region: Tage

**Was ist passiert?**
- Für jede Grid-Zelle, jeden Monat:
  1. Einträge wurden gelesen (aus Schritt 3)
  2. Transport simuliert (Schadstoffe fließen flussabwärts)
  3. Verdünnung berechnet (durch Zuflüsse)
  4. Abbau simuliert (Bakterien sterben ab)
  5. Ergebnis: Konzentration wurde berechnet
- Ergebnisse wurden gespeichert: `wwqa_worldqual_af.concentration`

#### Schritt 6: Ergebnisse prüfen

**Wie sehen Sie die Ergebnisse?**

```sql
-- In MySQL
USE wwqa_worldqual_af;

-- Konzentrationen für ein Jahr anzeigen
SELECT cell, month, concentration 
FROM concentration 
WHERE IDrun=403100091 AND year=2010 
LIMIT 10;

-- Durchschnittliche Konzentration pro Monat
SELECT month, AVG(concentration) as avg_conc
FROM concentration 
WHERE IDrun=403100091 AND year=2010
GROUP BY month;
```

#### Schritt 7: Statistik (Vergleich mit Messungen)

**Was passiert hier?** Sie vergleichen Ihre Simulation mit realen Messungen.

**Konkret:**

```bash
cd ../wq_stat_stations

# STAT.DAT anpassen falls nötig

./wq_stat_stations 403100091 2010 2010
```

**Ergebnis:** Textdateien mit statistischen Kennzahlen (R², RMSE, etc.)

#### Schritt 8: Visualisierung

**Wie erstellen Sie Diagramme?**

**Voraussetzung: R installieren**

**R installieren:**

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get update
sudo apt-get install r-base
```

**macOS:**
```bash
brew install r
```

**Windows:**
- Download von https://cran.r-project.org/
- Installer ausführen

**R-Pakete installieren:**
```r
# R öffnen (im Terminal: R)
install.packages("ggplot2")
install.packages("gridExtra")
# Falls nach CRAN-Mirror gefragt wird, wählen Sie einen aus (z.B. Deutschland)
```

**R-Skripte nutzen:**

**Schritt 1: R öffnen**
```bash
# Im Terminal
cd /pfad/zu/WorldQual
R
# R startet, Sie sehen jetzt ">"
```

**Schritt 2: In das richtige Verzeichnis wechseln**
```r
# In R
setwd("R-scripte")
# Prüfen ob es funktioniert hat:
getwd()
# Sollte zeigen: .../WorldQual/R-scripte
```

**Schritt 3: start.R anpassen**

**Option A: In R öffnen und ändern**
```r
# Datei öffnen
file.edit("start.R")
# Editor öffnet sich, ändern Sie die Variablen
```

**Option B: Mit Texteditor öffnen**
```bash
# In neuem Terminal (R läuft weiter)
nano R-scripte/start.R
# Oder
vim R-scripte/start.R
```

**Wichtige Variablen in start.R anpassen:**

```r
# Verzeichnis mit R-Skripten
r_script_dir <- "/vollständiger/pfad/zu/WorldQual/R-scripte"

# Verzeichnis für Ausgabe (Grafiken)
current_dir <- "/pfad/wo/grafiken/gespeichert/werden"

# Pfad zu Statistik-Dateien (Output von wq_stat_stations)
input.path <- "/pfad/zu/statistik/dateien"
input.filename <- ""  # leer = alle Dateien, oder spezifischer Dateiname

# Einheit für Konzentration
unit <- "cfu/100ml"   # Für FC, andere: "mg/l", "mg/m³", etc.

# Logarithmische Skalierung
log_scale <- "y"      # "y" = y-Achse logarithmisch, "" = linear

# Weitere Optionen je nach Skript
```

**Schritt 4: Skripte ausführen**
```r
# In R (nachdem start.R angepasst wurde)
source("start.R")
# Lädt die Konfiguration

# Dann spezifische Skripte ausführen:
source("station.R")              # Diagramme für einzelne Stationen
source("stations_scatterplot.R") # Streudiagramme (Simulation vs. Messung)
source("RiverSection.R")        # Flussabschnitte
```

**Wo werden die Grafiken gespeichert?**
- In dem Verzeichnis, das Sie in `current_dir` angegeben haben
- Normalerweise als PNG-Dateien (z.B. `station_plot.png`)

**R beenden:**
```r
# In R
q()
# Wird gefragt: "Save workspace image?" → 'n' für Nein
```

---

## Module im Detail

### fill_worldqual_load - Einträge berechnen

**Was macht es genau?**

Dieses Modul berechnet die **Einträge** von Schadstoffen in die Flüsse. Es berücksichtigt:

1. **Punktuelle Quellen** (Abwasserbehandlungsanlagen)
   - Bevölkerungsdichte → Wie viele Menschen?
   - Abwasserbehandlung → Wie viel wird gereinigt?
   - Effizienz → Wie gut ist die Reinigung?

2. **Diffuse Quellen** (Landwirtschaft, städtische Gebiete)
   - Landnutzung → Ackerland, Weiden, Städte
   - Dünger → Stickstoff, Phosphor
   - Erosion → Sedimente

3. **Geogene Quellen** (natürlich)
   - Geologie → Natürliche Salze, etc.

4. **Atmosphärische Deposition**
   - Regen → Nährstoffe aus der Luft

**Input-Daten die benötigt werden:**

- Bevölkerungsdichte (Grid-Daten)
- Abwasserbehandlungsraten (Länderdaten)
- Landnutzung (Grid-Daten)
- Düngeranwendung (Länderdaten)
- Geologische Karten

**Output:**

- Datenbanktabellen: `calc_cell_month_load_*`
- Eine Tabelle pro Eintragsquelle
- Monatliche Werte pro Grid-Zelle

**Wichtige Parameter in OPTIONS.DAT:**

```
project_id = 2          # Ihr Projekt
IDScen = 91            # Ihr Szenario
IDReg = 2              # Region (1=EU, 2=AF, etc.)
InputType = 1          # 1 = UNF-Dateien
input_dir = /pfad/zu/daten
```

**Tipp:** Detaillierte Erklärung aller Optionen finden Sie weiter unten in [Konfiguration verstehen](#konfiguration-verstehen)

### worldqual - Hauptsimulation

**Was macht es genau?**

Dieses Modul simuliert die **Wasserqualität im Fluss**. Es berechnet:

1. **Transport**
   - Schadstoffe werden flussabwärts transportiert
   - Routing-Informationen aus WaterGAP werden genutzt

2. **Verdünnung**
   - Durch Zuflüsse wird Wasser verdünnt
   - Abfluss (Q_out) bestimmt Verdünnung

3. **Abbau**
   - **Bakterien** (FC): Sterben ab (abhängig von Temperatur, Zeit)
   - **Nährstoffe** (TN, TP): Werden abgebaut (abhängig von Temperatur)
   - **Konservative Stoffe** (Salz): Werden NICHT abgebaut

4. **Temperatur-Einfluss**
   - Höhere Temperatur → Schnellerer Abbau
   - Wassertemperatur wird verwendet (aus water_temperature Modul)

**Input-Daten:**

- Einträge (aus fill_worldqual_load)
- Abfluss (Q_out) - monatlich
- Runoff - monatlich
- Wassertemperatur (optional) - monatlich
- Fließgeschwindigkeit (optional)
- Abbau-Raten (aus Datenbank)

**Output:**

- Konzentrationen pro Grid-Zelle, pro Monat
- Gespeichert in: `concentration` Tabelle

**Wichtige Parameter in OPTIONS.DAT:**

```
project_id = 2
IDScen = 91
IDrun = 403100091      # Aus Datenbank holen!
IDReg = 2
InputType = 1
input_dir = /pfad/zu/daten
```

**Wie finde ich die IDrun?**

**Schnellreferenz:** Siehe auch [QUICK_REFERENCE.md - IDrun finden](schnellreferenz.md#idrun-finden)

Die IDrun ist eine eindeutige Nummer für eine Simulation. Sie setzt sich zusammen aus:
- Parameter-ID (z.B. 2 für FC)
- Szenario-ID (z.B. 91)
- Weitere Konfigurationen

```sql
SELECT IDrun, runName, parameter_id, IDScen 
FROM wq_general._runlist 
WHERE parameter_id=2 AND IDScen=91;
```

### wq_stat_stations - Statistische Analyse

**Was macht es genau?**

Dieses Modul vergleicht Ihre **simulierten Werte** mit **gemessenen Werten** an Stationen.

**Wie funktioniert es?**

1. Findet Messstationen in der Datenbank
2. Liest gemessene Werte für diese Stationen
3. Liest simulierte Werte für die gleichen Orte
4. Berechnet statistische Kennzahlen:
   - **R²** (Bestimmtheitsmaß): Wie gut passt die Simulation? (0-1, höher = besser)
   - **RMSE** (Root Mean Square Error): Durchschnittlicher Fehler
   - **Bias**: Systematischer Fehler (zu hoch/zu niedrig?)
   - **Nash-Sutcliffe**: Weitere Güte-Maßzahl

**Input:**

- IDrun (aus Simulation)
- Start- und Endjahr
- Messstationen in Datenbank

**Output:**

- Textdateien mit statistischen Kennzahlen
- Vergleichsdaten für Visualisierung

**Konfiguration:**

Die Datei `STAT.DAT` enthält zusätzliche Einstellungen (optional).

### water_temperature - Wassertemperatur

**Was macht es genau?**

Berechnet die **Wassertemperatur** aus der **Lufttemperatur** mit einer nichtlinearen Funktion.

**Warum wichtig?**

- Temperatur beeinflusst Abbauprozesse
- Bakterien sterben schneller bei höherer Temperatur
- Nährstoffe werden schneller abgebaut

**Wie funktioniert es?**

Verwendet eine **logistische Kurve**:
```
Wassertemperatur = C0 / (1 + exp(C1 * Lufttemperatur + C2))
```

Die Konstanten (C0, C1, C2) werden in der Datenbank gespeichert (Tabelle `_water_temperature_list`).

**Input:**

- Lufttemperatur (UNF-Dateien oder Datenbank)
- IDTemp (welche Temperatur-Kurve verwenden?)

**Output:**

- Wassertemperatur pro Grid-Zelle, pro Monat
- Gespeichert in Datenbank oder UNF-Dateien

---

## Konfiguration verstehen

### OPTIONS.DAT - Die wichtigste Datei

**Schnellreferenz:** Siehe auch [QUICK_REFERENCE.md - OPTIONS.DAT](schnellreferenz.md#optionsdat---wichtige-einstellungen)

Jedes Modul hat eine `OPTIONS.DAT` Datei. Diese Datei enthält **alle Einstellungen** für die Simulation. Die Datei wird sequentiell gelesen – jede `Value:`-Zeile wird der nächsten Variablen zugeordnet. Jedes Modul hat eigene Optionen, gefolgt von den allgemeinen Optionen (IDVersion, IDReg, DB-Credentials, InputType).

### Format der Datei

```
Runtime options [modulname]

1. [Option Name]
   [Beschreibung der Option]
   [Mögliche Werte]
Value: [Aktueller Wert]

2. [Nächste Option]
...
```

### Wichtige Optionen erklärt

#### project_id

**Was ist das?** Die Nummer Ihres Projekts.

**Wo finde ich sie?**
```sql
SELECT project_id, Project_Name FROM wq_general._project;
```

**Beispiele:**
- 1 = CESR
- 2 = WWQA
- 3 = CESR Sensitivity Analysis
- 4 = test

#### IDScen

**Was ist das?** Die Nummer Ihres Szenarios.

**Wo finde ich sie?**
```sql
SELECT IDScen, ScenName FROM wq_general._szenario;
```

**Beispiele:**
- 91 = Baseline 2010
- 92 = Szenario A
- 93 = Szenario B

#### IDVersion

**Was ist das?** Welche WaterGAP-Version verwenden Sie?

- 2 = WaterGAP2
- 3 = WaterGAP3 (empfohlen)

#### IDReg

**Was ist das?** Welche Region simulieren Sie?

- 1 = Europa (eu)
- 2 = Afrika (af)
- 3 = Asien (as)
- 4 = Australien (au)
- 5 = Nordamerika (na)
- 6 = Südamerika (sa)

**Wichtig:** Die Region bestimmt, welche Datenbank verwendet wird:
- `wwqa_worldqual_eu` für Europa
- `wwqa_worldqual_af` für Afrika
- etc.

#### MyHost, MyUser, MyPassword

**Was ist das?** Verbindungsdaten für MySQL.

**Beispiele:**
```
MyHost = localhost        # Lokaler Server
MyHost = REMOVED_HOST    # Remote-Server
MyUser = worldqual
MyPassword = mein_passwort
```

#### InputType

**Was ist das?** Woher kommen die Input-Daten?

- 0 = Aus der Datenbank lesen
- 1 = Aus UNF-Dateien lesen (empfohlen)

**Wann welche Option?**
- **0 (Datenbank)**: Wenn Daten bereits in Datenbank sind
- **1 (UNF-Dateien)**: Wenn Sie WaterGAP-Dateien haben

#### input_dir

**Was ist das?** Pfad zu den UNF-Dateien (nur bei InputType=1).

**Beispiel:**
```
input_dir = /home/user/watergap_data/africa
```

**Wichtig:** Der Pfad muss existieren und lesbar sein!

### wq_load OPTIONS.DAT – Alle Optionen im Detail

Das `wq_load`-Modul berechnet Schadstoffeinträge in Flüsse. Es hat die umfangreichste OPTIONS.DAT aller Module, da es sowohl eigene Optionen (1–17) als auch die allgemeinen Optionen der Elternklasse (18–24) enthält.

#### Aufbau: Eigene Optionen (1–17) + Eltern-Optionen (18–24)

Der Parser (`options_wq_load.cpp`) liest zuerst die wq_load-spezifischen Optionen 1–17, dann übergibt er an die Basisklasse `optionsClass`, die ab Option 18 die allgemeinen Parameter (IDVersion, IDReg, DB-Credentials, InputType) liest.

!!! warning "Reihenfolge ist kritisch"
    Die Optionen werden **streng sequentiell** gelesen – jede `Value:`-Zeile wird der nächsten Variablen zugeordnet. Eine fehlende oder zusätzliche Option verschiebt **alle nachfolgenden** Zuordnungen.

#### Option 1: parameter_id

Bestimmt welchen Schadstoff-Parameter das Modul berechnet. Der Wert wird über die Datenbanktabelle `wq_general.wq_parameter` nachgeschlagen und intern in die Variable `toCalc` umgewandelt, die als zentrale Verzweigungsvariable den gesamten Berechnungsablauf steuert.

| parameter_id | Parameter | toCalc | Beschreibung |
|:---|:---|:---:|:---|
| 80 / 110 | BOD | 0 | Biologischer Sauerstoffbedarf |
| 210 / 260 | TDS | 1 | Salz (Total Dissolved Solids) |
| 310 | FC | 2 | Fäkale Coliforme |
| 410 | TN | 3 | Gesamtstickstoff |
| 510 | TP | 4 | Gesamtphosphor |
| 390 / 610 | Pestizide | 5 | Pestizide |

```
1. parameter_id aus der Tabelle wq_general.wq_parameter.
Value: 260
```

!!! info "parameter_id vs. toCalc"
    Im Code werden **beide** Variablen genutzt: `parameter_id` in SQL-Abfragen (z.B. `WHERE parameter_id=260`), `toCalc` für die Programmlogik (z.B. `if(toCalc==1)` für TDS-spezifische Berechnungen). Die `parameter_id` kann mehrere Werte pro Parametertyp haben (z.B. 210 und 260 für TDS), während `toCalc` immer eindeutig ist.

#### Option 2: following_year

Nur relevant für **Pestizide** (`toCalc==5`). Steuert ob die Dezember-Belastung des Vorjahres als Anfangsbedingung geladen wird.

| Wert | Bedeutung |
|:---:|:---|
| 0 | Erstes Berechnungsjahr (Standard) |
| 1 | Folgejahr – lädt Vorjahres-Belastung |

```
2. following_year only for pesticide;
0: first year to calculate; (default)
1: following year. there are a loading from december last year
Value: 0
```

#### Option 3: project_id

Projekt-Nummer aus `wq_general._project`. Bestimmt welche **Datenbank** verwendet wird.

```sql
SELECT project_id, Project_Name, database_name FROM wq_general._project;
```

Im Code wird daraus der Datenbankname abgeleitet: `project_id` → DB-Lookup → `MyDatabase` → Prefix in allen SQL-Queries (z.B. `wwqa_wq_load_au.cell_input`).

```
3. project_id, Tabelle wq_general._project
Value: 10
```

#### Option 4: IDScen

Szenario-ID aus `wq_general._szenario`. Der **am häufigsten verwendete Parameter** im gesamten Modul. Wird in ~20 SQL-Queries als Filter eingesetzt und an alle Hilfsklassen (COUNTRY_CLASS, LS_CLASS) weitergegeben.

```sql
SELECT IDScen, ScenName FROM wq_general._szenario WHERE project_id=10;
```

```
4. IDScen, wq_general._szenario
Value: 24
```

#### Option 5: manure_timing

Steuert die zeitliche Verteilung der Dung-Ausbringung auf Felder.

| Wert | Bedeutung | Effekt im Code |
|:---:|:---|:---|
| 0 | Ganzjährige Ausbringung | Dung wird über Regentage des ganzen Jahres verteilt |
| 1 | März bis Oktober | Nur Monate 2–9 bekommen Belastung, Rest = 0 |

```
5. manure timing
0: all year round application
1: application between March and October
Value: 0
```

#### Option 6: climate (Klimainput-Auflösung)

Bestimmt das Format der Klimadaten und welches Grid-Mapping verwendet wird.

| Wert | Bedeutung | Grid-Mapping |
|:---:|:---|:---|
| 0 | 0.5° Auflösung | `G_WG3_WG2WITH5MIN.UNF4` |
| 1 | 5 Minuten Auflösung | Kein Mapping (direkte Einlesung) |
| 2 | 0.5° CRU/WATCH | `G_WG3_WATCH.UNF4` (anderes Mapping!) |

```
6. Klimainput
0: in 0.5°
1: in 5 min
2: in 0.5° CRU/WATCH
Value: 2
```

!!! info "Unterschied climate=0 vs. climate=2"
    Beide nutzen 0.5°-Daten, aber mit **unterschiedlichen Grid-Mappings**: `climate=0` nutzt das Standard-WG2-zu-WG3-Mapping, `climate=2` nutzt ein spezielles WATCH-Mapping mit einer anderen Grid-Größe (67.420 Zellen).

#### Option 7: gridded_pop_from_file

Bestimmt die Quelle der räumlich verteilten Bevölkerungsdaten.

| Wert | Bedeutung | Datenquelle |
|:---:|:---|:---|
| 0 | Aus Datenbank | Tabelle `wq_load_[continent].cell_input` |
| 1 | Aus UNF-Dateien | `GURBPOP[year].UNF0` und `GRURPOP[year].UNF0` |

```
7. gridded_pop_from_file
0: nein, aus der Tabelle wq_load_[continent].cell_input (default)
1: ja, aus UNF-Datei (z.B. griddi-Output)
Value: 1
```

#### Option 8: gridded_pop_path

Pfad zu den Bevölkerungsdateien. Nur relevant wenn `gridded_pop_from_file=1`.

Erwartet folgende Dateien im Verzeichnis:

- `GURBPOP[year].UNF0` – Urbane Bevölkerung
- `GRURPOP[year].UNF0` – Rurale Bevölkerung

```
8.gridded_pop_path (nur für gridded_pop_from_file==1)
Value: /pfad/zu/griddi/OUTPUT/SSP2/au
```

#### Optionen 9–16: Pfade zu Input-Dateien

Alle Pfade werden direkt per `sprintf()` mit Dateinamen und Jahreszahl kombiniert. Nicht alle Pfade werden für jeden Parameter benötigt.

| Option | Variable | Benötigte Dateien | Verwendet für |
|:---:|:---|:---|:---|
| 9 | `path_watergap_output` | `G_URBAN_RUNOFF_[year].12.UNF0`, `G_SURFACE_RUNOFF_[year].12.UNF0`, `G_GW_RUNOFF_[year].12.UNF0` | Alle Parameter |
| 10 | `path_livestock_output` | `G_LIVESTOCK_NR_[year].12.UNF0` | Alle (Viehbestand) |
| 11 | `path_corr_factor` | `G_CORR_FACT_RTF_[year].12.UNF0` | Alle (Korrekturfaktoren) |
| 12 | `path_climate` | `GTEMP_[year].12.UNF2`, `GPREC_[year].12.UNF2` | Alle (Temperatur), Pestizide (Niederschlag) |
| 13 | `path_gnrd` | `GNRD_[year].12.UNF1` | Alle (Regentage) |
| 14 | `path_tp_input` | `G_SOILEROS.UNF0`, `G_PWEATHERING.UNF0`, `GLCC[year].UNF2` | Nur TN/TP |
| 15 | `path_tp_input2` | `CROPLAND_CORR_KM2_[year].UNF0`, `P_RATE_TON_KM2_[year].UNF0` | Nur TN/TP |
| 16 | `path_tp_input3` | `G_PATMDEPOS_[year].UNF0` | Nur TP |

```
9.path_watergap_output
Value: /pfad/zu/watergap/hydro/OUTPUT/SSP2/au/GFDL-ESM2M/rcp6p0

10.path_livestock_output
Value: /pfad/zu/livestock/Livestock_produced/SSP2_IMAGE/au

11.path_corr_factor
Value: /pfad/zu/corr_factor/OUTPUT/SSP2/au/GFDL-ESM2M/rcp6p0

12.path_climate
Value: /pfad/zu/climate/ISIMIP2b/UNF_watch/GFDL-ESM2M/rcp6p0

13.path_gnrd
Value: /pfad/zu/climate/ISIMIP2b/UNF_watch/GFDL-ESM2M/rcp6p0

14.path_tp_input
Value: /pfad/zu/tp_input/au

15.path_tp_input2
Value: /pfad/zu/tp_input/au/SSP2

16.path_tp_input3
Value: /pfad/zu/tp_input/au/SSP2
```

#### Option 17: IDInTableName

Steuert ob IDScen und parameter_id als Spalten (Standard) oder als Teil des Tabellennamens verwendet werden.

| Wert | Tabellenzugriff |
|:---:|:---|
| 0 | `cell_input WHERE IDScen=24` (Standard) |
| 1 | `cell_input_24` (IDScen im Tabellennamen) |

```
17. IDScen und parameterID in table name (cell_input, cell_) default: 0
0: cell_input, cell_input_rtf_irr, wateruse mit der Spalte IDScen
1: Wert IDScen in dem Namen der Tabellen: cell_input_24, cell_input_rtf_irr_24, wateruse_24
Value: 0
```

#### Optionen 18–24: Allgemeine Optionen (Elternklasse)

Ab Option 18 folgen die Optionen der Basisklasse `optionsClass`, die alle Module gemeinsam haben.

| Option | Variable | Werte | Beispiel |
|:---:|:---|:---|:---|
| 18 | `IDVersion` | 2 = WaterGAP2, 3 = WaterGAP3 | `3` |
| 19 | `IDReg` | 1=eu, 2=af, 3=as, 4=au, 5=na, 6=sa | `4` |
| 20 | `MyHost` | MySQL-Host (IP oder Hostname) | `<YOUR_MYSQL_HOST>` |
| 21 | `MyUser` | MySQL-Benutzername | `<YOUR_MYSQL_USER>` |
| 22 | `MyPassword` | MySQL-Passwort | `<YOUR_MYSQL_PASSWORD>` |
| 23 | `InputType` | 0 = Datenbank, 1 = UNF-Dateien | `1` |
| 24 | `input_dir` | Pfad zu Land-Area-Daten | `/pfad/zu/UNF_input/au` |

`IDVersion` und `IDReg` werden gemeinsam zu `continent_abb` kombiniert (z.B. IDVersion=3 + IDReg=4 → `"au"`), das dann als Suffix für Datenbanknamen dient: `wq_load_au`.

### Beispiel: Komplette wq_load OPTIONS.DAT (TDS, Australien, SSP2)

```
Runtime options worldqual

1. parameter_id aus der Tabelle wq_general.wq_parameter.
Value: 260

2. following_year only for pesticide;
0: first year to calculate; (default)
1: following year. there are a loading from december last year
Value: 0

3. project_id, Tabelle wq_general._project
Value: 10

4. IDScen, wq_general._szenario
Value: 24

5. manure timing
0: all year round application
1: application between March and October
Value: 0

6. Klimainput
0: in 0.5°
1: in 5 min
2: in 0.5° CRU/WATCH
Value: 2

7. gridded_pop_from_file
0: nein, aus der Tabelle wq_load_[continent].cell_input (default)
1: ja, aus UNF-Datei (z.B. griddi-Output)
Value: 1

8.gridded_pop_path (nur für gridded_pop_from_file==1)
Value: /pfad/zu/griddi/OUTPUT/SSP2/au

9.path_watergap_output
Value: /pfad/zu/watergap/hydro/OUTPUT/SSP2/au/GFDL-ESM2M/rcp6p0

10.path_livestock_output
Value: /pfad/zu/livestock/Livestock_produced/SSP2_IMAGE/au

11.path_corr_factor
Value: /pfad/zu/corr_factor/OUTPUT/SSP2/au/GFDL-ESM2M/rcp6p0

12.path_climate
Value: /pfad/zu/climate/ISIMIP2b/UNF_watch/GFDL-ESM2M/rcp6p0

13.path_gnrd
Value: /pfad/zu/climate/ISIMIP2b/UNF_watch/GFDL-ESM2M/rcp6p0

14.path_tp_input
Value: /pfad/zu/tp_input/au

15.path_tp_input2
Value: /pfad/zu/tp_input/au/SSP2

16.path_tp_input3
Value: /pfad/zu/tp_input/au/SSP2

17. IDScen und parameterID in table name (cell_input, cell_) default: 0
0: cell_input, cell_input_rtf_irr, wateruse mit der Spalte IDScen
1: Wert IDScen in dem Namen der Tabellen: cell_input_24, cell_input_rtf_irr_24, wateruse_24
Value: 0

18. IDVersion
2: WaterGAP2
3: WaterGAP3
Value: 3

19. IDReg
1 WaterGAP3 eu
2 WaterGAP3 af
3 WaterGAP3 as
4 WaterGAP3 au
5 WaterGAP3 na
6 WaterGAP3 sa
Value: 4

20. MyHost
Value: <YOUR_MYSQL_HOST>

21. MyUser
Value: <YOUR_MYSQL_USER>

22. MyPassword
Value: <YOUR_MYSQL_PASSWORD>

23. InputType
0: Daten aus der Datenbank einlesen
1: Daten aus UNF-Dateien
Value: 1

24. Path to Land Area data
Value: /pfad/zu/UNF_input/au
```

---

## Datenbank verstehen

### Übersicht über die Datenbanken

**Schnellreferenz:** Siehe auch [QUICK_REFERENCE.md - Datenbank-Abfragen](schnellreferenz.md#datenbank-abfragen)

WorldQual verwendet **mehrere Datenbanken**:

1. **wq_general**: Allgemeine Konfiguration (Projekte, Szenarien, etc.)
2. **wwqa_worldqual_[region]**: Simulationsergebnisse (Konzentrationen)
3. **wwqa_wq_load_[region]**: Eintragsdaten

### wq_general - Konfigurationsdatenbank

**Wichtige Tabellen:**

#### _project
Enthält alle Projekte.

```sql
SELECT * FROM wq_general._project;
```

**Spalten:**
- `project_id`: Eindeutige Nummer
- `Project_Name`: Name (z.B. "WWQA")
- `description`: Beschreibung

#### _szenario
Enthält alle Szenarien.

```sql
SELECT * FROM wq_general._szenario;
```

**Spalten:**
- `IDScen`: Eindeutige Nummer
- `ScenName`: Name (z.B. "Baseline 2010")
- `description`: Beschreibung

#### _parameter
Enthält alle Wasserqualitätsparameter.

```sql
SELECT * FROM wq_general._parameter;
```

**Spalten:**
- `parameter_id`: 0=BOD, 1=Salz, 2=FC, 3=TN, 4=TP
- `ParameterName`: Name (z.B. "FC")

#### _runlist
Enthält alle Run-Konfigurationen.

```sql
SELECT * FROM wq_general._runlist 
WHERE parameter_id=2 AND IDScen=91;
```

**Spalten:**
- `IDrun`: Eindeutige Nummer (wichtig!)
- `runName`: Name
- `parameter_id`: Parameter
- `IDScen`: Szenario
- `IDTemp`: Wassertemperatur-Konfiguration
- Weitere Einstellungen

### wwqa_worldqual_[region] - Ergebnisse

**Wichtige Tabellen:**

#### concentration
Enthält die simulierten Konzentrationen.

```sql
SELECT * FROM wwqa_worldqual_af.concentration 
WHERE IDrun=403100091 AND year=2010 
LIMIT 10;
```

**Spalten:**
- `IDrun`: Run-Nummer
- `cell`: Grid-Zelle
- `year`: Jahr
- `month`: Monat (1-12)
- `concentration`: Konzentration

### wwqa_wq_load_[region] - Eintragsdaten

**Wichtige Tabellen:**

#### calc_cell_month_load_*
Enthält die berechneten Einträge (eine Tabelle pro Quelle).

```sql
SELECT * FROM wwqa_wq_load_af.calc_cell_month_load_man_ww 
WHERE year=2010 
LIMIT 10;
```

**Spalten:**
- `cell`: Grid-Zelle
- `year`: Jahr
- `month`: Monat (1-12)
- `load`: Eintrag

---

## Datenbankstruktur komplett anlegen

Falls Sie ein **komplett neues System** aufsetzen müssen und keine bestehende Datenbank haben, müssen Sie die Datenbankstruktur von Grund auf erstellen.

### Voraussetzungen

- MySQL Server läuft
- MySQL-Benutzer mit Berechtigungen erstellt
- Root-Zugriff auf MySQL (für Schema-Erstellung)

### Schritt 1: Basis-Datenbank erstellen

```sql
-- Als root einloggen
mysql -u root -p

-- Datenbanken erstellen
CREATE DATABASE wq_general;
CREATE DATABASE wwqa_worldqual_af;
CREATE DATABASE wwqa_worldqual_eu;
CREATE DATABASE wwqa_worldqual_as;
CREATE DATABASE wwqa_worldqual_au;
CREATE DATABASE wwqa_worldqual_na;
CREATE DATABASE wwqa_worldqual_sa;

CREATE DATABASE wwqa_wq_load_af;
CREATE DATABASE wwqa_wq_load_eu;
CREATE DATABASE wwqa_wq_load_as;
CREATE DATABASE wwqa_wq_load_au;
CREATE DATABASE wwqa_wq_load_na;
CREATE DATABASE wwqa_wq_load_sa;

-- Berechtigungen geben
GRANT ALL PRIVILEGES ON wq_general.* TO 'worldqual'@'localhost';
GRANT ALL PRIVILEGES ON wwqa_worldqual_*.* TO 'worldqual'@'localhost';
GRANT ALL PRIVILEGES ON wwqa_wq_load_*.* TO 'worldqual'@'localhost';
FLUSH PRIVILEGES;
```

### Schritt 2: wq_general Tabellen erstellen

**Wichtig:** Die genauen Tabellendefinitionen können variieren. Diese sind Beispiele basierend auf dem Code.

#### _project Tabelle

```sql
USE wq_general;

CREATE TABLE _project (
    project_id INT PRIMARY KEY,
    Project_Name VARCHAR(255) NOT NULL,
    database_name VARCHAR(255) DEFAULT NULL,
    description TEXT
);

-- Standard-Projekte einfügen
INSERT INTO _project (project_id, Project_Name, description) VALUES
(1, 'CESR', 'Climate and Environment System Research'),
(2, 'WWQA', 'World Water Quality Assessment'),
(3, 'CESR Sensitivity Analysis', 'Sensitivity Analysis'),
(4, 'test', 'Test-Projekt für Entwickler');
```

#### _parameter Tabelle

```sql
CREATE TABLE _parameter (
    parameter_id INT PRIMARY KEY,
    ParameterName VARCHAR(255) NOT NULL,
    description TEXT,
    unit VARCHAR(50)
);

-- Standard-Parameter einfügen
INSERT INTO _parameter (parameter_id, ParameterName, unit) VALUES
(0, 'BOD', 'mg/l'),
(1, 'TDS', 'mg/l'),
(2, 'FC', 'cfu/100ml'),
(3, 'TN', 'mg/l'),
(4, 'TP', 'mg/l');
```

#### _szenario Tabelle

```sql
CREATE TABLE _szenario (
    IDScen INT PRIMARY KEY,
    ScenName VARCHAR(255) NOT NULL,
    description TEXT,
    project_id INT,
    FOREIGN KEY (project_id) REFERENCES _project(project_id)
);

-- Beispiel-Szenario
INSERT INTO _szenario (IDScen, ScenName, description, project_id) VALUES
(91, 'Baseline 2010', 'Baseline-Szenario für 2010', 2);
```

#### _runlist Tabelle

```sql
CREATE TABLE _runlist (
    IDrun INT PRIMARY KEY,
    runName VARCHAR(255),
    parameter_id INT,
    parameter_id_input INT,
    parameter_id_load INT,
    IDScen INT,
    IDScen_wq_load INT,
    project_id INT,
    IDTemp INT DEFAULT -9999,
    Q_low_limit DOUBLE DEFAULT NULL,
    Q_low_limit_type INT DEFAULT NULL,
    conservative INT DEFAULT 0,
    lake INT DEFAULT 0,
    UseWaterTemp INT DEFAULT 0,
    svn_version_id INT DEFAULT NULL,
    comment TEXT,
    FOREIGN KEY (parameter_id) REFERENCES _parameter(parameter_id),
    FOREIGN KEY (IDScen) REFERENCES _szenario(IDScen),
    FOREIGN KEY (project_id) REFERENCES _project(project_id)
);
```

#### _water_temperature_list Tabelle

```sql
CREATE TABLE _water_temperature_list (
    IDTemp INT PRIMARY KEY,
    Name VARCHAR(255),
    climate VARCHAR(255),
    comment TEXT,
    C0 DOUBLE,
    C1 DOUBLE,
    C2 DOUBLE
);

-- Beispiel-Temperatur-Konfiguration
INSERT INTO _water_temperature_list (IDTemp, Name, climate, C0, C1, C2) VALUES
(100, 'Standard', 'air_temp', 20.0, -0.1, 5.0);
```

#### watergap_region Tabelle

```sql
CREATE TABLE watergap_region (
    IDVersion INT,
    IDRegion INT,
    hydro_input VARCHAR(255),
    PRIMARY KEY (IDVersion, IDRegion)
);

-- Beispiel-Regionen
INSERT INTO watergap_region (IDVersion, IDRegion, hydro_input) VALUES
(3, 1, '/path/to/watergap/eu'),
(3, 2, '/path/to/watergap/af'),
(3, 3, '/path/to/watergap/as'),
(3, 4, '/path/to/watergap/au'),
(3, 5, '/path/to/watergap/na'),
(3, 6, '/path/to/watergap/sa');
```

### Schritt 3: Regionsspezifische Datenbanken - concentration Tabelle

**Für jede Region (af, eu, as, au, na, sa):**

```sql
USE wwqa_worldqual_af;

CREATE TABLE concentration (
    IDrun INT NOT NULL,
    cell INT NOT NULL,
    year INT NOT NULL,
    month INT NOT NULL,
    concentration DOUBLE,
    PRIMARY KEY (IDrun, cell, year, month),
    INDEX idx_cell (cell),
    INDEX idx_year (year),
    INDEX idx_idrun (IDrun),
    INDEX idx_idrun_year (IDrun, year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

### Schritt 4: Regionsspezifische Datenbanken - Eintragstabellen

**Für jede Region:**

```sql
USE wwqa_wq_load_af;

-- Industrieabwasser
CREATE TABLE calc_cell_month_load_man_ww (
    cell INT NOT NULL,
    year INT NOT NULL,
    month INT NOT NULL,
    load DOUBLE,
    PRIMARY KEY (cell, year, month),
    INDEX idx_cell (cell),
    INDEX idx_year (year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Haushaltsabwasser
CREATE TABLE calc_cell_month_load_dom_ww (
    cell INT NOT NULL,
    year INT NOT NULL,
    month INT NOT NULL,
    load DOUBLE,
    PRIMARY KEY (cell, year, month),
    INDEX idx_cell (cell),
    INDEX idx_year (year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Bewässerungs-Rückfluss
CREATE TABLE calc_cell_month_load_rtf_irr (
    cell INT NOT NULL,
    year INT NOT NULL,
    month INT NOT NULL,
    load DOUBLE,
    PRIMARY KEY (cell, year, month),
    INDEX idx_cell (cell),
    INDEX idx_year (year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Viehhaltung
CREATE TABLE calc_cell_month_load_ls (
    cell INT NOT NULL,
    year INT NOT NULL,
    month INT NOT NULL,
    load DOUBLE,
    PRIMARY KEY (cell, year, month),
    INDEX idx_cell (cell),
    INDEX idx_year (year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Landwirtschaft
CREATE TABLE calc_cell_month_load_crop (
    cell INT NOT NULL,
    year INT NOT NULL,
    month INT NOT NULL,
    load DOUBLE,
    PRIMARY KEY (cell, year, month),
    INDEX idx_cell (cell),
    INDEX idx_year (year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Weitere Tabellen je nach Parameter...
```

### Schritt 5: Weitere benötigte Tabellen

**cell_input Tabelle:**

```sql
USE wwqa_wq_load_af;

CREATE TABLE cell_input (
    cell INT NOT NULL,
    IDScen INT NOT NULL,
    year INT NOT NULL,
    -- Weitere Spalten je nach Parameter
    PRIMARY KEY (cell, IDScen, year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

**country_parameter_input Tabelle:**

```sql
CREATE TABLE country_parameter_input (
    country_id INT NOT NULL,
    IDScen INT NOT NULL,
    year INT NOT NULL,
    parameter_id INT NOT NULL,
    -- Weitere Spalten je nach Parameter
    PRIMARY KEY (country_id, IDScen, year, parameter_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

### Schritt 6: Routing-Tabellen (wichtig!)

WorldQual benötigt Routing-Informationen aus WaterGAP. Diese müssen importiert werden:

**Typische Tabellen:**
- `outflow_cell` - Abflusszellen
- `inflow` - Zuflüsse
- `river_length` - Flusslängen
- `routing_order` - Routing-Reihenfolge

**Wichtig:** Diese Tabellen müssen aus WaterGAP-Daten importiert werden oder von einem bestehenden System kopiert werden.

### Schritt 7: Schema exportieren und dokumentieren

**Nach erfolgreicher Erstellung:**

```bash
# Schema exportieren
mysqldump -u root -p --no-data wq_general > schema_wq_general.sql
mysqldump -u root -p --no-data wwqa_worldqual_af > schema_worldqual_af.sql
mysqldump -u root -p --no-data wwqa_wq_load_af > schema_wq_load_af.sql

# Für alle Regionen wiederholen
```

**Dokumentieren Sie:**
- Welche Tabellen erstellt wurden
- Welche Indizes gesetzt wurden
- Welche Daten importiert wurden

### Wichtiger Hinweis

**Die vollständige Datenbankstruktur ist komplex!**

- Viele Tabellen haben parameter-spezifische Spalten
- Routing-Tabellen müssen aus WaterGAP importiert werden
- Einige Tabellen werden dynamisch erstellt

**Empfehlung:**
- Nutzen Sie ein bestehendes System als Vorlage
- Exportieren Sie das Schema: `mysqldump --no-data`
- Importieren Sie auf neuem System: `mysql < schema.sql`

**Falls Sie komplett neu starten müssen:**
- Kontaktieren Sie das Entwicklungsteam
- Prüfen Sie ob Schema-Dateien vorhanden sind
- Siehe auch [docs/technical/DATABASE_SCHEMA.md](../technical/DATABASE_SCHEMA.md) für Details

---

## WaterGAP-Daten genau beschaffen

### Was sind WaterGAP-Daten?

WaterGAP (Water Global Assessment and Prognosis) ist ein globales hydrologisches Modell. WorldQual benötigt dessen **Output-Daten** als Input.

### Welche Daten werden benötigt?

#### Für worldqual (Hauptsimulation):

**Erforderlich:**
- `G_Q_out_m3_[year].12.UNF0` - Monatlicher Abfluss [m³/month]
- `G_RUNOFF_TOTAL_mm_[year].12.UNF0` - Monatlicher Gesamtabfluss [mm/month]

**Optional aber empfohlen:**
- `G_FLOW_VELOCITY_m_s_[year].12.UNF0` - Fließgeschwindigkeit [m/s]
- `G_WATER_TEMP_C_[year].12.UNF0` - Wassertemperatur [°C] (falls nicht berechnet)

#### Für fill_worldqual_load (Eintragsberechnung):

**Erforderlich:**
- Bevölkerungsdaten (Grid-Daten)
- Landnutzungsdaten (Grid-Daten)
- Abwasserbehandlungsraten (Länderdaten)

**Diese können aus verschiedenen Quellen kommen:**
- UNF-Dateien
- Datenbank-Tabellen
- CSV-Dateien (müssen importiert werden)

#### Für water_temperature:

**Erforderlich:**
- `G_AIR_TEMP_C_[year].12.UNF0` - Monatliche Lufttemperatur [°C]

### Wo bekommt man WaterGAP-Daten?

#### Option 1: Von WaterGAP-Projektgruppe

**Kontakt:**
- WaterGAP-Projektwebsite besuchen
- Projektgruppe kontaktieren
- Datenanfrage stellen

**Typischerweise erhalten Sie:**
- Zugriff auf Daten-Server
- Download-Links
- Dokumentation zum Datenformat

#### Option 2: Von bestehendem Projekt

**Falls Sie Teil eines bestehenden Projekts sind:**
- Fragen Sie Projektleiter/Kollegen
- Daten sind oft auf Projekt-Servern
- Möglicherweise bereits in Datenbank importiert

#### Option 3: Öffentlich verfügbare Daten

**Falls verfügbar:**
- WaterGAP kann öffentliche Datensätze bereitstellen
- Prüfen Sie WaterGAP-Veröffentlichungen
- Kontaktieren Sie die Autoren

### Datenformat verstehen

#### UNF-Dateien (Unformatted)

**Format:** Binäres Format (nicht lesbar als Text)

**Struktur:**
- Grid-basiert (0.5° x 0.5° Auflösung)
- Zeitreihen: Monatlich (12 Werte) oder jährlich (1 Wert)
- Byte-Order: Kann systemabhängig sein (Byte-Swapping erforderlich)

**Dateinamen-Konventionen:**

```
G_[PARAMETER]_[UNIT]_[YEAR].[MONTHS].UNF0

Beispiele:
G_Q_out_m3_2010.12.UNF0          # Monatlicher Abfluss 2010
G_RUNOFF_TOTAL_mm_2010.12.UNF0   # Monatlicher Runoff 2010
G_AIR_TEMP_C_2010.12.UNF0        # Monatliche Lufttemperatur 2010
```

**Dateinamen-Bestandteile:**
- `G_` - Grid-Daten
- `Q_out` - Parameter (Abfluss)
- `m3` - Einheit
- `2010` - Jahr
- `.12` - 12 Monate (monatlich)
- `.UNF0` - Dateiformat

#### Dateigröße abschätzen

**Für eine Region (z.B. Afrika):**
- Anzahl Zellen: ~50.000-100.000 (abhängig von Region)
- Datentyp: float (4 Bytes) oder double (8 Bytes)
- Monatlich: 12 Werte pro Jahr

**Beispiel-Berechnung:**
```
Afrika: ~70.000 Zellen
Float: 4 Bytes
Monatlich: 12 Werte
Jahr: 1 Jahr

Größe = 70.000 * 4 * 12 = 3.360.000 Bytes ≈ 3.2 MB pro Jahr
```

**Für mehrere Jahre:**
- 20 Jahre = ~64 MB pro Parameter
- Mehrere Parameter = mehrere hundert MB bis GB

### Daten prüfen

**Schritt 1: Dateien auflisten**

```bash
cd /pfad/zu/watergap/daten
ls -lh G_*.UNF0

# Sollten Sie sehen:
# - Dateien für verschiedene Jahre
# - Verschiedene Parameter
# - Korrekte Dateinamen
```

**Schritt 2: Dateigröße prüfen**

```bash
# Erwartete Größe berechnen
# Anzahl_Zellen * Datentyp_Größe * Werte_pro_Zelle

# Beispiel für Afrika, monatlich, float:
# 70.000 * 4 * 12 = 3.360.000 Bytes ≈ 3.2 MB

ls -lh G_Q_out_m3_2010.12.UNF0
# Sollte etwa 3-4 MB sein (abhängig von Region)
```

**Schritt 3: Dateien lesbar?**

```bash
# Prüfen ob Dateien existieren und lesbar sind
test -r G_Q_out_m3_2010.12.UNF0 && echo "Datei ist lesbar" || echo "FEHLER: Datei nicht lesbar"

# Rechte prüfen
ls -l G_Q_out_m3_2010.12.UNF0
# Sollte zeigen: -r--r--r-- oder ähnlich
```

**Schritt 4: Mit WorldQual testen**

```bash
# Test-Lesen mit einem Modul
cd src/worldqual
# OPTIONS.DAT anpassen: input_dir = /pfad/zu/daten
# Programm starten (wird Fehler geben wenn Daten fehlen, aber zeigt ob Dateien gefunden werden)
./worldqual 403100091 2010 2010
```

### Benötigte Dateien - Komplette Liste

#### Für eine komplette Simulation benötigen Sie:

**Hydrologische Daten (für worldqual):**
```
G_Q_out_m3_[year].12.UNF0              # Abfluss [m³/month]
G_RUNOFF_TOTAL_mm_[year].12.UNF0       # Runoff [mm/month]
G_FLOW_VELOCITY_m_s_[year].12.UNF0     # Fließgeschwindigkeit [m/s] (optional)
```

**Klimadaten (für water_temperature):**
```
G_AIR_TEMP_C_[year].12.UNF0            # Lufttemperatur [°C]
```

**Routing-Daten (aus Datenbank oder Dateien):**
- Abflusszellen (outflow_cell)
- Zuflüsse (inflow)
- Flusslängen (river_length)
- Routing-Reihenfolge (routing_order)

**Eintragsdaten (für fill_worldqual_load):**
- Bevölkerungsdichte (Grid oder Länderdaten)
- Abwasserbehandlungsraten (Länderdaten)
- Landnutzung (Grid-Daten)
- Düngeranwendung (Länderdaten)
- Geologische Karten

**Wichtig:** Nicht alle Daten müssen als UNF-Dateien vorliegen - viele können auch aus der Datenbank kommen!

### Daten importieren (falls nötig)

**Falls Daten als CSV/Text vorliegen:**

```sql
-- Beispiel: CSV importieren
LOAD DATA LOCAL INFILE '/path/to/data.csv'
INTO TABLE cell_input
FIELDS TERMINATED BY ','
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS;
```

**Falls Daten aus anderem Format:**

- Kontaktieren Sie das Entwicklungsteam
- Möglicherweise gibt es Import-Skripte
- Siehe auch [docs/technical/DATABASE_SCHEMA.md](../technical/DATABASE_SCHEMA.md)

---

## Alle Module kompilieren - Komplette Liste

Diese Liste zeigt **alle Module** und die **exakte Kompilierungsreihenfolge**.

### Kompilierungsreihenfolge (wichtig!)

**Phase 1: Gemeinsame Bibliotheken (MUSS zuerst)**

```bash
# 1. general_function (keine Abhängigkeiten)
cd src/general_function
make clean
make all
# Prüfen: ls -la general_functions.o timestring.o country.o

# 2. options (abhängig von general_function)
cd ../options
make clean
make all
# Prüfen: ls -la options.o options_scen.o
```

**Phase 2: Utility-Module**

```bash
# 3. newton (wird von water_temperature benötigt)
cd ../newton
make clean
make all
# Prüfen: ls -la Newton.o countryinfo.o
```

**Phase 3: Input-Module (können parallel kompiliert werden)**

```bash
# 4. fill_worldqual_load
cd ../fill_worldqual_load
make clean
make all
# Prüfen: ls -la fill_worldqual_load

# 5. water_temperature (nutzt newton)
cd ../water_temperature
make clean
make all
# Prüfen: ls -la water_temperature

# 6. rtf_input
cd ../rtf_input
make clean
make all
# Prüfen: ls -la rtf_input

# 7. rtf_input_corr_factor
cd ../rtf_input_corr_factor
make clean
make all
# Prüfen: ls -la rtf_input_corr_factor

# 8. pop_dens_input
cd ../pop_dens_input
make clean
make all
# Prüfen: ls -la pop_dens_input
```

**Phase 4: Core-Module**

```bash
# 9. wq_load (wird von fill_worldqual_load benötigt)
cd ../wq_load
make clean
make all
# Prüfen: ls -la wq_load

# 10. worldqual (Hauptprogramm)
cd ../worldqual
make clean
make all
# Prüfen: ls -la worldqual
```

**Phase 5: Analyse-Module**

```bash
# 11. wq_stat
cd ../wq_stat
make clean
make all
# Prüfen: ls -la wq_stat

# 12. wq_stat_stations
cd ../wq_stat_stations
make clean
make all
# Prüfen: ls -la wq_stat_stations

# 13. wq_stat_scen
cd ../wq_stat_scen
make clean
make all
# Prüfen: ls -la wq_stat_scen

# 14. wq_stat_riverdef
cd ../wq_stat_riverdef
make clean
make all
# Prüfen: ls -la river_start_end (oder ähnlich)
```

**Phase 6: Utility-Module**

```bash
# 15. MapRiverQuality
cd ../MapRiverQuality
make clean
make all
# Prüfen: ls -la mapRiverQuality

# 16. find_river_cells
cd ../find_river_cells
make clean
make all
# Prüfen: ls -la find_river_cells

# 17. wq_instream_daily
cd ../wq_instream_daily
make clean
make all
# Prüfen: ls -la wq_instream_daily

# 18. create_scenarios
cd ../create_scenarios
make clean
make all
# Prüfen: ls -la create_scenarios

# 19. copy_run
cd ../copy_run
make clean
make all
# Prüfen: ls -la copy_run

# 20. copy_scen
cd ../copy_scen
make clean
make all
# Prüfen: ls -la copy_scen
```

### Automatisiertes Build-Skript

**Erstellen Sie `build_all.sh`:**

```bash
#!/bin/bash
# Build-Skript für alle WorldQual-Module

set -e  # Bei Fehler abbrechen

echo "Building WorldQual modules..."
echo "================================"

# Funktion zum Kompilieren
build_module() {
    local module=$1
    echo ""
    echo "Building $module..."
    cd "src/$module"
    if [ -f makefile ]; then
        make clean
        make all
        if [ $? -eq 0 ]; then
            echo "✓ $module built successfully"
        else
            echo "✗ ERROR: $module failed to build"
            exit 1
        fi
        cd ../..
    else
        echo "⚠ WARNING: No makefile in $module"
    fi
}

# Phase 1: Gemeinsame Bibliotheken
build_module "general_function"
build_module "options"

# Phase 2: Utility
build_module "newton"

# Phase 3: Input-Module
build_module "fill_worldqual_load"
build_module "water_temperature"
build_module "rtf_input"
build_module "rtf_input_corr_factor"
build_module "pop_dens_input"

# Phase 4: Core
build_module "wq_load"
build_module "worldqual"

# Phase 5: Analyse
build_module "wq_stat"
build_module "wq_stat_stations"
build_module "wq_stat_scen"
build_module "wq_stat_riverdef"

# Phase 6: Utility
build_module "MapRiverQuality"
build_module "find_river_cells"
build_module "wq_instream_daily"
build_module "create_scenarios"
build_module "copy_run"
build_module "copy_scen"

echo ""
echo "================================"
echo "All modules built successfully!"
```

**Verwendung:**
```bash
chmod +x build_all.sh
./build_all.sh
```

### Modul-Übersicht

| Modul | Zweck | Abhängigkeiten | Priorität |
|-------|-------|----------------|-----------|
| general_function | Gemeinsame Funktionen | keine | **Höchste** |
| options | Konfiguration | general_function | **Höchste** |
| newton | Numerische Kurvenanpassung | keine | Hoch |
| fill_worldqual_load | Einträge berechnen | general_function, options | Hoch |
| water_temperature | Wassertemperatur | general_function, options, newton | Hoch |
| wq_load | Load-Berechnung | general_function, options | Hoch |
| worldqual | Hauptsimulation | general_function, options, wq_load | **Kritisch** |
| wq_stat* | Statistik | general_function, options | Mittel |
| MapRiverQuality | Kartierung | general_function, options | Niedrig |
| create_scenarios | Szenarien | general_function, options | Niedrig |
| copy_* | Verwaltung | general_function, options | Niedrig |

### Häufige Kompilierungsfehler

**Fehler: "No rule to make target '../general_function/general_functions.o'"**

**Lösung:** general_function muss zuerst kompiliert werden!
```bash
cd src/general_function && make all
cd ../options && make all
# Dann andere Module
```

**Fehler: "undefined reference to mysqlpp::..."**

**Lösung:** Bibliothek nicht gelinkt oder falscher Pfad
```makefile
# In Makefile prüfen:
LNKLIB = -lmysqlpp -lmysqlclient
# Reihenfolge kann wichtig sein!
```

**Fehler: Verschiedene Module kompilieren nicht**

**Lösung:** Prüfen Sie Abhängigkeiten
- Module die `general_function` nutzen: müssen nach general_function kompiliert werden
- Module die `options` nutzen: müssen nach options kompiliert werden
- Module die `newton` nutzen: müssen nach newton kompiliert werden

### Prüfen ob Kompilierung erfolgreich

**Nach jedem Modul:**

```bash
# Ausführbare Datei existiert?
ls -la modulname

# Sollte zeigen:
# -rwxr-xr-x 1 user user ... modulname
# Das 'x' bedeutet ausführbar

# Test-Ausführung (sollte Hilfe anzeigen oder Fehler wegen Parameter)
./modulname
```

**Alle Module auf einmal prüfen:**

```bash
# Im Root-Verzeichnis
for dir in src/*/; do
    if [ -f "${dir}makefile" ]; then
        module=$(basename "$dir")
        if [ -f "${dir}${module}" ] || [ -f "${dir}worldqual" ] || [ -f "${dir}wq_load" ]; then
            echo "✓ $module: OK"
        else
            echo "✗ $module: FEHLT"
        fi
    fi
done
```

---

## Fehlermeldungen verstehen - Kompletter Guide

Diese Sektion erklärt **alle möglichen Fehlermeldungen** die auftreten können und wie man sie behebt.

### Datenbank-Fehler

#### "database connection failed..."

**Was bedeutet das?**
MySQL-Verbindung konnte nicht hergestellt werden.

**Mögliche Ursachen:**
1. MySQL-Server läuft nicht
2. Falsche Verbindungsdaten
3. Firewall blockiert Verbindung
4. Benutzer hat keine Berechtigung

**Lösung:**
```bash
# 1. MySQL läuft?
systemctl status mysql
# oder
brew services list | grep mysql

# 2. Verbindung testen
mysql -u worldqual -p -h localhost
# Falls Fehler: Passwort oder Benutzer falsch

# 3. OPTIONS.DAT prüfen
# MyHost, MyUser, MyPassword korrekt?

# 4. Berechtigungen prüfen
mysql -u root -p
SHOW GRANTS FOR 'worldqual'@'localhost';
```

#### "ERROR: Wrong parameter: project_id"

**Was bedeutet das?**
Projekt-ID existiert nicht in der Datenbank.

**Lösung:**
```sql
# Projekte anzeigen
SELECT * FROM wq_general._project;

# Falls leer: Projekt anlegen (siehe Datenbankstruktur anlegen)
```

#### "ERROR: Wrong parameter: IDScen"

**Was bedeutet das?**
Szenario-ID existiert nicht.

**Lösung:**
```sql
# Szenarien anzeigen
SELECT * FROM wq_general._szenario;

# Falls leer: Szenario anlegen
INSERT INTO _szenario (IDScen, ScenName, project_id) 
VALUES (91, 'Baseline 2010', 2);
```

#### "ERROR: Wrong parameter: IDrun"

**Was bedeutet das?**
Run-ID existiert nicht oder ist nicht vollständig konfiguriert.

**Lösung:**
```sql
# Run prüfen
SELECT * FROM wq_general._runlist WHERE IDrun=403100091;

# Falls leer: Run anlegen (komplex, siehe IDrun anlegen)
# Oder: Prüfen ob alle benötigten Spalten gefüllt sind
```

#### "ERROR: Wrong parameter: parameter_id"

**Was bedeutet das?**
Parameter-ID existiert nicht.

**Lösung:**
```sql
# Parameter anzeigen
SELECT * FROM wq_general._parameter;

# Falls leer: Parameter anlegen
INSERT INTO _parameter (parameter_id, ParameterName) VALUES
(0, 'BOD'), (1, 'TDS'), (2, 'FC'), (3, 'TN'), (4, 'TP');
```

#### "Query error: ..."

**Was bedeutet das?**
SQL-Query-Fehler (MySQL++ Exception).

**Mögliche Ursachen:**
- Tabelle existiert nicht
- Spalte existiert nicht
- Syntax-Fehler in Query
- Datenbank nicht gewählt

**Lösung:**
```sql
# Tabelle existiert?
SHOW TABLES LIKE 'tabellenname';

# Struktur prüfen
DESCRIBE tabellenname;

# Datenbank gewählt?
SELECT DATABASE();
USE wq_general;  # Falls nötig
```

#### "Conversion error: ..."

**Was bedeutet das?**
Datenkonvertierungs-Fehler (z.B. String zu INT).

**Lösung:**
- Prüfen Sie Datentypen in Datenbank
- Prüfen Sie ob NULL-Werte vorhanden sind
- Prüfen Sie Datenformat

### Datei-Fehler

#### "Can't open input file /path/to/file.UNF0"

**Was bedeutet das?**
Datei konnte nicht geöffnet werden.

**Mögliche Ursachen:**
1. Datei existiert nicht
2. Falscher Pfad
3. Keine Leseberechtigung
4. Datei ist leer

**Lösung:**
```bash
# 1. Datei existiert?
ls -l /path/to/file.UNF0

# 2. Pfad richtig?
# In OPTIONS.DAT prüfen: input_dir = /pfad/zu/daten
# Vollständiger Pfad: /pfad/zu/daten/G_Q_out_m3_2010.12.UNF0

# 3. Rechte prüfen
ls -l /path/to/file.UNF0
# Sollte zeigen: -r--r--r-- oder -rw-r--r--
chmod 644 /path/to/file.UNF0  # Falls nötig

# 4. Datei nicht leer?
ls -lh /path/to/file.UNF0
# Sollte Größe > 0 zeigen
```

#### "Input file size mismatch"

**Was bedeutet das?**
Dateigröße stimmt nicht mit erwarteter Größe überein.

**Mögliche Ursachen:**
- Falsche Anzahl Zellen
- Falsches Format
- Datei beschädigt
- Falsche Werte pro Zelle (12 vs. 1)

**Lösung:**
```bash
# Erwartete Größe berechnen
# Anzahl_Zellen * Datentyp_Größe * Werte_pro_Zelle

# Beispiel: 70.000 Zellen, float (4 Bytes), 12 Monate
# Erwartet: 70.000 * 4 * 12 = 3.360.000 Bytes

# Tatsächliche Größe prüfen
ls -l G_Q_out_m3_2010.12.UNF0

# Falls unterschiedlich:
# - Prüfen Sie Anzahl Zellen (continent.ng)
# - Prüfen Sie ob Datei monatlich (12) oder jährlich (1) ist
```

#### "file ... bytesRead ... number_of_cells*valuesPerCell*type_size"

**Was bedeutet das?**
Datei wurde gelesen, aber Größe stimmt nicht.

**Lösung:**
- Prüfen Sie ob Datei vollständig ist
- Prüfen Sie ob Dateiformat korrekt ist
- Prüfen Sie Byte-Order (Big-Endian vs. Little-Endian)

### Kompilierungsfehler

#### "mysql++.h: No such file or directory"

**Was bedeutet das?**
MySQL++ Header-Datei nicht gefunden.

**Lösung:**
```bash
# MySQL++ installiert?
find /usr /usr/local -name "mysql++.h"

# Pfad in Makefile anpassen:
# -I/usr/local/include/mysql++  # Anpassen!
```

#### "cannot find -lmysqlpp"

**Was bedeutet das?**
MySQL++ Bibliothek nicht gefunden.

**Lösung:**
```bash
# Bibliothek finden
find /usr /usr/local -name "libmysqlpp.so*"

# Pfad in Makefile anpassen:
# -L/usr/local/lib  # Anpassen!
```

#### "undefined reference to mysqlpp::..."

**Was bedeutet das?**
Bibliothek wird nicht gelinkt.

**Lösung:**
```makefile
# In Makefile prüfen:
LNKLIB = -lmysqlpp -lmysqlclient
# Muss NACH Objektdateien stehen:
$(CC) $(OBJECTS) $(LNKLIB) -o $(APPNAME)
```

#### "multiple definition of ..."

**Was bedeutet das?**
Funktion mehrfach definiert.

**Lösung:**
- Funktionen in `.cpp` definieren, nicht in `.h`
- `inline` für Template-Funktionen verwenden
- Header-Guards prüfen (`#ifndef`)

### Laufzeit-Fehler

#### "not enough memory"

**Was bedeutet das?**
Nicht genug RAM verfügbar.

**Lösung:**
```bash
# RAM prüfen
free -h

# Swap aktivieren (falls nicht aktiv)
sudo swapon --show
# Falls leer: Swap-Datei erstellen

# Alternative: Kleinere Region/Zeitraum testen
```

#### "Segmentation fault" oder "Segfault"

**Was bedeutet das?**
Zugriff auf ungültigen Speicher.

**Lösung:**
```bash
# Mit Debugger
gdb ./worldqual
(gdb) run 403100091 2010 2010
# Programm crasht
(gdb) backtrace
# Zeigt wo Fehler auftrat

# Häufige Ursachen:
# - NULL-Pointer
# - Array-Überlauf
# - Nicht initialisierte Pointer
```

#### "Floating point exception"

**Was bedeutet das?**
Division durch Null oder ungültige Fließkomma-Operation.

**Lösung:**
- Prüfen Sie Divisionen (z.B. durch Q_out)
- Prüfen Sie ob Werte NULL oder NaN sind
- Prüfen Sie mathematische Funktionen (log, sqrt von negativen Werten)

### Logik-Fehler

#### "start year > end year"

**Was bedeutet das?**
Startjahr ist größer als Endjahr.

**Lösung:**
```bash
# Parameter prüfen
./worldqual 403100091 2010 2000  # FALSCH!
./worldqual 403100091 2000 2010  # RICHTIG!
```

#### "ERROR: Wrong parameter"

**Was bedeutet das?**
Parameter ist keine Zahl oder ungültig.

**Lösung:**
- Prüfen Sie ob alle Parameter Zahlen sind
- Prüfen Sie ob Parameter im erlaubten Bereich sind
- Prüfen Sie Help-Ausgabe: `./programm` (ohne Parameter)

### Daten-Fehler

#### "value = NODATA" oder "value = NaN"

**Was bedeutet das?**
Ungültiger Wert (NoData oder Not-a-Number).

**Lösung:**
- Prüfen Sie Input-Daten
- Prüfen Sie ob Berechnungen gültig sind
- Prüfen Sie Divisionen durch Null

#### "rout_area < min.rout.area" (in R-Skripten)

**Was bedeutet das?**
Einzugsgebietsfläche zu klein.

**Lösung:**
- Normal (Filter in R-Skripten)
- Können Sie in R-Skript ignorieren oder anpassen

### Performance-Probleme

#### Programm läuft sehr langsam

**Mögliche Ursachen:**
- Zu große Region
- Zu langer Zeitraum
- Zu wenig RAM (Swapping)
- Datenbank langsam

**Lösung:**
```bash
# 1. Mit kleiner Region testen
# 2. Mit einem Jahr testen
# 3. RAM prüfen
free -h

# 4. Datenbank-Performance prüfen
# Indizes vorhanden?
SHOW INDEX FROM concentration;

# 5. Parallele Prozesse vermeiden
# Nur eine Simulation gleichzeitig!
```

### Fehlerbehebungs-Workflow

**Systematisches Vorgehen:**

1. **Fehlermeldung genau lesen**
   - Was sagt die Meldung?
   - Welches Modul?
   - Welche Zeile (falls vorhanden)?

2. **Kontext prüfen**
   - Was wurde gerade gemacht?
   - Welche Parameter wurden verwendet?
   - Was wurde vorher ausgeführt?

3. **Häufige Ursachen prüfen**
   - Datenbankverbindung?
   - Dateien vorhanden?
   - Parameter korrekt?

4. **Logging aktivieren**
   ```bash
   ./programm parameter > log.txt 2>&1
   # Dann log.txt analysieren
   ```

5. **Debug-Modus aktivieren**
   ```cpp
   #define DEBUG
   #define DEBUG_queries
   ```

6. **Mit Debugger**
   ```bash
   gdb ./programm
   ```

---

## Praktische Beispiele

### Beispiel 1: Einfache Simulation (ein Jahr, ein Parameter)

**Ziel:** Simulieren Sie FC für Afrika, Jahr 2010.

**Schritte:**

1. Einträge berechnen:
```bash
cd fill_worldqual_load
# OPTIONS.DAT: project_id=2, IDScen=91, IDReg=2
./fill_worldqual_load 2010 2010
```

2. Wassertemperatur:
```bash
cd ../water_temperature
# OPTIONS.DAT: IDTemp=100
./water_temperature 2010 2010
```

3. Hauptsimulation:
```bash
cd ../worldqual
# OPTIONS.DAT: IDrun aus Datenbank holen
./worldqual 403100091 2010 2010
```

4. Ergebnisse prüfen:
```sql
SELECT AVG(concentration) FROM wwqa_worldqual_af.concentration 
WHERE IDrun=403100091 AND year=2010;
```

### Beispiel 2: Mehrere Jahre

**Ziel:** Simulieren Sie 1990-2010.

**Unterschied:** Geben Sie Start- und Endjahr an:

```bash
./fill_worldqual_load 1990 2010
./water_temperature 1990 2010
./worldqual 403100091 1990 2010
```

**Wichtig:** Dauert viel länger! Testen Sie zuerst mit einem Jahr.

### Beispiel 3: Szenarienvergleich

**Ziel:** Vergleichen Sie Baseline mit zwei Szenarien.

**Schritte:**

1. Baseline simulieren (wie Beispiel 1)
2. Szenario 1 simulieren (andere IDScen)
3. Szenario 2 simulieren
4. Vergleich:
```bash
cd wq_stat_scen
# OPTIONS.DAT: Baseline-IDrun, Szenario-IDruns
./wq_stat_scen 403100091 2010 2010
```

---

## Fehlerbehebung

### Problem 1: Datenbankverbindung schlägt fehl

**Fehlermeldung:**
```
database connection failed...
```

**Lösung:**

1. MySQL läuft?
```bash
systemctl status mysql
# oder
sudo systemctl start mysql
```

2. Verbindungsdaten richtig?
```bash
# In OPTIONS.DAT prüfen:
MyHost = localhost  # oder IP-Adresse
MyUser = worldqual
MyPassword = passwort
```

3. Test-Verbindung:
```bash
mysql -u worldqual -p -h localhost
```

4. Berechtigungen prüfen:
```sql
SHOW GRANTS FOR 'worldqual'@'localhost';
```

### Problem 2: Datei nicht gefunden

**Fehlermeldung:**
```
Can't open input file /pfad/zu/datei.UNF0
```

**Lösung:**

1. Pfad prüfen:
```bash
ls /pfad/zu/datei.UNF0
```

2. OPTIONS.DAT prüfen:
```
input_dir = /pfad/zu/daten  # Muss existieren!
```

3. Rechte prüfen:
```bash
ls -l /pfad/zu/datei.UNF0
chmod 644 /pfad/zu/datei.UNF0  # Falls nötig
```

### Problem 3: Falsche Parameter-ID

**Fehlermeldung:**
```
ERROR: Wrong parameter: parameter_id
```

**Lösung:**

1. Parameter existiert?
```sql
SELECT * FROM wq_general._parameter;
```

2. Parameter für Projekt konfiguriert?
```sql
SELECT * FROM wq_general._runlist 
WHERE parameter_id=2;
```

### Problem 4: Speicherprobleme

**Fehlermeldung:**
```
not enough memory
```

**Lösung:**

1. RAM prüfen:
```bash
free -h
```

2. Kleinere Region testen
3. Kürzeren Zeitraum testen (ein Jahr statt mehrere)
4. Swap aktivieren:
```bash
sudo swapon --show
```

### Problem 5: Falsche IDrun

**Fehlermeldung:**
```
ERROR: Wrong parameter: IDrun
```

**Lösung:**

1. IDrun existiert?
```sql
SELECT * FROM wq_general._runlist 
WHERE IDrun=403100091;
```

2. Run vollständig konfiguriert?
```sql
SELECT IDrun, parameter_id, IDScen, IDTemp 
FROM wq_general._runlist 
WHERE IDrun=403100091;
```

3. Run anlegen falls nötig (siehe Datenbank-Dokumentation)

---

## Tipps und Tricks

### Tipp 1: Immer zuerst testen

**Testen Sie immer mit:**
- Einem Jahr (statt mehreren)
- Kleiner Region (falls möglich)
- Einem Parameter

**Dann erweitern Sie schrittweise.**

### Tipp 2: Logging aktivieren

**Umleiten Sie Ausgaben:**
```bash
./worldqual 403100091 2010 2010 > log.txt 2>&1
```

**Dann können Sie später prüfen:**
```bash
tail -f log.txt  # Während der Ausführung
cat log.txt      # Nach der Ausführung
```

### Tipp 3: Datenbank-Backup

**Vor großen Simulationen:**
```bash
mysqldump -u username -p wwqa_worldqual_af > backup_$(date +%Y%m%d).sql
```

### Tipp 4: Fortschritt prüfen

**Während der Simulation können Sie prüfen:**
```sql
-- Wie viele Jahre sind fertig?
SELECT DISTINCT year FROM wwqa_worldqual_af.concentration 
WHERE IDrun=403100091 
ORDER BY year;
```

### Tipp 5: Ergebnisse exportieren

**Als CSV exportieren:**
```sql
SELECT * FROM wwqa_worldqual_af.concentration 
WHERE IDrun=403100091 AND year=2010
INTO OUTFILE '/tmp/ergebnisse.csv'
FIELDS TERMINATED BY ',' 
ENCLOSED BY '"'
LINES TERMINATED BY '\n';
```

---

**Weitere Hilfe:**
- [README.md](../index.md) - Übersicht und Einführung
- [QUICK_REFERENCE.md](schnellreferenz.md) - Schnellreferenz
- [STRUCTURE.md](projektstruktur.md) - Projektstruktur
- [INDEX.md](../index.md) - Dokumentations-Übersicht
