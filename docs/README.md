# WorldQual - Technische Dokumentation

Diese Dokumentation richtet sich an **Entwickler**, die WorldQual weiterentwickeln, warten oder erweitern möchten.

## Inhaltsverzeichnis

- [Architektur](technical/ARCHITECTURE.md) - Code-Struktur und Design
- [Entwicklungsumgebung](technical/DEVELOPMENT.md) - Build-System, Entwicklung, Testing
- [Code-Standards](technical/CODING_STANDARDS.md) - Coding-Konventionen und Best Practices
- [Contributing](technical/CONTRIBUTING.md) - Wie man zu WorldQual beiträgt
- [Datenbank-Schema](technical/DATABASE_SCHEMA.md) - Detaillierte Datenbankstruktur
- [API-Referenz](technical/API.md) - Wichtige Funktionen und Klassen
- [Build-System](technical/BUILD_SYSTEM.md) - Makefiles und Kompilierung verstehen

## Navigation

**Für Nutzer:**
- [Benutzer-Dokumentation](../README.md) - Hauptdokumentation im Root-Verzeichnis
- [Schnellstart](../README.md#schnellstart-quick-start) - Erste Schritte
- [Detaillierte Anleitung](../DOCUMENTATION.md) - Schritt-für-Schritt Anleitungen

**Für Entwickler:**
- Starten Sie mit [Architektur](technical/ARCHITECTURE.md)
- Dann [Entwicklungsumgebung](technical/DEVELOPMENT.md)
- Lesen Sie [Code-Standards](technical/CODING_STANDARDS.md) vor dem Coden

## Schnelleinstieg für Entwickler

1. **Repository klonen**
   ```bash
   git clone https://github.com/Leon-Muehlenbruch/WorldQual.git
   cd WorldQual
   ```

2. **Entwicklungsumgebung einrichten**
   - Siehe [DEVELOPMENT.md](technical/DEVELOPMENT.md)

3. **Code verstehen**
   - Siehe [ARCHITECTURE.md](technical/ARCHITECTURE.md)

4. **Beitragen**
   - Siehe [CONTRIBUTING.md](technical/CONTRIBUTING.md)

## Dokumentations-Struktur

```
WorldQual/
├── README.md              # Hauptdokumentation (für Nutzer)
├── DOCUMENTATION.md       # Detaillierte Anleitung (für Nutzer)
├── QUICK_REFERENCE.md     # Schnellreferenz (für Nutzer)
├── STRUCTURE.md           # Projektstruktur (für Nutzer)
├── INDEX.md               # Dokumentations-Index (für Nutzer)
│
└── docs/                  # Technische Dokumentation (für Entwickler)
    ├── README.md          # Dieser Index
    └── technical/
        ├── ARCHITECTURE.md      # Code-Architektur
        ├── DEVELOPMENT.md       # Entwicklungsumgebung
        ├── CODING_STANDARDS.md  # Code-Standards
        ├── CONTRIBUTING.md      # Contributing Guidelines
        ├── DATABASE_SCHEMA.md   # Datenbank-Schema
        ├── API.md               # API-Referenz
        └── BUILD_SYSTEM.md      # Build-System
```

**GitHub-Standard:**
- **Root-Verzeichnis**: Benutzer-Dokumentation (README.md, etc.)
- **docs/ Ordner**: Technische/Entwickler-Dokumentation
- Dies ist der übliche Weg für GitHub-Projekte!

---

**Hinweis:** Diese technische Dokumentation ergänzt die Benutzer-Dokumentation im Root-Verzeichnis.
