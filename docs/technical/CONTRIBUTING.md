# Contributing zu WorldQual

Vielen Dank für Ihr Interesse, zu WorldQual beizutragen! Diese Anleitung erklärt, wie Sie Beiträge leisten können.

## Inhaltsverzeichnis

- [Wie kann ich beitragen?](#wie-kann-ich-beitragen)
- [Entwicklungsworkflow](#entwicklungsworkflow)
- [Pull Request Prozess](#pull-request-prozess)
- [Code-Standards](#code-standards)
- [Dokumentation](#dokumentation)

## Wie kann ich beitragen?

### Arten von Beiträgen

1. **Bugfixes** - Fehler beheben
2. **Features** - Neue Funktionalität hinzufügen
3. **Dokumentation** - Dokumentation verbessern
4. **Code-Refactoring** - Code verbessern
5. **Tests** - Tests hinzufügen
6. **Performance** - Performance optimieren

### Erste Schritte

1. **Repository forken** (falls Sie kein Collaborator sind)
2. **Repository klonen**
   ```bash
   git clone https://github.com/Leon-Muehlenbruch/WorldQual.git
   cd WorldQual
   ```

3. **Entwicklungsumgebung einrichten**
   - Siehe [DEVELOPMENT.md](DEVELOPMENT.md)

4. **Bestehenden Code verstehen**
   - Siehe [ARCHITECTURE.md](ARCHITECTURE.md)

## Entwicklungsworkflow

### 1. Issue erstellen (empfohlen)

Bevor Sie Code schreiben:
1. Prüfen Sie, ob bereits ein Issue existiert
2. Falls nicht, erstellen Sie ein Issue mit:
   - Beschreibung des Problems/Features
   - Erwartetes Verhalten
   - Vorschlag zur Lösung (optional)

### 2. Branch erstellen

```bash
# Von main aus
git checkout main
git pull origin main

# Neuen Branch erstellen
git checkout -b feature/meine-funktion
# oder
git checkout -b bugfix/bug-beschreibung
# oder
git checkout -b docs/verbesserung
```

**Branch-Namen:**
- `feature/*` - Neue Features
- `bugfix/*` - Bugfixes
- `docs/*` - Dokumentation
- `refactor/*` - Refactoring
- `test/*` - Tests

### 3. Code entwickeln

- Code-Standards beachten (siehe [CODING_STANDARDS.md](CODING_STANDARDS.md))
- Kommentare hinzufügen
- Testen Sie Ihren Code

### 4. Commits erstellen

**Commit-Messages:**

Verwenden Sie konventionelle Commits:
```
feat: Neue Funktion für X hinzugefügt
fix: Bug in get_factor behoben
docs: README aktualisiert
refactor: Code in get_Q umstrukturiert
test: Tests für general_functions hinzugefügt
perf: Performance von calculate_riverLength verbessert
```

**Beispiele:**
```bash
git add src/general_function/general_functions.cpp
git commit -m "feat: Neue Funktion get_new_function hinzugefügt"

git add README.md
git commit -m "docs: Installation-Anleitung erweitert"
```

### 5. Push und Pull Request

```bash
git push origin feature/meine-funktion
```

Dann auf GitHub Pull Request erstellen.

## Pull Request Prozess

### Pull Request erstellen

1. **Auf GitHub gehen**
   - Repository öffnen
   - "Pull Request" klicken
   - Branch auswählen

2. **Beschreibung ausfüllen**

**Template:**
```markdown
## Beschreibung
Kurze Beschreibung der Änderungen

## Art der Änderung
- [ ] Bugfix
- [ ] Neues Feature
- [ ] Dokumentation
- [ ] Refactoring

## Änderungen
- Was wurde geändert?
- Warum wurde es geändert?

## Testing
- Wie wurde getestet?
- Welche Tests wurden durchgeführt?

## Checkliste
- [ ] Code kompiliert ohne Warnungen
- [ ] Code-Standards eingehalten
- [ ] Kommentare hinzugefügt
- [ ] Dokumentation aktualisiert
- [ ] Tests bestehen (falls vorhanden)
```

### Code-Review

**Was wird geprüft:**
- Code-Qualität
- Einhaltung der Standards
- Funktionalität
- Dokumentation
- Tests

**Nach Review:**
- Änderungen anfordern (falls nötig)
- Approve
- Merge

### Nach dem Merge

- Branch kann gelöscht werden
- Änderungen sind in `main`

## Code-Standards

Siehe [CODING_STANDARDS.md](CODING_STANDARDS.md) für Details.

**Kurzfassung:**
- C++11 Standard
- Konsistente Namenskonventionen
- Doxygen-Kommentare
- Fehlerbehandlung
- Memory-Management korrekt

## Dokumentation

### Code dokumentieren

**Doxygen-Kommentare:**
```cpp
/**
 * @brief Kurze Beschreibung
 * @param param Beschreibung
 * @return Rückgabewert
 */
int function(int param);
```

### Benutzer-Dokumentation aktualisieren

Wenn Sie Funktionalität ändern/hinzufügen:

1. **README.md** aktualisieren (falls relevant)
2. **DOCUMENTATION.md** aktualisieren (falls relevant)
3. **QUICK_REFERENCE.md** aktualisieren (falls relevant)

### Technische Dokumentation aktualisieren

Wenn Sie Architektur ändern:

1. **docs/technical/ARCHITECTURE.md** aktualisieren
2. **docs/technical/API.md** aktualisieren (falls API geändert)

## Häufige Szenarien

### Bugfix

```bash
# 1. Issue erstellen (falls nicht vorhanden)
# 2. Branch erstellen
git checkout -b bugfix/segfault-in-get-factor

# 3. Bug fixen
# Code ändern...

# 4. Testen
make clean && make all
./worldqual test_parameter

# 5. Commit
git add .
git commit -m "fix: Segmentation fault in get_factor behoben"

# 6. Push und PR
git push origin bugfix/segfault-in-get-factor
```

### Neues Feature

```bash
# 1. Issue erstellen
# 2. Branch erstellen
git checkout -b feature/neue-berechnung

# 3. Code entwickeln
# Neue Funktionen hinzufügen...

# 4. Dokumentation aktualisieren
# README.md, DOCUMENTATION.md aktualisieren

# 5. Testen
make clean && make all
./programm test

# 6. Commits
git add src/
git commit -m "feat: Neue Berechnungsfunktion hinzugefügt"
git add README.md DOCUMENTATION.md
git commit -m "docs: Dokumentation für neue Funktion aktualisiert"

# 7. Push und PR
git push origin feature/neue-berechnung
```

### Dokumentation verbessern

```bash
# 1. Branch erstellen
git checkout -b docs/verbesserung

# 2. Dokumentation ändern
# README.md, DOCUMENTATION.md, etc. bearbeiten

# 3. Commit
git add .
git commit -m "docs: Installation-Anleitung verbessert"

# 4. Push und PR
git push origin docs/verbesserung
```

## Fragen?

Bei Fragen:
- Issue erstellen mit Label "question"
- Code-Kommentare im Pull Request
- Diskussion im Pull Request

---

**Vielen Dank für Ihre Beiträge!**
