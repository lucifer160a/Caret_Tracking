# Caret Tracker Application

A cross-platform intelligent autocomplete application for tracking text cursor and providing smart word suggestions.

**Platforms:**
- **Linux/Fedora**: GTK 4-based autocomplete with suggestion popup
- **Windows**: Native C++ application with autocomplete suggestions

## Features

### Both Platforms
- Real-time text input monitoring
- Intelligent word suggestions from 400+ word dictionary
- Floating popup UI with suggestion buttons
- Prefix-based matching algorithm
- One-click word completion
- Low-resource footprint
- Non-intrusive UI

### Linux/Fedora (GTK Version)
- GTK 4 modern UI
- Keyboard event monitoring
- Floating suggestion popup
- Caret position tracking (via AT-SPI 2 when available)
- Works across all accessible applications

### Windows (C++ Version)
- Windows Accessibility API integration
- Low-level keyboard hook
- Global system monitoring
- Works with any Windows text editor

## Platform-Specific Setup

### Fedora/Linux (GTK Version - Autocomplete)

#### Requirements
- **System Dependencies**:
  ```bash
  sudo dnf install gtk4-devel python3-gobject
  ```
- **Python**: 3.8 or higher

#### Installation
```bash
pip3 install -r requirements.txt
```

#### Running
```bash
python3 src/main.py
```

#### How It Works
1. Start the application
2. Begin typing in any text field
3. Floating popup appears below cursor with suggestions
4. Click a suggestion to complete the word
5. Typed prefix is replaced with full word + space

### Windows (C++ Version)

#### Requirements
- **OS**: Windows 7 or later (Windows 10/11 recommended)
- **Visual Studio**: 2019 or later with C++ workload
- **Windows SDK**: 10.0 or later

#### Building
From Visual Studio:
1. Open `windows/Project2.vcxproj`
2. Select configuration (Debug/Release, Win32/x64)
3. Build → Build Solution

From Command Line:
```batch
cd windows
msbuild Project2.vcxproj /p:Configuration=Release /p:Platform=x64
```

#### Running
Execute the built `.exe` file:
```batch
Project2.exe
```

## Usage

### Linux/Ubuntu

### Using the Application

#### Fedora/Linux
1. Run the application: `python3 src/main.py`
2. The popup starts hidden and minimized
3. Begin typing in any text editor or input field
4. A popup appears near your cursor showing 16 relevant suggestions
5. Click a suggestion button to:
   - Replace your typed prefix with the complete word
   - Add a space after the word
   - Hide the popup
6. Continue typing or press special keys to reset

#### Windows
1. Run the application: `Project2.exe`
2. Start typing in any text editor or input field
3. A popup appears below the cursor with suggestions
4. Click a suggestion to complete it
5. The typed prefix is replaced with the full word + space

## Supported Applications

### Fedora/Linux (GTK)
Works with any application that allows text input:
- GNOME Text Editor (gedit)
- VS Code
- Firefox, Chrome
- LibreOffice
- GNOME Terminal
- Thunderbird
- Any accessible text field

### Windows (C++)
Works with any Windows text editor or input field:
- Notepad, Notepad++
- Visual Studio, VS Code
- Word, Excel, Google Docs
- Chrome, Edge, Firefox
- Any standard text input

## Project Structure

```
caret-tracker/
├── src/                        # Linux/Ubuntu (GTK) version
│   ├── main.py                 # Application entry point
│   ├── caret_tracker.py        # Core caret tracking logic (AT-SPI 2)
│   ├── ui.py                   # GTK 4 UI components
│   └── utils.py                # Utility functions
├── windows/                    # Windows (C++) version
│   ├── src/
│   │   ├── Project2.cpp        # Main Windows application
│   │   ├── Project2.h          # Header definitions
│   │   └── framework.h         # Windows framework includes
│   ├── Project2.vcxproj        # Visual Studio project file
│   └── README.md               # Windows-specific documentation
├── requirements.txt            # Python dependencies (Linux)
├── setup.py                    # Installation script (Linux)
├── README.md                   # This file
└── .github/
    └── copilot-instructions.md # Project instructions
```

## Architecture

### Fedora/Linux (GTK)

**main.py**
- Application wrapper using GTK
- Dictionary management (400+ words)
- Keyboard event monitoring setup
- Lifecycle management

