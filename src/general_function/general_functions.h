/**
 * @file general_functions.h
 * @brief Allgemeine Hilfsfunktionen für das WorldQual Wasserqualitätsmodell
 * @author reder
 * @date 08.01.2013
 * 
 * Diese Datei enthält zentrale Funktionen für Datei-I/O, Datenbankzugriffe,
 * hydrologische Berechnungen und Wasserqualitätsmodellierung.
 */

#include <mysql++.h>
#include <ssqls.h> //<custom.h>
#include <mysql.h>
#include <fstream>
#include "../options/options.h"
#include "../worldqual/options_wq.h"
#include "country.h"

using namespace std;

#ifndef GENERAL_FUNCTIONS_H_
#define GENERAL_FUNCTIONS_H_

using namespace mysqlpp;

#ifndef DEBUG_queries
#define DEBUG_queries
//#undef DEBUG_queries
#endif

/**
 * @brief Flag zur Steuerung der geogenen Konzentration
 * 
 * Kontrolliert ob c_geogen als Länderwerte oder Zellwerte verwendet wird:
 * - true (1): c_geogen aus country_parameter_input Tabelle (bis 3.2015)
 * - false (0): c_geogen_cell aus parameter_cell_input Tabelle (ab 3.2015)
 */
const bool is_c_geogen_country = 1;

#define BYTESWAP_NECESSARY

/// Maximale Zeichenkettenlänge für Dateipfade und Strings
const short maxcharlength = 25500;

/**
 * @struct continentStruct
 * @brief Speichert Kontinentdaten und Grid-Informationen
 */
struct continentStruct {
	int ng;              ///< Anzahl der Zellen
	int ng_land;         ///< Anzahl der Landzellen
	int nrows;           ///< Anzahl der Zeilen
	int ncols;           ///< Anzahl der Spalten
	int cellFraction;    ///< Zellfraktion
	char hydro_input[maxcharlength]; ///< Pfad zu WaterGAP-Eingabedaten
};

/// Anzahl der verschiedenen Load-Typen
const int loadTypes = 11;

/**
 * @struct loadStruct
 * @brief Struktur zur Verwaltung verschiedener Belastungstypen
 */
struct loadStruct {
		bool loadType[loadTypes];  ///< true = Tabelle wird benötigt, false = nicht benötigt
		char loadTab[loadTypes][maxcharlength]; ///< Tabellenname: *wq_load_*.calc_cell_month_load_[loadTab[i]]
		char loadTabAlias[loadTypes][loadTypes]; ///< Alias der Tabelle in SQL-Anweisung
		char loadTabLongName[loadTypes][maxcharlength]; ///< Beschreibung zur Information
};

/**
 * @brief Füllt die loadStruct Struktur basierend auf dem Berechnungstyp
 * @param loadStruct_ptr Zeiger auf die zu füllende loadStruct
 * @param toCalc Berechnungstyp: 0=BOD, 1=Salz, 2=FC (Fecal coliforms), 3=TN, 4=TP
 */
void fillLoadStruct (loadStruct *loadStruct_ptr, int toCalc);

/// NODATA Wert für fehlende oder ungültige Daten
extern const int NODATA;

/// MySQL++ Datenbankverbindung
extern mysqlpp::Connection con;

/// MySQL++ Query-Objekt
extern mysqlpp::Query query;

/// MySQL++ Ergebnis-Container
extern mysqlpp::StoreQueryResult res;

// ============================================================================
// Hydrologische Methoden
// ============================================================================

/**
 * @brief Überprüft die Temperatur-ID und gibt entsprechende Parameter zurück
 * @param IDTemp Temperatur-ID
 * @param ConstTemp Zeiger auf konstante Temperatur (Output)
 * @param climateTemp Zeiger auf Klima-Temperatur String (Output)
 * @return Status-Code
 */
short checkIDTemp(int IDTemp, double* ConstTemp, char* climateTemp);

