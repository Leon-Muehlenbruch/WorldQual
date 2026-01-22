/**
 * @file Newton.h
 * @brief Newton-Verfahren zur Kurvenanpassung (Curve Fitting)
 * @author Ellen Teichert
 * 
 * Implementiert das Newton-Raphson-Verfahren zur Anpassung von
 * Funktionen an gegebene Datenpunkte. Wird hauptsächlich für
 * Wassertemperatur-Kurven verwendet.
 * 
 * Approximationsfunktion: f(x) = c0 / (1 + exp(c1*x + c2))
 */

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "countryinfo.h"

/// Maximale Länge von Strings
const short maxcharlength = 255;

// ============================================================================
// Datei-Konstanten
// ============================================================================

/// Verzeichnis-Konfigurationsdatei
const char directory_file[] = "DATA.DIR";

/// Datenpunkte-Eingabedatei (X, Y Koordinaten)
const char points_file[] = "POINTS.DAT";

/// Initialisierungsdatei (Startwerte für Koeffizienten)
const char init_file[] = "INIT.DAT";

/// Maximaler SWI (Surface Water Index) Datei
const char maxSWI_file[] = "MaxSWI.DAT";

/// Ausgabedatei für berechnete Koeffizienten
const char output_file[] = "COEFF.OUT";

/// Ausgabedatei für Index of Agreement (Güte der Anpassung)
const char IndexAgreenment_file[] = "IndexOfAgreenment.OUT";

// ============================================================================
// Algorithmus-Konstanten
// ============================================================================

/// Anzahl der Koeffizienten in der Approximationsfunktion (c0, c1, c2)
const int n = 3;

/// Maximale Anzahl von Datenpunkten (Xi, Yi)
const int Nmax = 199999;

/// Maximale Anzahl innerer Iterationen
const int imax = 4;

/// Maximale Anzahl Newton-Iterationen
const int MaxIteration = 1000;

/// Tatsächliche Anzahl von Datenpunkten
double N;

// ============================================================================
// Partielle Ableitungen - Zweite Ordnung (Hesse-Matrix)
// ============================================================================

/**
 * @brief Berechnet partielle Ableitung ∂²F/∂c0² 
 * @param c Array der Koeffizienten [c0, c1, c2]
 * @param x Array der X-Koordinaten
 * @param w Array der Gewichte
 * @param pointNumber Anzahl der Datenpunkte
 * @return Wert der partiellen Ableitung
 */
double g0c0(double c[n], double x[Nmax], double w[Nmax], int pointNumber);

/**
 * @brief Berechnet partielle Ableitung ∂²F/∂c1∂c0
 * @param c Array der Koeffizienten [c0, c1, c2]
 * @param x Array der X-Koordinaten
 * @param y Array der Y-Koordinaten
 * @param w Array der Gewichte
 * @param pointNumber Anzahl der Datenpunkte
 * @return Wert der partiellen Ableitung
 */
double g1c0(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber);

/**
 * @brief Berechnet partielle Ableitung ∂²F/∂c2∂c0
 * @param c Array der Koeffizienten [c0, c1, c2]
 * @param x Array der X-Koordinaten
 * @param y Array der Y-Koordinaten
 * @param w Array der Gewichte
 * @param pointNumber Anzahl der Datenpunkte
 * @return Wert der partiellen Ableitung
 */
double g2c0(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber);

/**
 * @brief Berechnet partielle Ableitung ∂²F/∂c0∂c1
 * @param c Array der Koeffizienten [c0, c1, c2]
 * @param x Array der X-Koordinaten
 * @param y Array der Y-Koordinaten
 * @param w Array der Gewichte
 * @param pointNumber Anzahl der Datenpunkte
 * @return Wert der partiellen Ableitung
 */
double g0c1(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber);

/**
 * @brief Berechnet partielle Ableitung ∂²F/∂c1²
 * @param c Array der Koeffizienten [c0, c1, c2]
 * @param x Array der X-Koordinaten
 * @param y Array der Y-Koordinaten
 * @param w Array der Gewichte
 * @param pointNumber Anzahl der Datenpunkte
 * @return Wert der partiellen Ableitung
 */
