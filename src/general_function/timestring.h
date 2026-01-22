/**
 * @file timestring.h
 * @brief Hilfsfunktionen für Zeit- und Datum-Strings
 * @author reder
 * 
 * Stellt Funktionen zur Verfügung, um aktuelle Datum- und Zeitinformationen
 * in verschiedenen Formaten zu erhalten, insbesondere im ISO-Format.
 */

#if !defined (_timestring_h_)
#define _timestring_h_

/**
 * @brief Gibt einen formatierten Zeitstring zurück
 * @return Zeiger auf statischen String mit Zeitinformation
 * @warning Der zurückgegebene Zeiger zeigt auf einen statischen Buffer,
 *          der beim nächsten Aufruf überschrieben wird
 */
char* getTimeString();

/**
 * @brief Gibt aktuelles Datum im ISO-Format zurück
 * 
 * Format: YYYY-MM-DD
 * Beispiel: "2025-01-22"
 *
 * @param dateString Ziel-Buffer für Datum-String (Output)
 * @param maxLength Maximale Länge des Buffers
 */
void getISOdate(char *dateString, short maxLength);

/**
 * @brief Gibt aktuelle Uhrzeit im ISO-Format zurück
 * 
 * Format: HH:MM:SS
 * Beispiel: "14:30:45"
 *
 * @param timeString Ziel-Buffer für Zeit-String (Output)
 * @param maxLength Maximale Länge des Buffers
 */
void getISOtime(char *timeString, short maxLength);

/**
 * @brief Gibt aktuelles Datum und Uhrzeit im ISO-Format zurück
 * 
 * Format: YYYY-MM-DD HH:MM:SS
 * Beispiel: "2025-01-22 14:30:45"
 *
 * @param dateTimeString Ziel-Buffer für Datum-Zeit-String (Output)
 * @param maxLength Maximale Länge des Buffers
 */
void getISOdateTime(char *dateTimeString, short maxLength);

#endif
