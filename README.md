# Caret Tracking

A lightweight Python project for tracking caret (text cursor) movement and related UI interactions.

## Overview

This repository implements a caret (text cursor) tracking utility with a simple UI and helper utilities. It includes modules for tracking, a command-line entrypoint, and platform-specific build helpers for Windows.

## Features

- Track caret position and movement
- Simple UI integration
- Cross-platform helper scripts and Windows project files

## Installation

1. Create a Python virtual environment (recommended):

```bash
python -m venv .venv
.\.venv\Scripts\activate
```

2. Install dependencies:

```bash
pip install -r Linux/requirements.txt
```

## Usage

Run the main script for a quick demo:

```bash
python src\main.py
```

See `src/ui.py` for the UI entry points and `src\caret_tracker.py` for the caret-tracking logic.

## Repository structure

- `src/` - Core Python source files:
	- `caret_tracker.py` - Caret tracking implementation
	- `main.py` - CLI/entrypoint
	- `ui.py` - UI components
	- `utils.py` - Helper utilities
- `Linux/` - Linux packaging and integration notes
- `windows/`, `Windows/` - Visual Studio project files and build scripts for Windows

## Development

- Run the project locally using the Python commands above.
- For Windows-specific builds, see `windows/build.bat` and the Visual Studio project files under `Windows/`.

## Contributing

Contributions are welcome. Please open issues for bugs or feature requests, and submit pull requests for changes.

## License

Specify your project license here.

## Contact

For questions, open an issue or contact the repository owner.
