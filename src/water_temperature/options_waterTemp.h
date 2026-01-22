/**
 * @file options_waterTemp.h
 * @brief Optionen-Klasse für Wassertemperatur-Berechnungen
 * @author kynast
 * @date 22.05.2015
 * 
 * Verwaltet Parameter für die Berechnung von Wassertemperaturen
 * aus Klimadaten und hydrologischen Informationen.
 */

#ifndef OPTIONS_WATERTEMP_H_
#define OPTIONS_WATERTEMP_H_

/**
 * @class options_waterTempClass
 * @brief Konfigurationsklasse für Wassertemperatur-Berechnungen
 * 
 * Diese Klasse verwaltet Parameter für die Berechnung von
 * Fluss-Wassertemperaturen aus Lufttemperatur und anderen
 * klimatischen und hydrologischen Faktoren.
 */
class options_waterTempClass: public optionsClass {
public:
	/**
	 * @brief Konstruktor mit Standard-Werten
	 * @param option_c Anzahl der Kommandozeilen-Argumente
	 * @param option_v Array der Kommandozeilen-Argumente
	 * @param CL Kommandozeilen-String
	 */
	options_waterTempClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		project_id = 4;
		IDScen = 91;
		IDTemp = 0;
		climate = 0;
	}
	
	/**
	 * @brief Destruktor
	 */
	virtual ~options_waterTempClass();

	// ========================================================================
	// WaterTemp spezifische Optionen
	// ========================================================================
	
	/// Projekt-ID
	int project_id;
	
	/**
	 * @brief Szenario-ID
	 * 
	 * Wird nur für Sensitivitätsanalyse verwendet
	 * Sonst: NODATA
	 */
	int IDScen;
	
	/**
	 * @brief Temperatur-Berechnungs-ID
	 * 
	 * Identifiziert die verwendete Methode/Parameter für
	 * Wassertemperatur-Berechnung
	 */
	int IDTemp;
	
	/**
	 * @brief Klima-Input Auflösung
	 * 
	 * - 0: 0.5° Auflösung (default)
	 * - 1: 5 Minuten Auflösung
	 */
	short climate;

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

#endif /* OPTIONS_WATERTEMP_H_ */
