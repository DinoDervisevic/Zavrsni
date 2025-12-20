#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP

struct Obstacle {
    double x, y; // center position in cm
    double width;
    double length;
    double angle;

    Obstacle(double x, double y, double width, double length, double angle = 0)
        : x(x), y(y), width(width), length(length), angle(angle) {}
};

struct Wall {
    double x1, y1; // start point in cm
    double x2, y2; // end point in cm

    Wall(double x1, double y1, double x2, double y2)
        : x1(x1), y1(y1), x2(x2), y2(y2) {}
};

#endif