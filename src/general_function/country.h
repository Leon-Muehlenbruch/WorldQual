/**
 * @file country.h
 * @brief Klasse zur Verwaltung von Länderinformationen
 * @author Ellen Teichert
 * 
 * Diese Klasse stellt Funktionalität zur Verfügung, um Länderinformationen
 * aus der Datenbank zu lesen und zu verwalten, insbesondere ISO-Nummern
 * und Länderpositionen.
 */

#ifndef COUNTRY_H
#define COUNTRY_H

#include <stdio.h>
#include <mysql++.h>
#include "../general_function/general_functions.h"

using namespace std;
using namespace mysqlpp;

/// Name der MySQL-Datenbank
extern char MyDatabase[maxcharlength];

/// Kontinent-Abkürzung für Datenbank-Erkennung
extern char continent_abb[10];


/**
 * @class COUNTRY_CLASS
 * @brief Verwaltet Länderinformationen für ein bestimmtes Szenario und Jahr
 * 
 * Diese Klasse liest Länder-IDs (ISO-Nummern) aus der Datenbank und
 * stellt Methoden zur Verfügung, um zwischen Länder-Positionen im Array
 * und ISO-Nummern zu konvertieren.
 */
class COUNTRY_CLASS{
public:
	/// Anzahl der Länder
	int number;

	/**
	 * @brief Konstruktor - Liest Länder aus Datenbank
	 * @param IDScen Szenario-ID
	 * @param year Jahr für das die Länder geladen werden
	 */
	COUNTRY_CLASS(const int IDScen, const int year);
	
	/**
	 * @brief Findet Array-Position eines Landes anhand der ISO-Nummer
	 * @param country ISO-Nummer des Landes
	 * @return Array-Position des Landes, oder -1 wenn nicht gefunden
	 */
	int get_countryPos(int country);
	
	/**
	 * @brief Gibt ISO-Nummer für eine Länder-Position zurück
	 * @param country_pos Position im Länder-Array
	 * @return ISO-Nummer des Landes, oder 0 bei ungültiger Position
	 */
	int get_ISONUM(int country_pos) {
		if (country_pos<0 || country_pos>=number) 
			return 0; 
		else 
			return Country[country_pos];
	}
	

private:
	/// Szenario-ID
	int IDScen;
	
	/// Jahr
	int year;
	
	/// MySQL++ Ergebnis-Container
	mysqlpp::StoreQueryResult res;
	
	/// MySQL++ Zeile
	mysqlpp::Row row;
	
	/// Array der Länder-ISO-Nummern
	int *Country;

	/**
	 * @brief Bestimmt Anzahl der Länder im Szenario
	 * @return Anzahl der Länder
	 */
	int getCouintryNumber();
	
	/**
	 * @brief Lädt Länder-ISO-Nummern aus Datenbank
	 * @return 0 bei Erfolg, sonst Fehlercode
	 */
	int getCouintries();
	
};

#endif
