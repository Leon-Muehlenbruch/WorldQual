/**
 * @file gb.h
 * @brief Klasse zur Verwaltung von geogenem Hintergrund (Geogenic Background)
 * @author reder
 * @date 03.07.2012
 * 
 * Verwaltet Informationen über natürliche Hintergrundbelastungen wie
 * atmosphärische Deposition und geogene Konzentrationen für verschiedene
 * Landnutzungstypen.
 */

#ifndef GB_H_
#define GB_H_

#include <stdio.h>
#include <mysql++.h>
#include "../general_function/general_functions.h"

using namespace std;
using namespace mysqlpp;

/// Name der MySQL-Datenbank
extern char MyDatabase[maxcharlength];


/**
 * @class GB_CLASS
 * @brief Verwaltet geogene Hintergrund-Daten für verschiedene Landnutzungen
 * 
 * Diese Klasse liest und verwaltet natürliche Hintergrundbelastungen:
 * - Atmosphärische Deposition (für TN, TP)
 * - Geogene Konzentrationen im Oberflächenabfluss (für BOD)
 * - Landnutzungsspezifische Werte
 */
class GB_CLASS {
public:
	/**
	 * @brief Konstruktor - Initialisiert geogene Hintergrund-Daten
	 * @param parameter_id Parameter-ID (BOD, TN oder TP)
	 * @param toCalc Berechnungstyp (0=BOD, 3=TN, 4=TP)
	 */
	GB_CLASS(const int parameter_id, const int toCalc);

	/**
	 * @brief Gibt geogene Hintergrund-Daten für Landnutzungstyp zurück
	 * 
	 * Für TN/TP: Atmosphärische Depositionsrate in t/(a*ha)
	 * Für BOD: Konzentration im Oberflächenabfluss in t/L
	 *
	 * @param LU_cell Landnutzungs-ID der Zelle
	 * @param geoback Geogener Hintergrundwert (Output, Referenz)
	 * @return true bei Erfolg, false wenn Landnutzung nicht gefunden
	 */
	bool getGeoBackData(short LU_cell, double &geoback);

private:
	/// Parameter-ID (BOD, TN oder TP)
	int parameter_id;
	
	/// Array der Landnutzungs-IDs
	int* lu;
	
	/**
	 * @brief Array der geogenen Hintergrundwerte
	 * 
	 * Einheit abhängig vom Parameter:
	 * - TN/TP: Depositionsrate [t/(a*ha)]
	 * - BOD: Konzentration im Oberflächenabfluss [t/L]
	 */
	double *geo_back;
	
	/// Anzahl der Landnutzungstypen
	int nrLanduse;

	/// MySQL++ Ergebnis-Container
	mysqlpp::StoreQueryResult res;
	
	/**
	 * @brief Liest geogene Hintergrund-Daten aus Datenbank
	 */
	void getData();
	
	/**
	 * @brief Ermittelt Anzahl der Landnutzungstypen
	 * @return Anzahl der Landnutzungstypen
	 */
	int getNrLanduse();
	
	/**
	 * @brief Findet Array-Position für eine Landnutzungs-ID
	 * @param LU_cell Landnutzungs-ID
	 * @return Array-Position oder -1 wenn nicht gefunden
	 */
	int getLUpos(short LU_cell);

};

#endif /* GB_CLASS_H_ */