double g1c1(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber);

/**
 * @brief Berechnet partielle Ableitung ∂²F/∂c2∂c1
 * @param c Array der Koeffizienten [c0, c1, c2]
 * @param x Array der X-Koordinaten
 * @param y Array der Y-Koordinaten
 * @param w Array der Gewichte
 * @param pointNumber Anzahl der Datenpunkte
 * @return Wert der partiellen Ableitung
 */
double g2c1(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber);

/**
 * @brief Berechnet partielle Ableitung ∂²F/∂c0∂c2
 * @param c Array der Koeffizienten [c0, c1, c2]
 * @param x Array der X-Koordinaten
 * @param y Array der Y-Koordinaten
 * @param w Array der Gewichte
 * @param pointNumber Anzahl der Datenpunkte
 * @return Wert der partiellen Ableitung
 */
double g0c2(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber);

/**
 * @brief Berechnet partielle Ableitung ∂²F/∂c1∂c2
 * @param c Array der Koeffizienten [c0, c1, c2]
 * @param x Array der X-Koordinaten
 * @param y Array der Y-Koordinaten
 * @param w Array der Gewichte
 * @param pointNumber Anzahl der Datenpunkte
 * @return Wert der partiellen Ableitung
 */
double g1c2(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber);

/**
 * @brief Berechnet partielle Ableitung ∂²F/∂c2²
 * @param c Array der Koeffizienten [c0, c1, c2]
 * @param x Array der X-Koordinaten
 * @param y Array der Y-Koordinaten
 * @param w Array der Gewichte
 * @param pointNumber Anzahl der Datenpunkte
 * @return Wert der partiellen Ableitung
 */
double g2c2(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber);

// ============================================================================
// Erste Ableitungen (Gradient)
// ============================================================================

/**
 * @brief Berechnet partielle Ableitung ∂F/∂c0
 * @param c Array der Koeffizienten [c0, c1, c2]
 * @param x Array der X-Koordinaten
 * @param y Array der Y-Koordinaten
 * @param w Array der Gewichte
 * @param pointNumber Anzahl der Datenpunkte
 * @return Wert der ersten Ableitung nach c0
 */
double g0(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber);

/**
 * @brief Berechnet partielle Ableitung ∂F/∂c1
 * @param c Array der Koeffizienten [c0, c1, c2]
 * @param x Array der X-Koordinaten
 * @param y Array der Y-Koordinaten
 * @param w Array der Gewichte
 * @param pointNumber Anzahl der Datenpunkte
 * @return Wert der ersten Ableitung nach c1
 */
double g1(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber);

/**
 * @brief Berechnet partielle Ableitung ∂F/∂c2
 * @param c Array der Koeffizienten [c0, c1, c2]
 * @param x Array der X-Koordinaten
 * @param y Array der Y-Koordinaten
 * @param w Array der Gewichte
 * @param pointNumber Anzahl der Datenpunkte
 * @return Wert der ersten Ableitung nach c2
 */
double g2(double c[n], double x[Nmax], double y[Nmax], double w[Nmax], int pointNumber);

// ============================================================================
// Hilfsfunktionen
// ============================================================================

/**
 * @brief Gibt Vorzeichen einer Zahl zurück
 * @param a Eingabewert
 * @return 1 für positiv, -1 für negativ, 0 für null
 */
int sign(double a);

/**
 * @brief Approximationsfunktion für Kurvenanpassung
 * 
 * Berechnet: f(x) = c0 / (1 + exp(c1*x + c2))
 * Diese Funktion wird für Wassertemperatur-Kurven verwendet.
 *
 * @param c0 Koeffizient 0 (Amplitude)
 * @param c1 Koeffizient 1 (Steigung)
 * @param c2 Koeffizient 2 (Verschiebung)
 * @param x X-Koordinate (unabhängige Variable)
 * @return Funktionswert f(x)
 */
double F(double c0, double c1, double c2, double x);
