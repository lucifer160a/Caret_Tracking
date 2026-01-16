# Caret Tracker - Windows Edition

Windows version of the caret tracking application with text autocomplete suggestions.

## Features

- **Real-time Caret Tracking**: Monitors text cursor position in Windows applications
- **Smart Suggestions**: Displays autocomplete suggestions as you type
- **Floating Popup**: Unobtrusive popup window that appears near the cursor
- **Dictionary-based**: Comprehensive English dictionary with 400+ words
- **One-click Insertion**: Click suggestion buttons to auto-complete words
- **Multi-editor Support**: Works with any Windows application supporting text input
- **Keyboard Integration**: Transparent keyboard hook for seamless integration

## Requirements

- **OS**: Windows 7 or later (Windows 10/11 recommended)
- **Visual Studio**: 2019 or later (with C++ workload)
- **Windows SDK**: 10.0 or later
- **API Requirements**:
  - Windows Accessibility API (oleacc.lib)
  - Low-level keyboard hooks

## Building from Visual Studio

1. Open `windows/Project2.vcxproj` in Visual Studio
2. Select configuration:
   - Debug or Release
   - Win32 or x64
3. Build → Build Solution (Ctrl+Shift+B)
4. Output executable: `windows/bin/Debug/` or `windows/bin/Release/`

## Building from Command Line

Using Visual Studio Developer Command Prompt:

```batch
cd windows
msbuild Project2.vcxproj /p:Configuration=Release /p:Platform=x64
```

## Installation

1. Build the application (see above)
2. Run the generated `.exe` file:
   ```
   Project2.exe
   ```
3. The application starts minimized and activates when you start typing

## Usage

1. **Start the Application**: Run the executable
2. **Begin Typing**: Start typing in any text editor or input field
3. **View Suggestions**: A popup window appears below the cursor showing relevant suggestions
4. **Select Suggestion**: Click a suggestion button or press Enter
5. **Word Completion**: The typed prefix is replaced with the selected word + space

## How It Works

### Caret Tracking
- Uses `SetWinEventHook` with `EVENT_OBJECT_LOCATIONCHANGE` to monitor caret position
- Tracks which application window has focus
- Updates popup position based on caret location

### Keyboard Monitoring
- Low-level keyboard hook (`SetWindowsHookEx`) captures all keyboard input
- Buffers typed characters to build a prefix
- Special keys (Backspace, Space, Enter, Tab) reset the buffer

### Text Input
- Uses `SendInput` API with `KEYEVENTF_UNICODE` for reliable text insertion
- Thread input attachment ensures proper focus handling
- Backspace removes typed prefix before inserting full suggestion

### UI Layout
- Dynamic button layout: buttons arranged horizontally with wrapping
- Constrained to 600px max width
- Clamped to monitor work area to prevent off-screen positioning

## Architecture

```
windows/
├── src/
│   ├── Project2.cpp          # Main application code
│   ├── Project2.h            # Header definitions
│   └── framework.h           # Windows framework includes
├── Project2.vcxproj          # Visual Studio project file
└── README.md                 # This file
```

### Key Components

**CaretWinEventProc**: 
- Event callback for caret position changes
- Gets caret rectangle coordinates
- Triggers UI update

**LowLevelKeyboardProc**:
- Global keyboard hook callback
- Builds input buffer from typed characters
- Handles special keys (Backspace, Space, Enter)

**RebuildButtons**:
- Creates dynamic button UI
- Calculates layout based on content width
- Updates suggestions display

**SendTextToEditor**:
- Handles suggestion selection
- Removes typed prefix with Backspace
- Inserts full suggestion as Unicode text
- Manages thread input attachment

**GetSuggestions**:
- Prefix-based search in dictionary
- Case-insensitive matching
- Returns up to 16 matching words

## Configuration

### Layout Constants (in Project2.cpp)
```cpp
#define WINDOW_HEIGHT       50      // Popup height
#define WINDOW_MAX_WIDTH    600     // Max popup width
#define BTN_WIDTH           80      // Button width
#define BTN_HEIGHT          28      // Button height
#define BTN_MARGIN          5       // Margin between buttons
```

### Dictionary
The `g_dictionary` global contains 400+ words organized by categories:
- Common words
- Technology/Programming
- Database terms
- Machine Learning
- Networking
- And more...

To add custom words, edit `g_dictionary` in Project2.cpp.

## Troubleshooting

### Popup Doesn't Appear
- Ensure application window has focus
- Start typing in a text input field
- Check if Windows is blocking the keyboard hook

### Suggestions Not Matching
- Verify dictionary contains the words (edit g_dictionary)
- Check prefix matching logic in GetSuggestions()
- Suggestions use prefix matching (word must START with typed text)

### Text Not Inserting
- Verify target application supports text input
- Check keyboard hook is properly registered
- May need administrator privileges for some applications

### Application Crashes
- Enable exception handling in debugger
- Check Visual Studio output for error messages
- Verify oleacc.lib is linked properly

## Permissions

The application may require:
- **Administrator privileges**: For global keyboard hook on some systems
- **Accessibility permissions**: To access caret information (Windows 10+)
  - Settings → Privacy & Security → App permissions → Accessibility

## Performance

- Minimal CPU usage (event-driven)
- Low memory footprint (~10-20 MB)
- No background processes
- Cleans up resources on exit

## Known Limitations

- Works best with standard text input controls
- Some applications with custom UI may not support caret tracking
- Unicode input may have issues with certain keyboard layouts
- Maximum 16 suggestions displayed

## Future Enhancements

- Custom dictionary management
- Configurable UI styling
- Suggestion ranking/frequency
- Multi-language support
- Settings dialog
- System tray integration

## License

MIT License

## Support

For issues or questions, check the main project README at the root level.
