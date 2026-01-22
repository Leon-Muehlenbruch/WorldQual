/**
 * @file options_rtf_input.h
 * @brief Optionen-Klasse für Return Flow (RTF) Input-Verarbeitung
 * @author kynast
 * @date 22.05.2015
 * 
 * Verwaltet Parameter für die Verarbeitung von Rückfluss-Daten
 * (industriell, Bewässerung) und Korrekturfaktoren.
 */

#ifndef OPTIONS_RTF_INPUT_H_
#define OPTIONS_RTF_INPUT_H_

/**
 * @class options_rtfinput
 * @brief Konfigurationsklasse für Return Flow Input
 * 
 * Diese Klasse verwaltet Pfade und Parameter für die Verarbeitung
 * von verschiedenen Rückfluss-Quellen:
 * - Industrielle Abwässer (manufacturing, cooling water)
 * - Bewässerungs-Rückfluss
 * - Korrekturfaktoren für Rückfluss-Berechnungen
 */
class options_rtfinput: public optionsClass {
public:
	/**
	 * @brief Konstruktor mit Standard-Werten
	 * @param option_c Anzahl der Kommandozeilen-Argumente
	 * @param option_v Array der Kommandozeilen-Argumente
	 * @param CL Kommandozeilen-String
	 */
	options_rtfinput(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		project_id = 4;
		IDScen = 91;
	}
	
	/**
	 * @brief Destruktor
	 */
	virtual ~options_rtfinput();

	// ========================================================================
	// RTF-Input spezifische Optionen
	// ========================================================================
	
	/// Projekt-ID
	int project_id;
	
	/// Szenario-ID
	int IDScen;
	
	/**
	 * @brief Input-Verzeichnis für Griddi-Daten
	 * 
	 * Enthält industrielle Abwasser-Daten:
	 * - G_MAN_WW_m3_[year].UNF0 (manufacturing wastewater)
	 * - G_WC_m3_[year].UNF0 (water consumption)
	 * - G_WCOOL_m3_[year].UNF0 (cooling water)
	 */
	char* input_dir_griddi;
	
	/**
	 * @brief Input-Verzeichnis für GWSWS-Daten
	 * 
	 * Enthält Bewässerungs-Rückfluss-Daten:
	 * - G_IRRIG_RTF_SW_m3_[year].12.UNF0 (irrigation return flow surface water)
	 */
	char* input_dir_gwsws;
	
	/**
	 * @brief Input-Verzeichnis für Korrekturfaktoren
	 * 
	 * Enthält zeitliche Korrekturfaktoren für Rückfluss:
	 * - G_CORR_FACT_RTF_[year].12.UNF0
	 */
	char* input_dir_corrfactor;

	/**
	 * @brief Flag ob IDScen und parameter_id in Tabellennamen enthalten sind
	 * - 0: Standardtabellennamen (default)
	 * - 1: Tabellennamen enthalten IDScen und parameter_id
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

#endif /* OPTIONS_RTF_INPUT_H_ */
