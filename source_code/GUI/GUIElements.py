import math
from typing import List, Dict, Optional, Tuple

from PyQt5.QtCore import Qt, QTimer
from PyQt5.QtGui import QCursor
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, QTabWidget,
    QPushButton, QSlider, QLabel, QListWidget, QListWidgetItem, QComboBox,
    QSpinBox, QDoubleSpinBox, QLineEdit, QGroupBox, QGraphicsScene, QGraphicsView,
    QFormLayout, QInputDialog, QMessageBox, QFileDialog
)
from GUI.SceneObjects import SceneObject, WallObject, CheckpointObject, ObstacleObject, ZoneObject, RobotObject
from GUI.DataManagement import TaskData, SimulationData
from GUI.GraphicsItems import TaskScene, TaskGraphicsView
from GUI.RobotState import RobotState

class ViewerTab(QWidget):
    """Tab za prikaz simulacije"""
    
    def __init__(self, parent=None):
        super().__init__(parent)
        
        self.task_data = TaskData()
        self.sim_data = SimulationData()
        
        # Timeline
        self.time_label = QLabel("Vrijeme: 0.00s / 0.00s")
        self.slider = QSlider(Qt.Horizontal)
        self.slider.setMinimum(0)
        self.slider.setMaximum(100)
        self.slider.sliderMoved.connect(self.on_slider_moved)
        
        # Gumbi za učitavanje
        self.load_task_button = QPushButton("📂 Učitaj task")
        self.load_sim_button = QPushButton("📂 Učitaj simulaciju")
        self.load_task_button.clicked.connect(self.on_load_task)
        self.load_sim_button.clicked.connect(self.on_load_simulation)
        
        # Gumbi
        self.play_button = QPushButton("▶ Play")
        self.pause_button = QPushButton("⏸ Pause")
        self.stop_button = QPushButton("■ Stop")
        
        self.play_button.clicked.connect(self.on_play)
        self.pause_button.clicked.connect(self.on_pause)
        self.stop_button.clicked.connect(self.on_stop)
        
        # Speed
        speed_label = QLabel("Brzina:")
        self.speed_spinbox = QDoubleSpinBox()
        self.speed_spinbox.setValue(1.0)
        self.speed_spinbox.setMinimum(0.1)
        self.speed_spinbox.setMaximum(5.0)
        self.speed_spinbox.setSingleStep(0.1)
        
        # Score
        self.score_label = QLabel("Bodovi: 0 / ?")
        
        # Canvas
        self.scene = TaskScene()
        self.view = QGraphicsView(self.scene)
        
        # Layout
        controls_layout = QHBoxLayout()
        controls_layout.addWidget(self.load_task_button)
        controls_layout.addWidget(self.load_sim_button)
        controls_layout.addStretch()
        controls_layout.addWidget(self.play_button)
        controls_layout.addWidget(self.pause_button)
        controls_layout.addWidget(self.stop_button)
        controls_layout.addStretch()
        controls_layout.addWidget(speed_label)
        controls_layout.addWidget(self.speed_spinbox)
        controls_layout.addStretch()
        controls_layout.addWidget(self.score_label)
        
        timeline_layout = QHBoxLayout()
        timeline_layout.addWidget(self.time_label)
        timeline_layout.addWidget(self.slider)
        
        main_layout = QVBoxLayout()
        main_layout.addWidget(self.view, 1)
        main_layout.addLayout(timeline_layout)
        main_layout.addLayout(controls_layout)
        
        self.setLayout(main_layout)
        
        # Timer za animaciju
        self.animation_timer = QTimer()
        self.animation_timer.timeout.connect(self.on_animation_tick)
        self.current_time = 0.0
        self.is_playing = False
        self.max_time = 0.0
    
    def on_load_task(self):
        """Učitaj task JSON"""
        path, _ = QFileDialog.getOpenFileName(self, "Učitaj task", "", "JSON files (*.json)")
        if path:
            try:
                self.task_data.load_from_file(path)
                self.scene.load_task(self.task_data.objects, self.task_data.robot)
                
                QMessageBox.information(self, "Uspješno", f"Task učitan: {path}")
            except Exception as e:
                QMessageBox.critical(self, "Greška", f"Greška pri učitavanju task-a: {e}")
    
    def on_load_simulation(self):
        """Učitaj simulacijske rezultate"""
        path, _ = QFileDialog.getOpenFileName(self, "Učitaj simulaciju", "", "JSON files (*.json)")
        if path:
            try:
                self.sim_data.load_from_file(path)
                
                # Postavi maksimalno vrijeme
                if self.sim_data.robot_states:
                    self.max_time = self.sim_data.robot_states[-1].t
                    self.slider.setMaximum(int(self.max_time * 1000))
                    self.time_label.setText(f"Vrijeme: 0.00s / {self.max_time:.2f}s")
                    
                    # Dodaj robota na scenu
                    start_x, start_y, start_angle = 0, 0, 0
                    
                    # Ako postoji robot u task-u, koristi njegovu početnu poziciju
                    if self.task_data.robot:
                        start_x = self.task_data.robot.x
                        start_y = self.task_data.robot.y
                        start_angle = self.task_data.robot.angle
                    else:
                        # Inače koristi prvi state iz simulacije
                        if self.sim_data.robot_states:
                            start_x, start_y, start_angle = self.sim_data.get_robot_state_at_time(0)
                    
                    # Obriši statički robot prije dodavanja animiranog
                    self.scene.remove_static_robot()
                    self.scene.add_robot(start_x, start_y, start_angle)
                    
                    QMessageBox.information(self, "Uspješno", f"Simulacija učitana: {path}\nStanja: {len(self.sim_data.robot_states)}")
                else:
                    QMessageBox.warning(self, "Greška", "Nema robot_states u JSON-u!")
            except Exception as e:
                QMessageBox.critical(self, "Greška", f"Greška pri učitavanju simulacije: {e}")
    
    def load_task(self, task_path: str):
        """Učitaj task"""
        self.task_data.load_from_file(task_path)
        self.scene.load_task(self.task_data.objects, self.task_data.robot)
    
    def load_simulation(self, sim_path: str):
        """Učitaj rezultate simulacije"""
        self.sim_data.load_from_file(sim_path)
        
        # Postavi maksimalno vrijeme
        if self.sim_data.robot_states:
            max_time = self.sim_data.robot_states[-1].t
            self.slider.setMaximum(int(max_time * 1000))  # ms
            
            start_x, start_y, start_angle = 0, 0, 0
            if self.task_data.robot:
                start_x = self.task_data.robot.x
                start_y = self.task_data.robot.y
                start_angle = self.task_data.robot.angle
            self.scene.add_robot(start_x, start_y, start_angle)
    
    def _transform_sim_to_world(self, sim_x: float, sim_y: float, sim_angle: float):
        """Transformiraj simulacijske koordinate (od 0,0,0) u world koordinate.
        
        GUI konvencija: 0° = robot gleda gore (-y na ekranu)
        Sim konvencija: 0° = robot gleda desno (+x os)
        Offset -90° se uvijek primjenjuje za pretvorbu kutova.
        """
        ox = self.task_data.robot.x if self.task_data.robot else 0.0
        oy = self.task_data.robot.y if self.task_data.robot else 0.0
        oa = self.task_data.robot.angle if self.task_data.robot else 0.0
        
        # GUI 0° = gore, Sim 0° = desno, razlika je 90°
        angle_rad = math.radians(oa - 90)
        cos_a = math.cos(angle_rad)
        sin_a = math.sin(angle_rad)
        
        world_x = ox + sim_x * cos_a - sim_y * sin_a
        world_y = oy + sim_x * sin_a + sim_y * cos_a
        world_angle = sim_angle + oa
        
        return world_x, world_y, world_angle
    
    def on_slider_moved(self, value: int):
        """Korisnik pomakne slider"""
        self.current_time = value / 1000.0  # Vrati u sekunde
        self.time_label.setText(f"Vrijeme: {self.current_time:.2f}s")
        
        result = self.sim_data.get_robot_state_at_time(self.current_time)
        if result:
            x, y, angle = result
            x, y, angle = self._transform_sim_to_world(x, y, angle)
            self.scene.update_robot(x, y, angle)
        elif self.task_data.robot:
            self.scene.update_robot(self.task_data.robot.x, self.task_data.robot.y, self.task_data.robot.angle)
    
    def on_play(self):
        """Pokreni animaciju"""
        self.is_playing = True
        self.animation_timer.start(16)  # ~60 FPS
    
    def on_pause(self):
        """Pauziraj"""
        self.is_playing = False
        self.animation_timer.stop()
    
    def on_stop(self):
        """Zaustavi"""
        self.is_playing = False
        self.animation_timer.stop()
        self.current_time = 0.0
        self.slider.setValue(0)
        self.on_slider_moved(0)
    
    def on_animation_tick(self):
        """Animacija se ažurira svaki frame"""
        if not self.is_playing:
            return
        
        # Pomakni vrijeme prema brzini
        speed = self.speed_spinbox.value()
        self.current_time += (16 / 1000.0) * speed  # 16ms frame time
        
        # Provjeri granice
        max_time = self.sim_data.robot_states[-1].t if self.sim_data.robot_states else 0
        if self.current_time >= max_time:
            self.current_time = max_time
            self.is_playing = False
            self.animation_timer.stop()
        
        # Ažuriraj slider i robota
        self.slider.blockSignals(True)
        self.slider.setValue(int(self.current_time * 1000))
        self.slider.blockSignals(False)
        
        self.on_slider_moved(int(self.current_time * 1000))


