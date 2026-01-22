/**
 * @file countryinfo.h
 * @brief Klasse zur Verwaltung von Länder-Informationen aus Datei
 * @author kaspar
 * 
 * Diese Klasse liest 'COUNTRY_NAMES.DAT' und stellt Informationen über
 * Länder bereit, einschließlich ISO-Nummern, Namen und Array-Positionen.
 * 
 * @version $Revision: 1.3 $
 * @date $Date: 2000/05/21 23:03:32 $
 */

#if !defined (_countryinfo_h_)
#define _countryinfo_h_

#include <vector>

using namespace std;

/**
 * @class countryInfoClass
 * @brief Verwaltet Länder-Informationen aus COUNTRY_NAMES.DAT
 * 
 * Diese Klasse liest eine Datei mit Länderinformationen und stellt
 * Methoden zur Verfügung, um zwischen ISO-Nummern, Array-Positionen
 * und Ländernamen zu konvertieren.
 * 
 * @note Verwendet std::vector für dynamische Arrays
 */
class countryInfoClass {
 public:
  /**
   * @brief Initialisiert die Länder-Datenbank aus Datei
   * 
   * Liest COUNTRY_NAMES.DAT und lädt alle Länderinformationen
   *
   * @param input_dir Verzeichnis mit COUNTRY_NAMES.DAT Datei
   * @param maxLengthOfCountryName Maximale Länge eines Ländernamens (default=15)
   */
  void  init(const char *input_dir, 
	     const unsigned short maxLengthOfCountryName=15);
  
  /**
   * @brief Gibt ISO-Nummer für Array-Position zurück
   * @param n Array-Position (0-basiert)
   * @return ISO-Nummer des Landes an Position n
   */
  short getIsoNumber(const short n);
  
  /**
   * @brief Findet Array-Position für eine ISO-Nummer
   * @param IsoNumber ISO-Nummer des Landes
   * @return Array-Position des Landes, oder -1 wenn nicht gefunden
   */
  short getArrayPosition(const short IsoNumber);
  
  /**
   * @brief Gibt Gesamtzahl der Länder zurück
   * @return Anzahl der geladenen Länder
   */
  short getNumberOfCountries(void);
  
  /**
   * @brief Gibt Ländernamen für Array-Position zurück
   * @param n Array-Position (0-basiert)
   * @return Zeiger auf Ländernamen-String
   */
  char* getCountryName(const short n);

 private:
  /**
   * @brief Vector mit ISO-Nummern der Länder
   * 
   * Enthält die ISO-Nummer für jedes Land in der Reihenfolge
   * wie sie in COUNTRY_NAMES.DAT erscheinen
   */
  std::vector<short> countryIsoNumber;
  
  /**
   * @brief Vector mit Ländernamen
   * 
   * Enthält Zeiger auf Ländernamen-Strings in der gleichen
   * Reihenfolge wie countryIsoNumber
   */
  std::vector<char*> countryName;
  
  /**
   * @brief Anzahl der geladenen Länder
   */
  short numberOfCountries;
};

#endif
