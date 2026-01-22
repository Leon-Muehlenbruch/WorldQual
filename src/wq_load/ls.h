/**
 * @file ls.h
 * @brief Klasse zur Verwaltung von Viehbestand-Daten (Livestock)
 * @author reder
 * @date 18.04.2012
 * 
 * Verwaltet Informationen über verschiedene Vieharten und deren
 * Exkretionsraten für Nährstoffe und Fäkale Coliforme.
 */

#ifndef LS_H_
#define LS_H_

#include <stdio.h>
#include <mysql++.h>
#include "../general_function/country.h"
#include "../general_function/general_functions.h"

using namespace std;
using namespace mysqlpp;

/// Name der MySQL-Datenbank
extern char MyDatabase[maxcharlength];

/**
 * @class LS_CLASS
 * @brief Verwaltet Viehbestand-Daten und Exkretionsraten
 * 
 * Diese Klasse liest und verwaltet Informationen über:
 * - Verschiedene Vieharten (Livestock Types)
 * - FAO-Regionen für Länder
 * - Exkretionsraten von Nährstoffen und Pathogenen
 * - Länderspezifische Viehbestand-Parameter
 */
class LS_CLASS{
public:
	/// Anzahl der Vieharten
	int number;

	/**
	 * @brief Konstruktor - Initialisiert Viehbestand-Daten
	 * @param parameter_id Parameter-ID (TN, TP oder FC)
	 * @param IDScen Szenario-ID
	 * @param countries Zeiger auf COUNTRY_CLASS Objekt
	 * @param toCalc Berechnungstyp (2=FC, 3=TN, 4=TP)
	 */
	LS_CLASS(const int parameter_id, const int IDScen, COUNTRY_CLASS* countries, const int toCalc);

	/**
	 * @brief Gibt Exkretionsrate für eine Viehart zurück
	 * @param country Land-ID (ISO-Nummer)
	 * @param animal Vieh-Typ ID
	 * @param toCalc Berechnungstyp (2=FC, 3=TN, 4=TP)
	 * @param ex_ls Exkretionsrate (Output, Referenz) [t/(a*Tier)] für TN/TP, [10^10 cfu/(a*Tier)] für FC
	 * @param countries Zeiger auf COUNTRY_CLASS Objekt
	 * @return 0 bei Erfolg, sonst Fehlercode
	 */
	int getLS_ex_rate(int country, int animal, int toCalc, double &ex_ls, COUNTRY_CLASS* countries);

private:
	/// Anzahl der FAO-Regionen
	int numberFAOreg;
	
	/// Parameter-ID (TN, TP oder FC)
	int parameter_id;
	
	/// Szenario-ID
	int IDScen;

	/// MySQL++ Ergebnis-Container
	mysqlpp::StoreQueryResult res;

	/// Array der Vieharten-IDs
	short *ls;
	
	/**
	 * @brief Exkretionsraten der Vieharten
	 * 
	 * 2D-Array: [FAO-Region][Vieh-Typ]
	 * Einheit: [t/(a*Tier)] für TN/TP
	 *          [10^10 cfu/(a*Tier)] für FC (Fäkale Coliforme)
	 */
	double **ex_ls;
	
	/// Array: FAO-Region für jedes Land [Land-Position]
	int *FAORegCoun;

	/**
	 * @brief Ermittelt Anzahl der Vieharten
	 * @return Anzahl der Livestock Types
	 */
	int getDataNumber();
	
	/**
	 * @brief Ermittelt Anzahl der FAO-Regionen
	 * @return Anzahl der FAO-Regionen
	 */
	int getDataNumberFAOreg();
	
	/**
	 * @brief Liest alle Exkretionsraten-Daten
	 * @param isSenAnlys Flag ob Sensitivitätsanalyse
	 */
	void getData(bool isSenAnlys);
	
	/**
	 * @brief Ermittelt FAO-Region für alle Länder
	 * @param countries Zeiger auf COUNTRY_CLASS Objekt
	 * @return 0 bei Erfolg, sonst Fehlercode
	 */
	int getFAOREgion(COUNTRY_CLASS* countries);
	
	/**
	 * @brief Findet Array-Position einer Viehart
	 * @param animal Vieh-Typ ID
	 * @return Array-Position oder -1 wenn nicht gefunden
	 */
	int getAnimalPos(int animal);

};

#endif /* LS_H_ */
