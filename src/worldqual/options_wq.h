/**
 * @file options_wq.h
 * @brief Optionen-Klasse für WorldQual Wasserqualitätsmodellierung
 * @author kynast
 * @date 11.05.2015
 * 
 * Erweitert die Basis-optionsClass um spezifische Parameter für
 * Wasserqualitätsberechnungen, Routing und Temperaturmodellierung.
 */

#ifndef OPTIONS_WQ_H_
#define OPTIONS_WQ_H_

/**
 * @class options_wqClass
 * @brief Konfigurationsklasse für WorldQual-spezifische Einstellungen
 * 
 * Erbt von optionsClass und fügt Parameter für Fließgeschwindigkeit,
 * Routing, Klimadaten und Wassertemperatur hinzu.
 */
class options_wqClass: public optionsClass {
public:
	/**
	 * @brief Konstruktor für WorldQual-Optionen
	 * @param option_c Anzahl der Kommandozeilen-Argumente
	 * @param option_v Array der Kommandozeilen-Argumente
	 * @param CL Kommandozeilen-String
	 */
	options_wqClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		riverLength = 0;
		routingOrder = 0;
		flowVelocityVariant = 0;
		ConstVelocity = 0.0;
		climate = 0;
	}
	
	/**
	 * @brief Destruktor
	 */
	virtual ~options_wqClass();

	// ========================================================================
	// WorldQual-spezifische Optionen
	// ========================================================================
	
	/// Flag ob Flusslängen verwendet werden
	bool riverLength;
	
	/// Flag ob Routing-Reihenfolge verwendet wird
	bool routingOrder;
	
	/**
	 * @brief Variante der Fließgeschwindigkeitsberechnung
	 * 
	 * - 0: Variable Fließgeschwindigkeit aus Tabelle/UNF-Datei (Standard)
	 * - 1: Variable Fließgeschwindigkeit (berechnet)
	 * - 2: Konstante Fließgeschwindigkeit (ein Wert für alle Zellen)
	 */
	short flowVelocityVariant;
	
	/**
	 * @brief Konstante Fließgeschwindigkeit [m/s]
	 * 
	 * Nur relevant wenn flowVelocityVariant = 2
	 * Wert wird aus Kommandozeile entnommen
	 */
	double ConstVelocity;

	/**
	 * @brief Klima-Input Auflösung
	 * 
	 * - 0: 0.5° Auflösung (default)
	 * - 1: 5 Minuten Auflösung
	 * - 2: 0.5° CRU/WATCH
	 */
	short climate;

	/**
	 * @brief Flag für Wassertemperatur-Berechnung
	 * 
	 * - false (0): Wassertemperatur wird aus Datenbank eingelesen
	 * - true (1): Wassertemperatur wird aus Lufttemperatur berechnet
	 *   - Benötigt bei climate==0: G_ELEV_MEAN_CLM.UNF0
	 *   - Benötigt bei climate==2: G_ELEV_MEAN_WATCH.UNF0
	 */
	bool isWaterTempFromAir;

	// ========================================================================
	// Pfad-Konfigurationen
	// ========================================================================
	
	/**
	 * @brief Pfad zu WaterGAP Hydrology Output
	 * 
	 * Enthält Dateien wie:
	 * - G_RIVER_VELOCITY_%d.12.UNF0
	 * - G_GLO_LAKE_STOR_MEAN_%d.12.UNF0
	 * - G_RES_STOR_MEAN_%d.12.UNF0
	 */
	char* path_watergap_output;
	
	/**
	 * @brief Pfad zu Klimadaten
	 * 
	 * Enthält Dateien wie:
	 * - GSHORTWAVE_%d.12.UNF2
	 * - GTEMP_[year].12.UNF2 (für Wassertemperatur aus Lufttemperatur)
	 */
	char* path_climate;

	/**
	 * @brief Pfad zu hydrologischen Input-Daten
	 * 
	 * Enthält Dateien wie:
	 * - G_WG3_WG2WITH5MIN.UNF4
	 * - G_MEANDERING_RATIO.UNF0
	 * - G_ELEV_MEAN_CLM.UNF0 (wenn Wassertemp. aus Lufttemp., climate 0.5)
	 * - G_ELEV_MEAN_WATCH.UNF0 (wenn Wassertemp. aus Lufttemp., climate CRU/WATCH)
	 */
	char* path_hydro_input;
	
	/**
	 * @brief Pfad zu hydrologischen Routing-Daten
	 * 
	 * Enthält: GCELLDIST.9.UNF0
	 */
	char* path_hydro_routing;
	
	/**
	 * @brief Typ der Belastungsdaten (Loading Type)
	 * 
	 * - 0: Tabelle "load"
	 * - 1: Tabelle "load_[IDScen]_[parameter_id]"
	 * - 2: Aufsummieren aus wq_load Ergebnissen
	 */
	short loadingType;

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

#endif /* OPTIONS_H_ */
