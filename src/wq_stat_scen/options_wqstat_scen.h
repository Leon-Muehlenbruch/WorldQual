/**
 * @file options_wqstat_scen.h
 * @brief Optionen-Klasse für Szenario-Vergleichs-Statistiken
 * @author kynast
 * @date 11.05.2015
 * 
 * Erweitert die Basis-optionsClass für den Vergleich von Baseline-
 * und Szenario-Runs zur Bewertung von Zukunftsszenarien.
 */

#ifndef OPTIONS_WQSTAT_SCEN_H_
#define OPTIONS_WQSTAT_SCEN_H_

#include "options.h"

/**
 * @class options_wqstat_scenClass
 * @brief Konfigurationsklasse für Szenario-Vergleichsstatistiken
 * 
 * Verwaltet Parameter für den statistischen Vergleich zwischen einer
 * Baseline-Simulation und bis zu drei Szenario-Simulationen. Ermöglicht
 * die Bewertung von Zukunftsszenarien und Policy-Optionen.
 */
class options_wqstat_scenClass: public optionsClass {
public:
	/**
	 * @brief Konstruktor mit Standard-Werten
	 * @param option_c Anzahl der Kommandozeilen-Argumente
	 * @param option_v Array der Kommandozeilen-Argumente
	 * @param CL Kommandozeilen-String
	 */
	options_wqstat_scenClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		IDrun = 0;
		IDrun_scen1 = 0;
		IDrun_scen2 = 0;
		IDrun_scen3 = 0;
		DiffYears = 0;
		watertype = 1;
	}
	
	/**
	 * @brief Destruktor
	 */
	virtual ~options_wqstat_scenClass();

	// ========================================================================
	// WQ-Stat-Scenario spezifische Optionen
	// ========================================================================
	
	/**
	 * @brief Run-ID für Baseline-Szenario
	 * 
	 * Referenz-Run gegen den die Szenarien verglichen werden
	 */
	int IDrun;
	
	/**
	 * @brief Run-ID für Szenario 1
	 * 
	 * Erste alternative Zukunfts-/Policy-Simulation
	 */
	int IDrun_scen1;
	
	/**
	 * @brief Run-ID für Szenario 2
	 * 
	 * Zweite alternative Zukunfts-/Policy-Simulation
	 */
	int IDrun_scen2;
	
	/**
	 * @brief Run-ID für Szenario 3
	 * 
	 * Dritte alternative Zukunfts-/Policy-Simulation
	 */
	int IDrun_scen3;
	
	/**
	 * @brief Jahresunterschied zwischen Baseline und Szenarien
	 * 
	 * Gibt an, wie viele Jahre in die Zukunft die Szenarien projizieren.
	 * Beispiel: DiffYears=30 bedeutet Baseline-Jahr + 30 Jahre
	 */
	int DiffYears;
	
	/**
	 * @brief Gewässertyp
	 * 
	 * - 1: Fluss (default)
	 * - Andere Werte: Spezifische Gewässertypen
	 */
	int watertype;

	/**
	 * @brief Initialisiert die Optionen aus Kommandozeilenargumenten
	 * @param optionc Anzahl der Argumente
	 * @param optionv Array der Argumente
	 * @return Status-Code (0 bei Erfolg)
	 */
	short init(int optionc, char* optionv[]);
	
	/**
	 * @brief Zeigt Hilfe-Text an
	 * @param num Hilfe-Level (default=1)
	 */
	void Help(int num=1);

private:

};

#endif /* OPTIONS_WQSTAT_SCEN_H_ */
