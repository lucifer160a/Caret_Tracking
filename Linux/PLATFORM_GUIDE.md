# Caret Tracker - Platform Guide

This document provides a quick reference for building and running the Caret Tracker application on both platforms.

## Quick Start

### Windows (C++ - Autocomplete Focus)

**Build:**
```batch
cd windows
build.bat Release x64
```

**Run:**
```batch
windows\bin\x64\Release\Project2.exe
```

**Or directly from Visual Studio:**
- Open `windows\Project2.vcxproj`
- Select Release|x64 configuration
- Press Ctrl+Shift+B to build
- Press F5 to debug, Ctrl+F5 to run

### Linux/Ubuntu (Python - Position Tracking Focus)

**Setup:**
```bash
sudo apt install libgtk-4-dev python3-gi python3-gi-cairo gir1.2-gtk-4.0 at-spi2-core
pip3 install -r requirements.txt
```

**Run:**
```bash
python3 src/main.py
```

## Platform Capabilities

### Windows Version

**What it does:**
- Displays autocomplete suggestions as you type
- Shows 16 most relevant words from dictionary
- Click to insert suggested word
- Unobtrusive floating popup near cursor
- Works with any text editor/input field

**How to use:**
1. Run `Project2.exe`
2. Start typing in any application
3. Suggestions appear automatically
4. Click a suggestion to complete it
5. Typed prefix is replaced with full word + space

**Technical approach:**
- Windows Accessibility API for caret tracking
- Low-level keyboard hook for input capture
- Dynamic UI buttons for suggestions
- 400+ word dictionary with prefix matching

### Linux/Ubuntu Version

**What it does:**
- Displays current caret position (X, Y pixels)
- Shows active application name
- Displays text context around caret
- Real-time position updates
- Start/stop tracking controls

**How to use:**
1. Run `python3 src/main.py`
2. Click "Start Tracking"
3. Move cursor in any accessible application
4. Position and application info updated in real-time
5. Click "Stop Tracking" to pause

**Technical approach:**
- AT-SPI 2 accessibility service
- GTK 4 for user interface
- Event-driven architecture
- System-wide caret monitoring

## File Structure

```
caret-tracker/
├── src/                          # Linux version (Python)
│   ├── main.py                   # Entry point
│   ├── caret_tracker.py          # Tracking logic
│   ├── ui.py                     # GTK UI
│   └── utils.py                  # Helpers
│
├── windows/                      # Windows version (C++)
│   ├── src/
│   │   ├── Project2.cpp          # Main code
│   │   ├── Project2.h            # Headers
│   │   └── framework.h           # Windows includes
│   ├── Project2.vcxproj          # Visual Studio project
│   ├── build.bat                 # Build script
│   └── README.md                 # Windows-specific docs
│
├── requirements.txt              # Python dependencies
├── setup.py                      # Python setup script
├── README.md                     # Main documentation
├── PLATFORM_GUIDE.md             # This file
└── .github/
    └── copilot-instructions.md   # Project info
```

## Building Details

### Windows Build Options

**Visual Studio GUI:**
1. Open `windows\Project2.vcxproj`
2. Choose configuration:
   - **Debug|Win32**: 32-bit debug build
   - **Debug|x64**: 64-bit debug build
   - **Release|Win32**: 32-bit optimized build
   - **Release|x64**: 64-bit optimized build (recommended)
3. Build → Build Solution (Ctrl+Shift+B)

**Command Line:**
```batch
cd windows
msbuild Project2.vcxproj /p:Configuration=Release /p:Platform=x64 /m
```

**Build Script:**
```batch
cd windows
build.bat Release x64
build.bat Debug Win32
```

### Linux/Ubuntu Build

**No compilation needed** - it's Python!

Install dependencies:
```bash
sudo apt install libgtk-4-dev python3-gi python3-gi-cairo gir1.2-gtk-4.0 at-spi2-core
pip3 install -r requirements.txt
```

Optional: Create standalone executable with PyInstaller:
```bash
pip install pyinstaller
pyinstaller --onefile src/main.py
```

## Configuration

### Windows

Edit dictionary in `windows/src/Project2.cpp`:
```cpp
std::vector<std::wstring> g_dictionary = {
    L"word1", L"word2", L"word3", ...
};
```

Change UI constants:
```cpp
#define WINDOW_HEIGHT   50      // Popup height
#define WINDOW_MAX_WIDTH 600    // Max width
#define BTN_WIDTH   80          // Button width
#define BTN_HEIGHT  28          // Button height
#define BTN_MARGIN  5           // Spacing between buttons
```

### Linux/Ubuntu

Configure logging in `src/main.py`:
```python
logging.basicConfig(
    level=logging.INFO,  # Change to DEBUG for verbose output
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
```

Modify UI in `src/ui.py`:
- Window size
- Color scheme
- Label formatting
- Button layout

## System Requirements

### Windows
- **OS**: Windows 7 or later (Windows 10/11 recommended)
- **RAM**: 256 MB (typical usage: 10-20 MB)
- **CPU**: Any modern processor
- **Visual Studio**: 2019, 2022, or Build Tools for Visual Studio

### Linux/Ubuntu
- **OS**: Ubuntu 18.04 or later (tested on 20.04, 22.04)
- **Python**: 3.8 or higher
- **RAM**: 256 MB (typical usage: 30-50 MB)
- **GTK**: 4.0 or higher

## Permissions & Security

### Windows
- Application may request administrator privileges
- Needs to attach to other processes' input threads
- Global keyboard hook (system-wide)

### Linux/Ubuntu
- Requires access to accessibility system
- May need to enable accessibility permissions:
  - Settings → Accessibility
  - Or system configuration

## Performance

| Platform | Memory | CPU (Idle) | Startup Time |
|----------|--------|-----------|--------------|
| Windows | 10-20 MB | <1% | ~100 ms |
| Linux | 30-50 MB | <1% | ~500 ms |

## Troubleshooting

### Windows Won't Build
1. Ensure Visual Studio 2019 or later is installed
2. Install Windows SDK 10.0 or later
3. Install C++ workload in Visual Studio
4. Run as Administrator if encountering permission errors

### Linux Won't Start
1. Install all system dependencies: `sudo apt install libgtk-4-dev python3-gi python3-gi-cairo gir1.2-gtk-4.0 at-spi2-core`
2. Verify Python 3.8+: `python3 --version`
3. Install Python packages: `pip3 install -r requirements.txt`
4. Check for import errors: `python3 -c "import gi; gi.require_version('Gtk', '4')"`

### Application Won't Interact with Editor
- **Windows**: Try running as Administrator
- **Linux**: Enable accessibility in application settings
- Check that application is in focus
- Verify keyboard/mouse input is not blocked

## Development Workflow

### Quick Iteration on Windows
1. Visual Studio → Build (Ctrl+Shift+B)
2. Visual Studio → Debug (F5) or Run (Ctrl+F5)
3. Or use: `build.bat Debug x64` then run executable

### Quick Iteration on Linux
1. Make changes to Python files
2. Run: `python3 src/main.py`
3. No recompilation needed

## Next Steps

- **Windows**: See [windows/README.md](windows/README.md) for detailed Windows documentation
- **Linux**: See [README.md](README.md) for detailed cross-platform documentation
- **Contribute**: Submit improvements via pull requests

## Support

For issues specific to:
- **Windows version**: See [windows/README.md](windows/README.md#troubleshooting)
- **Linux version**: See [README.md](README.md#troubleshooting)
- **Both platforms**: Check the main README.md