/**
 * @brief Liest Abflusszellen (outflow cells) aus der Datenbank
 * @param g_outflc Array für Abflusszellen-IDs (Output)
 * @param options_ptr Zeiger auf Options-Objekt
 * @param continent_ptr Zeiger auf Kontinent-Struktur
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int get_outflc(int *g_outflc, const optionsClass* options_ptr, const continentStruct* continent_ptr);

/**
 * @brief Bestimmt die Abflusszelle für eine gegebene Zelle
 * @param cell Zellen-ID
 * @param options_ptr Zeiger auf Options-Objekt
 * @return ID der Abflusszelle
 */
int get_outflowCell(int cell, const optionsClass* options_ptr);

/**
 * @brief Liest Zufluss-Informationen (bis zu 8 Zuflüsse pro Zelle)
 * @param inflow 2D-Array für Zufluss-Daten [Zelle][max 8 Zuflüsse] (Output)
 * @param options_ptr Zeiger auf Options-Objekt
 * @param continent_ptr Zeiger auf Kontinent-Struktur
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int get_inflow(int (*inflow)[8], const optionsClass* options_ptr, const continentStruct* continent_ptr);

/**
 * @brief Liest Faktoren für Wasserqualitätsberechnungen
 * @param IDrun Run-ID
 * @param factor Array für Faktoren (Output)
 * @param c_geogen Array für geogene Konzentrationen (Output)
 * @param factor_lake Array für See-Faktoren (Output)
 * @param toCalc Berechnungstyp
 * @param options_ptr Zeiger auf Options-Objekt
 * @param continent_ptr Zeiger auf Kontinent-Struktur
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int get_factor(int IDrun, double *factor, double *c_geogen, double *factor_lake, int toCalc, const optionsClass* options_ptr, const continentStruct* continent_ptr);

/**
 * @brief Liest zellspezifischen Oberflächenabfluss für ein Jahr
 * @param actual_year Aktuelles Jahr
 * @param cell_runoff 2D-Array für monatlichen Abfluss [Zelle][12 Monate] (Output)
 * @param gcrc Array mit Zellen-IDs
 * @param options_ptr Zeiger auf WQ-Options-Objekt
 * @param continent_ptr Zeiger auf Kontinent-Struktur
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int get_cell_runoff(int actual_year, double (*cell_runoff)[12], int *gcrc, const options_wqClass* options_ptr, const continentStruct* continent_ptr);

/**
 * @brief Liest Grundwasserabfluss für ein Jahr
 * @param actual_year Aktuelles Jahr
 * @param cell_runoff 2D-Array für monatlichen GW-Abfluss [Zelle][12 Monate] (Output)
 * @param gcrc Array mit Zellen-IDs
 * @param grow Array mit Zeilen-Nummern
 * @param options_ptr Zeiger auf WQ-Options-Objekt
 * @param continent_ptr Zeiger auf Kontinent-Struktur
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int get_gw_runoff(int actual_year, double (*cell_runoff)[12], int *gcrc, int *grow, const options_wqClass* options_ptr, const continentStruct* continent_ptr);

/**
 * @brief Liest Abflussmengen Q für ein Jahr
 * @param actual_year Aktuelles Jahr
 * @param Q_out 2D-Array für monatliche Abflüsse [Zelle][12 Monate] (Output)
 * @param gcrc Array mit Zellen-IDs
 * @param options_ptr Zeiger auf WQ-Options-Objekt
 * @param continent_ptr Zeiger auf Kontinent-Struktur
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int get_Q(int actual_year, double (*Q_out)[12], int *gcrc, const options_wqClass* options_ptr, const continentStruct* continent_ptr);

/**
 * @brief Liest Wassertemperaturen für ein Jahr
 * @param actual_year Aktuelles Jahr
 * @param water_temp 2D-Array für monatliche Temperaturen [Zelle][12 Monate] (Output)
 * @param UseWaterTemp Flag ob Wassertemperatur verwendet wird
 * @param IDTemp Temperatur-ID
 * @param options_ptr Zeiger auf Options-Objekt
 * @param continent_ptr Zeiger auf Kontinent-Struktur
 * @param IDScen Szenario-ID (optional, default=-9999)
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int get_waterTemp(int actual_year, double (*water_temp)[12], short UseWaterTemp, int IDTemp, const optionsClass* options_ptr, const continentStruct* continent_ptr, int IDScen=-9999);

/**
 * @brief Berechnet Fließgeschwindigkeit aus Abfluss und Gefälle
 * @param Q_out Abflussmenge [m³/s]
 * @param g_slope Gefälle [m/m]
 * @return Fließgeschwindigkeit [m/s]
 */
