/**
 * @file crop.h
 * @brief Klasse zur Verwaltung von Kulturpflanzen-Daten und Düngerraten
 * @author reder
 * @date 18.04.2012
 * 
 * Verwaltet Informationen über Kulturpflanzen (Crop Types) und deren
 * Düngemittelanwendungsraten (Stickstoff und Phosphor) für verschiedene
 * Länder und Jahre.
 */

#ifndef CROP_H_
#define CROP_H_

#include <stdio.h>
#include <mysql++.h>
#include "../general_function/country.h"
#include "../general_function/general_functions.h"

using namespace std;
using namespace mysqlpp;

/// Name der MySQL-Datenbank
extern char MyDatabase[maxcharlength];

/// Kontinent-Abkürzung für Datenbank-Erkennung
extern char continent_abb[10];


/**
 * @class CROP_CLASS
 * @brief Verwaltet Kulturpflanzen-Daten und Düngemittelanwendungsraten
 * 
 * Diese Klasse liest und verwaltet Informationen über:
 * - Verschiedene Kulturpflanzen-Typen (Crop Types)
 * - GLCC (Global Land Cover Characterization) Äquivalente
 * - Anwendungsraten für industrielle Düngemittel (N und P)
 * - Länderspezifische Kulturpflanzen-Daten
 */
class CROP_CLASS{
public:
	/**
	 * @brief Konstruktor - Initialisiert Kulturpflanzen-Daten
	 * @param parameter_id Parameter-ID (TN oder TP)
	 * @param countries Zeiger auf COUNTRY_CLASS Objekt
	 * @param toCalc Berechnungstyp (3=TN, 4=TP)
	 * @param year Berechnungsjahr
	 */
	CROP_CLASS(const int parameter_id, COUNTRY_CLASS* countries, const int toCalc, const int year);

	/**
	 * @brief Gibt Düngemittel-Anwendungsrate zurück
	 * @param country Land-ID (ISO-Nummer)
	 * @param crop Kulturpflanzen-Typ
	 * @param cell Zellen-ID
	 * @return Anwendungsrate in t/(km²*a)
	 */
	double getCrop_Appl_Rate(int country, int crop, int cell);

private:
	/// Anzahl der Kulturpflanzen-Typen
	int numberCT;
	
	/// Parameter-ID (TN oder TP)
	int parameter_id;
	
	/// Berechnungstyp (3=TN, 4=TP)
	int toCalc;
	
	/**
	 * @brief Jahr aus dem die Werte genommen werden
	 * 
	 * Falls keine Werte für das aktuelle Jahr vorhanden sind,
	 * wird nicht in der Zeit zurückgegangen. Wert gilt global
	 * für alle Länder und Kulturpflanzen-Typen.
	 */
	int YearFrom;

	/// MySQL++ Ergebnis-Container
	mysqlpp::StoreQueryResult res;

	/// Array der Kulturpflanzen-Typen
	short *crop_type;
	
	/// GLCC (Global Land Cover Characterization) Äquivalente zu Kulturpflanzen-Typen
	short *glcc;

	/**
	 * @brief Anwendungsrate anorganischer Stickstoffdünger
	 * 
	 * 2D-Array: [Land][Kulturpflanzen-Typ]
	 * Einheit: t TN/(km²*a)
	 */
	double **rate_N;
	
	/**
	 * @brief Anwendungsrate anorganischer Phosphordünger
	 * 
	 * 2D-Array: [Land][Kulturpflanzen-Typ]
	 * Einheit: t TP/(km²*a)
	 */
	double **rate_P;

	/**
	 * @brief Ermittelt Anzahl der Kulturpflanzen-Typen
	 * @return Anzahl der Crop Types
	 */
	int getDataNumberCT();
	
	/**
	 * @brief Liest GLCC-Äquivalente für Kulturpflanzen-Typen
	 */
	void getGLCC();
	
	/**
	 * @brief Ermittelt Kulturpflanzen-Nummern für ein bestimmtes Land
	 * @param countryIsoNr ISO-Nummer des Landes
	 * @param crop_numb Array für Kulturpflanzen-Nummern (Output)
	 * @param ccn Anzahl der Kulturpflanzen im Land (Output)
	 * @return true bei Erfolg, false bei Fehler
	 */
	bool getDataNumberActCrops(int countryIsoNr, int *crop_numb, int *ccn);
	
	/**
	 * @brief Liest alle Daten für alle Länder
	 * @param countries Zeiger auf COUNTRY_CLASS Objekt
	 */
	void getData(COUNTRY_CLASS* countries);
	
	/**
	 * @brief Ermittelt verfügbares Jahr für Daten
	 * 
	 * Gibt das maximale Jahr zurück, für das Werte für rate_N und rate_P
	 * vorhanden sind, das kleiner oder gleich dem aktuellen Jahr ist.
	 *
	 * @param year Aktuelles Jahr
	 * @return Verfügbares Jahr mit Daten
	 */
	int getYearFrom(int year);

};

#endif /* CROP_H_ */
