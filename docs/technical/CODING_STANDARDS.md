# WorldQual - Code-Standards

Diese Dokumentation definiert die **Coding-Standards** für WorldQual. Bitte halten Sie sich an diese Standards, wenn Sie Code beitragen.

## Inhaltsverzeichnis

- [Allgemeine Prinzipien](#allgemeine-prinzipien)
- [Namenskonventionen](#namenskonventionen)
- [Code-Formatierung](#code-formatierung)
- [Kommentare und Dokumentation](#kommentare-und-dokumentation)
- [Fehlerbehandlung](#fehlerbehandlung)
- [Memory-Management](#memory-management)
- [Datenbankzugriff](#datenbankzugriff)

## Allgemeine Prinzipien

### C++ Standard

- **C++11** oder höher
- Keine C++14/17 Features ohne Absprache (für Kompatibilität)

### Code-Qualität

- **Lesbarkeit** vor Optimierung
- **Konsistenz** mit bestehendem Code
- **Einfachheit** - einfache Lösungen bevorzugen
- **Dokumentation** - Code sollte selbsterklärend sein

## Namenskonventionen

### Klassen

**Format:** `PascalCase` mit beschreibendem Suffix

**Beispiele:**
```cpp
class optionsClass { ... }
class options_wqClass { ... }
class ClassWQ_load { ... }
class COUNTRY_CLASS { ... }
```

**Konvention:** Options-Klassen enden mit `Class`, Load-Klassen mit `_CLASS`.

### Funktionen

**Format:** `snake_case` mit Präfix für Kategorien

**Beispiele:**
```cpp
get_outflc()        // get_* für Datenbankzugriff
get_inflow()        // get_* für Daten lesen
calculate_riverLength()  // calculate_* für Berechnungen
checkIDRun()        // check* für Validierung
```

**Kategorien:**
- `get_*` - Daten lesen (Datenbank, Dateien)
- `calculate_*` - Berechnungen durchführen
- `check*` - Validierung/Prüfung
- `fill*` - Strukturen befüllen
- `read*` - Dateien lesen

### Variablen

**Format:** `snake_case` oder `camelCase` (je nach Kontext)

**Beispiele:**
```cpp
int IDrun;              // IDs: camelCase
int cell;               // Einfache Variablen: snake_case
double (*Q_out)[12];    // Arrays: snake_case
char MyDatabase[255];   // Globale: PascalCase
```

**Konventionen:**
- IDs: `camelCase` (IDrun, IDScen, IDTemp)
- Lokale Variablen: `snake_case`
- Globale Variablen: `PascalCase` (MyDatabase, MyHost)
- Pointer: `*ptr` oder `ptr_ptr` für Zeiger auf Zeiger

### Konstanten

**Format:** `UPPER_SNAKE_CASE`

**Beispiele:**
```cpp
const int NODATA = -9999;
const int loadTypes = 11;
const short maxcharlength = 25500;
#define BYTESWAP_NECESSARY
```

### Strukturen

**Format:** `camelCase` mit `Struct` Suffix

**Beispiele:**
```cpp
struct continentStruct { ... }
struct loadStruct { ... }
struct Substance { ... }  // Ausnahme: kein Suffix
```

## Code-Formatierung

### Einrückung

- **4 Leerzeichen** (keine Tabs)
- Konsistent mit bestehendem Code

### Zeilenlänge

- Maximal **120 Zeichen** (wenn möglich)
- Lange Zeilen umbrechen mit Einrückung

### Klammern

**Stil:** Öffnende Klammer auf gleicher Zeile

```cpp
if (condition) {
    // Code
}

for (int i = 0; i < n; i++) {
    // Code
}

class MyClass {
public:
    // Code
};
```

### Leerzeilen

- **Eine Leerzeile** zwischen Funktionen
- **Zwei Leerzeilen** zwischen Sektionen
- **Keine Leerzeilen** innerhalb von Funktionen (außer für Lesbarkeit)

### Includes

**Reihenfolge:**
1. System-Header (`<stdio.h>`, `<vector>`, etc.)
2. MySQL++ Header (`<mysql++.h>`, `<ssqls.h>`)
3. Lokale Header (`"general_functions.h"`, etc.)

**Beispiel:**
```cpp
#include <stdio.h>
#include <mysql++.h>
#include <ssqls.h>
#include <vector>
#include "../general_function/general_functions.h"
#include "options_wq.h"
```

## Kommentare und Dokumentation

### Doxygen-Kommentare

**Für Funktionen:**

```cpp
/**
 * @brief Kurze Beschreibung
 * @param param1 Beschreibung Parameter 1
 * @param param2 Beschreibung Parameter 2
 * @return Rückgabewert-Beschreibung (0 bei Erfolg, sonst Fehlercode)
 */
int get_factor(int IDrun, double *factor, double *c_geogen, ...);
```

**Für Klassen:**

```cpp
/**
 * @class ClassWQ_load
 * @brief Hauptklasse zur Berechnung von Wasserqualitäts-Belastungen
 * 
 * Detaillierte Beschreibung der Klasse...
 */
class ClassWQ_load {
    // ...
};
```

**Für Strukturen:**

```cpp
/**
 * @struct continentStruct
 * @brief Speichert Kontinentdaten und Grid-Informationen
 */
struct continentStruct {
    int ng;              ///< Anzahl der Zellen
    int ng_land;         ///< Anzahl der Landzellen
    // ...
};
```

### Inline-Kommentare

**Wann kommentieren:**
- Komplexe Algorithmen
- Nicht-offensichtliche Logik
- Workarounds oder Hacks
- Warum etwas gemacht wird (nicht was)

**Beispiele:**

```cpp
// Gute Kommentare:
// Byte-Order muss geändert werden für Big-Endian Systeme
SimMemorySwap((char*)array, ...);

// Routing-Informationen werden aus WaterGAP übernommen
int outflow = get_outflowCell(cell, &options);

// Schlechte Kommentare:
int i = 0;  // Setze i auf 0 (offensichtlich!)
```

## Fehlerbehandlung

### Rückgabewerte

**Konvention:**
- `0` = Erfolg
- `1` oder `>0` = Fehler
- `-1` oder `NODATA` = Fehlender Wert

**Beispiel:**
```cpp
int get_factor(int IDrun, double *factor, ...) {
    // Prüfung
    if (IDrun < 0) {
        cerr << "ERROR: Invalid IDrun" << endl;
        return 1;  // Fehler
    }
    
    // Erfolgreich
    return 0;
}
```

### Fehlerausgaben

**Konvention:**
- `cout` für normale Status-Ausgaben
- `cerr` für Fehler und Warnungen

**Beispiel:**
```cpp
cout << "Program started..." << endl;
cout << "Processing year " << year << endl;

if (error) {
    cerr << "ERROR: Something went wrong" << endl;
    return 1;
}
```

### Exception-Handling

**Aktuell:** Keine Exceptions verwendet (C-Stil)

**Für neue Code:** Können Exceptions verwendet werden, aber konsistent bleiben.

## Memory-Management

### Dynamische Speicherallokation

**Konvention:** `new`/`delete` (kein `malloc`/`free`)

**Beispiel:**
```cpp
// Allokation
double *array = new double[size];
if (!array) {
    cerr << "ERROR: Memory allocation failed" << endl;
    return 1;
}

// Verwendung
// ...

// Freigabe
delete[] array;
array = NULL;  // Gute Praxis
```

### 2D-Arrays

**Konvention:** Array von Arrays

```cpp
// Allokation
double (*Q_out)[12] = new double[continent.ng][12];
if (!Q_out) {
    cerr << "ERROR: Memory allocation failed" << endl;
    exit(1);
}

// Verwendung
Q_out[cell][month] = value;

// Freigabe
delete[] Q_out;
Q_out = NULL;
```

### Memory-Leaks vermeiden

**Regeln:**
- Jedes `new` braucht ein `delete`
- Bei Fehler-Return: Speicher freigeben
- Pointer auf `NULL` setzen nach `delete`

**Beispiel:**
```cpp
double *data = new double[size];
if (function_fails()) {
    delete[] data;  // Wichtig!
    data = NULL;
    return 1;
}
// ... verwenden ...
delete[] data;
data = NULL;
```

## Datenbankzugriff

### MySQL++ Verwendung

**Konvention:** MySQL++ Library verwenden

**Beispiel:**
```cpp
mysqlpp::Connection con(use_exceptions);
mysqlpp::Query query = con.query();

// Query zusammenbauen
char sql[2000];
int j = sprintf(sql, "SELECT * FROM table WHERE id=%d", id);
query << sql;

// Ausführen
mysqlpp::StoreQueryResult res = query.store();

// Ergebnisse verarbeiten
if (res.empty()) {
    cerr << "ERROR: No results" << endl;
    return 1;
}

mysqlpp::String value = res.at(0)["column"];
```

### SQL-Injection vermeiden

**Sicher:**
```cpp
// Parameterisiert (MySQL++ macht das automatisch)
query << "SELECT * FROM table WHERE id=" << id;
```

**Unsicher (vermeiden):**
```cpp
// NIEMALS so:
sprintf(sql, "SELECT * FROM table WHERE name='%s'", user_input);
// Kann SQL-Injection ermöglichen!
```

### Query-Debugging

**Aktivieren:**
```cpp
#define DEBUG_queries
```

**Ausgabe:**
```cpp
#ifdef DEBUG_queries
    cout << "SQL: " << sql << endl;
#endif
```

## Code-Beispiele

### Gute Praxis

```cpp
/**
 * @brief Berechnet Abfluss für eine Zelle
 * @param cell Zellen-ID
 * @param year Jahr
 * @param Q_out Zeiger auf Abfluss-Array (Output)
 * @param options_ptr Zeiger auf Options-Objekt
 * @return 0 bei Erfolg, 1 bei Fehler
 */
int get_Q_for_cell(int cell, int year, double *Q_out, 
                    const optionsClass* options_ptr) {
    // Validierung
    if (cell < 0 || year < 1900) {
        cerr << "ERROR: Invalid parameters" << endl;
        return 1;
    }
    
    // Berechnung
    *Q_out = calculate_flow(cell, year);
    
    // Prüfung
    if (*Q_out < 0) {
        cerr << "ERROR: Negative flow calculated" << endl;
        return 1;
    }
    
    return 0;
}
```

### Schlechte Praxis (vermeiden)

```cpp
// Keine Kommentare
// Keine Validierung
// Keine Fehlerbehandlung
int f(int c, int y, double *q) {
    *q = c * y;  // Was macht das?
    return 0;    // Immer Erfolg?
}
```

## Code-Review Checkliste

Vor dem Commit prüfen:

- [ ] Code kompiliert ohne Warnungen
- [ ] Namenskonventionen eingehalten
- [ ] Kommentare vorhanden (Doxygen)
- [ ] Fehlerbehandlung implementiert
- [ ] Memory-Leaks vermieden
- [ ] Konsistent mit bestehendem Code
- [ ] Keine Magic Numbers (Konstanten verwenden)
- [ ] Keine Debug-Code-Reste (`cout << "test"`)

---

**Weitere Informationen:**
- [Entwicklungsumgebung](DEVELOPMENT.md) - Wie entwickelt man?
- [Architektur](ARCHITECTURE.md) - Code-Struktur verstehen