double get_flowVelocity(double Q_out, double g_slope);

/**
 * @brief Liest Theta-Parameter für Temperaturabhängigkeit
 * @param teta Theta-Wert für Fließgewässer (Output, Referenz)
 * @param teta_lake Theta-Wert für Seen (Output, Referenz)
 * @param IDScen Szenario-ID
 * @param parameter_id Parameter-ID
 * @param year Jahr
 * @return true bei Erfolg, false bei Fehler
 */
bool getTeta(double &teta, double &teta_lake, const int IDScen, const int parameter_id, const int year);

/**
 * @brief Ermittelt maximales Jahr für Parameter-Input
 * @param MaxYear Maximales Jahr (Output, Referenz)
 * @param IDScen Szenario-ID
 * @param parameter_id Parameter-ID
 * @param year Jahr
 * @return true bei Erfolg, false bei Fehler
 */
bool getYear_parameter_input(int &MaxYear, const int IDScen, const int parameter_id, const int year);

// ============================================================================
// Methoden zur Berechnung von Wasserkonzentrationen
// ============================================================================

/**
 * @brief Berechnet Abbauraten (Attenuation) für Wasserqualitätsparameter
 * @param ke Array für Abbauraten (Output)
 * @param tss_cell Array für Schwebstoffe
 * @param IDScen Szenario-ID
 * @param parameter_id Parameter-ID
 * @param year Jahr
 * @param Q_out Monatliche Abflüsse
 * @param gcrc Zellen-IDs
 * @param grow Zeilen-Nummern
 * @param options_ptr Zeiger auf Options-Objekt
 * @param continent_ptr Zeiger auf Kontinent-Struktur
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int calculateAttenuation(double *ke, double *tss_cell, int IDScen, int parameter_id, int year
		, double (*Q_out)[12], int *gcrc, int *grow, const optionsClass* options_ptr, const continentStruct* continent_ptr);

/**
 * @brief Liest Beziehungen für Abbauraten
 * @param keRel Array für Abbauraten-Beziehungen (Output)
 * @param IDScen Szenario-ID
 * @param parameter_id Parameter-ID
 * @param year Jahr
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int getKeRelations(double *keRel, const int IDScen, const int  parameter_id, const int  year);

/**
 * @brief Liest Schwebstoff-Werte (TSS) für Einzugsgebiet
 * @param tss_cell Array für TSS-Werte (Output)
 * @param IDReg Regions-ID
 * @param continent_ptr Zeiger auf Kontinent-Struktur
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int getTSSBasinValues(double *tss_cell, const int IDReg, const continentStruct* continent_ptr);

/**
 * @brief Liest Abbauraten für Parameter
 * @param decRate Array für Abbauraten (Output)
 * @param IDScen Szenario-ID
 * @param parameter_id Parameter-ID
 * @param year Jahr
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int getDecayRates(double *decRate, int IDScen, int parameter_id, int year);

/**
 * @brief Liest Schadstoffbelastung pro Zelle
 * @param IDScen Szenario-ID
 * @param parameter_id Parameter-ID
 * @param actual_year Aktuelles Jahr
 * @param s_cell 2D-Array für monatliche Belastungen [Zelle][12 Monate] (Output)
 * @param options_ptr Zeiger auf Options-Objekt
 * @param continent_ptr Zeiger auf Kontinent-Struktur
 * @param loadingType Typ der Belastung
 * @param toCalc Berechnungstyp
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int get_s_cell(int IDScen, int parameter_id, int actual_year, double (*s_cell)[12]
	, const optionsClass* options_ptr, const continentStruct* continent_ptr, short loadingType, int toCalc);

/**
 * @brief Liest direkte Schadstoffbelastung pro Zelle (ohne Routing)
 * @param IDScen Szenario-ID
 * @param parameter_id Parameter-ID
 * @param actual_year Aktuelles Jahr
 * @param s_cell 2D-Array für monatliche Belastungen [Zelle][12 Monate] (Output)
 * @param options_ptr Zeiger auf Options-Objekt
 * @param continent_ptr Zeiger auf Kontinent-Struktur
 * @param loadingType Typ der Belastung
 * @param toCalc Berechnungstyp
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int get_s_cell_direct(int IDScen, int parameter_id, int actual_year, double (*s_cell)[12]
	, const optionsClass* options_ptr, const continentStruct* continent_ptr, short loadingType, int toCalc);

// ============================================================================
// Allgemeine Methoden
// ============================================================================

/**
 * @brief Liest Zellen-Informationen aus der Datenbank
 * @param options_ptr Zeiger auf Options-Objekt
 * @param continent Zeiger auf Kontinent-Struktur (Output)
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int getCELLS(const optionsClass* options_ptr, continentStruct* continent);

/**
 * @brief Liest GCRC (Grid Cell Row Column) Informationen
 * @param gcrc Array für Zellen-IDs (Output)
 * @param grow Array für Zeilen-Nummern (Output)
 * @param options_ptr Zeiger auf Options-Objekt
 * @param continent_ptr Zeiger auf Kontinent-Struktur
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int getGCRC(int *gcrc, int *grow, const optionsClass* options_ptr, const continentStruct* continent_ptr);

/**
 * @brief Liest ArcGIS Global IDs
 * @param arc_glob_id Array für Global IDs (Output)
 * @param options_ptr Zeiger auf Options-Objekt
 * @param continent_ptr Zeiger auf Kontinent-Struktur
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int getArcGlobId(int *arc_glob_id, const optionsClass* options_ptr, const continentStruct* continent_ptr);

/**
 * @brief Prüft ob String eine Zahl ist
 * @param str Zu prüfender String
 * @return true wenn Zahl, false sonst
 */
