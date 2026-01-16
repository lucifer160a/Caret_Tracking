"""
GTK 4 UI components for the autocomplete suggestion popup.
Similar to Windows version with suggestion buttons.
"""

import gi
gi.require_version('Gtk', '4')
from gi.repository import Gtk, Gdk, GObject
import logging

logger = logging.getLogger(__name__)

# Constants for layout
POPUP_HEIGHT = 50
POPUP_MAX_WIDTH = 600
BTN_WIDTH = 80
BTN_HEIGHT = 28
BTN_MARGIN = 5


class SuggestionButton(Gtk.Button):
    """Custom button for suggestions."""
    
    def __init__(self, text):
        super().__init__(label=text)
        self.set_size_request(BTN_WIDTH, BTN_HEIGHT)


class CaretTrackerWindow(Gtk.ApplicationWindow):
    """
    Floating popup window showing autocomplete suggestions.
    Mimics Windows version UI and behavior.
    """
    
    def __init__(self, application):
        super().__init__(
            type=Gtk.WindowType.POPUP,
            application=application,
            decorated=False
        )
        
        self.set_default_size(300, POPUP_HEIGHT)
        self.set_keep_above(True)
        self.set_skip_taskbar_hint(True)
        self.set_skip_pager_hint(True)
        
        # Main container
        self.main_box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=BTN_MARGIN)
        self.main_box.set_margin_top(BTN_MARGIN)
        self.main_box.set_margin_bottom(BTN_MARGIN)
        self.main_box.set_margin_start(BTN_MARGIN)
        self.main_box.set_margin_end(BTN_MARGIN)
        
        # Scrollable container for suggestions
        scrolled = Gtk.ScrolledWindow()
        scrolled.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.NEVER)
        scrolled.set_child(self.main_box)
        
        self.set_child(scrolled)
        
        # Styling
        css_provider = Gtk.CssProvider()
        css_provider.load_from_data(b"""
            button {
                padding: 4px 8px;
                min-width: 70px;
                font-size: 10px;
            }
            button:hover {
                background-color: #e8e8e8;
            }
        """)
        style_context = self.get_style_context()
        style_context.add_provider(css_provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION)
        
        self.buttons = []
        self.button_map = {}
        self.content_width = 0
        self.caret_tracker = None
        self.keyboard_monitor = None
        self.dictionary = []
        
        # Initially hidden
        self.set_visible(False)
    
    def set_keyboard_monitor(self, monitor):
        """Set the keyboard monitor object."""
        self.keyboard_monitor = monitor
        if monitor:
            monitor.connect('input-changed', self.on_input_changed)
    
    def set_caret_tracker(self, tracker):
        """Set the caret tracker object."""
        self.caret_tracker = tracker
        if tracker:
            tracker.connect('caret-changed', self.on_caret_changed)
    
    def set_dictionary(self, dictionary):
        """Set the dictionary for suggestions."""
        self.dictionary = dictionary
    
    def on_caret_changed(self, tracker, x, y, text, app_name):
        """Handle caret position change."""
        if x == 0 and y == 0:
            return
        
        # Position window near caret
        self.position_near_caret(x, y)
    
    def on_input_changed(self, monitor, prefix):
        """Handle input buffer change."""
        if not prefix:
            self.set_visible(False)
            self.clear_suggestions()
            return
        
        # Get suggestions and update UI
        suggestions = self.get_suggestions(prefix)
        self.update_suggestions(suggestions)
        
        if suggestions:
            self.set_visible(True)
        else:
            self.set_visible(False)
    
    def update_suggestions(self, words):
        """Update suggestion buttons."""
        # Clear old buttons
        for btn in self.buttons:
            if btn.get_parent():
                self.main_box.remove(btn)
        self.buttons.clear()
        self.button_map.clear()
        
        if not words:
            self.content_width = 0
            return
        
        x = BTN_MARGIN
        
        # Create new buttons
        for i, word in enumerate(words):
            btn = SuggestionButton(word)
            btn.connect('clicked', self.on_suggestion_clicked, word)
            
            self.main_box.append(btn)
            self.buttons.append(btn)
            self.button_map[id(btn)] = word
            
            x += BTN_WIDTH + BTN_MARGIN
        
        self.content_width = x
        self.queue_draw()
    
    def clear_suggestions(self):
        """Clear all suggestions."""
        for btn in self.buttons:
            if btn.get_parent():
                self.main_box.remove(btn)
        self.buttons.clear()
        self.button_map.clear()
    
    def on_suggestion_clicked(self, button, word):
        """Handle suggestion button click."""
        if self.keyboard_monitor:
            self.keyboard_monitor.reset_on_special_key()
        self.set_visible(False)
    
    def position_near_caret(self, x, y):
        """Position window near the caret."""
        # Offset below the caret
        popup_x = x
        popup_y = y + 30
        
        # Clamp to screen
        popup_x, popup_y = self.clamp_to_screen(popup_x, popup_y)
        
        width = min(self.content_width, POPUP_MAX_WIDTH)
        self.set_size_request(width, POPUP_HEIGHT)
        self.move(popup_x, popup_y)
    
    def clamp_to_screen(self, x, y):
        """Clamp window position to screen."""
        display = Gdk.Display.get_default()
        if display:
            monitor = display.get_monitor_at_point(x, y)
            if monitor:
                geometry = monitor.get_geometry()
                
                if x < geometry.x:
                    x = geometry.x
                if y < geometry.y:
                    y = geometry.y
                if x + POPUP_MAX_WIDTH > geometry.x + geometry.width:
                    x = geometry.x + geometry.width - POPUP_MAX_WIDTH
                if y + POPUP_HEIGHT > geometry.y + geometry.height:
                    y = geometry.y + geometry.height - POPUP_HEIGHT
                
                return x, y
        return x, y
    
    def get_suggestions(self, prefix):
        """Get suggestions for prefix from dictionary."""
        if not prefix or not self.dictionary:
            return []
        
        results = []
        prefix_lower = prefix.lower()
        
        for word in self.dictionary:
            word_lower = word.lower()
            # Prefix matching
            if word_lower.startswith(prefix_lower):
                results.append(word)
                if len(results) >= 16:
                    break
        
        return results
    
    def set_caret_tracker(self, tracker):
        """Set the caret tracker object."""
        self.caret_tracker = tracker
        if tracker:
            tracker.connect('caret-changed', self.on_caret_changed)
    
    def on_caret_changed(self, tracker, x, y, text, app_name):
        """Handle caret position change signal."""
        self.x_label.set_label(f"X: {x} pixels")
        self.y_label.set_label(f"Y: {y} pixels")
        self.app_label.set_label(app_name)
        
        text_display = text if text else "(empty)"
        self.text_label.set_label(f"'{text_display}'")
    
    def on_start_clicked(self, button):
        """Handle start tracking button click."""
        if self.caret_tracker:
            self.caret_tracker.start_tracking()
            self.start_button.set_sensitive(False)
            self.stop_button.set_sensitive(True)
            self.status_label.set_label("Status: Tracking...")
            logger.info("Tracking started from UI")
    
    def on_stop_clicked(self, button):
        """Handle stop tracking button click."""
        if self.caret_tracker:
            self.caret_tracker.stop_tracking()
            self.start_button.set_sensitive(True)
            self.stop_button.set_sensitive(False)
            self.status_label.set_label("Status: Stopped")
            logger.info("Tracking stopped from UI")
