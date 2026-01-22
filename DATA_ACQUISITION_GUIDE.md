# WorldQual Daten-Akquise Guide

## Übersicht

WorldQual benötigt umfangreiche Input-Daten. Dieser Guide erklärt, woher die Daten kommen und wie Sie Zugang erhalten.

---

## 1. WaterGAP-Daten (Hauptquelle)

### Was ist WaterGAP?
**WaterGAP** = Water - Global Assessment and Prognosis

Ein globales hydrologisches Modell, das Wasserflüsse, -speicher und -verfügbarkeit simuliert.

### Institutionen:
- **Universität Kassel** (Deutschland)
- **Goethe-Universität Frankfurt** (Deutschland)
- **ISIMIP** (Inter-Sectoral Impact Model Intercomparison Project)

### Benötigte WaterGAP-Outputs:

| Datei-Typ | Beschreibung | Zeitauflösung |
|-----------|--------------|---------------|
| Discharge | Abfluss | Monatlich |
| Flow Velocity | Fließgeschwindigkeit | Monatlich |
| Lake Storage | Seespeicher | Monatlich |
| Reservoir Storage | Stausee-Speicher | Monatlich |

### Wie erhalten Sie Zugang?

#### Option A: ISIMIP Data Portal (Empfohlen)
**URL:** https://www.isimip.org/gettingstarted/data-access/

**Schritte:**
1. Registrierung auf ISIMIP Portal
2. Suche nach "WaterGAP" Daten
3. Download spezifischer Variablen und Zeiträume
4. Format: NetCDF (muss zu UNF konvertiert werden)

**Vorteile:**
- ✓ Frei zugänglich
- ✓ Gut dokumentiert
- ✓ Verschiedene Szenarien verfügbar

**Nachteile:**
- ✗ Format-Konversion nötig
- ✗ Nicht alle WaterGAP-Versionen verfügbar

#### Option B: Direkter Kontakt mit WaterGAP-Team
**Kontakte:**
- Prof. Dr. Petra Döll (Goethe Uni Frankfurt)
- Dr. Hannes Müller Schmied (Goethe Uni Frankfurt)

**E-Mail:**
- watergap@em.uni-frankfurt.de

**Anfrage sollte enthalten:**
1. Forschungszweck
2. Benötigte Region (global/kontinental)
3. Zeitraum
4. Benötigte Variablen
5. Geplante Publikationen

**Antwortzeit:** 2-4 Wochen

#### Option C: Zusammenarbeit mit bestehenden Nutzern
Wenn Sie Zugang zu WorldQual-Code haben, gibt es möglicherweise bereits Kontakte oder Daten-Sharing-Agreements.

---

## 2. Klimadaten

### CRU (Climate Research Unit)
**URL:** https://crudata.uea.ac.uk/

**Daten:**
- Temperatur
- Niederschlag
- Strahlung

**Zugang:**
- Frei verfügbar
- Registrierung erforderlich
- Format: NetCDF

### WATCH (Water and Global Change)
**URL:** http://www.eu-watch.org/

**Daten:**
- Meteorologische Forcing-Daten
- 20. Jahrhundert

**Zugang:**
- Teilweise frei
- Registrierung erforderlich

---

## 3. Nährstoffdaten

### NEWS2 (Nutrient Export from WaterSheds)
**URL:** http://www.marine.rutgers.edu/globalnews/

**Daten:**
- Globale Nährstoffexporte
- N, P, DOC, BOD
- Flussmündungen

**Zugang:**
- Publikationen zugänglich
- Daten auf Anfrage

### FAO (Food and Agriculture Organization)
**URL:** https://www.fao.org/faostat/

**Daten:**
- Düngemittel-Anwendung
- Viehbestand
- Agrarproduktion

**Zugang:**
- Frei verfügbar
- FAOSTAT Datenbank

### HYDE (History Database of the Global Environment)
**URL:** https://themasites.pbl.nl/tridion/en/themasites/hyde/

**Daten:**
- Historische Bevölkerung
- Landnutzung

**Zugang:**
- Frei verfügbar
- Registrierung empfohlen

---

## 4. Topographische/Geographische Daten

### SRTM (Shuttle Radar Topography Mission)
**URL:** https://www2.jpl.nasa.gov/srtm/

**Daten:**
- Digitales Höhenmodell
- Global, 90m Auflösung

**Zugang:**
- Frei verfügbar
- NASA EarthData Login

### HydroSHEDS
**URL:** https://www.hydrosheds.org/

**Daten:**
- Einzugsgebiete
- Flussnetzwerke
- Flow direction

**Zugang:**
- Frei verfügbar
- Verschiedene Auflösungen

---

## 5. Datenformat-Konversion

Die meisten öffentlichen Daten liegen in **NetCDF** vor, WorldQual benötigt jedoch **UNF**-Format.

### NetCDF → UNF Konverter (zu entwickeln)

