/**
 * @file options_wq_load.h
 * @brief Optionen-Klasse für WQ-Load Belastungsberechnungen
 * @author kynast
 * @date 22.05.2015
 * 
 * Erweitert die Basis-optionsClass um spezifische Parameter für
 * Wasserqualitäts-Belastungsberechnungen (wq_load Modul).
 */

#ifndef OPTIONS_WQ_LOAD_H_
#define OPTIONS_WQ_LOAD_H_

#include "../options/options.h"

/**
 * @class options_wq_loadClass
 * @brief Konfigurationsklasse für WQ-Load Berechnungen
 * 
 * Erbt von optionsClass und fügt Parameter für Belastungsberechnungen hinzu,
 * einschließlich Pfade zu Eingabedaten, Szenario-IDs und Timing-Parameter.
 */
class options_wq_loadClass: public optionsClass {
public:
	/**
	 * @brief Konstruktor mit Standard-Werten
	 * @param option_c Anzahl der Kommandozeilen-Argumente
	 * @param option_v Array der Kommandozeilen-Argumente
	 * @param CL Kommandozeilen-String
	 */
	options_wq_loadClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		parameter_id = 310;
		following_year = 0;
		project_id = 4;
		IDScen = 91;
		manure_timing = 0;
		climate = 0;
	}
	
	/**
	 * @brief Destruktor
	 */
	virtual ~options_wq_loadClass();

	// ========================================================================
	// WQ-Load spezifische Optionen
	// ========================================================================
	
	/// Parameter-ID (BOD=110, Salz=210, FC=310, TN=410, TP=510, Pestizide=610)
	int parameter_id;

	/**
	 * @brief Flag für Pestizid-Berechnungen mit Vorjahr
	 * - 0: Erstes Berechnungsjahr
	 * - 1: Folgejahr (verwendet Dezember-Belastung vom Vorjahr)
	 */
	int following_year;

	/// Projekt-ID
	int project_id;
	
	/// Szenario-ID
	int IDScen;
	
	/**
	 * @brief Zeitpunkt der Dung-Ausbringung
	 * - 0: Standard-Timing
	 * - Andere Werte: Spezifische Timing-Szenarien
	 */
	short manure_timing;

	/**
	 * @brief Klima-Input Auflösung
	 * - 0: 0.5° Auflösung (default)
	 * - 1: 5 Minuten Auflösung
	 */
	short climate;
	
	/**
	 * @brief Quelle für gegridete Bevölkerungsdaten
	 * - 0: Aus wq_load_[continent].cell_input Tabelle (default)
	 * - 1: Aus UNF-Dateien (griddi-Output)
	 */
	short gridded_pop_from_file;
	
	/// Pfad zu gegriddeten Bevölkerungs-UNF-Dateien (nur wenn gridded_pop_from_file=1)
	char* gridded_pop_path;

	// ========================================================================
	// Pfad-Konfigurationen
	// ========================================================================
	
	/**
	 * @brief Pfad zu WaterGAP Hydrology Output
	 * 
	 * Enthält Dateien:
	 * - G_URBAN_RUNOFF_[year].12.UNF0
	 * - G_SURFACE_RUNOFF_[year].12.UNF0
	 * - G_GW_RUNOFF_[year].12.UNF0
	 * - G_SURFACE_RUNOFF_MEAN.UNF0 (für TP)
	 */
	char* path_watergap_output;
	
	/**
	 * @brief Pfad zu Viehbestand-Output
	 * 
	 * Enthält: G_LIVESTOCK_NR_[year].12.UNF0
	 */
	char* path_livestock_output;
	
	/**
	 * @brief Pfad zu Korrekturfaktoren für Rückfluss
	 * 
	 * Enthält: G_CORR_FACT_RTF_[year].12.UNF0
	 */
	char* path_corr_factor;
	
	/**
	 * @brief Pfad zu Klimadaten
	 * 
	 * Enthält:
	 * - GTEMP_[year].12.UNF2
	 * - GPREC_[year].12.UNF2 (nur für Pestizide)
	 */
	char* path_climate;

	/**
	 * @brief Pfad zu GNRD-Daten (Nitrogen-related)
	 * 
	 * Enthält: GNRD_[year].12.UNF1
	 */
	char* path_gnrd;

	/**
	 * @brief Pfad zu TP-Input-Daten (Set 1)
	 * 
	 * Enthält:
	 * - G_SOILEROS.UNF0 (Bodenerosion)
	 * - G_PWEATHERING.UNF0 (Phosphor-Verwitterung)
	 * - GLCC[year].UNF2 (Land Cover für TN/TP)
	 */
	char* path_tp_input;

	/**
	 * @brief Pfad zu TP-Input-Daten (Set 2)
	 * 
	 * Enthält:
	 * - CROPLAND_CORR_KM2_[year].UNF0 (Korrigierte Ackerfläche für TN/TP)
	 * - P_RATE_TON_KM2_[year].UNF0 (Phosphor-Rate für TN/TP)
	 */
	char* path_tp_input2;

	/**
	 * @brief Pfad zu TP-Input-Daten (Set 3)
	 * 
	 * Enthält:
	 * - G_PATMDEPOS_[year].UNF0 (Atmosphärische P-Deposition)
	 */
	char* path_tp_input3;

	/**
	 * @brief Flag ob IDScen und parameter_id in Tabellennamen enthalten sind
	 * - 0: Standardtabellennamen (default)
	 * - 1: Tabellennamen enthalten IDScen und parameter_id (z.B. cell_input_91_510)
	 */
	short IDInTableName;

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

#endif /* OPTIONS_WQ_LOAD_H_ */
