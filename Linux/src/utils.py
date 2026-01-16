"""
Utility functions for the caret tracking application.
"""

import subprocess
import logging

logger = logging.getLogger(__name__)


def get_active_window():
    """Get the currently active window using xdotool."""
    try:
        result = subprocess.run(
            ['xdotool', 'getactivewindow', 'getwindowname'],
            capture_output=True,
            text=True,
            timeout=1
        )
        if result.returncode == 0:
            return result.stdout.strip()
    except Exception as e:
        logger.debug(f"Could not get active window: {e}")
    return "Unknown"


def get_mouse_position():
    """Get current mouse position using xdotool."""
    try:
        result = subprocess.run(
            ['xdotool', 'getmouselocation'],
            capture_output=True,
            text=True,
            timeout=1
        )
        if result.returncode == 0:
            # Parse output like "x:100 y:200 screen:0"
            parts = result.stdout.strip().split()
            x = int(parts[0].split(':')[1])
            y = int(parts[1].split(':')[1])
            return (x, y)
    except Exception as e:
        logger.debug(f"Could not get mouse position: {e}")
    return (0, 0)


def is_accessible_app(app_name):
    """Check if an application supports accessibility."""
    # List of known accessible apps
    accessible_apps = [
        'gedit', 'gnome-terminal', 'evolution', 'firefox',
        'thunderbird', 'libreoffice', 'geany', 'mousepad'
    ]
    
    app_lower = app_name.lower()
    return any(accessible_app in app_lower for accessible_app in accessible_apps)


def format_position(x, y):
    """Format position as human-readable string."""
    return f"({x}, {y})"
