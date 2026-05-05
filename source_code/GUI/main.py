from PyQt5.QtWidgets import QApplication, QMainWindow, QTabWidget
import sys
from GUIElements import EditorTab, ViewerTab

class MainWindow(QMainWindow):
    """Glavni prozor aplikacije"""
    
    def __init__(self):
        super().__init__()
        self.setWindowTitle("LEGO Spike Simulator — Task Editor & Viewer")
        self.setGeometry(100, 100, 1400, 800)
        
        # Tabovi
        self.tabs = QTabWidget()
        
        self.editor_tab = EditorTab()
        self.viewer_tab = ViewerTab()
        
        # Connector: kada se task učita/spremi u editoru, ažuriraj viewer
        self.editor_tab.on_task_updated = self._on_editor_task_updated
        
        self.tabs.addTab(self.editor_tab, "📝 Task Editor")
        self.tabs.addTab(self.viewer_tab, "▶️ Simulation Viewer")
        
        self.setCentralWidget(self.tabs)
    
    def _on_editor_task_updated(self):
        """Ažuriraj viewer kada se task promijeni u editoru"""
        # Prosljeđi task podatke vieweru
        self.viewer_tab.task_data = self.editor_tab.task_data
        self.viewer_tab.scene.render_task_objects(
            self.editor_tab.task_data.objects,
            self.editor_tab.task_data.robot
        )


def main():
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()