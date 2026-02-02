# WorldQual - Detaillierte Anleitung

**Navigation:** [Übersicht](README.md) | [Schnellreferenz](QUICK_REFERENCE.md) | [Index](INDEX.md) | [Struktur](STRUCTURE.md)

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
5. [Praktische Beispiele](#praktische-beispiele)
   - [Beispiel 1: Einfache Simulation](#beispiel-1-einfache-simulation-ein-jahr-ein-parameter)
   - [Beispiel 2: Mehrere Jahre](#beispiel-2-mehrere-jahre)
   - [Beispiel 3: Szenarienvergleich](#beispiel-3-szenarienvergleich)
6. [Fehlerbehebung](#fehlerbehebung)
   - [Problem 1: Datenbankverbindung schlägt fehl](#problem-1-datenbankverbindung-schlägt-fehl)
   - [Problem 2: Datei nicht gefunden](#problem-2-datei-nicht-gefunden)
   - [Problem 3: Falsche Parameter-ID](#problem-3-falsche-parameter-id)
   - [Problem 4: Speicherprobleme](#problem-4-speicherprobleme)
   - [Problem 5: Falsche IDrun](#problem-5-falsche-idrun)
7. [Tipps und Tricks](#tipps-und-tricks)

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

**Schnellreferenz:** Siehe auch [QUICK_REFERENCE.md - IDrun finden](QUICK_REFERENCE.md#idrun-finden)

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

**Schnellreferenz:** Siehe auch [QUICK_REFERENCE.md - OPTIONS.DAT](QUICK_REFERENCE.md#optionsdat---wichtige-einstellungen)

Jedes Modul hat eine `OPTIONS.DAT` Datei. Diese Datei enthält **alle Einstellungen** für die Simulation.

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

### Beispiel: Komplette OPTIONS.DAT

```
Runtime options worldqual

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
  3 WaterGAP3 as
  4 WaterGAP3 au
  5 WaterGAP3 na
  6 WaterGAP3 sa
Value: 2

5. MyHost
Value: localhost

6. MyUser
Value: worldqual

7. MyPassword
Value: mein_passwort

8. InputType
  0: Daten aus der Datenbank einlesen
  1: Daten aus UNF-Dateien einlesen
Value: 1

9. input_dir
Value: /home/user/watergap_data/africa
```

---

## Datenbank verstehen

### Übersicht über die Datenbanken

**Schnellreferenz:** Siehe auch [QUICK_REFERENCE.md - Datenbank-Abfragen](QUICK_REFERENCE.md#datenbank-abfragen)

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
- [README.md](README.md) - Übersicht und Einführung
- [QUICK_REFERENCE.md](QUICK_REFERENCE.md) - Schnellreferenz
- [STRUCTURE.md](STRUCTURE.md) - Projektstruktur
- [INDEX.md](INDEX.md) - Dokumentations-Übersicht
