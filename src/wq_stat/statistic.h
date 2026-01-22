/**
 * @file statistic.h
 * @brief Datenstruktur für statistische Ausgaben und Validierung
 * @author reder
 * 
 * Definiert die Struktur zur Speicherung von berechneten und gemessenen
 * Wasserqualitätsdaten für statistische Analysen und Modell-Validierung.
 */

#ifndef STATISTIC_H_
#define STATISTIC_H_

/**
 * @class statisticClass
 * @brief Speichert statistische Daten für einen Flussabschnitt
 * 
 * Diese Klasse enthält alle relevanten Informationen für die statistische
 * Auswertung und Validierung von Modellergebnissen, einschließlich:
 * - Berechnete Konzentrationen und Belastungen
 * - Gemessene Werte von Messstationen
 * - Abflussdaten (Q)
 * - Flussabschnitts-Geometrie
 */
class statisticClass {
	public:
	// ========================================================================
	// Identifikation und Geometrie
	// ========================================================================
	
	/// Flussabschnitts-ID
	int riverSection;
	
	/// Zellen-ID
	int cell;
	
	/// Datum (Format: YYYYMMDD)
	int date;
	
	/// Länge der Zelle [km]
	double lengthCell;
	
	/// Länge des Flussabschnitts [km]
	double lengthSection;
	
	/// Einzugsgebietsfläche [km²]
	int rout_area;

	// ========================================================================
	// Berechnete Konzentrationen und Fließeigenschaften
	// ========================================================================
	
	/// Berechnete Konzentration (Hauptparameter) [mg/l]
	double con;
	
	/// Berechnete Konzentration (zweiter Parameter, z.B. bei gekoppelten Berechnungen) [mg/l]
	double con2;
	
	/// Fließgeschwindigkeit [m/s]
	double flowVelocity;
	
	/// Fließzeit [Tage]
	double flowTime;

	// ========================================================================
	// Berechnete Belastungen (Loads)
	// ========================================================================
	
	/// Gesamtbelastung (Hauptparameter) [t/Monat]
	double load_total;
	
	/// Gesamtbelastung (zweiter Parameter) [t/Monat]
	double load_total_2;
	
	/// Belastung vom Oberlauf (Hauptparameter) [t/Monat]
	double load_up;
	
	/// Belastung vom Oberlauf (zweiter Parameter) [t/Monat]
	double load_up_2;
	
	/// Akkumulierte Belastung (Hauptparameter) [t/Monat]
	double load_acc;
	
	/// Akkumulierte Belastung (zweiter Parameter) [t/Monat]
	double load_acc_2;

	// ========================================================================
	// Abflussdaten
	// ========================================================================
	
	/// Abfluss aus der Zelle [m³/s]
	double Q_out;
	
	/// Diffuser Abfluss in die Zelle [m³/s]
	double Q_d;
	
	/// Zufluss in die Zelle [m³/s]
	double Q_in;
	
	/// Berechneter Abfluss an Station [m³/s]
	double Qx;

	// ========================================================================
	// Gemessene Daten (von Messstationen)
	// ========================================================================
	
	/// Messstations-ID (-9999 wenn keine Station)
	int station_id;
	
	/// Gemessene Konzentration [mg/l]
	double measured;
	
	/// Unsicherheitsflag für gemessene Konzentration
	int flag_uncert;
	
	/// Gemessene Leitfähigkeit [µS/cm]
	double measured_conductivity;
	
	/// Unsicherheitsflag für Leitfähigkeit
	int flag_uncert_conductivity;
	
	/// GRDC Station-ID (Global Runoff Data Centre)
	int GRDC;
	
	/// Berechnete Konzentration an Messstation [mg/l]
	double calculated;
	
	/// Gemessene Belastung [t/Monat]
	double measured_load;
	
	/// Unsicherheitsflag für gemessene Belastung
	int flag_uncert_load;
	
	/// Gemessener Abfluss [m³/s]
	double measured_discharge;
	
	/// Unsicherheitsflag für gemessenen Abfluss
	double flag_uncert_discharge;

	/**
	 * @brief Initialisiert alle Messwerte mit NODATA (-9999)
	 * 
	 * Diese Methode setzt alle gemessenen Werte auf NODATA.
	 * Die Werte werden später in get_Output() gefüllt wenn
	 * Messstationen vorhanden sind.
	 */
	void setValues() {
		station_id = -9999;
		measured = -9999;
		flag_uncert = -9999;
		measured_conductivity = -9999;
		flag_uncert_conductivity = -9999;
		GRDC = -9999;
		calculated = -9999;
		measured_load = -9999;
		flag_uncert_load = -9999;
		measured_discharge = -9999;
		flag_uncert_discharge = -9999;
	}
	
	/**
	 * @brief Kopiert alle Werte von einer anderen statisticClass Instanz
	 * @param line Quell-Objekt dessen Werte kopiert werden
	 */
	void copyValues(statisticClass line) {
		riverSection = line.riverSection;
		cell = line.cell;
		date = line.date;
		lengthCell = line.lengthCell;
		lengthSection = line.lengthSection;
		rout_area = line.rout_area;
		con = line.con;
		con2 = line.con2;
		flowVelocity = line.flowVelocity;
		flowTime = line.flowTime;
		load_total = line.load_total;
		load_total_2 = line.load_total_2;
		load_up = line.load_up;
		load_up_2 = line.load_up_2;
		load_acc = line.load_acc;
		load_acc_2 = line.load_acc_2;
		Q_out = line.Q_out;
		Q_d = line.Q_d;
		Q_in = line.Q_in;
		Qx = line.Qx;
		station_id = line.station_id;
		measured = line.measured;
		flag_uncert = line.flag_uncert;
		measured_conductivity = line.measured_conductivity;
		flag_uncert_conductivity = line.flag_uncert_conductivity;
		GRDC = line.GRDC;
		calculated = line.calculated;
		measured_load = line.measured_load;
		flag_uncert_load = line.flag_uncert_load;
		measured_discharge = line.measured_discharge;
		flag_uncert_discharge = line.flag_uncert_discharge;
	}
};

#endif /* STATISTIC_H_ */
