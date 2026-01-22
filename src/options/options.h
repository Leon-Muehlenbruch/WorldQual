/**
 * @file options.h
 * @brief Basis-Klasse für Konfigurationsoptionen aller WorldQual-Module
 * @author kynast
 * @date 11.05.2015
 * 
 * Diese abstrakte Basisklasse stellt gemeinsame Konfigurationsparameter
 * für alle WorldQual-Module bereit, insbesondere Datenbankverbindung,
 * Input-Typ und Kontinent-Informationen.
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

/**
 * @class optionsClass
 * @brief Abstrakte Basisklasse für Programmoptionen
 * 
 * Verwaltet grundlegende Konfigurationsparameter wie Datenbankzugriff,
 * Input-Quellen (DB vs. UNF-Dateien) und Kontinent-Spezifikationen.
 * Wird von spezialisierten Options-Klassen erweitert.
 */
class optionsClass {
public:
	/**
	 * @brief Konstruktor
	 * @param optionc Anzahl der Kommandozeilen-Argumente
	 * @param optionv Array der Kommandozeilen-Argumente
	 * @param CL Kommandozeilen-String für Hilfe-Ausgabe
	 */
	optionsClass(int optionc, char* optionv[], const char* CL);
	
	/**
	 * @brief Virtueller Destruktor
	 */
	virtual ~optionsClass();

	// ========================================================================
	// Datenbank und Kontinent-Konfiguration
	// ========================================================================
	
	/// WaterGAP Version-ID (z.B. 2 für WG2, 3 für WG3)
	int IDVersion;
	
	/// Regions-ID (z.B. 1=Europa, 2=Welt)
	int IDReg;
	
	/// Kontinent-Abkürzung (z.B. "wg2", "wg3", "eu") für Datenbank-Erkennung
	char continent_abb[10];

	// ========================================================================
	// MySQL Datenbankverbindung
	// ========================================================================
	
	/// MySQL Hostname oder IP-Adresse
	char*  MyHost;
	
	/// MySQL Benutzername
	char*  MyUser;
	
	/// MySQL Passwort
	char*  MyPassword;

	// ========================================================================
	// Input-Konfiguration
	// ========================================================================
	
	/**
	 * @brief Typ der Input-Datenquelle
	 * 
	 * - 0: Daten aus Datenbank (Q, runoff_total, ggf. Fließgeschwindigkeit)
	 * - 1: Daten aus UNF-Dateien
	 */
	short InputType;
	
	/// Input-Verzeichnis für Datendateien (bei InputType=1)
	char*  input_dir;

	/// Maximale Zeichenkettenlänge
	short maxcharlength;

protected:
	/// Pfad zur OPTIONS.DAT Konfigurationsdatei
	char *filename_opt;
	
	/**
	 * @brief Bereinigt Strings (entfernt Zeilenende, Leerzeichen, Tabs)
	 * @param str Zu bereinigender String
	 */
	void StringCheck(char* str);
	
	/**
	 * @brief Gibt alle aktuellen Optionen auf stdout aus
	 */
	void printOptions();
	
	/**
	 * @brief Initialisiert Optionen aus Kommandozeile
	 * @param optionc Anzahl der Argumente
	 * @param optionv Array der Argumente
	 * @return Status-Code (0 bei Erfolg)
	 */
	short init(int optionc, char* optionv[]);
	
	/**
	 * @brief Liest Basis-Optionen aus OPTIONS.DAT
	 * @param num Sektion-Nummer in der Datei
	 * @param file_ptr Datei-Pointer
	 * @return Status-Code (0 bei Erfolg)
	 */
	short read_parentOptions(int num, FILE *file_ptr);

	/**
	 * @brief Trennt Kommandozeilen-Option in Name und Wert
	 * @param CLoption Kommandozeilen-Option (z.B. "-host=localhost")
	 * @param option Options-Name (Output, z.B. "host")
	 * @param optionValue Options-Wert (Output, z.B. "localhost")
	 */
	void separateCommandLineOption(char* CLoption, char option[5], char* optionValue);

	/**
	 * @brief Zeigt Hilfe-Text (muss von abgeleiteten Klassen implementiert werden)
	 * @param num Hilfe-Level (default=1)
	 */
	virtual void Help(int num =1)=0;

	/// Name des Programms (z.B. "rtf_input")
	char* programme;
	
	/// Vollständige Kommandozeile (z.B. "./rtf_input startYear endYear")
	char* commandLine;

private:

};

#endif /* OPTIONS_H_ */