**caret_tracker.py**
- `KeyboardMonitor`: Captures keyboard input globally
- `CaretTracker`: Tracks caret position via AT-SPI 2
- Buffer management for typed characters
- Signal emission on input changes

**ui.py**
- `CaretTrackerWindow`: Floating popup window
- `SuggestionButton`: Dynamic button creation
- Real-time suggestion display
- Screen clamping and positioning
- Prefix matching algorithm

### Windows (C++)

**Project2.cpp**
- **CaretWinEventProc**: Caret position event callback
- **LowLevelKeyboardProc**: Global keyboard hook
- **RebuildButtons**: Dynamic button layout
- **SendTextToEditor**: Word insertion with prefix removal
- **GetSuggestions**: Dictionary prefix matching
- **Global g_dictionary**: 400+ word database

## Logging

### Fedora/Linux
Application logs are written to:
- Console output
- `/tmp/caret-tracker-fedora.log`

### Windows
- Output to Visual Studio debug console (if running in debugger)
- Or standard application output

## Troubleshooting

### Fedora/Linux

**Application won't start**
- Ensure GTK 4 is installed: `sudo dnf install gtk4-devel python3-gobject`
- Check Python 3.8+: `python3 --version`
- Verify dependencies: `pip3 install -r requirements.txt`

**Popup doesn't appear**
- Ensure text editor has focus
- Start typing to activate suggestions
- Check `/tmp/caret-tracker-fedora.log` for errors

**Keyboard input not detected**
- Verify text field supports standard input
- Try with a different application (e.g., gedit)
- Check that no other input interceptor is active

**Suggestions not matching**
- Prefix must match word start (case-insensitive)
- Check dictionary contains the words
- Try partial words: "hel" matches "hello"

### Windows

**Popup Doesn't Appear**
- Ensure application window has focus
- Start typing in a text input field
- Check if Windows is blocking the keyboard hook
- May need administrator privileges

**Suggestions Not Matching**
- Suggestions use prefix matching (word must START with typed text)
- Verify dictionary contains the words (edit g_dictionary in Project2.cpp)

**Text Not Inserting**
- Verify target application supports text input
- Check keyboard hook is properly registered
- Some applications may require administrator privileges

**Application Crashes**
- Enable exception handling in Visual Studio debugger
- Check output for error messages
- Verify oleacc.lib is linked properly in project settings

## Development

## Development

### Fedora/Linux

To modify or extend the application:

1. Install development dependencies:
   ```bash
   pip3 install -r requirements.txt
   ```

2. Modify the source files in `src/`

3. Test changes by running:
   ```bash
   python3 src/main.py
   ```

To customize the dictionary, edit DICTIONARY in `src/main.py`

### Windows

To modify or extend the application:

1. Open `windows/Project2.vcxproj` in Visual Studio 2019 or later
2. Modify the source files in `windows/src/`
3. Build and run from Visual Studio (F5 for debug)
4. Or build from command line and run the executable

For custom dictionary:
- Edit `g_dictionary` in `windows/src/Project2.cpp`
- Rebuild the project

## Cross-Platform Comparison

| Feature | Fedora/Linux | Windows |
|---------|--------------|---------|
| **UI Framework** | GTK 4 | Native Windows |
| **Tracking Method** | Keyboard events | Win32 API |
| **Primary Function** | Autocomplete | Autocomplete |
| **Dictionary** | 400+ words | 400+ words |
| **Language** | Python | C++ |
| **Platform Support** | Fedora/Linux | Windows 7+ |
| **Memory Usage** | 30-50 MB | 10-20 MB |

| Feature | Fedora/Linux | Windows |
|---------|--------------|---------|
| **UI Framework** | GTK 4 | Native Windows |
| **Tracking Method** | Keyboard events | Win32 API |
| **Primary Function** | Autocomplete | Autocomplete |
| **Dictionary** | 400+ words | 400+ words |
| **Language** | Python | C++ |
| **Platform Support** | Fedora/Linux | Windows 7+ |
| **Memory Usage** | 30-50 MB | 10-20 MB |

## License

MIT License - See LICENSE file for details

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## Support

For issues or questions:
1. Check `/tmp/caret-tracker.log` for error messages
2. Ensure all system dependencies are installed
3. Verify your system supports AT-SPI 2 accessibility
