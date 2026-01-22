/**
 * @file options_mapRiverQuality.h
 * @brief Optionen-Klasse für Flussqualitäts-Kartierung
 * @author kynast
 * @date 11.05.2015
 * 
 * Diese Klasse verwaltet Parameter für die räumliche Darstellung und
 * Kartierung von Wasserqualitätsdaten entlang von Flussnetzen.
 */

#ifndef OPTIONS_MAPRIVERQUALITY_H_
#define OPTIONS_MAPRIVERQUALITY_H_

/**
 * @class options_mapRiverQualityClass
 * @brief Konfigurationsklasse für Flussqualitäts-Kartierung
 * 
 * Verwaltet Einstellungen für die Erstellung von Karten und
 * räumlichen Darstellungen der Wasserqualität, einschließlich
 * verschiedener Schwellenwert-Systeme.
 */
class options_mapRiverQualityClass: public optionsClass {
public:
	/**
	 * @brief Konstruktor mit Standard-Werten
	 * @param option_c Anzahl der Kommandozeilen-Argumente
	 * @param option_v Array der Kommandozeilen-Argumente
	 * @param CL Kommandozeilen-String
	 */
	options_mapRiverQualityClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		flagTH = 0;
	}
	
	/**
	 * @brief Destruktor
	 */
	virtual ~options_mapRiverQualityClass();

	// ========================================================================
	// MapRiverQuality spezifische Optionen
	// ========================================================================
	
	/**
	 * @brief Flag für verwendete Schwellenwerte (Thresholds)
	 * 
	 * - 0: Standard-Schwellenwerte (default)
	 * - 1: Spezielle Schwellenwerte (z.B. EU-Badegewässerrichtlinie)
	 *      abhängig vom untersuchten Schadstoff
	 * 
	 * Die Wahl der Schwellenwerte beeinflusst die Klassifizierung
	 * der Wasserqualität in den Karten.
	 */
	short flagTH;

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

#endif /* OPTIONS_MAPRIVERQUALITY_H_ */
