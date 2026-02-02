# WorldQual - API-Referenz

Referenz der wichtigsten **Funktionen und Klassen** für Entwickler.

## Inhaltsverzeichnis

- [general_functions API](#general_functions-api)
- [options API](#options-api)
- [ClassWQ_load API](#classwq_load-api)
- [Datenstrukturen](#datenstrukturen)

## general_functions API

### Datenbankzugriff

#### getProject()

```cpp
bool getProject(int project_id, const optionsClass* options_ptr, 
                mysqlpp::Connection con, const char* application);
```

**Zweck:** Lädt Projekt-Informationen und verbindet mit regionsspezifischer Datenbank.

**Parameter:**
- `project_id`: Projekt-ID aus `_project` Tabelle
- `options_ptr`: Zeiger auf Options-Objekt
- `con`: MySQL-Verbindung
- `application`: "worldqual" oder "wq_load"

**Rückgabe:**
- `false` (0): Erfolg
- `true` (1): Fehler

**Beispiel:**
```cpp
mysqlpp::Connection con(use_exceptions);
if (!con.connect("wq_general", "localhost", "user", "pass", 3306)) {
    return 1;
}

if (getProject(2, &options, con, "worldqual")) {
    cerr << "ERROR: Project not found" << endl;
    return 1;
}
// Jetzt verbunden mit wwqa_worldqual_[region]
```

#### checkParameter()

```cpp
bool checkParameter(int parameter_id, int& toCalc, 
                    mysqlpp::String& ParameterName, bool ReadToCalc=true);
```

**Zweck:** Prüft ob Parameter existiert und gibt Informationen zurück.

**Parameter:**
- `parameter_id`: Parameter-ID (0-4)
- `toCalc`: Referenz für Berechnungstyp (Output)
- `ParameterName`: Referenz für Parametername (Output)
- `ReadToCalc`: Ob toCalc gelesen werden soll

**Rückgabe:**
- `false` (0): Erfolg
- `true` (1): Fehler

**toCalc Werte:**
- 0 = BOD
- 1 = Salz
- 2 = FC
- 3 = TN
- 4 = TP

#### checkIDRun()

```cpp
short checkIDRun(int IDrun, mysqlpp::String& runName, 
                  int& parameter_id_input, int& parameter_id, 
                  int& parameter_id_load, int& project_id, 
                  int& IDScen, int& IDScen_wq_load, 
                  int& IDTemp, double& Q_low_limit, 
                  short& Q_low_limit_type, short& FlgCons, 
                  short& FlgLake, short& UseWaterTemp);
```

**Zweck:** Prüft Run-Konfiguration und gibt alle wichtigen Parameter zurück.

**Parameter:** Alle Output-Parameter (Referenzen)

**Rückgabe:**
- `0`: Erfolg
- `1`: Fehler

### Hydrologische Funktionen

#### get_outflc()

```cpp
int get_outflc(int *g_outflc, const optionsClass* options_ptr, 
               const continentStruct* continent_ptr);
```

**Zweck:** Liest Abflusszellen (outflow cells) aus der Datenbank.

**Parameter:**
- `g_outflc`: Array für Abflusszellen-IDs (Output) [ng]
- `options_ptr`: Options-Objekt
- `continent_ptr`: Kontinent-Struktur

**Rückgabe:**
- `0`: Erfolg
- `1`: Fehler

**Verwendung:**
```cpp
int *g_outflc = new int[continent.ng];
if (get_outflc(g_outflc, &options, &continent)) {
    cerr << "ERROR: Could not read outflow cells" << endl;
    return 1;
}
// g_outflc[cell] enthält jetzt die Abflusszelle für jede Zelle
```

#### get_inflow()

```cpp
int get_inflow(int (*inflow)[8], const optionsClass* options_ptr, 
               const continentStruct* continent_ptr);
```

**Zweck:** Liest Zufluss-Informationen (bis zu 8 Zuflüsse pro Zelle).

**Parameter:**
- `inflow`: 2D-Array für Zuflüsse [ng][8] (Output)
- `options_ptr`: Options-Objekt
- `continent_ptr`: Kontinent-Struktur

**Rückgabe:**
- `0`: Erfolg
- `1`: Fehler

**Verwendung:**
```cpp
int (*inflow)[8] = new int[continent.ng][8];
if (get_inflow(inflow, &options, &continent)) {
    return 1;
}
// inflow[cell][0..7] enthält Zufluss-Zellen
```

#### get_Q()

```cpp
int get_Q(int actual_year, double (*Q_out)[12], int *gcrc, 
          const options_wqClass* options_ptr, 
          const continentStruct* continent_ptr);
```

**Zweck:** Liest monatlichen Abfluss für ein Jahr.

**Parameter:**
- `actual_year`: Jahr
- `Q_out`: 2D-Array für Abfluss [ng][12] (Output) [km³/month]
- `gcrc`: Array mit Zellen-IDs
- `options_ptr`: WQ-Options-Objekt
- `continent_ptr`: Kontinent-Struktur

**Rückgabe:**
- `0`: Erfolg
- `1`: Fehler

**Verwendung:**
```cpp
double (*Q_out)[12] = new double[continent.ng][12];
if (get_Q(2010, Q_out, gcrc, &options, &continent)) {
    return 1;
}
// Q_out[cell][month-1] enthält Abfluss für jede Zelle, jeden Monat
```

#### get_cell_runoff()

```cpp
int get_cell_runoff(int actual_year, double (*cell_runoff)[12], 
                    int *gcrc, const options_wqClass* options_ptr, 
                    const continentStruct* continent_ptr);
```

**Zweck:** Liest zellspezifischen Oberflächenabfluss.

**Parameter:**
- `actual_year`: Jahr
- `cell_runoff`: 2D-Array für Abfluss [ng][12] (Output) [mm/month]
- `gcrc`: Array mit Zellen-IDs
- `options_ptr`: WQ-Options-Objekt
- `continent_ptr`: Kontinent-Struktur

**Rückgabe:**
- `0`: Erfolg
- `1`: Fehler

### Wasserqualitäts-Funktionen

#### get_factor()

```cpp
int get_factor(int IDrun, double *factor, double *c_geogen, 
               double *factor_lake, int toCalc, 
               const optionsClass* options_ptr, 
               const continentStruct* continent_ptr);
```

**Zweck:** Liest Faktoren für Wasserqualitätsberechnungen.

**Parameter:**
- `IDrun`: Run-ID
- `factor`: Array für Faktoren (Output) [ng]
- `c_geogen`: Array für geogene Konzentrationen (Output) [ng]
- `factor_lake`: Array für See-Faktoren (Output) [ng]
- `toCalc`: Berechnungstyp (0-4)
- `options_ptr`: Options-Objekt
- `continent_ptr`: Kontinent-Struktur

**Rückgabe:**
- `0`: Erfolg
- `1`: Fehler

#### get_s_cell()

```cpp
int get_s_cell(int IDScen, int parameter_id, int actual_year, 
               double (*s_cell)[12], const optionsClass* options_ptr, 
               const continentStruct* continent_ptr, 
               short loadingType, int toCalc);
```

**Zweck:** Liest Schadstoff-Einträge pro Zelle für ein Jahr.

**Parameter:**
- `IDScen`: Szenario-ID
- `parameter_id`: Parameter-ID
- `actual_year`: Jahr
- `s_cell`: 2D-Array für Einträge [ng][12] (Output) [t/month]
- `options_ptr`: Options-Objekt
- `continent_ptr`: Kontinent-Struktur
- `loadingType`: Load-Typ
- `toCalc`: Berechnungstyp

**Rückgabe:**
- `0`: Erfolg
- `1`: Fehler

#### calculateAttenuation()

```cpp
int calculateAttenuation(double *ke, double *tss_cell, 
                          int IDScen, int parameter_id, int year,
                          double (*Q_out)[12], int *gcrc, int *grow, 
                          const optionsClass* options_ptr, 
                          const continentStruct* continent_ptr);
```

**Zweck:** Berechnet Abschwächungskoeffizienten.

**Parameter:**
- `ke`: Array für Abschwächungskoeffizienten (Output) [ng]
- `tss_cell`: Array für TSS-Werte (Output) [ng]
- `IDScen`: Szenario-ID
- `parameter_id`: Parameter-ID
- `year`: Jahr
- `Q_out`: Abfluss-Array
- `gcrc`, `grow`: Grid-Arrays
- `options_ptr`: Options-Objekt
- `continent_ptr`: Kontinent-Struktur

**Rückgabe:**
- `0`: Erfolg
- `1`: Fehler

### Datei-I/O Funktionen

#### readInputFile()

```cpp
template <class T>
int readInputFile(char *input_file, int type_size, 
                  int valuesPerCell, long number_of_cells,
                  T *G_array, int *gcrc, 
                  const optionsClass* options_ptr, 
                  const continentStruct* continent_ptr, 
                  float nodata=(T)NODATA);
```

**Zweck:** Liest UNF-Dateien (Template-Funktion).

**Parameter:**
- `input_file`: Pfad zur UNF-Datei
- `type_size`: Größe des Datentyps (sizeof(T))
- `valuesPerCell`: Werte pro Zelle (z.B. 12 für monatlich)
- `number_of_cells`: Anzahl Zellen
- `G_array`: Array für Daten (Output)
- `gcrc`: Array mit Zellen-IDs
- `options_ptr`: Options-Objekt
- `continent_ptr`: Kontinent-Struktur
- `nodata`: NoData-Wert

**Rückgabe:**
- `0`: Erfolg
- `1`: Fehler

**Verwendung:**
```cpp
double (*Q_out)[12] = new double[continent.ng][12];
char filename[] = "/path/to/G_Q_out_m3_2010.12.UNF0";
if (readInputFile(filename, sizeof(double), 12, continent.ng, 
                  Q_out, gcrc, &options, &continent)) {
    return 1;
}
```

### Grid-Management

#### getCELLS()

```cpp
int getCELLS(const optionsClass* options_ptr, continentStruct* continent);
```

**Zweck:** Lädt Grid-Informationen (Anzahl Zellen, Zeilen, Spalten).

**Parameter:**
- `options_ptr`: Options-Objekt
- `continent`: Kontinent-Struktur (Output)

**Rückgabe:**
- `0`: Erfolg
- `1`: Fehler

**Verwendung:**
```cpp
continentStruct continent;
if (getCELLS(&options, &continent)) {
    return 1;
}
// continent.ng, continent.ng_land, continent.nrows, etc. sind jetzt gefüllt
```

#### getGCRC()

```cpp
int getGCRC(int *gcrc, int *grow, const optionsClass* options_ptr, 
            const continentStruct* continent_ptr);
```

**Zweck:** Lädt Grid-Zell-IDs (gcrc) und Zeilen-Indizes (grow).

**Parameter:**
- `gcrc`: Array für Zellen-IDs (Output) [ng]
- `grow`: Array für Zeilen-Indizes (Output) [ng]
- `options_ptr`: Options-Objekt
- `continent_ptr`: Kontinent-Struktur

**Rückgabe:**
- `0`: Erfolg
- `1`: Fehler

## options API

### optionsClass - Basisklasse

```cpp
class optionsClass {
public:
    optionsClass(int optionc, char* optionv[], const char* CL);
    virtual ~optionsClass();
    
    int IDVersion;
    int IDReg;
    char continent_abb[10];
    char* MyHost;
    char* MyUser;
    char* MyPassword;
    short InputType;
    char* input_dir;
    
protected:
    virtual void Help(int num = 1) = 0;
    short init(int optionc, char* optionv[]);
    void read_parentOptions(int num, FILE *file_ptr);
};
```

**Verwendung:**
```cpp
optionsClass* options = new options_wqClass(argc, argv, "IDrun startYear endYear");
if (options->init(argc-1, &argv[1])) {
    return 1;
}
```

## ClassWQ_load API

### Konstruktor

```cpp
ClassWQ_load(options_wq_loadClass* options_ptr, int year);
```

**Zweck:** Berechnet Einträge für ein Jahr beim Erstellen.

**Parameter:**
- `options_ptr`: WQ-Load Options-Objekt
- `year`: Berechnungsjahr

**Verwendung:**
```cpp
options_wq_loadClass options(...);
ClassWQ_load wq_load(&options, 2010);
// Einträge werden automatisch berechnet
```

### Wichtige Member-Variablen

```cpp
class ClassWQ_load {
    int toCalc;              // Berechnungstyp (0-4)
    int year;                // Jahr
    continentStruct continent; // Grid-Informationen
    COUNTRY_CLASS* countries; // Länderinformationen
    CROP_CLASS* crops;       // Kulturpflanzen
    LS_CLASS* LS;            // Landnutzung
    GB_CLASS* GB;            // Viehhaltung
    // ... viele weitere Member
};
```

## Datenstrukturen

### continentStruct

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

### loadStruct

```cpp
struct loadStruct {
    bool loadType[loadTypes];  // Welche Load-Typen werden benötigt?
    char loadTab[loadTypes][maxcharlength]; // Tabellennamen
    char loadTabAlias[loadTypes][loadTypes]; // SQL-Aliase
    char loadTabLongName[loadTypes][maxcharlength]; // Beschreibungen
};

void fillLoadStruct(loadStruct *loadStruct_ptr, int toCalc);
```

**Verwendung:**
```cpp
loadStruct load;
fillLoadStruct(&load, 2);  // Für FC (toCalc=2)
// load.loadType[] zeigt welche Tabellen benötigt werden
```

---

**Weitere Informationen:**
- [Architektur](ARCHITECTURE.md) - Wie alles zusammenpasst
- [Entwicklungsumgebung](DEVELOPMENT.md) - Wie entwickelt man?
