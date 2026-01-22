/**
 * @file optionsfindRiverCells.h
 * @brief Optionen-Klasse für das Finden von Flusszellen
 * @author kynast
 * @date 17.08.2016
 * 
 * Ermöglicht das Identifizieren und Extrahieren von Flusszellen zwischen
 * zwei Punkten im Flussnetzwerk, entweder aus einer Input-Datei oder
 * über direkt angegebene Start- und Endpunkte.
 */

#ifndef OPTIONSFINDRIVERCELLS_H_
#define OPTIONSFINDRIVERCELLS_H_

#include "../options/options.h"
#include "general_functions.h"


/**
 * @class options_findRiverCellsClass
 * @brief Konfigurationsklasse für das Finden von Flusszellen
 * 
 * Diese Klasse verwaltet Parameter zum Identifizieren von Flusszellen
 * zwischen Start- und Endpunkten, entweder aus einer Stations-Datei
 * oder über direkt spezifizierte Global-IDs und Arc-IDs.
 */
class options_findRiverCellsClass: public optionsClass {
public:
	/**
	 * @brief Konstruktor mit Standard-Werten
	 * @param option_c Anzahl der Kommandozeilen-Argumente
	 * @param option_v Array der Kommandozeilen-Argumente
	 * @param CL Kommandozeilen-String
	 */
	options_findRiverCellsClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL)
	{
		inputfile = new char[maxcharlength];
		useInputFile = true;

		strcpy(inputfile,"STAT.DAT");

		startGlobal = NODATA;
		startArcID = NODATA;
		endGlobal = NODATA;
		endArcID = NODATA;
	}
	
	/**
	 * @brief Destruktor
	 */
	virtual ~options_findRiverCellsClass();

	// ========================================================================
	// Find-River-Cells spezifische Optionen
	// ========================================================================
	
	/**
	 * @brief Pfad zur Input-Datei mit Stations-Informationen
	 * 
	 * Standard: "STAT.DAT"
	 * Enthält Start- und Endpunkte für Flussabschnitte
	 */
	char* inputfile;
	
	/**
	 * @brief Flag ob Input-Datei verwendet wird
	 * - true: Start/End-Punkte aus inputfile lesen (default)
	 * - false: Start/End-Punkte über Kommandozeile spezifizieren
	 */
	bool useInputFile;
	
	/**
	 * @brief Start-Global-ID
	 * 
	 * Global-ID des Startpunkts (nur wenn useInputFile=false)
	 * Standard: NODATA
	 */
	int startGlobal;
	
	/**
	 * @brief Start-Arc-ID
	 * 
	 * Arc-ID des Startpunkts (nur wenn useInputFile=false)
	 * Standard: NODATA
	 */
	int startArcID;
	
	/**
	 * @brief End-Global-ID
	 * 
	 * Global-ID des Endpunkts (nur wenn useInputFile=false)
	 * Standard: NODATA
	 */
	int endGlobal;
	
	/**
	 * @brief End-Arc-ID
	 * 
	 * Arc-ID des Endpunkts (nur wenn useInputFile=false)
	 * Standard: NODATA
	 */
	int endArcID;

	/**
	 * @brief Initialisiert die Optionen aus Kommandozeilenargumenten
	 * @param optionc Anzahl der Argumente
	 * @param optionv Array der Argumente
	 * @return Status-Code (0 bei Erfolg)
	 */
	short init(int optionc, char* optionv[]);
	
	/**
	 * @brief Gibt alle aktuellen Optionen auf stdout aus
	 */
	void printOptions();
	
	/**
	 * @brief Zeigt Hilfe-Text an
	 * @param num Hilfe-Level (default=1)
	 */
	void Help(int num=1);

};

#endif /* OPTIONSFINDRIVERCELLS_H_ */
