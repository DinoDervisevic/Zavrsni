from PyQt5.QtCore import Qt, QRectF, pyqtSignal, QObject
from PyQt5.QtGui import QPainter, QPen, QColor, QBrush, QCursor
from PyQt5.QtWidgets import (
    QGraphicsScene,
    QGraphicsItem,
    QGraphicsView,
    QGraphicsLineItem
)
import math
from typing import List, Dict, Optional, Tuple
from GUI.SceneObjects import SceneObject, WallObject, CheckpointObject, ObstacleObject, ZoneObject, RobotObject
from GUI.RobotState import RobotState


class TaskGraphicsView(QGraphicsView):
    """Custom QGraphicsView sa hover detektijom"""
    
    # Class-level signal deklaracije (trebaju biti dostupne prije __init__)
    object_hovered = pyqtSignal(object)  # Emituje SceneObject ili None
    mouse_moved = pyqtSignal(float, float)  # Emituje scene x, y
    
    def __init__(self, scene, parent=None):
        super().__init__(scene, parent)
        self.current_hovered: Optional[SceneObject] = None
        self.setMouseTracking(True)  # VAŽNO: Omogući mouse tracking čak i bez klika!
    
    def mouseMoveEvent(self, event):
        """Detektuj hover tijekom mouse move-a"""
        super().mouseMoveEvent(event)
        
        scene_pos = self.mapToScene(event.pos())
        self.mouse_moved.emit(scene_pos.x(), scene_pos.y())
        
        if self.scene():  # Provjeravajući da li scene postoji
            hovered_obj = self.scene().get_object_at_position(scene_pos.x(), scene_pos.y())
            
            if hovered_obj != self.current_hovered:
                self.current_hovered = hovered_obj
                self.object_hovered.emit(hovered_obj)


class RobotGraphicsItem(QGraphicsItem):
    """Crtač robota na sceni"""
    
    ROBOT_WIDTH = 10.0  # cm (iste jedinice kao u simulaciji)
    ROBOT_LENGTH = 15.0
    ROBOT_SCALE = 8.0   # piksela po cm (za prikaz)
    
    def __init__(self, x: float, y: float, angle: float, parent=None):
        super().__init__(parent)
        self.angle = angle
        self.setPos(x * self.ROBOT_SCALE, y * self.ROBOT_SCALE)
    
    def boundingRect(self):
        s = self.ROBOT_SCALE
        w = self.ROBOT_WIDTH * s
        l = self.ROBOT_LENGTH * s
        # Dijagonala pravokutnika — dovoljno velik rect za bilo koju rotaciju
        import math
        diag = math.sqrt(w * w + l * l) / 2 + 5
        return QRectF(-diag, -diag, diag * 2, diag * 2)
    
    def paint(self, painter: QPainter, option, widget=None):
        # Spremi trenutnu transformaciju
        painter.save()
        
        # Samo rotacija (pozicija je već postavljena sa setPos)
        painter.rotate(self.angle)  # Pozitivan kut = clockwise
        
        # Crtaj pravokutnik robota
        s = self.ROBOT_SCALE
        w = self.ROBOT_WIDTH * s / 2
        l = self.ROBOT_LENGTH * s / 2
        
        painter.setPen(QPen(QColor(0, 100, 200), 2))
        painter.setBrush(QBrush(QColor(200, 220, 255)))
        painter.drawRect(int(-w), int(-l), int(2*w), int(2*l))
        
        # Crtaj strelicu prema "naprijed" (unutar robota)
        painter.setPen(QPen(QColor(255, 0, 0), 2))
        painter.drawLine(int(0), int(0), int(0), int(-l * 0.85))
        
        painter.restore()
    
    def update_position(self, x: float, y: float, angle: float):
        """Ažuriraj poziciju i kut"""
        self.angle = angle
        self.setPos(x * self.ROBOT_SCALE, y * self.ROBOT_SCALE)