bool IsNumber(char* str);

/**
 * @brief Liest Projekt-Informationen aus Datenbank
 * @param project_id Projekt-ID
 * @param options_ptr Zeiger auf Options-Objekt
 * @param con MySQL++ Verbindung
 * @param application Name der Anwendung
 * @return true bei Erfolg, false bei Fehler
 */
bool getProject(int project_id, const optionsClass* options_ptr, mysqlpp::Connection con, const char* application);

/**
 * @brief Überprüft Parameter-ID und ermittelt Berechnungstyp
 * @param parameter_id Parameter-ID
 * @param toCalc Berechnungstyp (Output, Referenz)
 * @param ParameterName Parameter-Name (Output, Referenz)
 * @param ReadToCalc Flag ob toCalc gelesen werden soll (default=true)
 * @return true bei Erfolg, false bei Fehler
 */
bool checkParameter(int parameter_id, int& toCalc, mysqlpp::String& ParameterName, bool ReadToCalc=true);

/**
 * @brief Überprüft Szenario-ID
 * @param project_id Projekt-ID
 * @param IDScen Szenario-ID
 * @return true wenn gültig, false sonst
 */
bool checkIDScen(int project_id, int IDScen);

/**
 * @brief Überprüft Run-ID und liest zugehörige Parameter (erweiterte Version)
 * @param IDrun Run-ID
 * @param runName Run-Name (Output, Referenz)
 * @param parameter_id_input Input-Parameter-ID (Output, Referenz)
 * @param parameter_id Parameter-ID (Output, Referenz)
 * @param parameter_id_load Load-Parameter-ID (Output, Referenz)
 * @param project_id Projekt-ID (Output, Referenz)
 * @param IDScen Szenario-ID (Output, Referenz)
 * @param IDScen_wq_load WQ-Load Szenario-ID (Output, Referenz)
 * @param IDTemp Temperatur-ID (Output, Referenz)
 * @param Q_low_limit Untere Q-Grenze (Output, Referenz)
 * @param Q_low_limit_type Typ der Q-Grenze (Output, Referenz)
 * @param FlgCons Conservation-Flag (Output, Referenz)
 * @param FlgLake See-Flag (Output, Referenz)
 * @param UseWaterTemp Wassertemperatur-Flag (Output, Referenz)
 * @return Status-Code
 */
