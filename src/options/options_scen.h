/**
 * @file options_scen.h
 * @brief Basis-Optionen-Klasse für Szenario-basierte Module
 * @author kynast
 * @date 22.05.2015
 * 
 * Erweitert die Basis-optionsClass um grundlegende Szenario-Parameter,
 * die von verschiedenen Modulen geerbt werden können.
 */

#ifndef OPTIONS_SCEN_H_
#define OPTIONS_SCEN_H_

/**
 * @class options_scenClass
 * @brief Basis-Konfigurationsklasse mit Szenario-Support
 * 
 * Diese Klasse erweitert optionsClass um grundlegende Szenario-Parameter
 * (Projekt-ID und Szenario-ID) und dient als Basisklasse für
 * szenario-spezifische Options-Klassen.
 */
class options_scenClass: public optionsClass {
public:
	/**
	 * @brief Konstruktor mit Standard-Werten
	 * @param option_c Anzahl der Kommandozeilen-Argumente
	 * @param option_v Array der Kommandozeilen-Argumente
	 * @param CL Kommandozeilen-String
	 */
	options_scenClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		project_id = 4;
		IDScen = 91;
	}
	
	/**
	 * @brief Destruktor
	 */
	virtual ~options_scenClass();

	// ========================================================================
	// Szenario-Parameter
	// ========================================================================
	
	/**
	 * @brief Projekt-ID
	 * 
	 * Identifiziert das WorldQual-Projekt
	 * Standard: 4
	 */
	int project_id;
	
	/**
	 * @brief Szenario-ID
	 * 
	 * Identifiziert das spezifische Szenario innerhalb des Projekts
	 * Standard: 91
	 */
	int IDScen;

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

#endif /* OPTIONS_SCEN_H_ */
