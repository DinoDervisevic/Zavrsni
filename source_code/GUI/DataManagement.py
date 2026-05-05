import json
import sys
import math
from typing import List, Dict, Optional, Tuple

from SceneObjects import SceneObject, WallObject, CheckpointObject, ObstacleObject, ZoneObject, RobotObject
from RobotState import RobotState

class TaskData:
    """Učitaj i spremi task JSON"""
    
    def __init__(self):
        self.objects: List[SceneObject] = []
        self.robot: Optional[RobotObject] = None  # Početni robot
    
    def load_from_file(self, path: str):
        """Učitaj task iz JSON datoteke"""
        with open(path, 'r') as f:
            data = json.load(f)
        
        self.objects.clear()
        self.robot = None
        
        for obj_data in data.get("objects", []):
            obj = self._parse_object(obj_data)
            if obj:
                if isinstance(obj, RobotObject):
                    self.robot = obj
                else:
                    self.objects.append(obj)
    
    def save_to_file(self, path: str):
        """Spremi task u JSON datoteku"""
        objects_list = [obj.to_dict() for obj in self.objects]
        if self.robot:
            objects_list.append(self.robot.to_dict())
        
        data = {
            "objects": objects_list
        }
        with open(path, 'w') as f:
            json.dump(data, f, indent=2)
    
    def _parse_object(self, obj_data: Dict) -> Optional[SceneObject]:
        """Parsiraj objekt iz JSON-a"""
        obj_type = obj_data.get("type")
        obj_id = obj_data.get("id")
        metadata = obj_data.get("metadata", {})
        points = obj_data.get("points", 0)
        
        try:
            if obj_type == "wall":
                return WallObject(
                    obj_id, obj_data["x1"], obj_data["y1"],
                    obj_data["x2"], obj_data["y2"], metadata, points
                )
            elif obj_type == "checkpoint":
                return CheckpointObject(
                    obj_id, obj_data["x1"], obj_data["y1"],
                    obj_data["x2"], obj_data["y2"],
                    metadata, points,
                    obj_data.get("direction_requirement")
                )
            elif obj_type == "obstacle":
                return ObstacleObject(
                    obj_id, obj_data["x"], obj_data["y"],
                    obj_data["width"], obj_data["length"],
                    obj_data["angle"], metadata, points
                )
            elif obj_type == "zone":
                return ZoneObject(
                    obj_id, obj_data["x"], obj_data["y"],
                    obj_data["radius"],
                    obj_data["angle_required"],
                    obj_data["angle_tolerance"],
                    metadata, points
                )
            elif obj_type == "robot":
                return RobotObject(
                    obj_id, obj_data["x"], obj_data["y"],
                    obj_data.get("angle", 0.0), metadata
                )
        except (KeyError, ValueError) as e:
            print(f"Greška pri parsiranju objekta {obj_id}: {e}")
            return None
        
        return None
    
    def add_object(self, obj: SceneObject):
        """Dodaj objekt u task"""
        self.objects.append(obj)
    
    def remove_object(self, obj_id: str):
        """Ukloni objekt po ID-u"""
        self.objects = [obj for obj in self.objects if obj.id != obj_id]
    
    def get_next_id(self, obj_type: str) -> str:
        """Generiraj sljedeći ID za tip objekta"""
        count = sum(1 for obj in self.objects if obj.type == obj_type)
        return f"{obj_type}_{count + 1}"


class SimulationData:
    """Učitaj simulacijske rezultate"""
    
    def __init__(self):
        self.robot_states: List[RobotState] = []
        self.obstacles: List[Dict] = []
        self.walls: List[Dict] = []
    
    def load_from_file(self, path: str):
        """Učitaj simulation result JSON"""
        with open(path, 'r') as f:
            data = json.load(f)
        
        self.robot_states.clear()
        for state_data in data.get("robot_states", []):
            state = RobotState(
                t=state_data["t"],
                x=state_data["x"],
                y=state_data["y"],
                angle=state_data["angle"],
                vx=state_data.get("vx", 0),
                vy=state_data.get("vy", 0)
            )
            self.robot_states.append(state)
        
        self.obstacles = data.get("obstacles", [])
        self.walls = data.get("walls", [])
    
    def get_robot_state_at_time(self, t: float) -> Optional[Tuple[float, float, float]]:
        """
        Nađi robota na vremenu t s interpolacijom
        Vraća (x, y, angle)
        """
        if not self.robot_states:
            return None
        
        # Nađi dva stanja između kojih je t
        for i in range(len(self.robot_states) - 1):
            state1 = self.robot_states[i]
            state2 = self.robot_states[i + 1]
            
            if state1.t <= t <= state2.t:
                # Interpolacija
                if state2.t == state1.t:
                    return (state1.x, state1.y, state1.angle)
                
                alpha = (t - state1.t) / (state2.t - state1.t)
                x = state1.x + alpha * (state2.x - state1.x)
                y = state1.y + alpha * (state2.y - state1.y)
                angle = self._interpolate_angle(state1.angle, state2.angle, alpha)
                
                return (x, y, angle)
        
        # Ako t je izvan granica, vrati zadnje stanje
        last = self.robot_states[-1]
        return (last.x, last.y, last.angle)
    
    @staticmethod
    def _interpolate_angle(angle1: float, angle2: float, alpha: float) -> float:
        """Interpoliraj kutove s brisanjem razlike > 180°"""
        diff = angle2 - angle1
        if diff > 180:
            diff -= 360
        elif diff < -180:
            diff += 360
        
        angle = angle1 + alpha * diff
        if angle < 0:
            angle += 360
        elif angle >= 360:
            angle -= 360
        
        return angle