short checkIDRun(int IDrun, mysqlpp::String& runName, int& parameter_id_input, int& parameter_id, int& parameter_id_load
		, int& project_id, int& IDScen, int& IDScen_wq_load
		, int& IDTemp, double& Q_low_limit, short& Q_low_limit_type, short& FlgCons, short& FlgLake, short& UseWaterTemp);

/**
 * @brief Überprüft Run-ID und liest zugehörige Parameter (Basisversion)
 * @param IDrun Run-ID
 * @param runName Run-Name (Output, Referenz)
 * @param parameter_id_input Input-Parameter-ID (Output, Referenz)
 * @param parameter_id Parameter-ID (Output, Referenz)
 * @param parameter_id_load Load-Parameter-ID (Output, Referenz)
 * @param project_id Projekt-ID (Output, Referenz)
 * @param IDScen Szenario-ID (Output, Referenz)
 * @param IDScen_wq_load WQ-Load Szenario-ID (Output, Referenz)
 * @return Status-Code
 */
short checkIDRun(int IDrun, mysqlpp::String& runName, int& parameter_id_input, int& parameter_id, int& parameter_id_load
		, int& project_id, int& IDScen, int& IDScen_wq_load);

/**
 * @brief Prüft ob Tabelle in Datenbank existiert
 * @param TableName Name der Tabelle
 * @return true wenn existiert, false sonst
 */
bool isTableExists(const char* TableName);

/**
 * @brief Erstellt neue Datenbank-Tabelle
 * @param TableName Name der zu erstellenden Tabelle
 * @param sql_createTable SQL CREATE TABLE Statement
 * @param QueryExec Flag ob Query ausgeführt werden soll
 * @param mySQLfile Ausgabe-Stream für SQL-Datei
 * @param TableExists Flag ob Tabelle existiert (Output, Referenz)
 * @return Status-Code
 */
short CreateTable(const char* TableName, const char* sql_createTable, bool QueryExec, ofstream &mySQLfile, bool &TableExists);

/**
 * @brief Liest Zellparameter (Flusslänge, Gefälle, Geschwindigkeit)
 * 
 * Hinweis: RoutOrder kann NULL sein, falls nicht benötigt (z.B. in wq_stat)
 *
 * @param riverLength Array für Flusslängen (Output)
 * @param RoutOrder Array für Routing-Reihenfolge (Output, kann NULL sein)
 * @param g_slope Array für Gefälle (Output)
 * @param flowVelocity_const Array für konstante Fließgeschwindigkeiten (Output)
 * @param gcrc Array mit Zellen-IDs
 * @param options_ptr Zeiger auf WQ-Options-Objekt
 * @param continent_ptr Zeiger auf Kontinent-Struktur
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int get_CellParam(double *riverLength, int *RoutOrder, double *g_slope, double *flowVelocity_const, int *gcrc, const options_wqClass* options_ptr, const continentStruct* continent_ptr);

/**
 * @brief Ermittelt ArcID aus globaler ArcID
 * @param options_ptr Zeiger auf Options-Objekt
 * @param arcid_global Globale Arc-ID
 * @return Lokale Arc-ID
 */
int get_ArcID(const optionsClass* options_ptr, int arcid_global);

/**
 * @brief Setzt Länder-Input-Parameter
 * @param number_countries Anzahl der Länder
 * @param count_cont 2D-Array für Länder-Kontinente
 * @param var_one Erste Variable
 * @param var_two Zweite Variable
 * @param var_three Dritte Variable
 * @param IDScen Szenario-ID
 * @param actual_year Aktuelles Jahr
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int setCountryInput(int number_countries, int (*count_cont)[4], std::string var_one, std::string var_two, std::string var_three, int IDScen, int actual_year);

/**
 * @brief Liest Flussnamen aus Input-Datei
 * @param riverName Array für Flussnamen (Output)
 * @param inputfile Input-Stream
 */
void getRiverName(char* riverName, ifstream &inputfile);

