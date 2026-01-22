/**
 * @file options_wqstat.h
 * @brief Optionen-Klasse für WQ-Stat Statistik-Berechnungen
 * @author kynast
 * @date 11.05.2015
 * 
 * Erweitert die Basis-optionsClass um spezifische Parameter für
 * statistische Auswertungen von Flussabschnitten (wq_stat Modul).
 */

#ifndef OPTIONS_WQSTAT_H_
#define OPTIONS_WQSTAT_H_

/**
 * @class options_wqstatClass
 * @brief Konfigurationsklasse für WQ-Stat Berechnungen
 * 
 * Erbt von optionsClass und fügt Parameter für statistische Auswertungen
 * von Flussabschnitten hinzu, einschließlich Fließgeschwindigkeits-Varianten
 * und Gewässertyp.
 */
class options_wqstatClass: public optionsClass {
public:
	/**
	 * @brief Konstruktor mit Standard-Werten
	 * @param option_c Anzahl der Kommandozeilen-Argumente
	 * @param option_v Array der Kommandozeilen-Argumente
	 * @param CL Kommandozeilen-String
	 */
	options_wqstatClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		flowVelocityVariant = 0;
		ConstVelocity = 0.;
		climate = 0;
		watertype = 1;
	};
	
	/**
	 * @brief Destruktor
	 */
	virtual ~options_wqstatClass();

	// ========================================================================
	// WQ-Stat spezifische Optionen
	// ========================================================================
	
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
	 */
	short climate;
	
	/**
	 * @brief Gewässertyp
	 * 
	 * - 1: Fluss (default)
	 * - Andere Werte: Spezifische Gewässertypen (See, Reservoir, etc.)
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

#endif /* OPTIONS_WQSTAT_H_ */