class TaskScene(QGraphicsScene):
    """Scena za prikaz task-a i robot-a"""
    
    GRID_SIZE = 50  # Veličina grid-a (piksela)
    SCALE = 8.0  # piksela po cm
    
    def __init__(self, parent=None):
        super().__init__(parent)
        # Postavi dovoljno velik rect da robot može slobodno kretati
        self.setSceneRect(-10000, -10000, 20000, 20000)  # Dovoljno velik rect za robot kretanje
        self.setBackgroundBrush(QBrush(QColor(240, 240, 240)))
        
        self.task_objects: List[SceneObject] = []
        self.robot_item: Optional[RobotGraphicsItem] = None
        self.robot: Optional[RobotObject] = None  # Spremi robot podatke
        self.robot_static_item = None  # Static robot item iz render_task_objects za kasnije brisanje
        self.grid_enabled = True
        self.object_bounds: Dict[str, Tuple] = {}  # Čuvaj bounds svakog objekta za click detection
        
        # Preview objekti tijekom dodavanja
        self.preview_line_item = None  # Privremena linija između točaka
        self.preview_point_item = None  # Prva točka
        self.preview_ghost_item = None  # Ghost objekt koji prati mišu
    
    def render_task_objects(self, objects: List[SceneObject], robot: Optional[RobotObject] = None):
        """Crtaj sve objekte iz task-a"""
        self.clear()
        # Reset referenci jer clear() briše sve item-e sa scene
        self.robot_item = None
        self.robot_static_item = None
        
        self.task_objects = objects
        self.robot = robot
        
        for obj in objects:
            self._draw_object(obj)
        
        # Iscrta robota ako postoji
        if self.robot:
            self._draw_object(self.robot)
    
    def load_task(self, objects: List[SceneObject], robot: Optional[RobotObject] = None):
        """Učitaj i prikaži objekte iz task-a"""
        self.render_task_objects(objects, robot)
    
    def _draw_object(self, obj: SceneObject):
        """Crtaj jedan objekt"""
        r, g, b = obj.get_color()
        color = QColor(r, g, b)
        pen = QPen(color, 2)
        brush = QBrush(color)
        brush.setStyle(Qt.Dense4Pattern)  # Semi-transparent
        
        SCALE = 8.0  # piksela po cm
        
        if isinstance(obj, WallObject):
            # Linija
            self.addLine(
                obj.x1 * SCALE, obj.y1 * SCALE,
                obj.x2 * SCALE, obj.y2 * SCALE,
                pen
            )
        
        elif isinstance(obj, CheckpointObject):
            # Debelo linija (checkpoint)
            pen.setWidth(4)
            self.addLine(
                obj.x1 * SCALE, obj.y1 * SCALE,
                obj.x2 * SCALE, obj.y2 * SCALE,
                pen
            )
            # Dodaj label
            label = self.addText(f"{obj.id}\n{obj.points}pt")
            label.setPos(((obj.x1 + obj.x2) / 2) * SCALE, ((obj.y1 + obj.y2) / 2) * SCALE)
        
        elif isinstance(obj, ObstacleObject):
            # Pravokutnik
            x, y = obj.x, obj.y
            w, h = obj.width / 2, obj.length / 2
            rect_item = self.addRect(
                (x - w) * SCALE, (y - h) * SCALE,
                obj.width * SCALE, obj.length * SCALE,
                pen, brush
            )
            # Rotacija
            rect_item.setRotation(-obj.angle)
        
        elif isinstance(obj, ZoneObject):
            # Kružnica
            r, g, b = obj.get_color()
            color = QColor(r, g, b, 100)  # Semi-transparent
            pen = QPen(color, 2)
            self.addEllipse(
                (obj.x - obj.radius) * SCALE,
                (obj.y - obj.radius) * SCALE,
                obj.radius * 2 * SCALE,
                obj.radius * 2 * SCALE,
                pen, QBrush(color)
            )
            # Strelica za smjer
            angle_rad = math.radians(obj.angle_required)
            x_end = obj.x + obj.radius * 0.7 * math.cos(angle_rad)
            y_end = obj.y + obj.radius * 0.7 * math.sin(angle_rad)
            pen.setColor(QColor(0, 0, 0))
            pen.setWidth(1)
            self.addLine(
                obj.x * SCALE, obj.y * SCALE,
                x_end * SCALE, y_end * SCALE,
                pen
            )
        
        elif isinstance(obj, RobotObject):
            # Crtaj robota kao pravokutnik s orijentacijom
            ROBOT_WIDTH = 10.0  # cm
            ROBOT_LENGTH = 15.0
            w = ROBOT_WIDTH / 2
            l = ROBOT_LENGTH / 2
            
            pen = QPen(QColor(0, 100, 200), 2)
            brush = QBrush(QColor(200, 220, 255))
            
            # Kreiraj rect oko (0,0) pa onda koristi setPos za pozicioniranje
            rect_item = self.addRect(
                -w * SCALE, -l * SCALE,
                ROBOT_WIDTH * SCALE, ROBOT_LENGTH * SCALE,
                pen, brush
            )
            rect_item.setPos(obj.x * SCALE, obj.y * SCALE)
            rect_item.setRotation(obj.angle)
            self.robot_static_item = rect_item  # Spremi za kasnije brisanje
            
            # Crvena linija za smjer (naprijed) — child of rect, unutar robota
            direction_pen = QPen(QColor(255, 0, 0), 2)
            line_item = QGraphicsLineItem(0, 0, 0, -l * SCALE * 0.85, rect_item)
            line_item.setPen(direction_pen)
    
    def add_robot(self, x: float, y: float, angle: float):
        """Dodaj robot na scenu"""
        # Obriši postojećeg animiranog robota ako postoji
        if self.robot_item:
            self.removeItem(self.robot_item)
            self.robot_item = None
        self.robot_item = RobotGraphicsItem(x, y, angle)
        self.addItem(self.robot_item)
        self.robot_item.setZValue(1000)
    
    def remove_static_robot(self):
        """Obriši statički robot kreirani iz render_task_objects"""
        if self.robot_static_item:
            self.removeItem(self.robot_static_item)
            self.robot_static_item = None
    
    def update_robot(self, x: float, y: float, angle: float):
        """Ažuriraj robota"""
        if self.robot_item:
            self.robot_item.update_position(x, y, angle)
    
    def get_object_at_position(self, scene_x: float, scene_y: float) -> Optional[SceneObject]:
        """Nađi objekt na određenoj poziciji scene-a"""
        # Konvertiraj scene koordinate u cm
        x_cm = scene_x / self.SCALE
        y_cm = scene_y / self.SCALE
        
        # Prvo provjeri robota ako postoji
        if self.robot:
            if self._is_point_in_object(self.robot, x_cm, y_cm):
                return self.robot
        
        # Iteriraj kroz objekte obrnutim redoslijedom (noviji se pojavljuje prvi)
        for obj in reversed(self.task_objects):
            if self._is_point_in_object(obj, x_cm, y_cm):
                return obj
        
        return None
    
    def _is_point_in_object(self, obj: SceneObject, x: float, y: float) -> bool:
        """Provjeri je li točka (x, y) u objektu"""
        TOLERANCE = 5  # cm tolerancija za klik
        
        if isinstance(obj, WallObject):
            # Provjeri je li točka blizu linije
            x1, y1 = obj.x1, obj.y1
            x2, y2 = obj.x2, obj.y2
            dist = self._distance_point_to_line(x, y, x1, y1, x2, y2)
            return dist < TOLERANCE
        
        elif isinstance(obj, CheckpointObject):
            # Isto kao wall
            x1, y1 = obj.x1, obj.y1
            x2, y2 = obj.x2, obj.y2
            dist = self._distance_point_to_line(x, y, x1, y1, x2, y2)
            return dist < TOLERANCE
        
        elif isinstance(obj, ObstacleObject):
            # Provjeri je li točka u rotiranom pravokutniku
            return self._is_point_in_rotated_rect(
                x, y, obj.x, obj.y, obj.width, obj.length, obj.angle
            )
        
        elif isinstance(obj, ZoneObject):
            # Provjeri je li točka u kružnici
            dist = math.sqrt((x - obj.x) ** 2 + (y - obj.y) ** 2)
            return dist <= obj.radius + TOLERANCE
        
        elif isinstance(obj, RobotObject):
            # Provjeri je li točka u robotu (kao rotiranom pravokutniku)
            ROBOT_WIDTH = 10.0  # cm
            ROBOT_LENGTH = 15.0
            return self._is_point_in_rotated_rect(
                x, y, obj.x, obj.y, ROBOT_WIDTH, ROBOT_LENGTH, obj.angle
            )
        
        return False
    
    @staticmethod
    def _distance_point_to_line(px: float, py: float, x1: float, y1: float, x2: float, y2: float) -> float:
        """Udaljenost točke od linije"""
        dx = x2 - x1
        dy = y2 - y1
        if dx == 0 and dy == 0:
            return math.sqrt((px - x1) ** 2 + (py - y1) ** 2)
        
        t = max(0, min(1, ((px - x1) * dx + (py - y1) * dy) / (dx * dx + dy * dy)))
        closest_x = x1 + t * dx
        closest_y = y1 + t * dy
        return math.sqrt((px - closest_x) ** 2 + (py - closest_y) ** 2)
    
    @staticmethod
    def _is_point_in_rotated_rect(px: float, py: float, cx: float, cy: float, 
                                   width: float, length: float, angle: float) -> bool:
        """Provjeri je li točka u rotiranom pravokutniku"""
        # Prebaci u lokalni koordinatni sustav
        angle_rad = math.radians(angle)
        cos_a = math.cos(-angle_rad)
        sin_a = math.sin(-angle_rad)
        
        dx = px - cx
        dy = py - cy
        
        local_x = dx * cos_a - dy * sin_a
        local_y = dx * sin_a + dy * cos_a
        
        return (abs(local_x) <= width / 2 + 2 and abs(local_y) <= length / 2 + 2)
    
    def clear_preview(self):
        """Obriši sve preview objekte"""
        if self.preview_line_item:
            self.removeItem(self.preview_line_item)
            self.preview_line_item = None
        if self.preview_point_item:
            self.removeItem(self.preview_point_item)
            self.preview_point_item = None
        if self.preview_ghost_item:
            self.removeItem(self.preview_ghost_item)
            self.preview_ghost_item = None
    
    def show_first_point(self, x_cm: float, y_cm: float):
        """Prikaži prvu točku"""
        self.clear_preview()
        
        x = x_cm * self.SCALE
        y = y_cm * self.SCALE
        
        # Crvena točka
        self.preview_point_item = self.addEllipse(
            x - 5, y - 5, 10, 10,
            QPen(QColor(255, 0, 0), 2),
            QBrush(QColor(255, 100, 100))
        )
    
    def update_preview(self, x1_cm: float, y1_cm: float, x2_cm: float, y2_cm: float, obj_type: str):
        """Ažuriraj preview liniju i ghost objekt"""
        x1 = x1_cm * self.SCALE
        y1 = y1_cm * self.SCALE
        x2 = x2_cm * self.SCALE
        y2 = y2_cm * self.SCALE
        
        # Obriši staru liniju ako postoji
        if self.preview_line_item:
            self.removeItem(self.preview_line_item)
        if self.preview_ghost_item:
            self.removeItem(self.preview_ghost_item)
        
        # Linija između točaka
        self.preview_line_item = self.addLine(
            x1, y1, x2, y2,
            QPen(QColor(0, 150, 255), 2, Qt.DashLine)  # Plava isprekidana linija
        )
        
        # Ghost objekt (privremeni prikaz kako će objekt izgledati)
        if obj_type in ["wall", "checkpoint"]:
            pen = QPen(QColor(150, 150, 150), 1, Qt.DashLine)
            self.preview_ghost_item = self.addLine(x1, y1, x2, y2, pen)
        
        elif obj_type == "obstacle":
            width_cm = abs(x2_cm - x1_cm)
            length_cm = abs(y2_cm - y1_cm)
            center_x = (x1 + x2) / 2
            center_y = (y1 + y2) / 2
            
            pen = QPen(QColor(150, 150, 150), 1, Qt.DashLine)
            brush = QBrush(QColor(255, 150, 150, 50))
            
            self.preview_ghost_item = self.addRect(
                center_x - width_cm * self.SCALE / 2,
                center_y - length_cm * self.SCALE / 2,
                width_cm * self.SCALE,
                length_cm * self.SCALE,
                pen, brush
            )
        
        elif obj_type == "zone":
            radius_cm = math.sqrt((x2_cm - x1_cm) ** 2 + (y2_cm - y1_cm) ** 2)
            
            pen = QPen(QColor(150, 150, 150), 1, Qt.DashLine)
            brush = QBrush(QColor(150, 150, 255, 30))
            
            self.preview_ghost_item = self.addEllipse(
                x1 - radius_cm * self.SCALE,
                y1 - radius_cm * self.SCALE,
                radius_cm * 2 * self.SCALE,
                radius_cm * 2 * self.SCALE,
                pen, brush
            )

