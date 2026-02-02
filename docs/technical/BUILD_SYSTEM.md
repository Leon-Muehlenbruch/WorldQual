# WorldQual - Build-System

Detaillierte Dokumentation des **Build-Systems** und der Makefiles.

## Inhaltsverzeichnis

- [Makefile-Struktur](#makefile-struktur)
- [Kompilierungsprozess](#kompilierungsprozess)
- [Abhängigkeiten](#abhängigkeiten)
- [Pfad-Konfiguration](#pfad-konfiguration)
- [Troubleshooting](#troubleshooting)

## Makefile-Struktur

### Standard-Makefile-Template

Jedes Modul folgt diesem Muster:

```makefile
# ============================================================================
# Compiler und Flags
# ============================================================================
CC         = g++ -Wall -Wno-unused-but-set-variable
MAKE       = make -f makefile
APPNAME    = modulname
OBJECTS    = modulname.o \
             ../general_function/timestring.o \
             ../general_function/general_functions.o \
             ../options/options.o
CCOPTS     = -c
OPTIMIZE   = -O3
#DEBUG      = -g  # Für Debugging aktivieren

# ============================================================================
# Include- und Library-Pfade
# ============================================================================
INCDIROPTS = -L/usr/local/lib/libmysqlpp.so \
             -L/usr/lib64/mysql \
             -I/usr/include/mysql \
             -I/usr/local/include/mysql++ \
             -I../general_function \
             -I../options

LNKLIB     = -lmysqlpp -lmysqlclient

# ============================================================================
# Build-Regeln
# ============================================================================
$(APPNAME): $(OBJECTS)
	$(CC) $(INCDIROPTS) $(LNKOPTS) -o $(APPNAME) $(OBJECTS) $(LNKLIB)

.cpp.o:
	$(CC) -c $(INCDIROPTS) $(DEBUG) -o $*.o $*.cpp

all: clean
	@$(MAKE) $(APPNAME)
	echo all: make complete

clean:
	rm -f $(OBJECTS) $(APPNAME) *~ *% core a.out
```

### Variablen erklärt

**CC:** Compiler und Flags
- `g++`: GNU C++ Compiler
- `-Wall`: Alle Warnungen aktivieren
- `-Wno-unused-but-set-variable`: Spezifische Warnung deaktivieren

**OPTIMIZE:** Optimierungs-Level
- `-O3`: Maximale Optimierung (für Produktion)
- `-O0`: Keine Optimierung (für Debugging)
- `-O2`: Standard-Optimierung

**DEBUG:** Debug-Flags
- `-g`: Debug-Informationen generieren
- Aktivieren für Debugging mit gdb

**INCDIROPTS:** Include- und Library-Pfade
- `-L`: Library-Pfade (wo `.so` Dateien sind)
- `-I`: Include-Pfade (wo `.h` Dateien sind)

**LNKLIB:** Zu linkende Bibliotheken
- `-lmysqlpp`: MySQL++ Bibliothek
- `-lmysqlclient`: MySQL Client Bibliothek

## Kompilierungsprozess

### Schritt-für-Schritt

**1. Preprocessing**
```bash
# C++ Preprocessor verarbeitet #include, #define, etc.
cpp source.cpp -o source.i
```

**2. Compilation**
```bash
# Compiler erstellt Objektdateien (.o)
g++ -c source.cpp -o source.o
```

**3. Linking**
```bash
# Linker verbindet Objektdateien und Bibliotheken
g++ source.o -o executable -lmysqlpp -lmysqlclient
```

### Make-Targets

**all:**
```bash
make all
# Führt clean aus, dann kompiliert
```

**clean:**
```bash
make clean
# Löscht Objektdateien und ausführbare Dateien
```

**Spezifisches Target:**
```bash
make worldqual
# Kompiliert nur worldqual (ohne clean)
```

## Abhängigkeiten

### Kompilierungs-Abhängigkeiten

**Reihenfolge ist wichtig:**

```
1. general_function
   ↓ (wird benötigt von)
2. options
   ↓ (beide werden benötigt von)
3. newton (optional, für water_temperature)
   ↓
4. Alle anderen Module
```

**Warum diese Reihenfolge?**

- `general_function` hat keine Abhängigkeiten zu anderen Modulen
- `options` nutzt `general_function`
- Alle anderen Module nutzen beide

### Laufzeit-Abhängigkeiten

**Datenbank-Abhängigkeiten:**

```
fill_worldqual_load → worldqual
  (muss Daten in Datenbank schreiben, bevor worldqual sie liest)

water_temperature → worldqual
  (optional, aber empfohlen)

worldqual → wq_stat_*
  (muss Ergebnisse in Datenbank schreiben)
```

## Pfad-Konfiguration

### Problem: Unterschiedliche Systeme

MySQL++ kann auf verschiedenen Systemen unterschiedlich installiert sein:

**Linux (Standard):**
- Headers: `/usr/include/mysql++`
- Libraries: `/usr/lib/libmysqlpp.so`

**Linux (Custom):**
- Headers: `/usr/local/include/mysql++`
- Libraries: `/usr/local/lib/libmysqlpp.so`

**macOS (Homebrew):**
- Headers: `/usr/local/include/mysql++`
- Libraries: `/usr/local/lib/libmysqlpp.so`

**macOS (MacPorts):**
- Headers: `/opt/local/include/mysql++`
- Libraries: `/opt/local/lib/libmysqlpp.so`

### Lösung: Pfade finden

**Automatisch finden:**
```bash
# Header-Dateien
find /usr /usr/local /opt -name "mysql++.h" 2>/dev/null

# Bibliotheken
find /usr /usr/local /opt -name "libmysqlpp.so" 2>/dev/null
find /usr /usr/local /opt -name "libmysqlpp.dylib" 2>/dev/null  # macOS
```

**Manuell in Makefile anpassen:**

```makefile
# Für Linux Standard
INCDIROPTS = -L/usr/lib -I/usr/include/mysql -I/usr/include/mysql++ ...

# Für macOS Homebrew
INCDIROPTS = -L/usr/local/lib -I/usr/include/mysql -I/usr/local/include/mysql++ ...
```

### Zentrale Konfiguration (empfohlen)

**Erstellen Sie `config.mk` im Root:**

```makefile
# config.mk - Zentrale Build-Konfiguration

# MySQL++ Pfade (anpassen je nach System)
MYSQLPP_INCLUDE = /usr/local/include/mysql++
MYSQLPP_LIB = /usr/local/lib
MYSQL_INCLUDE = /usr/include/mysql
MYSQL_LIB = /usr/lib64/mysql

# Compiler-Flags
CXX = g++
CXXFLAGS = -Wall -Wno-unused-but-set-variable -O3
DEBUG_FLAGS = -g

# Standard-Includes
STD_INCLUDES = -I../general_function -I../options

# Standard-Libraries
STD_LIBS = -lmysqlpp -lmysqlclient
```

**In Makefiles einbinden:**

```makefile
include ../../config.mk

INCDIROPTS = -L$(MYSQLPP_LIB) -L$(MYSQL_LIB) \
             -I$(MYSQL_INCLUDE) -I$(MYSQLPP_INCLUDE) \
             $(STD_INCLUDES)

LNKLIB = $(STD_LIBS)

CC = $(CXX) $(CXXFLAGS)
```

## Troubleshooting

### Fehler: "mysql++.h: No such file or directory"

**Problem:** Header-Datei nicht gefunden.

**Lösung:**
1. MySQL++ installiert?
   ```bash
   # Linux
   dpkg -l | grep mysql++
   # oder
   rpm -qa | grep mysql++
   
   # macOS
   brew list | grep mysql++
   ```

2. Pfad in Makefile anpassen:
   ```makefile
   # Aktuell (falsch):
   -I/usr/include/mysql++
   
   # Anpassen zu (richtig):
   -I/usr/local/include/mysql++
   ```

### Fehler: "cannot find -lmysqlpp"

**Problem:** Bibliothek nicht gefunden.

**Lösung:**
1. Bibliothek existiert?
   ```bash
   find /usr /usr/local -name "libmysqlpp.so*"
   ```

2. Pfad in Makefile anpassen:
   ```makefile
   # Aktuell (falsch):
   -L/usr/lib
   
   # Anpassen zu (richtig):
   -L/usr/local/lib
   ```

3. Bibliothek richtig verlinkt?
   ```makefile
   LNKLIB = -lmysqlpp -lmysqlclient
   # Reihenfolge kann wichtig sein!
   ```

### Fehler: "undefined reference to mysqlpp::..."

**Problem:** Bibliothek wird nicht gelinkt.

**Lösung:**
1. Bibliothek in LNKLIB?
   ```makefile
   LNKLIB = -lmysqlpp -lmysqlclient
   ```

2. Reihenfolge prüfen:
   ```makefile
   # Bibliotheken NACH Objektdateien
   $(CC) $(OBJECTS) $(LNKLIB) -o $(APPNAME)
   ```

### Fehler: "multiple definition of..."

**Problem:** Funktionen mehrfach definiert.

**Lösung:**
- Funktionen in `.cpp` Dateien definieren
- Nur Deklarationen in `.h` Dateien
- `inline` für Template-Funktionen

### Performance-Optimierung

**Optimierungs-Level:**

```makefile
# Für Produktion
OPTIMIZE = -O3

# Für Debugging
OPTIMIZE = -O0
DEBUG = -g
```

**Weitere Flags:**
```makefile
# Spezifische CPU-Optimierung
OPTIMIZE = -O3 -march=native

# Link-Time Optimization
OPTIMIZE = -O3 -flto
```

---

**Weitere Informationen:**
- [Entwicklungsumgebung](DEVELOPMENT.md) - Entwicklungsworkflow
- [Architektur](ARCHITECTURE.md) - Code-Struktur
