# WorldQual - Entwicklungsumgebung

Diese Dokumentation erklärt, wie Sie eine **Entwicklungsumgebung** für WorldQual einrichten und wie Sie Code entwickeln, testen und debuggen.

## Inhaltsverzeichnis

- [Entwicklungsumgebung einrichten](#entwicklungsumgebung-einrichten)
- [Build-System verstehen](#build-system-verstehen)
- [Code entwickeln](#code-entwickeln)
- [Debugging](#debugging)
- [Testing](#testing)
- [Versionierung](#versionierung)

## Entwicklungsumgebung einrichten

### Voraussetzungen

**Erforderliche Software:**
- C++ Compiler (g++ oder clang++) mit C++11 Support
- MySQL Server (für Entwicklungstests)
- MySQL++ Bibliothek
- Make
- Git (für Versionskontrolle)
- Texteditor oder IDE (Vim, Emacs, VS Code, CLion, etc.)

**Optional aber empfohlen:**
- Debugger (gdb)
- Valgrind (für Memory-Checks)
- cppcheck (für statische Code-Analyse)

### Schritt 1: Repository klonen

```bash
git clone https://github.com/Leon-Muehlenbruch/WorldQual.git
cd WorldQual
```

### Schritt 2: Entwicklungsumgebung konfigurieren

**IDE einrichten:**

**VS Code:**
```bash
# C++ Extension installieren
# MySQL Extension installieren (optional)
# Makefile Extension installieren (optional)
```

**CLion:**
- Öffnen Sie das Projekt-Verzeichnis
- CLion erkennt Makefiles automatisch
- CMakeLists.txt erstellen (optional, für bessere IDE-Unterstützung)

**Vim/Emacs:**
- C++ Syntax-Highlighting ist standardmäßig aktiviert
- Plugins für Autocomplete (optional)

### Schritt 3: MySQL++ Pfade konfigurieren

**Problem:** Makefiles müssen wissen, wo MySQL++ installiert ist.

**Lösung:** Erstellen Sie eine Konfigurationsdatei oder passen Sie Makefiles an.

**Option A: Makefiles anpassen (aktueller Ansatz)**

Jedes Makefile hat Zeilen wie:
```makefile
INCDIROPTS = -L/usr/lib/libmysqlpp.so -I/usr/include/mysql -I/usr/include/mysql++
```

**Pfade finden:**
```bash
# Header-Dateien
find /usr -name "mysql++.h" 2>/dev/null
find /usr/local -name "mysql++.h" 2>/dev/null

# Bibliotheken
find /usr -name "libmysqlpp.so" 2>/dev/null
find /usr/local -name "libmysqlpp.so" 2>/dev/null
```

**Option B: Umgebungsvariablen (empfohlen für Entwicklung)**

Erstellen Sie `config.mk` im Root:
```makefile
# config.mk - Zentrale Konfiguration
MYSQLPP_INCLUDE = /usr/local/include/mysql++
MYSQLPP_LIB = /usr/local/lib
MYSQL_INCLUDE = /usr/include/mysql
MYSQL_LIB = /usr/lib64/mysql
```

Dann in Makefiles einbinden:
```makefile
include ../../config.mk
INCDIROPTS = -L$(MYSQLPP_LIB) -I$(MYSQL_INCLUDE) -I$(MYSQLPP_INCLUDE) ...
```

### Schritt 4: Test-Datenbank einrichten

**Für Entwicklungstests:**

```sql
-- Test-Datenbank erstellen
CREATE DATABASE worldqual_dev;
CREATE USER 'worldqual_dev'@'localhost' IDENTIFIED BY 'dev_password';
GRANT ALL PRIVILEGES ON worldqual_dev.* TO 'worldqual_dev'@'localhost';
FLUSH PRIVILEGES;

-- Schema importieren (falls vorhanden)
mysql -u worldqual_dev -p worldqual_dev < schema.sql
```

## Build-System verstehen

### Makefile-Struktur

Jedes Modul hat ein eigenes Makefile mit folgender Struktur:

```makefile
# Compiler und Flags
CC = g++ -Wall -Wno-unused-but-set-variable
OPTIMIZE = -O3
#DEBUG = -g  # Für Debugging aktivieren

# Programmname
APPNAME = modulname

# Objektdateien (abhängig von anderen Modulen)
OBJECTS = modulname.o \
          ../general_function/timestring.o \
          ../general_function/general_functions.o \
          ../options/options.o

# Include- und Library-Pfade
INCDIROPTS = -L/pfad/lib -I/pfad/include ...
LNKLIB = -lmysqlpp -lmysqlclient

# Build-Regeln
$(APPNAME): $(OBJECTS)
	$(CC) $(INCDIROPTS) -o $(APPNAME) $(OBJECTS) $(LNKLIB)

.cpp.o:
	$(CC) -c $(INCDIROPTS) $(DEBUG) -o $*.o $*.cpp

all: clean
	@$(MAKE) $(APPNAME)
	echo all: make complete

clean:
	rm -f $(OBJECTS) $(APPNAME)
```

### Kompilierungsreihenfolge

**Wichtig:** Abhängigkeiten beachten!

```bash
# 1. Zuerst gemeinsame Bibliotheken
cd src/general_function && make all
cd ../options && make all

# 2. Dann Utility-Module
cd ../newton && make all

# 3. Dann alle anderen Module (können parallel sein)
cd ../water_temperature && make all
cd ../fill_worldqual_load && make all
cd ../worldqual && make all
# etc.
```

**Automatisiertes Build-Skript:**

Erstellen Sie `build_all.sh`:
```bash
#!/bin/bash
# Build-Skript für alle Module

set -e  # Bei Fehler abbrechen

MODULES=(
    "general_function"
    "options"
    "newton"
    "water_temperature"
    "fill_worldqual_load"
    "worldqual"
    "wq_stat_stations"
    # ... weitere Module
)

for module in "${MODULES[@]}"; do
    echo "Building $module..."
    cd "src/$module"
    make clean
    make all
    cd ../..
done

echo "All modules built successfully!"
```

### Debug-Builds

**Debug-Modus aktivieren:**

1. In Makefile `DEBUG` aktivieren:
```makefile
DEBUG = -g  # Kommentar entfernen
```

2. Kompilieren:
```bash
make clean
make all
```

3. Mit Debugger starten:
```bash
gdb ./worldqual
(gdb) run 403100091 2010 2010
(gdb) break main
(gdb) continue
```

## Code entwickeln

### Workflow für neue Features

1. **Branch erstellen**
   ```bash
   git checkout -b feature/neue-funktion
   ```

2. **Code entwickeln**
   - Neue Funktionen in entsprechende Dateien
   - Header-Dateien aktualisieren
   - Kommentare hinzufügen

3. **Kompilieren und testen**
   ```bash
   make clean && make all
   ./programm test_parameter
   ```

4. **Commit und Push**
   ```bash
   git add .
   git commit -m "feat: Neue Funktion hinzugefügt"
   git push origin feature/neue-funktion
   ```

5. **Pull Request erstellen**

### Neue Module hinzufügen

**Schritt 1: Verzeichnis erstellen**
```bash
mkdir src/neues_modul
cd src/neues_modul
```

**Schritt 2: Makefile erstellen**

Kopieren Sie ein bestehendes Makefile und passen Sie an:
```makefile
CC = g++ -Wall -Wno-unused-but-set-variable
MAKE = make -f makefile
APPNAME = neues_modul
OBJECTS = neues_modul.o \
          ../general_function/timestring.o \
          ../general_function/general_functions.o \
          ../options/options.o

INCDIROPTS = -L/usr/local/lib/libmysqlpp.so -I/usr/include/mysql -I/usr/local/include/mysql++ -I../general_function -I../options
LNKLIB = -lmysqlpp -lmysqlclient

$(APPNAME): $(OBJECTS)
	$(CC) $(INCDIROPTS) -o $(APPNAME) $(OBJECTS) $(LNKLIB)

.cpp.o:
	$(CC) -c $(INCDIROPTS) -o $*.o $*.cpp

all: clean
	@$(MAKE) $(APPNAME)
	echo all: make complete

clean:
	rm -f $(OBJECTS) $(APPNAME) *~ *% core a.out
```

**Schritt 3: Options-Klasse erstellen**

Erstellen Sie `options_neues_modul.h` und `options_neues_modul.cpp`:

```cpp
// options_neues_modul.h
#ifndef OPTIONS_NEUES_MODUL_H_
#define OPTIONS_NEUES_MODUL_H_

#include "../options/options.h"

class options_neues_modulClass : public optionsClass {
public:
    options_neues_modulClass(int optionc, char* optionv[], const char* CL);
    virtual ~options_neues_modulClass();
    
    // Spezifische Optionen für dieses Modul
    int spezifische_option;
    
protected:
    virtual void Help(int num = 1);
};

#endif
```

**Schritt 4: Hauptprogramm erstellen**

```cpp
// neues_modul.cpp
#include "options_neues_modul.h"
#include "../general_function/general_functions.h"

int main(int argc, char *argv[]) {
    options_neues_modulClass options(argc, &argv[0], "parameter");
    
    if (options.init(argc-1, &argv[1])) {
        return 1;
    }
    
    // Ihre Logik hier
    
    return 0;
}
```

**Schritt 5: OPTIONS.DAT erstellen**

Kopieren Sie eine bestehende OPTIONS.DAT und passen Sie an.

**Schritt 6: Dokumentation aktualisieren**

- Fügen Sie das Modul zu STRUCTURE.md hinzu
- Dokumentieren Sie die Funktionalität

### Code-Standards

Siehe [CODING_STANDARDS.md](CODING_STANDARDS.md) für Details.

**Kurzfassung:**
- C++11 Standard
- Konsistente Namenskonventionen
- Doxygen-Kommentare für Funktionen
- Fehlerbehandlung mit Rückgabewerten
- Status-Ausgaben auf `stdout`, Fehler auf `stderr`

## Debugging

### Debugger verwenden

**gdb (GNU Debugger):**

```bash
# Programm mit Debugger starten
gdb ./worldqual

# Breakpoints setzen
(gdb) break main
(gdb) break WorldQual.cpp:100
(gdb) break get_factor

# Programm starten
(gdb) run 403100091 2010 2010

# Schrittweise ausführen
(gdb) step      # Ein Schritt
(gdb) next      # Nächste Zeile
(gdb) continue  # Weiter bis nächster Breakpoint

# Variablen anzeigen
(gdb) print variable_name
(gdb) print *pointer
(gdb) print array[0]@10  # Array anzeigen

# Stack-Trace
(gdb) backtrace
(gdb) frame 1  # Zu Frame 1 wechseln

# Beenden
(gdb) quit
```

**Valgrind (Memory-Checker):**

```bash
# Memory-Leaks prüfen
valgrind --leak-check=full ./worldqual 403100091 2010 2010

# Speicherfehler finden
valgrind --tool=memcheck ./worldqual 403100091 2010 2010
```

### Debug-Ausgaben aktivieren

**In Code:**

```cpp
#define DEBUG
// oder
#define DEBUG_queries

#ifdef DEBUG
    cout << "Debug: Variable = " << variable << endl;
#endif
```

**Logging umleiten:**

```bash
# Alle Ausgaben in Datei
./worldqual 403100091 2010 2010 > log.txt 2>&1

# Nur Fehler in Datei
./worldqual 403100091 2010 2010 2> errors.txt

# Beides trennen
./worldqual 403100091 2010 2010 > output.txt 2> errors.txt
```

### Häufige Debugging-Szenarien

**Problem: Segmentation Fault**

```bash
# Mit gdb
gdb ./worldqual
(gdb) run 403100091 2010 2010
# Programm crasht
(gdb) backtrace
# Zeigt wo der Fehler auftrat
```

**Problem: Unerwartete Werte**

```cpp
// In Code einfügen
#ifdef DEBUG
    cout << "DEBUG: cell=" << cell << " value=" << value << endl;
#endif
```

**Problem: Datenbank-Query-Fehler**

```cpp
#define DEBUG_queries
// Zeigt alle SQL-Queries
```

## Testing

### Unit-Tests (aktuell nicht vorhanden, aber empfohlen)

**Framework:** Google Test oder Catch2

**Beispiel-Struktur:**

```cpp
// test_general_functions.cpp
#include <gtest/gtest.h>
#include "general_functions.h"

TEST(GeneralFunctions, get_outflowCell) {
    // Test-Logik
    int result = get_outflowCell(12345, &options);
    EXPECT_EQ(result, 12346);
}
```

**Ausführen:**
```bash
make test
./test_general_functions
```

### Integrationstests

**Manuelle Tests:**

1. **Kleine Test-Simulation**
   ```bash
   # Mit einem Jahr, kleiner Region
   ./fill_worldqual_load 2010 2010
   ./worldqual 403100091 2010 2010
   ```

2. **Ergebnisse prüfen**
   ```sql
   SELECT COUNT(*) FROM wwqa_worldqual_af.concentration 
   WHERE IDrun=403100091 AND year=2010;
   -- Sollte > 0 sein
   ```

3. **Vergleich mit bekannten Werten**
   - Bekannte Test-Daten verwenden
   - Ergebnisse mit erwarteten Werten vergleichen

### Performance-Tests

```bash
# Zeit messen
time ./worldqual 403100091 2010 2010

# Detailliertes Profiling
gprof ./worldqual gmon.out > profile.txt
```

## Versionierung

### Git-Workflow

**Branch-Strategie:**

- `main` - Stabile Version
- `develop` - Entwicklung (optional)
- `feature/*` - Neue Features
- `bugfix/*` - Bugfixes
- `docs/*` - Dokumentation

**Commit-Messages:**

Verwenden Sie konventionelle Commits:
```
feat: Neue Funktion hinzugefügt
fix: Bug in get_factor behoben
docs: Dokumentation erweitert
refactor: Code umstrukturiert
test: Tests hinzugefügt
```

**Beispiel:**
```bash
git checkout -b feature/neue-funktion
# Code ändern
git add .
git commit -m "feat: Neue Funktion für X hinzugefügt"
git push origin feature/neue-funktion
# Pull Request erstellen
```

### Code-Review

**Vor dem Merge:**
1. Code kompiliert ohne Warnungen
2. Tests bestehen (falls vorhanden)
3. Dokumentation aktualisiert
4. Code-Standards eingehalten

---

**Weitere Informationen:**
- [Code-Standards](CODING_STANDARDS.md) - Wie codet man?
- [Architektur](ARCHITECTURE.md) - Code-Struktur verstehen
- [Contributing](CONTRIBUTING.md) - Wie beitragen?