/**
 * @brief Führt Memory-Swap (Byte-Swapping) durch
 * 
 * Konvertiert zwischen Big-Endian und Little-Endian Byte-Reihenfolge
 *
 * @param sourceArray Quell-Array
 * @param destArray Ziel-Array
 * @param dataTypeSize Größe des Datentyps in Bytes
 * @param arrayByteSize Gesamtgröße des Arrays in Bytes
 */
void SimMemorySwap(char* sourceArray,
           char* destArray,
           const unsigned short dataTypeSize,
           const unsigned long  arrayByteSize);

/**
 * @brief Liest Flächen-Daten (Area)
 * @param G_area Array für Flächenwerte (Output)
 * @param G_land_area Array für Landflächen (Output)
 * @param gcrc Array mit Zellen-IDs
 * @param options_ptr Zeiger auf Options-Objekt
 * @param continent_ptr Zeiger auf Kontinent-Struktur
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int readArea(float *G_area, char *G_land_area, int *gcrc, const optionsClass* options_ptr, const continentStruct* continent_ptr);

// ============================================================================
// Template-Funktionen für Datei-I/O
// ============================================================================

/**
 * @brief Liest binäre UNF-Datei (Universal Format)
 * @tparam T Datentyp der zu lesenden Werte
 * @param input_file Pfad zur Input-Datei
 * @param output_grid Output-Array
 * @param valuesPerCell Anzahl der Werte pro Zelle (z.B. 12 für monatliche Daten)
 * @param cells Anzahl der Zellen
 * @return 0 bei Erfolg, sonst Fehlercode
 */
template <class T> int readUnf(char *input_file, T *output_grid, int valuesPerCell, int cells);

/**
 * @brief Liest Datei generisch
 * @tparam T Datentyp der zu lesenden Werte
 * @param file Pfad zur Datei
 * @param n_values Anzahl der Werte
 * @param grid Output-Array
 * @return 0 bei Erfolg, sonst Fehlercode
 */
template <class T> int read_file(char *file, int n_values, T *grid);

/**
 * @brief Liest Input-Datei und ordnet Daten auf Grid zu (Non-Template Version)
 * @param input_file Pfad zur Input-Datei
 * @param type_size Größe des Datentyps in Bytes
 * @param valuesPerCell Anzahl der Werte pro Zelle
 * @param number_of_cells Anzahl der Zellen
 * @param G_array Output-Array
 * @param gcrc Array mit Zellen-IDs
 * @param options_ptr Zeiger auf Options-Objekt
 * @param continent_ptr Zeiger auf Kontinent-Struktur
 * @param nodata NODATA-Wert (default=NODATA)
 * @return 0 bei Erfolg, sonst Fehlercode
 */
int readInputFile(char *input_file, int  type_size, int  valuesPerCell, long number_of_cells
		, double *G_array, int *gcrc, const optionsClass* options_ptr, const continentStruct* continent_ptr, float nodata=(float)NODATA);

/**
 * @brief Liest Input-Datei und ordnet Daten auf Grid zu (Template Version)
 * 
 * Diese Template-Funktion liest binäre Dateien im WaterGAP-Format und ordnet
 * die Daten korrekt auf das Grid zu. Unterstützt Byte-Swapping und verschiedene
 * Dateiformate (WG2 vs WG3).
 *
 * @tparam T Datentyp der zu lesenden Werte
 * @param input_file Pfad zur Input-Datei
 * @param type_size Größe des Datentyps in Bytes
 * @param valuesPerCell Anzahl der Werte pro Zelle (z.B. 12 für monatliche Daten)
 * @param number_of_cells Anzahl der Zellen
 * @param G_array Output-Array
 * @param gcrc Array mit Zellen-IDs (Arc-IDs)
 * @param options_ptr Zeiger auf Options-Objekt
 * @param continent_ptr Zeiger auf Kontinent-Struktur
 * @param nodata NODATA-Wert (default=NODATA für Typ T)
 * @return 0 bei Erfolg, 1 bei Fehler
 */
