"""
Main entry point for the Caret Tracking Autocomplete application.
Fedora/Linux version with GUI text suggestions.
"""

import gi
gi.require_version('Gtk', '4')
from gi.repository import Gtk, GLib, Gdk
import logging
import sys
import os

# Add src to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from caret_tracker import KeyboardMonitor, CaretTracker
from ui import CaretTrackerWindow

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.StreamHandler(),
        logging.FileHandler('/tmp/caret-tracker-fedora.log')
    ]
)

logger = logging.getLogger(__name__)

# Dictionary of suggestions (same as Windows version)
DICTIONARY = [
    "the", "be", "to", "of", "and", "a", "in", "that", "have", "I",
    "it", "for", "not", "on", "with", "he", "as", "you", "do", "at",
    "this", "but", "his", "by", "from", "they", "we", "say", "her", "she",
    "or", "an", "will", "my", "one", "all", "would", "there", "their", "what",
    "so", "up", "out", "if", "about", "who", "get", "which", "go", "me",
    "when", "make", "can", "like", "time", "no", "just", "him", "know", "take",
    "people", "into", "year", "your", "good", "some", "could", "them", "see", "other",
    "than", "then", "now", "look", "only", "come", "its", "over", "think", "also",
    "back", "after", "use", "two", "how", "our", "work", "first", "well", "way",
    "even", "new", "want", "because", "any", "these", "give", "day", "most", "us",

    "house", "book", "car", "tree", "computer", "phone", "window", "door", "table", "chair",
    "water", "food", "music", "movie", "city", "country", "school", "student", "teacher", "friend",
    "family", "child", "mother", "father", "sister", "brother", "dog", "cat", "animal", "sun",
    "moon", "star", "sky", "earth", "sea", "river", "mountain", "road", "street", "shop",
    "market", "store", "price", "money", "bank", "office", "company", "business", "job", "work",
    "project", "program", "code", "developer", "engineer", "design", "idea", "plan", "system", "engine",
    "network", "signal", "data", "file", "folder", "image", "picture", "sound", "voice", "language",
    "word", "sentence", "paragraph", "article", "report", "research", "science", "history", "nature", "art",
    "culture", "travel", "holiday", "vacation", "hotel", "airport", "ticket", "journey", "adventure", "map",
    "guide", "story", "author", "reader", "page", "index", "chapter", "title", "subject", "topic",
    
    "question", "answer", "help", "support", "service", "product", "feature", "update", "release", "version",
    "install", "setup", "config", "option", "preference", "setting", "control", "button", "menu", "dialog",
    "search", "find", "replace", "paste", "copy", "cut", "undo", "redo", "save", "load",
    "open", "close", "start", "stop", "run", "execute", "build", "compile", "link", "deploy",
    "test", "debug", "trace", "log", "error", "exception", "crash", "fix", "patch",
    "user", "account", "profile", "login", "logout", "password", "token", "session", "permission", "access",
    "security", "privacy", "encrypt", "decrypt", "key", "certificate", "firewall", "virus", "malware", "backup",
    
    "database", "sql", "table", "query", "select", "insert", "update", "delete", "join", "index",
    "cluster", "primary", "foreign", "unique", "constraint", "trigger", "procedure", "function", "view", "schema",
    "transaction", "commit", "rollback", "lock", "isolation", "consistency", "availability", "partition", "replication",
    
    "python", "java", "javascript", "csharp", "cpp", "rust", "golang", "ruby", "php", "swift",
    "kotlin", "dart", "scala", "groovy", "clojure", "haskell", "erlang", "elixir", "perl", "lua",
    "react", "angular", "vue", "svelte", "ember", "backbone", "meteor", "express", "django", "flask",
    "fastapi", "spring", "hibernate", "mybatis", "sequelize", "typeorm", "sqlalchemy", "entity", "model", "controller",
    
    "machine", "learning", "neural", "network", "deep", "tensor", "layer", "activation", "loss", "optimizer",
    "gradient", "descent", "backprop", "forward", "backward", "epoch", "batch", "accuracy", "precision", "recall",
    "classification", "regression", "clustering", "prediction", "training", "testing", "validation", "cross", "fold",
    
    "docker", "kubernetes", "container", "pod", "service", "deployment", "replica", "namespace", "label", "selector",
    "ingress", "egress", "volume", "persistent", "claim", "storage", "network", "policy", "resource", "limit",
    "request", "probe", "liveness", "readiness", "startup", "health", "check", "monitoring", "alert", "dashboard",
    
    "git", "commit", "branch", "merge", "rebase", "pull", "push", "clone", "fork", "issue",
    "pull", "request", "review", "approve", "reject", "conflict", "resolve", "stash", "tag", "release",
    "version", "semantic", "major", "minor", "patch", "prerelease", "metadata", "changelog", "license",
]


