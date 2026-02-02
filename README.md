# WorldQual - Globales Wasserqualitäts-Modellierungssystem

**Dokumentations-Übersicht:** [INDEX.md](INDEX.md) | [Schnellreferenz](QUICK_REFERENCE.md) | [Detaillierte Anleitung](DOCUMENTATION.md) | [Projektstruktur](STRUCTURE.md)

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

## Schnellstart (Quick Start)

### Für neue Nutzer: Erste Schritte

**Voraussetzung:** Sie haben bereits:
- ✅ MySQL-Datenbank eingerichtet
- ✅ WaterGAP-Daten verfügbar
- ✅ Alle Module kompiliert

### Schritt 1: Konfiguration vorbereiten

Jedes Modul braucht eine `OPTIONS.DAT` Datei. Kopieren Sie eine Beispiel-Datei:

**Tipp:** Detaillierte Erklärung der Konfiguration finden Sie in [DOCUMENTATION.md - Konfiguration verstehen](DOCUMENTATION.md#konfiguration-verstehen)

```bash
cd fill_worldqual_load
cp OPTIONS.DAT OPTIONS.DAT.mein_projekt
```

Öffnen Sie die Datei und ändern Sie:
- `project_id`: Ihre Projektnummer (z.B. 2 für WWQA)
- `IDScen`: Ihre Szenario-Nummer (z.B. 91)
- `IDReg`: Region (1=EU, 2=AF, 3=AS, 4=AU, 5=NA, 6=SA)
- `MyHost`, `MyUser`, `MyPassword`: Ihre MySQL-Daten
- `input_dir`: Pfad zu Ihren Input-Dateien

### Schritt 2: Einträge berechnen

Berechnen Sie, wie viele Schadstoffe in die Flüsse gelangen:

**Mehr Details:** Siehe [DOCUMENTATION.md - fill_worldqual_load](DOCUMENTATION.md#fill_worldqual_load---einträge-berechnen)

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

**Mehr Details:** Siehe [DOCUMENTATION.md - worldqual](DOCUMENTATION.md#worldqual---hauptsimulation)

```bash
cd ../worldqual
# OPTIONS.DAT anpassen! IDrun aus Datenbank holen
./worldqual 403100091 2010 2010
```

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

```r
# In R öffnen
setwd("R-scripte")
source("start.R")
# Konfiguration in start.R anpassen (siehe unten)
source("station.R")  # Diagramme für einzelne Stationen
```

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

**Schnellreferenz:** Siehe [QUICK_REFERENCE.md - IDrun finden](QUICK_REFERENCE.md#idrun-finden)

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

**Detaillierte Fehlerbehebung:** Siehe [DOCUMENTATION.md - Fehlerbehebung](DOCUMENTATION.md#fehlerbehebung)

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

### 1. Abhängigkeiten installieren

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install g++ mysql-server libmysql++-dev r-base
```

**R-Pakete:**
```r
install.packages(c("ggplot2", "gridExtra"))
```

### 2. Datenbank einrichten

MySQL-Datenbank mit Tabellen einrichten (siehe Datenbank-Dokumentation).

### 3. Module kompilieren

```bash
# Zuerst gemeinsame Module
cd general_function && make all
cd ../options && make all

# Dann spezifische Module
cd ../water_temperature && make all
cd ../fill_worldqual_load && make all
# etc.
```

**Wichtig:** Pfade in Makefiles anpassen, falls MySQL++ nicht im Standardpfad ist!

---

## Weitere Hilfe

- **[Detaillierte Dokumentation](DOCUMENTATION.md)** - Schritt-für-Schritt Anleitungen
- **[Schnellreferenz](QUICK_REFERENCE.md)** - Wichtige Befehle und Nummern
- **[Projektstruktur](STRUCTURE.md)** - Verzeichnisse und Dateien
- **[Dokumentations-Index](INDEX.md)** - Übersicht aller Dokumentationen
- **[Fehlerbehebung](DOCUMENTATION.md#fehlerbehebung)** - Probleme lösen

---

**Version:** 1.0  
**Letzte Aktualisierung:** Februar 2026