class EditorTab(QWidget):
    """Tab za editor zadataka"""
    
    def __init__(self, parent=None):
        super().__init__(parent)
        
        self.task_data = TaskData()
        self.current_mode = "idle"  # "idle", "adding_wall", "adding_checkpoint", "adding_obstacle", "adding_zone"
        self.selection_points: List[Tuple[float, float]] = []
        self.on_task_updated = None  # Callback za kada se task promijeni
        
        # Canvas
        self.scene = TaskScene()
        self.view = TaskGraphicsView(self.scene)
        self.view.object_hovered.connect(self._on_object_hovered)
        self.view.mouse_moved.connect(self._on_mouse_moved)
        self.view.mouseReleaseEvent = self._on_canvas_click  # Postavi custom event handler
        
        # Toolbar
        self.add_wall_button = QPushButton("➕ Dodaj zid")
        self.add_checkpoint_button = QPushButton("➕ Dodaj checkpoint")
        self.add_obstacle_button = QPushButton("➕ Dodaj prepreku")
        self.add_zone_button = QPushButton("➕ Dodaj ciljnu zonu")
        self.add_robot_button = QPushButton("🤖 Postavi robota")
        self.save_button = QPushButton("💾 Spremi task")
        self.load_button = QPushButton("📂 Učitaj task")
        
        self.add_wall_button.clicked.connect(lambda: self.set_mode("adding_wall"))
        self.add_checkpoint_button.clicked.connect(lambda: self.set_mode("adding_checkpoint"))
        self.add_obstacle_button.clicked.connect(lambda: self.set_mode("adding_obstacle"))
        self.add_zone_button.clicked.connect(lambda: self.set_mode("adding_zone"))
        self.add_robot_button.clicked.connect(lambda: self.set_mode("adding_robot"))
        self.save_button.clicked.connect(self.on_save_task)
        self.load_button.clicked.connect(self.on_load_task)
        
        toolbar_layout = QHBoxLayout()
        toolbar_layout.addWidget(self.add_wall_button)
        toolbar_layout.addWidget(self.add_checkpoint_button)
        toolbar_layout.addWidget(self.add_obstacle_button)
        toolbar_layout.addWidget(self.add_zone_button)
        toolbar_layout.addWidget(self.add_robot_button)
        toolbar_layout.addStretch()
        toolbar_layout.addWidget(self.load_button)
        toolbar_layout.addWidget(self.save_button)
        
        # Object list
        self.object_list = QListWidget()
        self.object_list.itemSelectionChanged.connect(self.on_object_selected)
        
        # Properties panel
        self.properties_group = QGroupBox("Svojstva objekta")
        self.properties_layout = QFormLayout()
        self.properties_group.setLayout(self.properties_layout)
        
        # Layout
        left_layout = QVBoxLayout()
        left_layout.addLayout(toolbar_layout)
        left_layout.addWidget(self.view, 1)
        
        right_layout = QVBoxLayout()
        right_layout.addWidget(QLabel("Objekti na sceni:"))
        right_layout.addWidget(self.object_list)
        right_layout.addWidget(self.properties_group)
        right_layout.addStretch()
        
        main_layout = QHBoxLayout()
        main_layout.addLayout(left_layout, 3)
        main_layout.addLayout(right_layout, 1)
        
        self.setLayout(main_layout)
    
    def keyPressEvent(self, event):
        """Detektuj Delete tipku"""
        if event.key() == Qt.Key_Delete:
            self.delete_selected_object()
        else:
            super().keyPressEvent(event)
    
    def delete_selected_object(self):
        """Obriši odabrani objekt"""
        row = self.object_list.currentRow()
        if row < 0:
            QMessageBox.warning(self, "Greška", "Nema odabranog objekta za brisanje!")
            return
        
        # Ako je redni broj 0 i postoji robot, obriši robota
        if row == 0 and self.task_data.robot:
            reply = QMessageBox.question(
                self, 
                "Potvrdi brisanje", 
                f"Obriši '{self.task_data.robot.metadata.get('name', self.task_data.robot.id)}'?",
                QMessageBox.Yes | QMessageBox.No,
                QMessageBox.No
            )
            if reply == QMessageBox.Yes:
                self.task_data.robot = None
                self.scene.render_task_objects(self.task_data.objects, self.task_data.robot)
                self.refresh_object_list()
                if self.on_task_updated:
                    self.on_task_updated()
                QMessageBox.information(self, "Obrisano", "Robot je obrisan.")
        else:
            # Inače obriši objekt iz liste
            obj_index = row - (1 if self.task_data.robot else 0)
            if obj_index < 0 or obj_index >= len(self.task_data.objects):
                return
            obj = self.task_data.objects[obj_index]
            reply = QMessageBox.question(
                self, 
                "Potvrdi brisanje", 
                f"Obriši '{obj.metadata.get('name', obj.id)}'?",
                QMessageBox.Yes | QMessageBox.No,
                QMessageBox.No
            )
            if reply == QMessageBox.Yes:
                self.task_data.remove_object(obj.id)
                self.scene.render_task_objects(self.task_data.objects, self.task_data.robot)
                self.refresh_object_list()
                if self.on_task_updated:
                    self.on_task_updated()
                QMessageBox.information(self, "Obrisano", f"Objekt '{obj.metadata.get('name', obj.id)}' je obrisan.")
        self.setFocus()  # Vrati focus na widget nakon dijaloga
    
    def set_mode(self, mode: str):
        """Postavi mod i promijeni cursor"""
        self.current_mode = mode
        self.selection_points.clear()
        self.scene.clear_preview()  # Obriši stari preview
        
        mode_names = {
            "adding_wall": "👉 Klikni dva puta: početna i završna točka zida",
            "adding_checkpoint": "👉 Klikni dva puta: početna i završna točka checkpointa",
            "adding_obstacle": "👉 Klikni dva puta: centar i kut prepreke", 
            "adding_zone": "👉 Klikni dva puta: centar i radius ciljne zone",
            "adding_robot": "👉 Klikni: pozicija robota, zatim drag za orijentaciju"
        }
        
        print(mode_names.get(mode, ""))
        
        # Promijeni cursor
        if mode == "idle":
            self.view.setCursor(QCursor(Qt.ArrowCursor))
        else:
            self.view.setCursor(QCursor(Qt.CrossCursor))  # Crosshair cursor za dodavanje
    
    def _on_canvas_click(self, event):
        """Korisnik klikne na canvas"""
        # Konvertiraj pixel koordimate u scene koordinate
        scene_pos = self.view.mapToScene(event.pos())
        x_scene = scene_pos.x()
        y_scene = scene_pos.y()
        x_cm = x_scene / 8.0
        y_cm = y_scene / 8.0
        
        if self.current_mode == "idle":
            # U idle modu, detektuj klik na objekt
            clicked_obj = self.scene.get_object_at_position(x_scene, y_scene)
            if clicked_obj:
                # Provjeri je li robot kliknut
                if isinstance(clicked_obj, RobotObject) and self.task_data.robot:
                    self.object_list.setCurrentRow(0)  # Robot je prvi u listi
                else:
                    # Nađi red u object_list i odaberi ga
                    for i, obj in enumerate(self.task_data.objects):
                        if obj.id == clicked_obj.id:
                            # Offset zbog robota u listi
                            row = i + (1 if self.task_data.robot else 0)
                            self.object_list.setCurrentRow(row)
                            break
            return
        
        # U adding modu
        if self.current_mode == "adding_robot":
            # Za robota: jedan klik postavlja poziciju, zatim dialog za kut
            angle, ok = QInputDialog.getDouble(
                self, "Orijentacija robota", 
                "Unesi početnu orijentaciju (0-360°):",
                0.0, 0.0, 360.0, 2
            )
            if ok:
                robot = RobotObject(
                    "robot_0", x_cm, y_cm, angle,
                    {"name": "Robot", "description": ""}
                )
                self.task_data.robot = robot
                self.scene.render_task_objects(self.task_data.objects, self.task_data.robot)
                self.refresh_object_list()
                self.current_mode = "idle"
                self.view.setCursor(QCursor(Qt.ArrowCursor))
                if self.on_task_updated:
                    self.on_task_updated()
        else:
            # Za druge objekte
            self.selection_points.append((x_cm, y_cm))
            print(f"Točka {len(self.selection_points)}: ({x_cm:.1f}, {y_cm:.1f})")
            
            # Ako imamo dovoljno točaka, prihvati unos
            if len(self.selection_points) == 2:
                self.on_object_confirmed()
    
    def _on_object_hovered(self, obj: Optional[SceneObject]):
        """Objekt je hoverman ili više nije"""
        if obj and self.current_mode == "idle":
            # Postavi hand cursor za clickable objekt
            self.view.setCursor(QCursor(Qt.PointingHandCursor))
        else:
            # Vrati na crosshair ili strelicu ovisno o modu
            if self.current_mode == "idle":
                self.view.setCursor(QCursor(Qt.ArrowCursor))
            else:
                self.view.setCursor(QCursor(Qt.CrossCursor))
    
    def _on_mouse_moved(self, x_scene: float, y_scene: float):
        """Mouse se pomakao — ažuriraj preview tijekom dodavanja"""
        x_cm = x_scene / 8.0
        y_cm = y_scene / 8.0
        
        if self.current_mode == "idle":
            # NisU u modu dodavanja — nema previewa
            self.scene.clear_preview()
            return
        
        if len(self.selection_points) == 0:
            # Nema početne točke — nema previewa
            self.scene.clear_preview()
            return
        
        if len(self.selection_points) == 1:
            # Imamo prvu točku — prikaži je i živi preview
            x1, y1 = self.selection_points[0]
            self.scene.show_first_point(x1, y1)
            
            # Prikaži ghost objekt
            mode_map = {
                "adding_wall": "wall",
                "adding_checkpoint": "checkpoint",
                "adding_obstacle": "obstacle",
                "adding_zone": "zone"
            }
            obj_type = mode_map.get(self.current_mode, "wall")
            self.scene.update_preview(x1, y1, x_cm, y_cm, obj_type)
    
    def on_object_confirmed(self):
        """Korisnik je završio unos objekta"""
        x1, y1 = self.selection_points[0]
        x2, y2 = self.selection_points[1]
        
        if self.current_mode == "adding_wall":
            obj = WallObject(
                self.task_data.get_next_id("wall"),
                x1, y1, x2, y2,
                {"name": "Zid", "description": ""}
            )
            self.task_data.add_object(obj)
        
        elif self.current_mode == "adding_checkpoint":
            obj = CheckpointObject(
                self.task_data.get_next_id("checkpoint"),
                x1, y1, x2, y2,
                {"name": "Checkpoint", "description": ""},
                points=5
            )
            self.task_data.add_object(obj)
        
        elif self.current_mode == "adding_obstacle":
            width = abs(x2 - x1)
            length = abs(y2 - y1)
            obj = ObstacleObject(
                self.task_data.get_next_id("obstacle"),
                (x1 + x2) / 2, (y1 + y2) / 2,
                width, length, 0,
                {"name": "Prepreka", "description": ""}
            )
            self.task_data.add_object(obj)
        
        elif self.current_mode == "adding_zone":
            radius = math.sqrt((x2 - x1) ** 2 + (y2 - y1) ** 2)
            obj = ZoneObject(
                self.task_data.get_next_id("zone"),
                x1, y1, radius, 0, 10,
                {"name": "Ciljna zona", "description": ""},
                points=20
            )
            self.task_data.add_object(obj)
        
        # Osvježi prikaz
        self.scene.clear_preview()  # Obriši preview
        self.scene.render_task_objects(self.task_data.objects, self.task_data.robot)
        self.refresh_object_list()
        self.selection_points.clear()
        self.current_mode = "idle"
        self.view.setCursor(QCursor(Qt.ArrowCursor))  # Vrati na normalnu strelicu
        if self.on_task_updated:
            self.on_task_updated()
    
    def refresh_object_list(self):
        """Osvježi listu objekata"""
        self.object_list.clear()
        if self.task_data.robot:
            item_text = f"{self.task_data.robot.id}  ({self.task_data.robot.type}) — {self.task_data.robot.angle}°"
            self.object_list.addItem(item_text)
        for obj in self.task_data.objects:
            item_text = f"{obj.id}  ({obj.type}) — {obj.points}pt"
            self.object_list.addItem(item_text)
    
    def on_object_selected(self):
        """Korisnik odabere objekt iz liste"""
        row = self.object_list.currentRow()
        if row < 0:
            return
        
        # Ako je redni broj 0 i postoji robot, prikaži robota
        if row == 0 and self.task_data.robot:
            self.show_properties(self.task_data.robot)
        else:
            # Inače prikaži objekt iz liste
            obj_index = row - (1 if self.task_data.robot else 0)
            if obj_index < 0 or obj_index >= len(self.task_data.objects):
                return
            obj = self.task_data.objects[obj_index]
            self.show_properties(obj)
    
    def show_properties(self, obj: SceneObject):
        """Prikaži svojstva objekta u panelu"""
        # Očisti prethodni layout
        while self.properties_layout.rowCount() > 0:
            self.properties_layout.removeRow(0)
        
        # ID (read-only)
        id_label = QLineEdit(obj.id)
        id_label.setReadOnly(True)
        self.properties_layout.addRow("ID:", id_label)
        
        # Naziv
        name_edit = QLineEdit(obj.metadata.get("name", ""))
        def save_name(text):
            obj.metadata["name"] = text
        name_edit.textChanged.connect(save_name)
        self.properties_layout.addRow("Naziv:", name_edit)
        
        # Bodovi
        points_spinbox = QSpinBox()
        points_spinbox.setValue(obj.points)
        def save_points(value):
            obj.points = value
        points_spinbox.valueChanged.connect(save_points)
        self.properties_layout.addRow("Bodovi:", points_spinbox)
        
        # Specifična svojstva
        if isinstance(obj, RobotObject):
            x_spinbox = QDoubleSpinBox()
            x_spinbox.setRange(-10000, 10000)
            x_spinbox.setSingleStep(0.1)
            x_spinbox.setDecimals(2)
            x_spinbox.setValue(obj.x)
            def save_x(value):
                obj.x = value
                self.scene.render_task_objects(self.task_data.objects, self.task_data.robot)
                if self.on_task_updated:
                    self.on_task_updated()
            x_spinbox.valueChanged.connect(save_x)
            self.properties_layout.addRow("Pozicija X (cm):", x_spinbox)
            
            y_spinbox = QDoubleSpinBox()
            y_spinbox.setRange(-10000, 10000)
            y_spinbox.setSingleStep(0.1)
            y_spinbox.setDecimals(2)
            y_spinbox.setValue(obj.y)
            def save_y(value):
                obj.y = value
                self.scene.render_task_objects(self.task_data.objects, self.task_data.robot)
                if self.on_task_updated:
                    self.on_task_updated()
            y_spinbox.valueChanged.connect(save_y)
            self.properties_layout.addRow("Pozicija Y (cm):", y_spinbox)
            
            angle_spinbox = QDoubleSpinBox()
            angle_spinbox.setRange(0, 360)
            angle_spinbox.setSingleStep(1)
            angle_spinbox.setDecimals(1)
            angle_spinbox.setValue(obj.angle)
            def save_angle(value):
                obj.angle = value
                self.scene.render_task_objects(self.task_data.objects, self.task_data.robot)
                if self.on_task_updated:
                    self.on_task_updated()
            angle_spinbox.valueChanged.connect(save_angle)
            self.properties_layout.addRow("Orijentacija (°):", angle_spinbox)
        
        elif isinstance(obj, CheckpointObject):
            direction_spinbox = QSpinBox()
            direction_spinbox.setRange(-1, 360)
            direction_spinbox.setValue(obj.direction_requirement if obj.direction_requirement is not None else -1)
            def save_direction(value):
                obj.direction_requirement = value if value >= 0 else None
            direction_spinbox.valueChanged.connect(save_direction)
            self.properties_layout.addRow("Smjer (−1=bilo):", direction_spinbox)
        
        elif isinstance(obj, ZoneObject):
            angle_spinbox = QSpinBox()
            angle_spinbox.setRange(0, 360)
            angle_spinbox.setValue(int(obj.angle_required))
            def save_angle(value):
                obj.angle_required = float(value)
            angle_spinbox.valueChanged.connect(save_angle)
            self.properties_layout.addRow("Требани kut:", angle_spinbox)
            
            tolerance_spinbox = QSpinBox()
            tolerance_spinbox.setRange(0, 180)
            tolerance_spinbox.setValue(int(obj.angle_tolerance))
            def save_tolerance(value):
                obj.angle_tolerance = float(value)
            tolerance_spinbox.valueChanged.connect(save_tolerance)
            self.properties_layout.addRow("Tolerancija:", tolerance_spinbox)
        
        # Delete gumb
        delete_button = QPushButton("🗑️ Obriši objekt")
        delete_button.setStyleSheet("background-color: #ff6b6b; color: white;")
        delete_button.clicked.connect(self.delete_selected_object)
        self.properties_layout.addRow(delete_button)
    
    def on_save_task(self):
        """Spremi task — file dialog"""
        path, _ = QFileDialog.getSaveFileName(
            self, 
            "Spremi task", 
            "", 
            "JSON files (*.json);;All files (*)"
        )
        if path:
            try:
                self.task_data.save_to_file(path)
                if self.on_task_updated:
                    self.on_task_updated()
                QMessageBox.information(self, "Uspješno", f"Task je spreman:\n{path}")
            except Exception as e:
                QMessageBox.critical(self, "Greška", f"Greška pri spremanju:\n{e}")
    
    def on_load_task(self):
        """Učitaj task — file dialog"""
        path, _ = QFileDialog.getOpenFileName(
            self, 
            "Učitaj task", 
            "", 
            "JSON files (*.json);;All files (*)"
        )
        if path:
            try:
                self.task_data.load_from_file(path)
                self.scene.render_task_objects(self.task_data.objects, self.task_data.robot)
                self.refresh_object_list()
                if self.on_task_updated:
                    self.on_task_updated()
                QMessageBox.information(self, "Uspješno", f"Task je učitan:\n{path}")
            except Exception as e:
                QMessageBox.critical(self, "Greška", f"Greška pri učitavanju:\n{e}")

