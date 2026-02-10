# WorldQual - Globales Wasserqualitäts-Modellierungssystem

**Dokumentations-Übersicht:** [INDEX.md](index.md) | [Schnellreferenz](user/schnellreferenz.md) | [Detaillierte Anleitung](user/anleitung.md) | [Projektstruktur](user/projektstruktur.md)

## Inhaltsverzeichnis

- [Was macht WorldQual?](#was-macht-worldqual)
- [Schnellstart (Quick Start)](#schnellstart-quick-start)
- [Detaillierte Anleitung](#detaillierte-anleitung)
- [Module im Detail](#module-im-detail)
- [Häufige Fragen (FAQ)](#häufige-fragen-faq)
- [Systemanforderungen](#systemanforderungen)
- [Installation](#installation)
- [Weitere Hilfe](#weitere-hilfe)

## Was macht WorldQual?

WorldQual ist ein Computerprogramm, das **vorhersagt, wie sauber oder verschmutzt Wasser in Flüssen weltweit ist**. 

### Einfach erklärt:

Stellen Sie sich vor, Sie möchten wissen:
- **Wie viel Schadstoffe** sind in einem Fluss?
- **Woher kommen** diese Schadstoffe? (z.B. aus Kläranlagen, Landwirtschaft, Industrie)
- **Wie verändern sich** die Werte, wenn sich die Bedingungen ändern? (z.B. mehr Bevölkerung, Klimawandel)

WorldQual beantwortet diese Fragen durch **mathematische Simulationen**. Es berechnet:
1. **Einträge**: Wie viele Schadstoffe gelangen in die Flüsse (aus verschiedenen Quellen)
2. **Transport**: Wie werden die Schadstoffe im Fluss transportiert?
3. **Abbau**: Wie schnell werden Schadstoffe abgebaut oder verdünnt?
4. **Ergebnis**: Welche Konzentration hat das Wasser am Ende?

### Was kann WorldQual berechnen?

WorldQual kann verschiedene **Wasserqualitätsparameter** simulieren:

| Parameter | Was bedeutet das? | Warum wichtig? |
|-----------|-------------------|----------------|
| **BOD** (Biochemischer Sauerstoffbedarf) | Wie viel Sauerstoff wird verbraucht, um organische Stoffe abzubauen | Zeigt Verschmutzung durch Abwasser an |
| **Salz** (TDS) | Gesamter Salzgehalt im Wasser | Wichtig für Trinkwasser und Bewässerung |
| **Fäkalcoliforme Bakterien** (FC) | Bakterien aus Fäkalien | Gesundheitsrisiko, zeigt Verunreinigung an |
| **Stickstoff** (TN) | Gesamtstickstoff | Kann zu Algenblüte führen, aus Dünger |
| **Phosphor** (TP) | Gesamtphosphor | Kann zu Algenblüte führen, aus Dünger |

### Typische Anwendungen

- **Forschung**: Wie verändert sich die Wasserqualität durch Klimawandel?
- **Planung**: Was passiert, wenn sich Bevölkerungsdichte oder Landnutzung ändert?
- **Validierung**: Stimmen unsere Vorhersagen mit gemessenen Daten überein?
- **Szenarien**: Vergleich verschiedener Zukunftsszenarien

---

## Voraussetzungen - Schritt für Schritt

Bevor Sie WorldQual nutzen können, müssen Sie drei Dinge einrichten. Diese Anleitung erklärt jeden Schritt detailliert, als ob Sie niemanden hätten, den Sie fragen können.

### 1. MySQL-Datenbank einrichten

#### Was ist MySQL?
MySQL ist ein Datenbankserver, der die Daten für WorldQual speichert (Projekte, Szenarien, Simulationsergebnisse, etc.).

#### Schritt 1.1: MySQL installieren

**Linux (Ubuntu/Debian):**
```bash
# MySQL Server installieren
sudo apt-get update
sudo apt-get install mysql-server

# MySQL starten
sudo systemctl start mysql
sudo systemctl enable mysql  # Startet automatisch beim Booten
```

**macOS:**
```bash
# Mit Homebrew installieren
brew install mysql@8.0
brew services start mysql@8.0
```

**Prüfen ob MySQL läuft:**
```bash
sudo systemctl status mysql  # Linux
# oder
brew services list | grep mysql  # macOS
```

#### Schritt 1.2: MySQL konfigurieren

**Erste Einrichtung (nur beim ersten Start):**
```bash
# Sicherheits-Skript ausführen (Linux)
sudo mysql_secure_installation

# Oder direkt als root einloggen (macOS)
mysql -u root
```

**Benutzer für WorldQual erstellen:**
```sql
-- In MySQL einloggen
mysql -u root -p

-- Benutzer erstellen
CREATE USER 'worldqual'@'localhost' IDENTIFIED BY 'ihr_passwort_hier';

-- Berechtigungen geben
GRANT ALL PRIVILEGES ON *.* TO 'worldqual'@'localhost';
FLUSH PRIVILEGES;

-- Testen
mysql -u worldqual -p
-- Passwort eingeben, dann sollte es funktionieren
```

**Wichtig:** Merken Sie sich:
- Benutzername: `worldqual` (oder wie Sie es genannt haben)
- Passwort: Das, was Sie oben eingegeben haben
- Host: `localhost` (wenn lokal) oder die IP-Adresse des Servers

#### Schritt 1.3: Datenbankstruktur einrichten

WorldQual benötigt spezielle Datenbanktabellen. Diese müssen Sie entweder:
- **Von einem bestehenden Projekt kopieren**, oder
- **Neu erstellen** (benötigt Datenbank-Schema-Dateien)

**Falls Sie ein bestehendes Projekt haben:**
```bash
# Datenbank exportieren
mysqldump -u root -p wq_general > wq_general_backup.sql

# Auf neuem System importieren
mysql -u root -p < wq_general_backup.sql
```

**Falls Sie neu starten:**
Sie benötigen die Datenbank-Schema-Dateien (normalerweise `.sql` Dateien). Diese sollten vom Projekt bereitgestellt werden.

**Prüfen ob Datenbanken existieren:**
```sql
mysql -u worldqual -p
SHOW DATABASES;
-- Sollten Sie sehen: wq_general (und eventuell weitere)
```

### 2. WaterGAP-Daten beschaffen

#### Was sind WaterGAP-Daten?
WaterGAP (Water Global Assessment and Prognosis) ist ein hydrologisches Modell. WorldQual benötigt dessen Output-Daten:
- Abfluss (Q_out) - wie viel Wasser fließt im Fluss?
- Runoff - wie viel Wasser fließt ab?
- Weitere hydrologische Daten

#### Schritt 2.1: Datenquelle finden

**Option A: Sie haben bereits Zugriff**
- Fragen Sie Ihren Betreuer/Kollegen nach dem Pfad zu den Daten
- Daten sind normalerweise in UNF-Dateien (binäres Format)

**Option B: Daten müssen beschafft werden**
- Kontaktieren Sie die WaterGAP-Projektgruppe
- Oder nutzen Sie öffentlich verfügbare WaterGAP-Daten (falls verfügbar)

#### Schritt 2.2: Datenformat verstehen

WaterGAP-Daten kommen als **UNF-Dateien** (Unformatted):
- Format: Binär (nicht lesbar als Text)
- Struktur: Grid-Daten (0.5° x 0.5° Auflösung)
- Zeitreihen: Monatlich (12 Werte pro Jahr) oder jährlich

**Typische Dateinamen:**
```
G_Q_out_m3_2010.12.UNF0    # Monatlicher Abfluss für 2010
G_RUNOFF_TOTAL_mm_2010.12.UNF0  # Monatlicher Runoff für 2010
```

#### Schritt 2.3: Daten prüfen

**Prüfen ob Dateien existieren:**
```bash
# In das Verzeichnis wechseln
cd /pfad/zu/watergap/daten

# Dateien anzeigen
ls -lh G_*.UNF0

# Sollten Sie sehen:
# - Dateien für verschiedene Jahre
# - Verschiedene Parameter (Q_out, RUNOFF, etc.)
```

**Wichtig:** 
- Notieren Sie den vollständigen Pfad (z.B. `/home/user/watergap_data/africa`)
- Dieser Pfad wird später in `OPTIONS.DAT` benötigt

### 3. Module kompilieren

#### Was bedeutet "kompilieren"?
Kompilieren bedeutet, den C++-Quellcode in ausführbare Programme umzuwandeln.

#### Schritt 3.1: Abhängigkeiten installieren

**C++ Compiler:**
```bash
# Linux
sudo apt-get install build-essential g++

# macOS (Xcode Command Line Tools)
xcode-select --install
```

**MySQL++ Bibliothek (C++ MySQL Connector):**
```bash
# Linux (Ubuntu/Debian)
sudo apt-get install libmysql++-dev libmysqlclient-dev

# macOS
brew install mysql-connector-c++
```

**Prüfen ob installiert:**
```bash
g++ --version  # Sollte Version anzeigen
mysql_config --version  # Sollte MySQL-Version anzeigen
```

#### Schritt 3.2: Pfade in Makefiles anpassen

**Problem:** Die Makefiles müssen wissen, wo MySQL++ installiert ist. Dies kann je nach System unterschiedlich sein.

**Schritt 1: MySQL++ Pfade finden**
```bash
# Header-Dateien finden
find /usr -name "mysql++.h" 2>/dev/null
# Oder
find /usr/local -name "mysql++.h" 2>/dev/null

# Bibliotheken finden
find /usr -name "libmysqlpp.so" 2>/dev/null
# Oder
find /usr/local -name "libmysqlpp.so" 2>/dev/null
```

**Schritt 2: Makefile anpassen**

Öffnen Sie ein Makefile (z.B. `water_temperature/makefile`):

```makefile
# Aktuelle Zeile (kann unterschiedlich sein):
INCDIROPTS  = -L/usr/lib/libmysqlpp.so -I/usr/include/mysql  -I/usr/include/mysql++ ...

# Anpassen falls nötig:
INCDIROPTS  = -L/usr/local/lib/libmysqlpp.so -I/usr/include/mysql  -I/usr/local/include/mysql++ ...
```

**Typische Pfade:**
- Linux: `/usr/include/mysql++`, `/usr/lib/libmysqlpp.so`
- macOS: `/usr/local/include/mysql++`, `/usr/local/lib/libmysqlpp.so`

#### Schritt 3.3: Module kompilieren

**Reihenfolge ist wichtig! Zuerst gemeinsame Module:**

```bash
# 1. general_function (wird von allen anderen benötigt)
cd general_function
make clean
make all
# Sollte keine Fehler geben

# 2. options (wird auch von allen benötigt)
cd ../options
make clean
make all
# Sollte keine Fehler geben
```

**Dann spezifische Module:**

```bash
# 3. water_temperature
cd ../water_temperature
make clean
make all
# Prüfen: ls -la water_temperature (sollte ausführbare Datei sein)

# 4. fill_worldqual_load
cd ../fill_worldqual_load
make clean
make all
# Prüfen: ls -la fill_worldqual_load

# 5. worldqual (Hauptprogramm)
cd ../worldqual
make clean
make all
# Prüfen: ls -la worldqual

# 6. Weitere Module nach Bedarf...
```

#### Schritt 3.4: Kompilierungsfehler beheben

**Häufige Fehler:**

**Fehler: "mysql++.h: No such file or directory"**
- Lösung: MySQL++ Header-Pfad in Makefile anpassen (siehe Schritt 3.2)

**Fehler: "cannot find -lmysqlpp"**
- Lösung: Bibliotheks-Pfad in Makefile anpassen

**Fehler: "undefined reference to mysqlpp::..."**
- Lösung: Bibliothek nicht gefunden, Pfade prüfen

**Test ob Kompilierung erfolgreich:**
```bash
# Prüfen ob ausführbare Dateien existieren
ls -la worldqual fill_worldqual_load water_temperature

# Sollten ausführbar sein (grün, x-Berechtigung)
# Test-Ausführung (sollte Hilfe anzeigen oder Fehler wegen fehlender Parameter)
./worldqual
```

### Checkliste: Alles bereit?

Prüfen Sie vor dem Start:

- [ ] MySQL läuft: `systemctl status mysql` (Linux) oder `brew services list | grep mysql` (macOS)
- [ ] MySQL-Benutzer existiert: `mysql -u worldqual -p` funktioniert
- [ ] Datenbanken existieren: `SHOW DATABASES;` zeigt `wq_general`
- [ ] WaterGAP-Daten vorhanden: `ls /pfad/zu/daten/*.UNF0` zeigt Dateien
- [ ] Module kompiliert: `ls -la worldqual fill_worldqual_load` zeigt ausführbare Dateien
- [ ] Pfade bekannt: Sie wissen wo MySQL++ installiert ist

**Wenn alles erledigt ist, können Sie mit dem Schnellstart beginnen!**

---

## Schnellstart (Quick Start)

### Für neue Nutzer: Erste Schritte

**Voraussetzung:** Sie haben die obigen Schritte abgeschlossen:
- ✅ MySQL-Datenbank eingerichtet und getestet
- ✅ WaterGAP-Daten verfügbar und Pfad notiert
- ✅ Alle Module erfolgreich kompiliert

### Schritt 1: Konfiguration vorbereiten

Jedes Modul braucht eine `OPTIONS.DAT` Datei. Kopieren Sie eine Beispiel-Datei:

**Tipp:** Detaillierte Erklärung der Konfiguration finden Sie in [DOCUMENTATION.md - Konfiguration verstehen](user/anleitung.md#konfiguration-verstehen)

```bash
cd fill_worldqual_load
cp OPTIONS.DAT OPTIONS.DAT.mein_projekt
```

**Wie öffnet man OPTIONS.DAT?**

```bash
# Mit nano (einfacher Editor)
nano OPTIONS.DAT

# Navigation in nano:
# - Pfeiltasten: Bewegen
# - Strg+O: Speichern (dann Enter drücken)
# - Strg+X: Beenden
```

**Was genau ändern?**

Die Datei enthält Zeilen wie:
```
Value: 2
Value: 91
Value: localhost
Value: worldqual
Value: passwort
Value: /pfad/zu/daten
```

**Ändern Sie die Werte nach "Value:":**

- `Value: 2` → Ihre Projektnummer (z.B. 2 für WWQA)
- `Value: 91` → Ihre Szenario-Nummer (z.B. 91)
- `Value: 2` → Region (1=EU, 2=AF, 3=AS, 4=AU, 5=NA, 6=SA)
- `Value: localhost` → MySQL-Server (localhost oder IP-Adresse)
- `Value: worldqual` → MySQL-Benutzername
- `Value: passwort` → MySQL-Passwort
- `Value: /pfad/zu/daten` → Vollständiger Pfad zu WaterGAP-Daten

**Wichtig:** 
- Ändern Sie NUR die Werte nach "Value:"
- Lassen Sie den Rest der Datei unverändert
- Speichern Sie die Datei (Strg+O, dann Enter)

**Detaillierte Erklärung:** Siehe [DOCUMENTATION.md - Konfiguration verstehen](user/anleitung.md#konfiguration-verstehen)

### Schritt 2: Einträge berechnen

Berechnen Sie, wie viele Schadstoffe in die Flüsse gelangen:

**Mehr Details:** Siehe [DOCUMENTATION.md - fill_worldqual_load](user/anleitung.md#fill_worldqual_load---einträge-berechnen)

```bash
cd fill_worldqual_load
# OPTIONS.DAT anpassen!
./fill_worldqual_load 2010 2010
```

**Was passiert hier?**
- Das Programm liest Daten über Bevölkerungsdichte, Abwasserbehandlung, Landnutzung, etc.
- Es berechnet für jede Grid-Zelle (ca. 0.5° x 0.5°), wie viele Schadstoffe pro Monat eingetragen werden
- Die Ergebnisse werden in der Datenbank gespeichert

**Dauer:** Abhängig von Region und Zeitraum (kann Minuten bis Stunden dauern)

### Schritt 3: Hauptsimulation ausführen

Simulieren Sie die Wasserqualität im Fluss:

**Mehr Details:** Siehe [DOCUMENTATION.md - worldqual](user/anleitung.md#worldqual---hauptsimulation)

```bash
cd ../worldqual

# 1. IDrun aus Datenbank holen (siehe unten)
# 2. OPTIONS.DAT anpassen (wie Schritt 1)
# 3. Simulation starten
./worldqual 403100091 2010 2010
```

**Wie finde ich die IDrun?**

```sql
# In MySQL einloggen
mysql -u worldqual -p

# Datenbank wählen
USE wq_general;

# IDrun suchen
SELECT IDrun, runName FROM _runlist 
WHERE parameter_id=2 AND IDScen=91;
# Ergebnis z.B.: 403100091
```

**Was wenn keine IDrun existiert?**
Sie müssen eine IDrun in der Datenbank anlegen. Dies ist komplex - siehe [DOCUMENTATION.md - IDrun finden](user/anleitung.md#schritt-5-hauptsimulation) für Details.

**Was passiert hier?**
- Das Programm liest die Einträge aus Schritt 2
- Es simuliert, wie die Schadstoffe im Fluss transportiert werden
- Es berechnet Abbauprozesse (z.B. Bakterien sterben ab, Nährstoffe werden abgebaut)
- Ergebnis: Konzentrationen für jede Grid-Zelle, jeden Monat

**Dauer:** Kann Stunden bis Tage dauern (abhängig von Region und Zeitraum)

### Schritt 4: Ergebnisse analysieren

Vergleichen Sie Ihre Simulation mit gemessenen Daten:

```bash
cd ../wq_stat_stations
./wq_stat_stations 403100091 2010 2010
```

**Was passiert hier?**
- Das Programm findet Messstationen in der Datenbank
- Es vergleicht Ihre simulierten Werte mit den gemessenen Werten
- Es berechnet statistische Kennzahlen (z.B. R², RMSE)
- Ergebnis: Textdateien mit Vergleichsdaten

### Schritt 5: Visualisierung (optional)

Erstellen Sie Diagramme mit R:

**Voraussetzung: R installieren**

```bash
# Linux
sudo apt-get install r-base

# macOS
brew install r
```

**R-Pakete installieren:**
```r
# R öffnen (im Terminal: R)
install.packages(c("ggplot2", "gridExtra"))
```

**R-Skripte nutzen:**

```r
# 1. R öffnen
R

# 2. In Verzeichnis wechseln
setwd("R-scripte")

# 3. start.R anpassen (mit Texteditor oder file.edit("start.R"))
# Wichtige Variablen:
# - input.path: Pfad zu Statistik-Dateien
# - current_dir: Wo Grafiken gespeichert werden
# - unit: Einheit (z.B. "cfu/100ml" für FC)
# - log_scale: "y" für logarithmische y-Achse

# 4. Skripte ausführen
source("start.R")
source("station.R")  # Diagramme für einzelne Stationen
```

**Detaillierte Anleitung:** Siehe [DOCUMENTATION.md - Visualisierung](user/anleitung.md#schritt-8-visualisierung)

---

## Detaillierte Anleitung

### Voraussetzungen prüfen

Bevor Sie starten, prüfen Sie:

1. **MySQL-Datenbank läuft?**
   ```bash
   systemctl status mysql
   # oder
   mysql -u username -p -e "SHOW DATABASES;"
   ```

2. **Module kompiliert?**
   ```bash
   cd general_function && make all
   cd ../options && make all
   cd ../water_temperature && make all
   # etc.
   ```

3. **Input-Daten vorhanden?**
   - WaterGAP-Daten (Abfluss, Runoff, etc.)
   - Bevölkerungsdaten
   - Landnutzungsdaten
   - Weitere je nach Parameter

### Konfiguration verstehen

Die `OPTIONS.DAT` Datei ist das Herzstück der Konfiguration. Hier die wichtigsten Einstellungen:

#### Projekt und Szenario

```
1. project_id
   Welches Projekt verwenden Sie?
   1: CESR
   2: WWQA
   3: CESR Sensitivity Analysis
   4: test
Value: 2

2. IDScen
   Welches Szenario simulieren Sie?
   (Nummer aus Datenbanktabelle wq_general._szenario)
Value: 91
```

**Wo finde ich die richtigen Nummern?**
- In der MySQL-Datenbank: `SELECT * FROM wq_general._project;`
- In der MySQL-Datenbank: `SELECT * FROM wq_general._szenario;`

#### Region und Version

```
3. IDVersion
   Welche WaterGAP-Version?
   2: WaterGAP2
   3: WaterGAP3
Value: 3

4. IDReg
   Welche Region?
   1: Europa (eu)
   2: Afrika (af)
   3: Asien (as)
   4: Australien (au)
   5: Nordamerika (na)
   6: Südamerika (sa)
Value: 2
```

#### Datenbankverbindung

```
5. MyHost
   MySQL-Server-Adresse
Value: localhost
   # oder IP-Adresse, z.B. REMOVED_HOST

6. MyUser
   MySQL-Benutzername
Value: worldqual

7. MyPassword
   MySQL-Passwort
Value: mein_passwort
```

#### Input-Daten

```
8. InputType
   Woher kommen die Daten?
   0: Aus der Datenbank lesen
   1: Aus UNF-Dateien lesen
Value: 1

9. input_dir
   Pfad zu den UNF-Dateien (nur bei InputType=1)
Value: /pfad/zu/meinen/daten
```

### Kompletter Workflow: Beispiel

Hier ein **vollständiges Beispiel** für eine Simulation:

#### Beispiel: Simulation für Afrika, Jahr 2010, Parameter FC (Fäkalcoliforme)

**Vorbereitung:**
1. Datenbank prüfen: Projekt 2 (WWQA), Szenario 91 existiert
2. Input-Daten vorhanden: WaterGAP3-Daten für Afrika
3. Module kompiliert

**Schritt 1: Einträge berechnen**

```bash
cd fill_worldqual_load

# OPTIONS.DAT anpassen:
# project_id = 2
# IDScen = 91
# IDReg = 2 (Afrika)
# Parameter-ID für FC ist 2 (wird in Datenbank gespeichert)

./fill_worldqual_load 2010 2010
```

**Ausgabe:**
```
Program fill_worldqual_load started at: 2026-02-02 10:00:00
project_id = 2 (WWQA)
Database: wwqa_wq_load_af connection ok...
Berechne Einträge für Parameter 2 (FC)...
Jahr 2010...
Fertig!
```

**Schritt 2: Wassertemperatur (optional, aber empfohlen)**

```bash
cd ../water_temperature

# OPTIONS.DAT anpassen:
# IDTemp = 100 (aus Datenbank)

./water_temperature 2010 2010
```

**Schritt 3: Hauptsimulation**

```bash
cd ../worldqual

# OPTIONS.DAT anpassen:
# IDrun muss aus Datenbank geholt werden:
# mysql> SELECT IDrun FROM wq_general._runlist WHERE parameter_id=2 AND IDScen=91;
# Ergebnis z.B.: 403100091

./worldqual 403100091 2010 2010
```

**Ausgabe:**
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

**Schritt 4: Statistik**

```bash
cd ../wq_stat_stations

# STAT.DAT anpassen falls nötig

./wq_stat_stations 403100091 2010 2010
```

**Ergebnis:** Textdateien mit statistischen Vergleichen

**Schritt 5: Visualisierung**

```r
# In R
setwd("R-scripte")

# start.R öffnen und anpassen:
input.path <- "/pfad/zu/statistik/dateien"
input.filename <- ""  # leer = alle Dateien
unit <- "cfu/100ml"  # Einheit für FC
log_scale <- "y"     # logarithmische y-Achse

source("start.R")
source("station.R")  # Erstellt Diagramme
```

---

## Module im Detail

### Kern-Module (müssen Sie verstehen)

#### 1. fill_worldqual_load
**Was macht es?** Berechnet, wie viele Schadstoffe in die Flüsse gelangen.

**Wann brauche ich es?** **IMMER** vor der Hauptsimulation!

**Wie funktioniert es?**
- Liest Daten über:
  - Bevölkerungsdichte → Abwassermengen
  - Abwasserbehandlung → Wie viel wird gereinigt?
  - Landnutzung → Dünger, Erosion
  - Geologie → Natürliche Quellen
- Berechnet für jede Grid-Zelle und jeden Monat die Einträge
- Speichert in Datenbank: `wwqa_wq_load_[region].calc_cell_month_load_*`

**Beispiel:**
```bash
./fill_worldqual_load 1990 2010
# Berechnet Einträge für Jahre 1990-2010
```

#### 2. worldqual (Hauptsimulation)
**Was macht es?** Simuliert die Wasserqualität im Fluss.

**Wann brauche ich es?** Nach `fill_worldqual_load`.

**Wie funktioniert es?**
- Liest Einträge aus Schritt 1
- Liest hydrologische Daten (Abfluss, Runoff)
- Simuliert:
  1. **Transport**: Schadstoffe werden flussabwärts transportiert
  2. **Verdünnung**: Durch Zuflüsse wird Wasser verdünnt
  3. **Abbau**: Bakterien sterben ab, Nährstoffe werden abgebaut
  4. **Temperatur**: Beeinflusst Abbauprozesse
- Ergebnis: Konzentrationen für jede Zelle, jeden Monat

**Beispiel:**
```bash
./worldqual 403100091 1990 2010
# Simuliert Jahre 1990-2010 für Run 403100091
```

#### 3. wq_stat_stations
**Was macht es?** Vergleicht Simulation mit Messungen.

**Wann brauche ich es?** Nach der Simulation, zur Validierung.

**Wie funktioniert es?**
- Findet Messstationen in der Datenbank
- Liest gemessene Werte
- Liest simulierte Werte
- Berechnet: R², RMSE, Bias, etc.
- Erstellt Vergleichsdateien für Visualisierung

**Beispiel:**
```bash
./wq_stat_stations 403100091 1990 2010
# Vergleicht für Jahre 1990-2010
```

### Unterstützungs-Module

#### water_temperature
**Was macht es?** Berechnet Wassertemperatur aus Lufttemperatur.

**Wann brauche ich es?** Wenn Temperatur für Abbauprozesse wichtig ist (z.B. bei Bakterien).

#### create_scenarios
**Was macht es?** Erstellt neue Szenarien basierend auf bestehenden.

**Wann brauche ich es?** Wenn Sie viele ähnliche Szenarien erstellen wollen.

#### MapRiverQuality
**Was macht es?** Erstellt Karten mit Wasserqualitätsklassen.

**Wann brauche ich es?** Für Visualisierung und Präsentationen.

### R-Skripte

Die R-Skripte erstellen **Diagramme und Grafiken** aus den Ergebnissen.

**Wichtigste Skripte:**
- `start.R`: Konfiguration (MUSS angepasst werden!)
- `station.R`: Diagramme für einzelne Stationen
- `stations_scatterplot.R`: Streudiagramme (Simulation vs. Messung)
- `RiverSection.R`: Diagramme für Flussabschnitte

**Wie nutze ich sie?**
1. `start.R` öffnen
2. Variablen anpassen (siehe Kommentare)
3. Skript ausführen: `source("start.R")`
4. Dann andere Skripte ausführen

---

## Häufige Fragen (FAQ)

### Wie finde ich die richtige IDrun?

Die IDrun ist eine eindeutige Nummer für eine Simulation. Sie finden sie in der Datenbank:

**Schnellreferenz:** Siehe [QUICK_REFERENCE.md - IDrun finden](user/schnellreferenz.md#idrun-finden)

```sql
SELECT IDrun, runName, parameter_id, IDScen 
FROM wq_general._runlist 
WHERE parameter_id=2 AND IDScen=91;
```

Die IDrun setzt sich zusammen aus: `parameter_id` + `IDScen` + weitere Nummern.

### Wie lange dauert eine Simulation?

- **Einträge berechnen**: Minuten bis Stunden (abhängig von Region)
- **Hauptsimulation**: Stunden bis Tage (abhängig von Region und Zeitraum)
- **Statistik**: Minuten bis Stunden

**Tipp:** Testen Sie immer zuerst mit einem Jahr und kleiner Region!

### Was mache ich bei Fehlern?

**Detaillierte Fehlerbehebung:** Siehe [DOCUMENTATION.md - Fehlerbehebung](user/anleitung.md#fehlerbehebung)

1. **Datenbankverbindung schlägt fehl**
   - MySQL läuft? `systemctl status mysql`
   - Passwort richtig? In OPTIONS.DAT prüfen
   - Berechtigungen? MySQL-User hat Zugriff?

2. **Datei nicht gefunden**
   - Pfad richtig? `input_dir` in OPTIONS.DAT prüfen
   - Dateien existieren? `ls /pfad/zu/dateien`
   - Rechte? `chmod` prüfen

3. **Falsche Parameter-ID**
   - Parameter existiert? `SELECT * FROM wq_general._parameter;`
   - Parameter für Projekt konfiguriert?

4. **Speicherprobleme**
   - Zu große Region? Kleinere Region testen
   - Zu langer Zeitraum? Ein Jahr testen
   - RAM prüfen: `free -h`

### Kann ich mehrere Simulationen parallel laufen lassen?

**Nein!** Die Programme schreiben in die gleiche Datenbank. Parallele Ausführung kann zu Konflikten führen.

### Wie exportiere ich Ergebnisse?

Ergebnisse sind in der Datenbank. Sie können sie exportieren:

```sql
SELECT * FROM wwqa_worldqual_af.concentration 
WHERE IDrun=403100091 AND year=2010;
```

Oder als CSV:
```sql
SELECT * FROM wwqa_worldqual_af.concentration 
WHERE IDrun=403100091 AND year=2010
INTO OUTFILE '/tmp/ergebnisse.csv';
```

---

## Systemanforderungen

### Software

- **C++ Compiler** (g++ mit C++11)
- **MySQL** Datenbankserver (5.5+)
- **MySQL++** Bibliothek
- **R** (3.0+) mit ggplot2, gridExtra
- **Make**

### Hardware

- **RAM**: Mindestens 8 GB (16+ GB empfohlen für große Regionen)
- **Festplatte**: Abhängig von Datenmenge (mehrere GB)
- **CPU**: Multi-Core empfohlen (Programm nutzt aber nur einen Kern)

### Daten

- WaterGAP2/3 hydrologische Daten
- Bevölkerungsdaten
- Landnutzungsdaten
- Weitere je nach Parameter

---

## Installation

**Wichtig:** Die detaillierte Schritt-für-Schritt Installation finden Sie oben im Abschnitt [Voraussetzungen - Schritt für Schritt](#voraussetzungen---schritt-für-schritt).

### Kurzübersicht

1. **MySQL installieren und konfigurieren** (siehe oben, Abschnitt 1)
2. **WaterGAP-Daten beschaffen** (siehe oben, Abschnitt 2)
3. **Module kompilieren** (siehe oben, Abschnitt 3)

### Schnellreferenz für Erfahrene

**Abhängigkeiten installieren:**
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential g++ mysql-server libmysql++-dev libmysqlclient-dev r-base

# R-Pakete
R -e "install.packages(c('ggplot2', 'gridExtra'))"
```

**Module kompilieren:**
```bash
cd general_function && make clean && make all
cd ../options && make clean && make all
cd ../water_temperature && make clean && make all
cd ../fill_worldqual_load && make clean && make all
cd ../worldqual && make clean && make all
```

**Wichtig:** Pfade in Makefiles anpassen, falls MySQL++ nicht im Standardpfad ist! Siehe [Voraussetzungen - Schritt für Schritt](#voraussetzungen---schritt-für-schritt) für Details.

---

## Weitere Hilfe

**Für Nutzer:**
- **[Detaillierte Dokumentation](user/anleitung.md)** - Schritt-für-Schritt Anleitungen
- **[Schnellreferenz](user/schnellreferenz.md)** - Wichtige Befehle und Nummern
- **[Projektstruktur](user/projektstruktur.md)** - Verzeichnisse und Dateien
- **[Dokumentations-Index](index.md)** - Übersicht aller Dokumentationen
- **[Fehlerbehebung](user/anleitung.md#fehlerbehebung)** - Probleme lösen

**Für Entwickler:**
- **[Technische Dokumentation](index.md)** - Entwickler-Dokumentation
- **[Architektur](technical/ARCHITECTURE.md)** - Code-Struktur und Design
- **[Entwicklungsumgebung](technical/DEVELOPMENT.md)** - Build-System und Entwicklung
- **[Code-Standards](technical/CODING_STANDARDS.md)** - Coding-Konventionen
- **[Contributing](technical/CONTRIBUTING.md)** - Wie man beiträgt

---

**Version:** 1.0  
**Letzte Aktualisierung:** Februar 2026
