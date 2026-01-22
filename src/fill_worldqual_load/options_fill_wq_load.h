/**
 * @file options_fill_wq_load.h
 * @brief Optionen-Klasse für das Befüllen der WQ-Load Tabelle
 * @author kynast
 * @date 22.05.2015
 * 
 * Verwaltet Parameter für das Befüllen der worldqual_load Datenbanktabelle
 * mit Belastungsdaten für einen oder mehrere Szenarien.
 */

#ifndef OPTIONS_FILL_WQ_LOAD_H_
#define OPTIONS_FILL_WQ_LOAD_H_

/**
 * @class options_fill_wq_loadClass
 * @brief Konfigurationsklasse für das Befüllen der Load-Tabelle
 * 
 * Diese Klasse verwaltet Parameter für das Befüllen der worldqual_load
 * Datenbanktabelle mit berechneten Belastungsdaten. Unterstützt Batch-
 * Verarbeitung mehrerer Szenarien in einem Durchlauf.
 * 
 * @note InputType und input_dir aus der Basisklasse werden NICHT verwendet!
 */
class options_fill_wq_loadClass: public optionsClass {
public:
	/**
	 * @brief Konstruktor mit Standard-Werten
	 * @param option_c Anzahl der Kommandozeilen-Argumente
	 * @param option_v Array der Kommandozeilen-Argumente
	 * @param CL Kommandozeilen-String
	 */
	options_fill_wq_loadClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		parameter_id = 310;
		project_id = 4;
		IDScenBeg = 91;
		IDScenEnd = 91;
	}
	
	/**
	 * @brief Destruktor
	 */
	virtual ~options_fill_wq_loadClass();

	// ========================================================================
	// Fill-WQ-Load spezifische Optionen
	// ========================================================================
	
	/**
	 * @brief Parameter-ID
	 * 
	 * ID des zu befüllenden Parameters:
	 * - 110: BOD
	 * - 210: Salz (TDS)
	 * - 310: FC (Fäkale Coliforme)
	 * - 410: TN (Gesamtstickstoff)
	 * - 510: TP (Gesamtphosphor)
	 * - 610: Pestizide
	 */
	int parameter_id;
	
	/**
	 * @brief Projekt-ID
	 */
	int project_id;
	
	/**
	 * @brief Start-Szenario-ID für Batch-Verarbeitung
	 * 
	 * Erste Szenario-ID des zu verarbeitenden Bereichs
	 */
	int IDScenBeg;
	
	/**
	 * @brief End-Szenario-ID für Batch-Verarbeitung
	 * 
	 * Letzte Szenario-ID des zu verarbeitenden Bereichs
	 * Wenn IDScenBeg == IDScenEnd: Nur ein Szenario wird befüllt
	 */
	int IDScenEnd;

	/**
	 * @warning InputType und input_dir aus optionsClass werden in dieser
	 * Klasse NICHT verwendet! Die Daten kommen direkt aus der Datenbank.
	 */

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

#endif /* OPTIONS_FILL_WQ_LOAD_H_ */