template <class T> int readInputFile(char *input_file, int  type_size, int  valuesPerCell, long number_of_cells
		, T *G_array, int *gcrc, const optionsClass* options_ptr, const continentStruct* continent_ptr, float nodata=(T)NODATA)
{
	cout <<"readInputFile for template; sizeof = " <<sizeof(T)<< "\n";
	T nodata_value;
	if(sizeof(nodata_value)==1) nodata_value=0;
	else nodata_value=nodata;
		ifstream ifile(input_file);
	if (!ifile) { cerr << "Can't open input file "<< input_file<<endl; return 1;}
	T* G_tmp_array = new T[number_of_cells*valuesPerCell];
	long  bytesRead;
	ifile.read((char*)G_tmp_array, number_of_cells*valuesPerCell*type_size);
	bytesRead = ifile.gcount();
	cout << "file "<< input_file << ' '<< bytesRead <<" bytesRead\n"
			<<"number_of_cells: " <<number_of_cells<<" valuesPerCell: "<<valuesPerCell
			<<" type_size: "<<type_size<<' ';
	if (type_size==1 ) cout <<"G_tmp_array "<< (int)G_tmp_array[0]<<endl;
	else  cout <<"G_tmp_array "<<G_tmp_array[5157]<<endl;
	ifile.close();
	if (bytesRead!=number_of_cells*valuesPerCell*type_size) {
		cerr << "Input file size mismatch "<< input_file<<" bytesRead: "<<bytesRead<<" number_of_cells*valuesPerCell*type_size: "<<number_of_cells*valuesPerCell*type_size<<endl;
		return 1;
	}
		// Aendern der Reihenfolge der Bytes (SimMemorySwap)
	#ifdef BYTESWAP_NECESSARY
	SimMemorySwap((char*) G_tmp_array,
	(char*) G_tmp_array,
	type_size,
	number_of_cells*valuesPerCell*type_size);
	#endif
	cout << "file "<< input_file << ' '<< bytesRead <<" bytesRead\n"
		<<"number_of_cells: " <<number_of_cells<<" valuesPerCell: "<<valuesPerCell
		<<" type_size: "<<type_size<<' ';
	if (type_size==1 ) cout << (int)G_tmp_array[0]<<endl;
	else  cout << G_tmp_array[5157]<<endl;

	bool order;
	//WaterGAP2 has rows and columns in an different order than WG3
	//This only applies to UNF files in the row format
	if(strcmp(options_ptr->continent_abb,"wg2")==0 && (number_of_cells==continent_ptr->nrows)){
		order=false; //cout<<"order false"<<endl;
	}else{
		order=true; //cout<<"order true"<<endl;
	}

	// gcrc -> arcid
    int index;
    for (int cell=0; cell<number_of_cells; cell++) {
        for (int month=0; month<valuesPerCell; month++) {
        	if (number_of_cells == continent_ptr->ng || number_of_cells== continent_ptr->ng_land)
        		index = (gcrc[cell]-1)*valuesPerCell+month;
        	else index = cell*valuesPerCell+month;
        	// gcrc[cell] == arcid
            //G_array[index] = G_tmp_array[cell*valuesPerCell+month];
        	if (order) G_array[index] = G_tmp_array[cell*valuesPerCell+month];
        	else  G_array[index] = G_tmp_array[month*number_of_cells+cell];
        	if (G_array[index]!=G_array[index]) {
        		cerr << "file\t"<<input_file<<"\tcell\t"<<cell<<"\tgcrc\t"<<gcrc[cell]
        			<<"\tmonth\t"<<month+1<<'\t'<<G_array[index]<<"\tvalue ="<<NODATA<<endl;
                G_array[index]=nodata_value;
        	}
        }
    }// for(cell)


	//cout<<"before delete "<<endl;
	delete[] G_tmp_array; G_tmp_array = NULL;
	//cout<<"before return"<<endl;
	return 0;
}



/**
 * @brief Konvertiert WaterGAP2 Grid-Daten zu WaterGAP3 Format
 * 
 * Liest WG2-Daten (0.5° Auflösung) und ordnet sie dem WG3-Grid zu
 *
 * @tparam T Datentyp der zu lesenden Werte
 * @param input_file Pfad zur WG2 Input-Datei
 * @param type_size Größe des Datentyps in Bytes
 * @param valuesPerCell Anzahl der Werte pro Zelle
 * @param number_of_cells Anzahl der WG3-Zellen
 * @param G_array 2D Output-Array [Zellen][Monate]
 * @param gcrc Array mit Zellen-IDs
 * @param G_wg3_to_wg2 Mapping-Array von WG3 zu WG2
 * @param ng_wg22 Anzahl der WG2-Zellen (default=70412)
 * @return 0 bei Erfolg, 1 bei Fehler
 */