**Konzept:**
```python
# Pseudocode
import netCDF4
import struct

def netcdf_to_unf(nc_file, unf_file, variable):
    # NetCDF öffnen
    nc = netCDF4.Dataset(nc_file)
    data = nc.variables[variable][:]
    
    # Als binary UNF schreiben
    with open(unf_file, 'wb') as f:
        # Header (falls nötig)
        f.write(struct.pack('i', data.shape[0]))
        f.write(struct.pack('i', data.shape[1]))
        
        # Daten
        for row in data:
            for value in row:
                f.write(struct.pack('f', value))
```

**Status:** Muss noch implementiert werden

---

## 6. Minimaler Test-Datensatz

Für erste Tests können Sie einen **synthetischen Mini-Datensatz** erstellen:

### Komponenten:
1. **Kleines Einzugsgebiet** (z.B. 100 Zellen)
2. **1 Jahr Daten** (12 Monate)
3. **Vereinfachte Hydrologie** (konstante Werte)

### Erstellungs-Skript (zu entwickeln):
```bash
# create_test_data.sh
mysql -u root -p < database_schema.sql
python generate_test_unf.py --cells 100 --years 1
python populate_test_db.py
```

---

## 7. Daten-Checkliste

### Für produktiven Einsatz:

**Hydrologische Daten (WaterGAP):**
- [ ] Abfluss-Zeitreihen
- [ ] Fließgeschwindigkeiten
- [ ] Speicher (Seen, Reservoire)
- [ ] Grid-Informationen

**Klimadaten:**
- [ ] Temperatur
- [ ] Strahlung
- [ ] Niederschlag (für wq_load)

**Nährstoff-Inputs:**
- [ ] Bevölkerungsdaten
- [ ] Viehbestand
- [ ] Düngemittel-Statistiken
- [ ] Industriedaten

**Geographische Daten:**
- [ ] Höhenmodell
- [ ] Einzugsgebiete
- [ ] Flussnetzwerk

**Datenbank:**
- [ ] Schema erstellt
- [ ] Routing-Tabellen gefüllt
- [ ] Parameter konfiguriert

---

## 8. Zeitplan für Datenakquise

### Woche 1-2: Registrierung & Anfragen
- ISIMIP Portal Account
- WaterGAP-Team kontaktieren
- FAO/HYDE Daten browsen

### Woche 3-4: Downloads
- Verfügbare öffentliche Daten herunterladen
- Format prüfen
- Dokumentation lesen

### Woche 5-6: Konversion & Vorbereitung
- NetCDF → UNF Konverter entwickeln
- Datenbank aufsetzen
- Test-Läufe

### Woche 7-8: Integration & Validierung
- Daten in WorldQual laden
- Erste Simulationen
- Ergebnisse validieren

---

## 9. Kosten

### Kostenlose Quellen:
- ✓ ISIMIP (WaterGAP)
- ✓ FAO Statistiken
- ✓ HYDE Daten
- ✓ HydroSHEDS
- ✓ CRU Klimadaten

### Möglicherweise kostenpflichtig:
- ⚠ Spezielle WaterGAP-Läufe (Zusammenarbeit)
- ⚠ Hochauflösende Satellitendaten
- ⚠ Kommerzielle Klimaszenarien

**Erwartete Kosten für akademische Nutzung: 0€**
**Erwartete Kosten für kommerzielle Nutzung: Verhandlungssache**

---

## 10. Datenschutz & Lizenzen

### Typische Lizenzen:
- **CC-BY 4.0** - Freie Nutzung mit Namensnennung
- **Academic Use Only** - Nur für Forschung
- **Custom Agreements** - Spezielle Vereinbarungen

### Wichtig:
- ✓ Immer Quellen zitieren
- ✓ Lizenzbedingungen beachten
- ✓ Bei Publikationen: Datenquellen angeben

---

## 11. Kontakte & Support

### WaterGAP:
- **E-Mail:** watergap@em.uni-frankfurt.de
- **Web:** https://www.uni-frankfurt.de/45217668/WaterGAP

### ISIMIP:
- **E-Mail:** info@isimip.org
- **Web:** https://www.isimip.org/

### WorldQual Community:
- **GitHub Issues:** https://github.com/Leon-Muehlenbruch/WorldQual/issues
- **Diskussionen:** GitHub Discussions (wenn aktiviert)

---

## 12. Nächste Schritte

**Was möchten Sie tun?**

### A. Test-Setup ohne echte Daten
→ Synthetische Testdaten generieren
→ Minimale Datenbank für Entwicklung

### B. Echte Daten beschaffen
→ ISIMIP Account erstellen
→ WaterGAP-Team kontaktieren
→ Daten-Download starten

### C. Konverter entwickeln
→ NetCDF zu UNF Konverter
→ Automatische Daten-Pipeline

---

## Zusammenfassung

**Daten beschaffen:**
1. ✅ ISIMIP für WaterGAP-Daten (kostenlos)
2. ✅ FAO für Nährstoff-Inputs (kostenlos)
3. ✅ HydroSHEDS für Topographie (kostenlos)

**Zeitbedarf:** 4-8 Wochen
**Kosten:** 0€ (akademisch)
**Schwierigkeit:** Mittel (Format-Konversion)

**ODER: Synthetische Testdaten in 1 Tag! 🚀**