class CaretTrackerApplication(Gtk.Application):
    """Main application class."""
    
    def __init__(self):
        super().__init__(application_id='org.fedora.CaretTrackerAutocomplete')
        self.keyboard_monitor = None
        self.caret_tracker = None
        self.window = None
        
        # Connect signals
        self.connect('activate', self.on_activate)
        self.connect('shutdown', self.on_shutdown)
        
        logger.info("Caret Tracker Autocomplete Application initialized")
    
    def on_activate(self, app):
        """Handle application activation."""
        if self.window:
            self.window.present()
            return
        
        # Create keyboard monitor
        self.keyboard_monitor = KeyboardMonitor()
        self.keyboard_monitor.start_tracking()
        
        # Create caret tracker
        self.caret_tracker = CaretTracker()
        self.caret_tracker.start_tracking()
        
        # Create main window (floating popup)
        self.window = CaretTrackerWindow(self)
        self.window.set_keyboard_monitor(self.keyboard_monitor)
        self.window.set_caret_tracker(self.caret_tracker)
        self.window.set_dictionary(DICTIONARY)
        
        # Setup keyboard event monitoring
        self._setup_keyboard_events()
        
        logger.info("Application activated")
    
    def _setup_keyboard_events(self):
        """Setup keyboard event monitoring via GDK."""
        try:
            # Create an event controller to capture key events
            key_controller = Gtk.EventControllerKey.new()
            key_controller.connect('key-pressed', self._on_key_pressed)
            key_controller.connect('key-released', self._on_key_released)
            
            if self.window:
                self.window.add_controller(key_controller)
            
            logger.info("Keyboard event monitoring setup complete")
        except Exception as e:
            logger.warning(f"Could not setup advanced keyboard monitoring: {e}")
    
    def _on_key_pressed(self, controller, keyval, keycode, state):
        """Handle key press events."""
        try:
            if not self.keyboard_monitor:
                return False
            
            # Handle special keys
            if keyval == Gdk.KEY_BackSpace:
                self.keyboard_monitor.backspace()
                return True
            elif keyval in (Gdk.KEY_space, Gdk.KEY_Return, Gdk.KEY_Tab):
                self.keyboard_monitor.reset_on_special_key()
                return False
            
            # Handle regular characters
            try:
                char = chr(keyval)
                if char.isalnum() or char.isspace():
                    self.keyboard_monitor.add_character(char)
                    return True
            except:
                pass
            
            return False
        except Exception as e:
            logger.debug(f"Error in key press handler: {e}")
            return False
    
    def _on_key_released(self, controller, keyval, keycode, state):
        """Handle key release events."""
        return False
    
    def on_shutdown(self, app):
        """Handle application shutdown."""
        if self.keyboard_monitor:
            self.keyboard_monitor.stop_tracking()
        if self.caret_tracker:
            self.caret_tracker.stop_tracking()
        logger.info("Application shutdown")


def main():
    """Application entry point."""
    try:
        app = CaretTrackerApplication()
        exit_status = app.run(sys.argv)
        sys.exit(exit_status)
    except Exception as e:
        logger.error(f"Failed to start application: {e}", exc_info=True)
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == '__main__':
    main()
