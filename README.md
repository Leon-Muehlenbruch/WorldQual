# WorldQual

**C++ Wasserqualitätsmodell für Flusssysteme**

Ein Modell zur Simulation von Wasserqualität in Flüssen, entwickelt für globale und regionale Analysen.

---

## Schnellstart

```bash
# 1. Repository klonen
git clone https://github.com/Leon-Muehlenbruch/WorldQual.git
cd WorldQual

# 2. Dependencies installieren
./install_dependencies.sh

# 3. Kompilieren
cd src/worldqual
make clean && make

# 4. Ausführen
./worldqual 1 startjahr endjahr
```
---

##  Dokumentation

**Online:** https://leon-muehlenbruch.github.io/WorldQual/

---

## Voraussetzungen

### Erforderlich:
- **C++ Compiler** (g++ oder clang++)
- **MySQL 8.0+** (Datenbank)
- **MySQL++** (C++ MySQL Connector)
- **Make** (Build-Tool)

### Automatische Installation (macOS/Linux):
```bash
./install_dependencies.sh
```

### Manuelle Installation:

**macOS:**
```bash
# Xcode Command Line Tools installieren
xcode-select --install

# Dependencies installieren
brew install mysql@8.0 mysql-connector-c++
brew services start mysql@8.0
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get update
sudo apt-get install build-essential mysql-server libmysqlclient-dev libmysql++-dev
sudo systemctl start mysql
```

**Windows:**
- **Empfohlen:** WSL (Windows Subsystem for Linux) + Ubuntu, dann Linux-Anweisungen folgen
- **Alternativ:** MSYS2/MinGW oder Visual Studio (siehe [requirements.txt](requirements.txt) für Details)

---

## Module

WorldQual besteht aus mehreren unabhängigen Modulen:

| Modul | Beschreibung | Pfad |
|-------|--------------|------|
| **worldqual** | Hauptprogramm | `src/worldqual/` |
| **wq_load** | Nährstoffeinträge | `src/wq_load/` |
| **wq_stat** | Statistische Auswertung | `src/wq_stat/` |
| **MapRiverQuality** | Visualisierung | `src/MapRiverQuality/` |
| **water_temperature** | Wassertemperatur | `src/water_temperature/` |
| **find_river_cells** | Flusszellen-Identifikation | `src/find_river_cells/` |

Jedes Modul kann separat kompiliert werden:
```bash
cd src/[modulname]
make clean && make
```

---

## Verwendung

### Basis-Aufruf:
```bash
./worldqual IDrun startYear endYear
```

### Parameter:
- `IDrun`: Run-ID aus der Datenbank
- `startYear`: Startjahr der Simulation
- `endYear`: Endjahr der Simulation

### Beispiel:
```bash
./worldqual 1 2000 2010
```

### Mit Options-Datei:
```bash
./worldqual 1 2000 2010 -o my_options.dat
```

---

## Datenbank einrichten

WorldQual benötigt eine MySQL-Datenbank mit Eingabedaten:

```sql
CREATE DATABASE worldqual_db;
CREATE USER 'worldqual_user'@'localhost' IDENTIFIED BY 'password';
GRANT ALL PRIVILEGES ON worldqual_db.* TO 'worldqual_user'@'localhost';
FLUSH PRIVILEGES;
```

Konfiguration in `src/worldqual/OPTIONS.DAT`:
```
HOST          localhost
USER          worldqual_user
PASSWORD      your_password
DATABASE      worldqual_db
```

---

## 🛠️ Entwicklung

### Dokumentation generieren:
```bash
cd docs
doxygen Doxyfile
open html/index.html
```

### Alle Module kompilieren:
```bash
for dir in src/*/; do
    if [ -f "${dir}makefile" ]; then
        (cd "$dir" && make clean && make)
    fi
done
```

### Tests ausführen:
```bash
cd src/worldqual
./worldqual 1 2000 2010
```

---

## Weitere Ressourcen

- **Dokumentation:** https://leon-muehlenbruch.github.io/WorldQual/
- **GitHub:** https://github.com/Leon-Muehlenbruch/WorldQual
- **Issues:** https://github.com/Leon-Muehlenbruch/WorldQual/issues

---

## AutorInnen und Kontakt

- **Ursprüngliche Entwicklung:** Ellen Teichert (2007)
- **Weiterentwicklung:** kynast (2015-2018)
- **Dokumentation & Build System:** Leon Mühlenbruch (2026)

Für Kontaktaufnahem kontaktieren Sie [email hier].

---

## Lizenz

[Lizenz hier angeben]