template<class T>
int readInputFileWG2toWG3( char *input_file, int  type_size, int  valuesPerCell, int number_of_cells, T (*G_array)[12], int *gcrc, int *G_wg3_to_wg2, int ng_wg22=70412)
{
	T nodata_value;
	if(sizeof(nodata_value)==1) nodata_value=0;
	else nodata_value=(T)NODATA;
		ifstream ifile(input_file);
	if (!ifile) { cerr << "Can't open input file "<< input_file<<endl; return 1;}
	cout<<"G_* Input on 0.5°"<<endl;
	
	T* G_tmp_array = new T[ng_wg22*valuesPerCell];
	long  bytesRead;
	ifile.read((char*)G_tmp_array, ng_wg22*valuesPerCell*type_size);
	bytesRead = ifile.gcount();
	cout << "file "<< input_file << ' '<< bytesRead <<" bytesRead\n"
			<<"ng_wg22: " <<ng_wg22<<" valuesPerCell: "<<valuesPerCell
			<<" type_size: "<<type_size<<' ';
	if (type_size==1 ) cout <<"G_tmp_array "<< (int)G_tmp_array[33988]<<endl;
	else  cout <<"G_tmp_array "<<G_tmp_array[33988]<<endl;
		ifile.close();
		if (bytesRead!=ng_wg22*valuesPerCell*type_size) {
		cerr << "Input file size mismatch "<< input_file<<" bytesRead: "<<bytesRead<<" ng_wg22*valuesPerCell*type_size: "<<ng_wg22*valuesPerCell*type_size<<endl;
		return 1;
	}
		// Aendern der Reihenfolge der Bytes (SimMemorySwap)
	#ifdef BYTESWAP_NECESSARY
	SimMemorySwap((char*) G_tmp_array,
	(char*) G_tmp_array,
	type_size,
	ng_wg22*valuesPerCell*type_size);
	#endif
	cout << "file "<< input_file << ' '<< bytesRead <<" bytesRead\n"
		<<"number_of_cells: " <<number_of_cells<<" valuesPerCell: "<<valuesPerCell
		<<" type_size: "<<type_size<<' ';
	if (type_size==1 ) cout << (int)G_tmp_array[33988]<<endl;
	else  cout << G_tmp_array[33988]<<endl;

	//for(int i=0;i<ng_wg22*valuesPerCell; i++)
	//	cout<<"G_tmp_array["<<i<<"]"<<G_tmp_array[i]<<endl;

	// gcrc -> arcid
	for (int cell=0; cell<number_of_cells; cell++) {
		for (int month=0; month<valuesPerCell; month++) {
		// gcrc[cell] == arcid
			G_array[cell][month] = G_tmp_array[(G_wg3_to_wg2[cell]-1)*valuesPerCell+month];
			/*if(cell==144248)
				cout<<"G_wg3_to_wg2[cell]: "<<G_wg3_to_wg2[cell]<<" G_tmp_array[(G_wg3_to_wg2[cell]-1)*valuesPerCell+month]: "<<G_tmp_array[(G_wg3_to_wg2[cell]-1)*valuesPerCell+month]
				       <<"G_tmp_array[(33989-1)*12+month]"<<endl;
			 */
			if (G_array[gcrc[cell]-1][month]!=G_array[gcrc[cell]-1][month]) {
				cerr << "file\t"<<input_file<<"\tcell\t"<<cell<<"\tgcrc\t"<<gcrc[cell]
				    <<"\tmonth\t"<<month+1<<'\t'<<G_array[gcrc[cell]-1][month]<<"\tvalue ="<<NODATA<<endl;
				G_array[gcrc[cell]-1][month]=nodata_value;
			}
		}
	}// for(cell)

	delete[] G_tmp_array; G_tmp_array = NULL;
	return 0;
}

#endif /* GENERAL_FUNCTIONS_H_ */
