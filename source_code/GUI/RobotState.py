from dataclasses import dataclass

@dataclass
class RobotState:
    """Jedno stanje robota iz simulacije"""
    t: float            # vrijeme (sekunde)
    x: float            # pozicija x (cm)
    y: float            # pozicija y (cm)
    angle: float        # kut (stupnjevi, 0-360)
    vx: float = 0       
    vy: float = 0      