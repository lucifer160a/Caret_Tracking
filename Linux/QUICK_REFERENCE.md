# Quick Reference - Commands & Building

## Windows Version

### Building

**Visual Studio GUI (Recommended):**
```
1. Open: windows\Project2.vcxproj
2. Configure: Select Release|x64 from toolbar
3. Build: Ctrl+Shift+B or Build → Build Solution
```

**Command Line:**
```batch
cd windows
msbuild Project2.vcxproj /p:Configuration=Release /p:Platform=x64
```

**Build Script:**
```batch
cd windows
build.bat Release x64
```

### Running

**After Build:**
```batch
windows\bin\x64\Release\Project2.exe
```

**From Visual Studio:**
- Debug: F5
- Run: Ctrl+F5

## Linux/Ubuntu Version

### Setup (One-time)

**Install System Dependencies:**
```bash
sudo apt install libgtk-4-dev python3-gi python3-gi-cairo gir1.2-gtk-4.0 at-spi2-core
```

**Install Python Dependencies:**
```bash
pip3 install -r requirements.txt
```

### Running

```bash
python3 src/main.py
```

## Project Files

| File | Purpose |
|------|---------|
| `README.md` | Main documentation |
| `PLATFORM_GUIDE.md` | Quick platform comparison |
| `WINDOWS_INTEGRATION.md` | Windows integration details |
| `windows/README.md` | Windows-specific documentation |
| `windows/Project2.vcxproj` | Visual Studio project |
| `windows/build.bat` | Windows build script |
| `src/main.py` | Linux application entry |
| `requirements.txt` | Python dependencies |

## Testing

### Windows
1. Run `Project2.exe`
2. Open Notepad or VS Code
3. Start typing
4. Suggestions should appear below cursor
5. Click a suggestion to insert it

### Linux
1. Run `python3 src/main.py`
2. Click "Start Tracking"
3. Move cursor in any accessible app
4. Position and text context update in real-time
5. Click "Stop Tracking" to pause

## Customization

### Windows - Add Words to Dictionary

Edit `windows/src/Project2.cpp`:
```cpp
std::vector<std::wstring> g_dictionary = {
    L"the", L"be", L"to", /* your words here */
};
```

Then rebuild: `build.bat Release x64`

### Windows - Adjust UI

Edit constants in `windows/src/Project2.cpp`:
```cpp
#define WINDOW_HEIGHT       50      // Height of popup
#define WINDOW_MAX_WIDTH    600     // Max width
#define BTN_WIDTH           80      // Button width
#define BTN_HEIGHT          28      // Button height
```

### Linux - Logging Level

Edit `src/main.py`:
```python
level=logging.DEBUG  # Change from logging.INFO
```

## Common Issues

### Windows Build Error
```
Solution: 
1. Open windows\Project2.vcxproj
2. Right-click project → Properties
3. Verify: Windows SDK version and Toolset
4. Rebuild: Ctrl+Shift+B
```

### Linux Import Error
```
error: No module named 'gi'

Solution:
pip3 install PyGObject
sudo apt install python3-gi
```

### Keyboard Hook Not Working
```
Windows Solution:
1. Run Project2.exe as Administrator
2. Check Task Manager - process should be running
3. Try typing in Notepad to test
```

### No Caret Events
```
Linux Solution:
1. Ensure application supports AT-SPI 2
2. Check accessibility is enabled
3. Try with gedit: gedit /tmp/test.txt
4. Move cursor and watch for events
```

## Performance

| Metric | Windows | Linux |
|--------|---------|-------|
| Memory | 10-20 MB | 30-50 MB |
| CPU (idle) | <1% | <1% |
| Startup | ~100ms | ~500ms |
| Suggestion Match | <10ms | N/A |

## Deployment

### Windows (.exe)
- Standalone executable
- No dependencies needed
- Can be placed anywhere
- Run with: `Project2.exe`

### Linux (Python)
- Requires Python 3.8+
- Requires system packages
- Can be run directly: `python3 src/main.py`
- Or installed: `pip install -e .`

## Development Environment Setup

### Windows Development
1. Install Visual Studio Community 2019 or 2022
2. Include C++ workload
3. Install Windows SDK 10.0+
4. Open `windows/Project2.vcxproj`
5. Build and debug with F5

### Linux Development
1. Install Python 3.8+
2. Install system dependencies (see above)
3. `pip3 install -r requirements.txt`
4. Run with: `python3 src/main.py`
5. Edit source in any text editor

## Version Information

- **Windows**: C++17, Visual Studio 2019+
- **Linux**: Python 3.8+, GTK 4
- **Project**: Multi-platform caret tracking
- **Status**: Production ready

## Support Resources

- [Main README](README.md)
- [Platform Guide](PLATFORM_GUIDE.md)
- [Windows Details](windows/README.md)
- [Windows Integration](WINDOWS_INTEGRATION.md)

---

**Last Updated**: January 2026
**Status**: Both platforms fully integrated and ready to build
