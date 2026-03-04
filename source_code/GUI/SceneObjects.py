from abc import ABC, abstractmethod

import json
import sys
import math
from typing import List, Dict, Optional, Tuple

class SceneObject(ABC):
    """Bazna klasa za sve objekte na sceni"""
    
    def __init__(self, obj_id: str, obj_type: str, metadata: Dict, points: int):
        self.id = obj_id
        self.type = obj_type  # "wall", "checkpoint", "obstacle", "zone"
        self.metadata = metadata  # {"name": ..., "description": ...}
        self.points = points
        self.cleared = False
    
    @abstractmethod
    def to_dict(self) -> Dict:
        """Spremi u JSON format"""
        pass
    
    @abstractmethod
    def get_color(self) -> Tuple[int, int, int]:
        """RGB boja za prikaz"""
        pass


class WallObject(SceneObject):
    def __init__(self, obj_id: str, x1: float, y1: float, x2: float, y2: float,
                 metadata: Dict, points: int = 0):
        super().__init__(obj_id, "wall", metadata, points)
        self.x1, self.y1 = x1, y1
        self.x2, self.y2 = x2, y2
    
    def to_dict(self) -> Dict:
        return {
            "id": self.id,
            "type": self.type,
            "x1": self.x1, "y1": self.y1,
            "x2": self.x2, "y2": self.y2,
            "metadata": self.metadata,
            "points": self.points
        }
    
    def get_color(self) -> Tuple[int, int, int]:
        return (100, 100, 100)  # Siva


class CheckpointObject(SceneObject):
    def __init__(self, obj_id: str, x1: float, y1: float, x2: float, y2: float,
                 metadata: Dict, points: int, direction_req: Optional[float] = None):
        super().__init__(obj_id, "checkpoint", metadata, points)
        self.x1, self.y1 = x1, y1
        self.x2, self.y2 = x2, y2
        self.direction_requirement = direction_req  # None ili kut 0-360
    
    def to_dict(self) -> Dict:
        return {
            "id": self.id,
            "type": self.type,
            "x1": self.x1, "y1": self.y1,
            "x2": self.x2, "y2": self.y2,
            "metadata": self.metadata,
            "points": self.points,
            "direction_requirement": self.direction_requirement
        }
    
    def get_color(self) -> Tuple[int, int, int]:
        return (0, 200, 0) if not self.cleared else (100, 255, 100)  # Zelena/svjetlozelena


class ObstacleObject(SceneObject):
    def __init__(self, obj_id: str, x: float, y: float, width: float, length: float,
                 angle: float, metadata: Dict, points: int = 0):
        super().__init__(obj_id, "obstacle", metadata, points)
        self.x, self.y = x, y
        self.width, self.length = width, length
        self.angle = angle
    
    def to_dict(self) -> Dict:
        return {
            "id": self.id,
            "type": self.type,
            "x": self.x, "y": self.y,
            "width": self.width, "length": self.length,
            "angle": self.angle,
            "metadata": self.metadata,
            "points": self.points
        }
    
    def get_color(self) -> Tuple[int, int, int]:
        return (255, 50, 50)  # Crvena


class ZoneObject(SceneObject):
    def __init__(self, obj_id: str, x: float, y: float, radius: float,
                 angle_req: float, tolerance: float, metadata: Dict, points: int):
        super().__init__(obj_id, "zone", metadata, points)
        self.x, self.y = x, y
        self.radius = radius
        self.angle_required = angle_req
        self.angle_tolerance = tolerance
    
    def to_dict(self) -> Dict:
        return {
            "id": self.id,
            "type": self.type,
            "x": self.x, "y": self.y,
            "radius": self.radius,
            "metadata": self.metadata,
            "points": self.points,
            "angle_required": self.angle_required,
            "angle_tolerance": self.angle_tolerance
        }
    
    def get_color(self) -> Tuple[int, int, int]:
        return (50, 120, 255) if not self.cleared else (150, 200, 255)  # Plava/svjetloplava


class RobotObject(SceneObject):
    """Robot s početnom pozicijom i orijentacijom"""
    def __init__(self, obj_id: str, x: float, y: float, angle: float = 0.0,
                 metadata: Dict = None):
        super().__init__(obj_id, "robot", metadata or {}, 0)
        self.x = x
        self.y = y
        self.angle = angle
    
    def to_dict(self) -> Dict:
        return {
            "id": self.id,
            "type": self.type,
            "x": self.x, "y": self.y,
            "angle": self.angle,
            "metadata": self.metadata
        }
    
    def get_color(self) -> Tuple[int, int, int]:
        return (0, 100, 200)  # Plava (ista kao u prikazu)
