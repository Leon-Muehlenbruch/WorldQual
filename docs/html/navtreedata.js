/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "WorldQual", "index.html", [
    [ "WorldQual Übersicht", "index.html", null ],
    [ "Installation", "tutorial_installation.html", [
      [ "WorldQual Installation", "tutorial_installation.html#autotoc_md11", [
        [ "Schritt 1: Repository klonen", "tutorial_installation.html#autotoc_md13", null ],
        [ "Schritt 2: Dependencies installieren", "tutorial_installation.html#autotoc_md15", null ],
        [ "Schritt 3: Kompilieren", "tutorial_installation.html#autotoc_md17", null ],
        [ "Schritt 4: Ausführen", "tutorial_installation.html#autotoc_md19", null ],
        [ "Fertig! 🎉", "tutorial_installation.html#autotoc_md21", null ],
        [ "Datenbank einrichten (Optional)", "tutorial_installation.html#autotoc_md23", [
          [ "Datenbank erstellen:", "tutorial_installation.html#autotoc_md24", null ],
          [ "Verbindung konfigurieren:", "tutorial_installation.html#autotoc_md25", null ]
        ] ],
        [ "Weitere Module kompilieren", "tutorial_installation.html#autotoc_md27", [
          [ "Nährstoffeinträge (wq_load):", "tutorial_installation.html#autotoc_md28", null ],
          [ "Statistik (wq_stat):", "tutorial_installation.html#autotoc_md29", null ],
          [ "Visualisierung (MapRiverQuality):", "tutorial_installation.html#autotoc_md30", null ],
          [ "Wassertemperatur:", "tutorial_installation.html#autotoc_md31", null ]
        ] ],
        [ "Troubleshooting", "tutorial_installation.html#autotoc_md33", [
          [ "Problem: \"brew: command not found\"", "tutorial_installation.html#autotoc_md34", null ],
          [ "Problem: \"make: command not found\"", "tutorial_installation.html#autotoc_md35", null ],
          [ "Problem: \"mysql++.h not found\"", "tutorial_installation.html#autotoc_md36", null ],
          [ "Problem: Datenbankverbindung fehlgeschlagen", "tutorial_installation.html#autotoc_md37", null ]
        ] ],
        [ "System-Anforderungen", "tutorial_installation.html#autotoc_md39", [
          [ "Unterstützte Systeme:", "tutorial_installation.html#autotoc_md40", null ],
          [ "Erforderlich:", "tutorial_installation.html#autotoc_md41", null ],
          [ "Windows-Benutzer:", "tutorial_installation.html#autotoc_md42", null ],
          [ "Empfohlen:", "tutorial_installation.html#autotoc_md43", null ]
        ] ],
        [ "Schnellstart-Zusammenfassung", "tutorial_installation.html#autotoc_md45", null ],
        [ "Weitere Hilfe", "tutorial_installation.html#autotoc_md47", null ]
      ] ]
    ] ],
    [ "Konfiguration", "tutorial_konfiguration.html", [
      [ "WorldQual Konfiguration &amp; Daten-Setup", "tutorial_konfiguration.html#autotoc_md48", [
        [ "Überblick", "tutorial_konfiguration.html#autotoc_md50", null ],
        [ "1. OPTIONS.DAT Konfiguration", "tutorial_konfiguration.html#autotoc_md52", [
          [ "Speicherort:", "tutorial_konfiguration.html#autotoc_md53", null ],
          [ "Vollständige Parameter-Übersicht:", "tutorial_konfiguration.html#autotoc_md54", null ],
          [ "Beispiel-Konfiguration für Afrika:", "tutorial_konfiguration.html#autotoc_md55", null ]
        ] ],
        [ "2. MySQL-Datenbank Setup", "tutorial_konfiguration.html#autotoc_md57", [
          [ "Schritt 1: Datenbank-Schema installieren", "tutorial_konfiguration.html#autotoc_md58", null ],
          [ "Schritt 2: User und Rechte einrichten", "tutorial_konfiguration.html#autotoc_md59", null ],
          [ "Schritt 3: Test-Daten einfügen", "tutorial_konfiguration.html#autotoc_md60", null ],
          [ "Datenbank-Struktur:", "tutorial_konfiguration.html#autotoc_md61", null ]
        ] ],
        [ "3. Input-Daten: UNF-Dateien", "tutorial_konfiguration.html#autotoc_md63", [
          [ "Was sind UNF-Dateien?", "tutorial_konfiguration.html#autotoc_md64", null ],
          [ "Benötigte UNF-Dateien:", "tutorial_konfiguration.html#autotoc_md65", [
            [ "A. Hydrologische Zeitreihen (path_watergap_output)", "tutorial_konfiguration.html#autotoc_md66", null ],
            [ "B. Klimadaten (path_climate)", "tutorial_konfiguration.html#autotoc_md67", null ],
            [ "C. Statische Hydro-Daten (path_hydro_input)", "tutorial_konfiguration.html#autotoc_md68", null ],
            [ "D. Routing (path_hydro_routing)", "tutorial_konfiguration.html#autotoc_md69", null ]
          ] ],
          [ "Verzeichnis-Struktur (Empfohlen):", "tutorial_konfiguration.html#autotoc_md70", null ]
        ] ],
        [ "4. Daten beschaffen", "tutorial_konfiguration.html#autotoc_md72", [
          [ "Option A: Echte Daten von ISIMIP", "tutorial_konfiguration.html#autotoc_md73", null ],
          [ "Option B: Synthetische Test-Daten", "tutorial_konfiguration.html#autotoc_md74", null ]
        ] ],
        [ "5. Daten-Import Workflow", "tutorial_konfiguration.html#autotoc_md76", [
          [ "Kompletter Workflow:", "tutorial_konfiguration.html#autotoc_md77", null ]
        ] ],
        [ "6. Konfiguration prüfen", "tutorial_konfiguration.html#autotoc_md79", [
          [ "Datenbank-Verbindung testen:", "tutorial_konfiguration.html#autotoc_md80", null ],
          [ "UNF-Dateien prüfen:", "tutorial_konfiguration.html#autotoc_md81", null ],
          [ "OPTIONS.DAT validieren:", "tutorial_konfiguration.html#autotoc_md82", null ]
        ] ],
        [ "7. Erstes Simulations-Beispiel", "tutorial_konfiguration.html#autotoc_md84", [
          [ "Minimal-Beispiel (Test-Lauf):", "tutorial_konfiguration.html#autotoc_md85", null ],
          [ "Ergebnisse prüfen:", "tutorial_konfiguration.html#autotoc_md86", null ]
        ] ],
        [ "8. Erweiterte Konfiguration", "tutorial_konfiguration.html#autotoc_md88", [
          [ "Mehrere Parameter simulieren:", "tutorial_konfiguration.html#autotoc_md89", null ],
          [ "Verschiedene Szenarien:", "tutorial_konfiguration.html#autotoc_md90", null ]
        ] ],
        [ "9. Troubleshooting", "tutorial_konfiguration.html#autotoc_md92", [
          [ "Problem: \"Database connection failed\"", "tutorial_konfiguration.html#autotoc_md93", null ],
          [ "Problem: \"UNF file not found\"", "tutorial_konfiguration.html#autotoc_md94", null ],
          [ "Problem: \"No data in load tables\"", "tutorial_konfiguration.html#autotoc_md95", null ],
          [ "Problem: \"Segmentation fault\"", "tutorial_konfiguration.html#autotoc_md96", null ]
        ] ],
        [ "10. Performance-Optimierung", "tutorial_konfiguration.html#autotoc_md98", [
          [ "MySQL-Optimierung:", "tutorial_konfiguration.html#autotoc_md99", null ],
          [ "Parallel-Verarbeitung:", "tutorial_konfiguration.html#autotoc_md100", null ]
        ] ],
        [ "11. Weitere Module", "tutorial_konfiguration.html#autotoc_md102", [
          [ "wq_load (Nährstoff-Einträge berechnen):", "tutorial_konfiguration.html#autotoc_md103", null ],
          [ "wq_stat (Statistik):", "tutorial_konfiguration.html#autotoc_md104", null ],
          [ "MapRiverQuality (Visualisierung):", "tutorial_konfiguration.html#autotoc_md105", null ]
        ] ],
        [ "12. Best Practices", "tutorial_konfiguration.html#autotoc_md107", [
          [ "Daten-Organisation:", "tutorial_konfiguration.html#autotoc_md108", null ],
          [ "Simulations-Workflow:", "tutorial_konfiguration.html#autotoc_md109", null ],
          [ "Versionskontrolle:", "tutorial_konfiguration.html#autotoc_md110", null ]
        ] ],
        [ "Zusammenfassung", "tutorial_konfiguration.html#autotoc_md112", null ]
      ] ]
    ] ],
    [ "Klassen", "annotated.html", [
      [ "Auflistung der Klassen", "annotated.html", "annotated_dup" ],
      [ "Klassen-Verzeichnis", "classes.html", null ],
      [ "Klassenhierarchie", "hierarchy.html", "hierarchy" ],
      [ "Klassen-Elemente", "functions.html", [
        [ "Alle", "functions.html", "functions_dup" ],
        [ "Funktionen", "functions_func.html", null ],
        [ "Variablen", "functions_vars.html", "functions_vars" ]
      ] ]
    ] ],
    [ "Dateien", "files.html", [
      [ "Auflistung der Dateien", "files.html", "files_dup" ],
      [ "Datei-Elemente", "globals.html", [
        [ "Alle", "globals.html", "globals_dup" ],
        [ "Funktionen", "globals_func.html", null ],
        [ "Variablen", "globals_vars.html", null ],
        [ "Makrodefinitionen", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"_class_w_qload_8cpp.html",
"class_l_s___c_l_a_s_s.html#a48aa9326ec724fcec43f8b08fadb7d43",
"copy__scen_8cpp.html#adc58c9862730d1b6c6bc472aaa13f6a7",
"map_river_quality_8cpp.html#ae2b9d5fbeb940658c8ab8ec58f40bd62",
"wq__load_2def_8h.html#a48cab1269f5c8a1d17e0d7bec446b151"
];

var SYNCONMSG = 'Klicken um Panelsynchronisation auszuschalten';
var SYNCOFFMSG = 'Klicken um Panelsynchronisation einzuschalten';
var LISTOFALLMEMBERS = 'Aufstellung aller Elemente';