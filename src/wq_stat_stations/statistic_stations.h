/**
 * @file statistic_stations.h
 * @brief Datenstruktur für Stations-basierte statistische Ausgaben
 * @author reder
 * 
 * Definiert die Struktur zur Speicherung von berechneten und gemessenen
 * Wasserqualitätsdaten an spezifischen Messstationen für Validierung.
 */

#ifndef STATISTIC_STATIONS_H_
#define STATISTIC_STATIONS_H_

/**
 * @class statisticStationsClass
 * @brief Speichert statistische Daten für einzelne Messstationen
 * 
 * Diese Klasse enthält alle relevanten Informationen für die statistische
 * Auswertung an Messstationen, einschließlich:
 * - Berechnete Konzentrationen
 * - Gemessene Werte
 * - Unsicherheits-Flags
 * - Leitfähigkeitsmessungen
 * 
 * Im Gegensatz zu statisticClass fokussiert diese Klasse auf punktuelle
 * Stationsmessungen anstatt auf Flussabschnitte.
 */
class statisticStationsClass {
	public:
	// ========================================================================
	// Identifikation
	// ========================================================================
	
	/// Zellen-ID der Messstation
	int cell;
	
	/// Datum der Messung (Format: YYYYMMDD)
	int date;

	// ========================================================================
	// Berechnete Konzentrationen
	// ========================================================================
	
	/// Berechnete Konzentration (Hauptparameter) [mg/l]
	double con;
	
	/// Berechnete Konzentration (zweiter Parameter) [mg/l]
	double con2;

	// ========================================================================
	// Gemessene Daten
	// ========================================================================
	
	/// Messstations-ID
	int station_id;
	
	/// Gemessene Konzentration [mg/l]
	double measured;
	
	/// Unsicherheitsflag für gemessene Konzentration
	int flag_uncert;
	
	/// Gemessene Leitfähigkeit [µS/cm]
	double measured_conductivity;
	
	/// Unsicherheitsflag für Leitfähigkeit
	int flag_uncert_conductivity;

	/**
	 * @brief Initialisiert alle Werte mit NODATA (-9999)
	 * 
	 * Setzt alle Member-Variablen auf den NODATA-Wert um
	 * fehlende Messungen oder berechnete Werte zu kennzeichnen.
	 */
	void setValues() {
		cell = -9999;
		date = -9999;
		con = -9999;
		con2 = -9999;
		station_id = -9999;
		measured = -9999;
		flag_uncert = -9999;
		measured_conductivity = -9999;
		flag_uncert_conductivity = -9999;
	}
	
	/**
	 * @brief Kopiert alle Werte von einer anderen Instanz
	 * @param line Quell-Objekt dessen Werte kopiert werden
	 */
	void copyValues(statisticStationsClass line) {
		cell = line.cell;
		date = line.date;
		con = line.con;
		con2 = line.con2;
		station_id = line.station_id;
		measured = line.measured;
		flag_uncert = line.flag_uncert;
		measured_conductivity = line.measured_conductivity;
		flag_uncert_conductivity = line.flag_uncert_conductivity;
	}
};

#endif /* STATISTIC_STATIONS_H_ */
