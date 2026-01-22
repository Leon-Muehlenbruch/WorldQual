/**
 * @file options_copyRun.h
 * @brief Optionen-Klasse für das Kopieren von Modell-Runs
 * @author kynast
 * @date 06.07.2015
 * 
 * Ermöglicht das Kopieren und Anpassen von bestehenden WorldQual-Runs
 * mit verschiedenen Parametern und Szenarien.
 */

#ifndef OPTIONS_COPYRUN_H_
#define OPTIONS_COPYRUN_H_

/**
 * @class options_copyRunClass
 * @brief Konfigurationsklasse für das Kopieren von Modell-Runs
 * 
 * Diese Klasse verwaltet Parameter zum Kopieren eines Referenz-Runs
 * zu einem neuen Run, einschließlich Options für SQL-Ausführung und
 * Belastungsdaten-Kopierung.
 */
class options_copyRunClass: public optionsClass {
public:
	/**
	 * @brief Konstruktor mit Standard-Werten
	 * @param option_c Anzahl der Kommandozeilen-Argumente
	 * @param option_v Array der Kommandozeilen-Argumente
	 * @param CL Kommandozeilen-String
	 */
	options_copyRunClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		SQLexecute = false;
		refIDRun = 203100009;
		IDRun = 403100091;
		CopyLoad = false;
		project_id = 4;
		IDScen = 91;
	};
	
	/**
	 * @brief Destruktor
	 */
	virtual ~options_copyRunClass();

	// ========================================================================
	// Copy-Run spezifische Optionen
	// ========================================================================
	
	/**
	 * @brief SQL-Ausführungs-Modus
	 * - false: SQL-Befehle werden nur in Datei gespeichert, nicht ausgeführt (default)
	 * - true: SQL-Befehle werden gespeichert UND ausgeführt
	 */
	bool SQLexecute;
	
	/**
	 * @brief Referenz-Run-ID
	 * 
	 * ID des Runs, der als Vorlage kopiert werden soll
	 */
	int refIDRun;
	
	/**
	 * @brief Neue Run-ID
	 * 
	 * ID für den neu zu erstellenden Run
	 */
	int IDRun;
	
	/**
	 * @brief Flag ob Belastungsdaten kopiert werden
	 * 
	 * - false: Load-Tabelle nicht kopieren, selbst mit fill_worldqual_load berechnen (default)
	 * - true: Load-Tabelle aus Referenz-Run kopieren
	 */
	bool CopyLoad;
	
	/**
	 * @brief Projekt-ID
	 * 
	 * - -1: Projekt-ID aus Referenz übernehmen
	 * - Andere Werte: Spezifische Projekt-ID setzen
	 */
	int project_id;
	
	/**
	 * @brief Szenario-ID
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

#endif /* OPTIONS_COPYRUN_H_ */
