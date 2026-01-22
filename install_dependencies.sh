#!/bin/bash
# WorldQual Dependency Installer
# PLATFORM: macOS and Linux (with Homebrew)
# For Windows: Use WSL (Windows Subsystem for Linux) and run this script inside WSL
# Automatically installs all required dependencies

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo "╔══════════════════════════════════════════════════════════════╗"
echo "║        WorldQual Dependency Installer                        ║"
echo "║        Platforms: macOS, Linux (with Homebrew)               ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""

# Check platform
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo -e "${BLUE}Linux detected${NC}"
    echo ""
    echo -e "${YELLOW}This script uses Homebrew.${NC}"
    echo "For native package managers (apt/yum), please see requirements.txt"
    echo ""
    read -p "Continue with Homebrew installation? (y/n): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Installation cancelled."
        echo ""
        echo "For manual installation on Linux:"
        echo "  Ubuntu/Debian: sudo apt-get install mysql-server libmysqlclient-dev libmysql++-dev"
        echo "  RedHat/CentOS: sudo yum install mysql-server mysql-devel mysql++-devel"
        exit 0
    fi
elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo -e "${GREEN}macOS detected${NC}"
elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    echo -e "${RED}Windows detected${NC}"
    echo ""
    echo "This script does not support native Windows."
    echo ""
    echo "Recommended options for Windows:"
    echo "  1. WSL (Windows Subsystem for Linux) - Easiest"
    echo "     • Install WSL: wsl --install"
    echo "     • Run this script inside WSL Ubuntu"
    echo ""
    echo "  2. MSYS2/MinGW - For advanced users"
    echo "     • See requirements.txt for details"
    echo ""
    echo "  3. Visual Studio - Requires manual setup"
    echo "     • See requirements.txt for details"
    exit 1
else
    echo -e "${YELLOW}Unknown platform: $OSTYPE${NC}"
    echo "This script is designed for macOS and Linux."
    echo "See requirements.txt for manual installation instructions."
    exit 1
fi

# Check if Homebrew is installed
if ! command -v brew &> /dev/null; then
    echo -e "${YELLOW}Homebrew not found. Installing Homebrew...${NC}"
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    
    # Add Homebrew to PATH for Apple Silicon
    if [[ $(uname -m) == 'arm64' ]]; then
        echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
        eval "$(/opt/homebrew/bin/brew shellenv)"
    fi
else
    echo -e "${GREEN}✓ Homebrew found${NC}"
fi

# Check if Xcode Command Line Tools are installed
if ! command -v gcc &> /dev/null; then
    echo -e "${YELLOW}Installing Xcode Command Line Tools...${NC}"
    xcode-select --install
    echo -e "${YELLOW}Please complete the Xcode installation and run this script again${NC}"
    exit 1
else
    echo -e "${GREEN}✓ Xcode Command Line Tools found${NC}"
fi

# Install dependencies using Brewfile
echo ""
echo -e "${BLUE}Installing dependencies from Brewfile...${NC}"
echo ""

cd "$(dirname "$0")"

if [ -f "Brewfile" ]; then
    brew bundle --file=Brewfile
else
    echo -e "${YELLOW}Brewfile not found, installing manually...${NC}"
    brew install mysql@8.0 mysql-connector-c++ doxygen graphviz
fi

echo ""
echo -e "${GREEN}╔══════════════════════════════════════════════════════════════╗${NC}"
echo -e "${GREEN}║           Dependencies installed successfully!               ║${NC}"
echo -e "${GREEN}╚══════════════════════════════════════════════════════════════╝${NC}"
echo ""

# Start MySQL if not running
echo -e "${BLUE}Checking MySQL status...${NC}"
if brew services list | grep mysql | grep started > /dev/null; then
    echo -e "${GREEN}✓ MySQL is running${NC}"
else
    echo -e "${YELLOW}Starting MySQL...${NC}"
    brew services start mysql@8.0
    echo -e "${GREEN}✓ MySQL started${NC}"
fi

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "                    Next Steps"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "1. Configure MySQL (optional, for first time):"
echo "   $ mysql_secure_installation"
echo ""
echo "2. Build WorldQual:"
echo "   $ cd src/worldqual"
echo "   $ make clean && make"
echo ""
echo "3. Run WorldQual:"
echo "   $ ./worldqual IDrun startYear endYear"
echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
