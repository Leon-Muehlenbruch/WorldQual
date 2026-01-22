/**
 * @file options_copyScen.h
 * @brief Optionen-Klasse für das Kopieren von Szenarien
 * @author kynast
 * @date 22.05.2015
 * 
 * Ermöglicht das Kopieren von Szenarien zwischen Projekten oder innerhalb
 * eines Projekts, mit Unterstützung für Batch-Kopierung mehrerer Szenarien.
 */

#ifndef OPTIONS_COPYSCEN_H_
#define OPTIONS_COPYSCEN_H_

/**
 * @class options_copyScenClass
 * @brief Konfigurationsklasse für das Kopieren von Szenarien
 * 
 * Diese Klasse verwaltet Parameter zum Kopieren von Referenz-Szenarien
 * zu neuen Szenario-IDs, mit Support für Bereichs-basiertes Kopieren.
 */
class options_copyScenClass: public optionsClass {
public:
	/**
	 * @brief Konstruktor mit Standard-Werten
	 * @param option_c Anzahl der Kommandozeilen-Argumente
	 * @param option_v Array der Kommandozeilen-Argumente
	 * @param CL Kommandozeilen-String
	 */
	options_copyScenClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		SQLexecute = false;
		refProject_id = 4;
		refScen = 9;
		project_id = 4;
		IDScenBeg = 91;
		IDScenEnd = 91;
		parameter_id = 310;
	}
	
	/**
	 * @brief Destruktor
	 */
	virtual ~options_copyScenClass();

	// ========================================================================
	// Copy-Scenario spezifische Optionen
	// ========================================================================
	
	/**
	 * @brief SQL-Ausführungs-Modus
	 * - false: SQL-Befehle werden nur in Datei gespeichert, nicht ausgeführt (default)
	 * - true: SQL-Befehle werden gespeichert UND ausgeführt
	 */
	bool SQLexecute;
	
	/**
	 * @brief Referenz-Projekt-ID
	 * 
	 * ID des Projekts aus dem das Szenario kopiert wird
	 */
	int refProject_id;
	
	/**
	 * @brief Referenz-Szenario-ID
	 * 
	 * ID des Szenarios, das als Vorlage kopiert werden soll
	 */
	int refScen;
	
	/**
	 * @brief Ziel-Projekt-ID
	 * 
	 * ID des Projekts in das das Szenario kopiert wird
	 */
	int project_id;
	
	/**
	 * @brief Start-Szenario-ID für Batch-Kopierung
	 * 
	 * Erste ID des Bereichs neuer Szenario-IDs
	 */
	int IDScenBeg;
	
	/**
	 * @brief End-Szenario-ID für Batch-Kopierung
	 * 
	 * Letzte ID des Bereichs neuer Szenario-IDs
	 * Wenn IDScenBeg == IDScenEnd: Nur ein Szenario wird kopiert
	 */
	int IDScenEnd;
	
	/**
	 * @brief Parameter-ID
	 * 
	 * ID des zu kopierenden Parameters (BOD, Salz, FC, TN, TP, etc.)
	 */
	int parameter_id;

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

#endif /* OPTIONS_COPYSCEN_H_ */
