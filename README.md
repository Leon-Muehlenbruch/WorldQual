# WorldQual - Global Water Quality Modeling System

## Overview

WorldQual is a scientific C++ program for modeling water quality in river systems at global or continental scales. The system simulates pollutant transport, water temperature, and various water quality parameters in river networks.

## Project Information

- **Original Author**: Ellen Teichert
- **Initial Development**: 2007
- **Language**: C++
- **Database**: MySQL
- **Grid Resolution**: 0.5°

## Key Features

- River water quality simulation
- Pollutant load calculations (agriculture, livestock, domestic)
- Water temperature modeling
- Scenario analysis capabilities
- Integration with WaterGAP (Water - Global Assessment and Prognosis)
- Statistical evaluation tools
- R-based visualization

## Project Structure

```
C++/
├── worldqual/           # Main simulation program
├── wq_load/            # Pollutant load calculations
├── wq_stat/            # Statistical evaluation
├── water_temperature/  # Water temperature modeling
├── newton/             # Numerical algorithms (Newton method)
├── general_function/   # Shared utility functions
├── options/            # Configuration handling
├── R-scripte/          # R scripts for visualization
└── MapRiverQuality/    # River quality mapping tools
```

## Prerequisites

### System Requirements
- C++ compiler (g++)
- MySQL database server
- MySQL++ library
- Make build system

### Optional Requirements
- R (for visualization scripts)
- WaterGAP data files

## Installation

### 1. Install Dependencies

**On macOS:**
```bash
brew install mysql
brew install mysql++
```

**On Linux (Ubuntu/Debian):**
```bash
sudo apt-get install mysql-server
sudo apt-get install libmysqlclient-dev
sudo apt-get install libmysql++-dev
```

### 2. Database Setup

```bash
# Create database and user
mysql -u root -p
CREATE DATABASE worldqual;
CREATE USER 'worldqual'@'localhost' IDENTIFIED BY 'your_password';
GRANT ALL PRIVILEGES ON worldqual.* TO 'worldqual'@'localhost';
FLUSH PRIVILEGES;
```

### 3. Build the Project

```bash
cd worldqual
make
```

## Usage

### Basic WorldQual Execution

```bash
./worldqual IDrun startYear endYear [-o options_file]
```

**Parameters:**
- `IDrun`: Run identifier
- `startYear`: Start year of simulation
- `endYear`: End year of simulation
- `-o`: Optional configuration file (default: OPTIONS.DAT)

### Example

```bash
./worldqual 1 2000 2010 -o OPTIONS.DAT
```

## Configuration

Configuration files (OPTIONS.DAT) contain:
- Database connection parameters
- Input/output paths
- Model parameters
- Regional settings

## Modules

### Core Modules

- **worldqual**: Main water quality simulation
- **wq_load**: Calculate pollutant inputs from various sources
- **wq_stat**: Statistical analysis and validation
- **water_temperature**: Water temperature calculations

### Utility Modules

- **newton**: Numerical curve fitting algorithms
- **general_function**: Shared functions and utilities
- **options**: Configuration file parsing

## Data Requirements

- River network topology
- Flow routing data
- Pollutant load data (agriculture, domestic, industry)
- Climate data (for water temperature)
- Validation station data (optional)

## Output

The system produces:
- Time series of water quality parameters
- Statistical summaries
- Validation metrics
- Visualization-ready data files

## Contributing

[To be added: Guidelines for contributing to this project]

## License

[To be specified]

## Contact

[To be added: Contact information for questions]

## Acknowledgments

- WaterGAP team for hydrological data
- Original development team

## Documentation Status

⚠️ **This documentation is currently being developed and expanded.**

For detailed technical documentation, see:
- [ARCHITECTURE.md](ARCHITECTURE.md) - System architecture (coming soon)
- [API.md](API.md) - Module interfaces (coming soon)
- Individual module README files in subdirectories
