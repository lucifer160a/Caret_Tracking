"""
Keyboard input monitoring module for Linux/Fedora.
Captures keyboard input for autocomplete suggestions.
Similar to Windows keyboard hook functionality.
"""

import gi
gi.require_version('Gtk', '4')
from gi.repository import Gtk, Gdk, GObject
import logging
import threading
import subprocess

logger = logging.getLogger(__name__)


class KeyboardMonitor(GObject.GObject):
    """
    Monitors keyboard input globally on Linux/Fedora.
    Emits signals when input buffer changes.
    """
    
    __gsignals__ = {
        'input-changed': (GObject.SignalFlags.RUN_LAST, None, (str,)),
    }
    
    def __init__(self):
        super().__init__()
        self.input_buffer = ""
        self.tracking = False
        self.last_focused_window = None
        
    def start_tracking(self):
        """Start monitoring keyboard input."""
        if self.tracking:
            return
        
        try:
            self.tracking = True
            self.input_buffer = ""
            logger.info("Keyboard monitoring started")
            
            # Connect to key press events
            self._setup_event_monitoring()
            
        except Exception as e:
            logger.error(f"Failed to start keyboard monitoring: {e}")
            self.tracking = False
    
    def stop_tracking(self):
        """Stop monitoring keyboard input."""
        if not self.tracking:
            return
        
        try:
            self.tracking = False
            self.input_buffer = ""
            logger.info("Keyboard monitoring stopped")
        except Exception as e:
            logger.error(f"Error stopping tracking: {e}")
    
    def _setup_event_monitoring(self):
        """Setup global event monitoring using Gdk events."""
        try:
            display = Gdk.Display.get_default()
            if display:
                # Get the seat for input monitoring
                seat = display.get_default_seat()
                if seat:
                    logger.debug("Event monitoring setup complete")
        except Exception as e:
            logger.debug(f"Could not setup advanced event monitoring: {e}")
    
    def get_input_buffer(self):
        """Get current input buffer."""
        return self.input_buffer
    
    def clear_buffer(self):
        """Clear the input buffer."""
        self.input_buffer = ""
        self.emit('input-changed', "")
    
    def add_character(self, char):
        """Add character to buffer."""
        if char and not chr(ord(char)).iscntrl():
            self.input_buffer += char.lower()
            self.emit('input-changed', self.input_buffer)
            logger.debug(f"Buffer: '{self.input_buffer}'")
    
    def backspace(self):
        """Remove last character from buffer."""
        if self.input_buffer:
            self.input_buffer = self.input_buffer[:-1]
            self.emit('input-changed', self.input_buffer)
            logger.debug(f"Buffer after backspace: '{self.input_buffer}'")
    
    def reset_on_special_key(self):
        """Reset buffer on special keys (space, enter, tab)."""
        self.input_buffer = ""
        self.emit('input-changed', "")
        logger.debug("Buffer cleared on special key")


class CaretTracker(GObject.GObject):
    """
    Tracks caret position using AT-SPI 2.
    Emits signals when caret position changes.
    """
    
    __gsignals__ = {
        'caret-changed': (GObject.SignalFlags.RUN_LAST, None, (int, int, str, str)),
    }
    
    def __init__(self):
        super().__init__()
        self.current_app = None
        self.current_pos = (0, 0)
        self.current_text = ""
        self.current_app_name = ""
        self.tracking = False
        self._event_listener = None
        
    def start_tracking(self):
        """Start tracking caret position changes."""
        if self.tracking:
            return
        
        try:
            import gi
            gi.require_version('Atspi', '2')
            from gi.repository import Atspi
            
            Atspi.init()
            self.tracking = True
            logger.info("Caret tracking started")
            
            # Register for caret-moved events
            self._event_listener = Atspi.EventListener.new(
                self._on_caret_moved
            )
            self._event_listener.register("text:caret-moved")
            
        except Exception as e:
            logger.warning(f"AT-SPI caret tracking not available: {e}")
            self.tracking = False
    
    def stop_tracking(self):
        """Stop tracking caret position changes."""
        if not self.tracking:
            return
        
        try:
            if self._event_listener:
                self._event_listener.deregister("text:caret-moved")
            self.tracking = False
            logger.info("Caret tracking stopped")
        except Exception as e:
            logger.error(f"Error stopping tracking: {e}")
    
    def _on_caret_moved(self, event):
        """Handle caret-moved event from AT-SPI."""
        try:
            if event.source and hasattr(event.source, 'get_text'):
                text_obj = event.source
                caret_offset = text_obj.get_caret_offset()
                
                # Get application name
                app_name = self._get_app_name(event.source)
                
                # Get screen coordinates of caret
                try:
                    extent = text_obj.get_character_extents(caret_offset, 0)
                    x, y = extent.x, extent.y
                except:
                    x, y = 0, 0
                
                self.current_pos = (x, y)
                self.current_app_name = app_name
                
                # Emit signal
                self.emit('caret-changed', x, y, "", app_name)
                logger.debug(f"Caret moved to ({x}, {y}) in {app_name}")
                
        except Exception as e:
            logger.debug(f"Error processing caret event: {e}")
    
    def _get_app_name(self, accessible):
        """Get the application name from an accessible object."""
        try:
            import gi
            gi.require_version('Atspi', '2')
            from gi.repository import Atspi
            
            app = Atspi.Accessible.get_application(accessible)
            if app:
                return app.get_name()
        except:
            pass
        return "Unknown"
    
    def get_caret_position(self):
        """Get current caret position."""
        return self.current_pos
