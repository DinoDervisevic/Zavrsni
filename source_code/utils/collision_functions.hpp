#ifndef COLLISION_FUNCTIONS_HPP
#define COLLISION_FUNCTIONS_HPP

#include "../robot/robot.hpp"
#include "obstacle.hpp"

#define M_PI 3.14159265358979323846

struct Point { double x, y; };

static Point normalize(const Point& v) {
    double len = sqrt(v.x * v.x + v.y * v.y);
    if (len < 1e-12) return {0, 0};
    return {v.x / len, v.y / len};
}

vector<Point> get_rectangle_corners(double center_x, double center_y, double width, double length, double angle) {
    vector<Point> corners(4);
    double half_width = width / 2.0;
    double half_length = length / 2.0;

    // Calculate the corners relative to the center
    corners[0] = {-half_length, -half_width};
    corners[1] = { half_length, -half_width};
    corners[2] = { half_length,  half_width};
    corners[3] = {-half_length,  half_width};

    // Rotate and translate the corners
    double rad = angle * M_PI / 180.0; // Convert angle to radians
    double cos_a = cos(rad);
    double sin_a = sin(rad);

    for (auto& corner : corners) {
        double new_x = corner.x * cos_a - corner.y * sin_a + center_x;
        double new_y = corner.x * sin_a + corner.y * cos_a + center_y;
        corner.x = new_x;
        corner.y = new_y;
    }

    return corners;
}

bool sat_collision_check_obstacle(Robot& robot, Obstacle& obstacle){
    // Get the corners of the robot
    vector<Point> robot_corners = get_rectangle_corners(robot.x, robot.y, robot.robot_width, robot.robot_length, robot.angle);

    // Get the corners of the obstacle
    vector<Point> obstacle_corners = get_rectangle_corners(obstacle.x, obstacle.y, obstacle.width, obstacle.length, obstacle.angle);

    // Define the axes to test (normals of the edges)
    Point axes[4];
    // Robot edges
    for(int i = 0; i < 2; ++i) {
        Point edge = {robot_corners[(i + 1) % 4].x - robot_corners[i].x,
                      robot_corners[(i + 1) % 4].y - robot_corners[i].y};
        axes[i] = {-edge.y, edge.x}; // Perpendicular vector
    }
    // Obstacle edges
    for(int i = 0; i < 2; ++i) {
        Point edge = {obstacle_corners[(i + 1) % 4].x - obstacle_corners[i].x,
                      obstacle_corners[(i + 1) % 4].y - obstacle_corners[i].y};
        axes[i + 2] = {-edge.y, edge.x}; // Perpendicular vector
    }

    // Check for overlap on each axis
    for(const auto& axis : axes) {
        // Project robot corners onto axis
        double robot_min = 1e9, robot_max = -1e9;
        for (int i = 0; i < 4; ++i) {
            double projection = (robot_corners[i].x * axis.x + robot_corners[i].y * axis.y);
            robot_min = min(robot_min, projection);
            robot_max = max(robot_max, projection);
        }

        // Project obstacle corners onto axis
        double obstacle_min = 1e9, obstacle_max = -1e9;
        for (int i = 0; i < 4; ++i) {
            double projection = (obstacle_corners[i].x * axis.x + obstacle_corners[i].y * axis.y);
            obstacle_min = min(obstacle_min, projection);
            obstacle_max = max(obstacle_max, projection);
        }

        // Check for gap
        if(robot_max <= obstacle_min || obstacle_max <= robot_min) {
            return false; // Found a separating axis
        }
    }
    return true; // No separating axis found --> collision detected
}

bool sat_collision_check_wall(Robot& robot, Wall& wall){
    // Get the corners of the robot
    vector<Point> robot_corners = get_rectangle_corners(robot.x, robot.y, robot.robot_width, robot.robot_length, robot.angle);

    // Define the wall corners
    vector<Point> wall_corners(2);
    wall_corners[0] = {wall.x1, wall.y1};
    wall_corners[1] = {wall.x2, wall.y2};

    // Define the axes to test (normals of the edges)
    Point axes[3];
    // Robot edges
    for(int i = 0; i < 2; ++i) {
        Point edge = {robot_corners[(i + 1) % 4].x - robot_corners[i].x,
                      robot_corners[(i + 1) % 4].y - robot_corners[i].y};
        axes[i] = {-edge.y, edge.x}; // Perpendicular vector
    }
    // Wall edge
    Point wall_edge = {wall.x2 - wall.x1, wall.y2 - wall.y1};
    axes[2] = {-wall_edge.y, wall_edge.x}; // Perpendicular vector

    // Check for overlap on each axis
    for(const auto& axis : axes) {
        // Project robot corners onto axis
        double robot_min = 1e9, robot_max = -1e9;
        for (int i = 0; i < 4; ++i) {
            double projection = (robot_corners[i].x * axis.x + robot_corners[i].y * axis.y);
            robot_min = min(robot_min, projection);
            robot_max = max(robot_max, projection);
        }

        // Project wall corners onto axis
        double wall_min = 1e9, wall_max = -1e9;
        for (int i = 0; i < 2; ++i) {
            double projection = (wall_corners[i].x * axis.x + wall_corners[i].y * axis.y);
            wall_min = min(wall_min, projection);
            wall_max = max(wall_max, projection);
        }

        // Check for gap
        if(robot_max <= wall_min || wall_max <= robot_min) {
            return false; // Found a separating axis
        }
    }
    return true; // No separating axis found --> collision detected
}

bool segment_segment_intersect(Robot& robot, Wall& wall){
    // Get the corners of the robot
    vector<Point> robot_corners = get_rectangle_corners(robot.x, robot.y, robot.robot_width, robot.robot_length, robot.angle);

    // Check each edge of the robot against the wall segment
    for(int i = 0; i < 4; ++i) {
        Point p1 = robot_corners[i];
        Point p2 = robot_corners[(i + 1) % 4];
        Point q1 = {wall.x1, wall.y1};
        Point q2 = {wall.x2, wall.y2};

        double d1 = (p2.x - p1.x) * (q1.y - p1.y) - (p2.y - p1.y) * (q1.x - p1.x);
        double d2 = (p2.x - p1.x) * (q2.y - p1.y) - (p2.y - p1.y) * (q2.x - p1.x);
        double d3 = (q2.x - q1.x) * (p1.y - q1.y) - (q2.y - q1.y) * (p1.x - q1.x);
        double d4 = (q2.x - q1.x) * (p2.y - q1.y) - (q2.y - q1.y) * (p2.x - q1.x);

        if (((d1 >= 0 && d2 <= 0) || (d1 <= 0 && d2 >= 0)) &&
            ((d3 >= 0 && d4 <= 0) || (d3 <= 0 && d4 >= 0))) {
            return true; // Segments intersect
        }
    }
    return false; // No intersection found
}

// Function to find exact intersection point of two line segments
static bool segment_intersection_point(const Point& p1, const Point& p2,
                                       const Point& q1, const Point& q2,
                                       double& x, double& y) {
    double s1_x = p2.x - p1.x;
    double s1_y = p2.y - p1.y;
    double s2_x = q2.x - q1.x;
    double s2_y = q2.y - q1.y;

    double d = (-s2_x * s1_y + s1_x * s2_y);
    if (fabs(d) < 1e-12) return false;

    double s = (-s1_y * (p1.x - q1.x) + s1_x * (p1.y - q1.y)) / d;
    double t = ( s2_x * (p1.y - q1.y) - s2_y * (p1.x - q1.x)) / d;

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
        x = p1.x + (t * s1_x);
        y = p1.y + (t * s1_y);
        return true;
    }
    return false;
}

void handle_collision(Robot& robot, double prev_x, double prev_y, double prev_angle) {
    double angle = (robot.angle - prev_angle) * M_PI / 180.0;
    double dx = robot.x - prev_x;
    double dy = robot.y - prev_y;
    double d = sqrt(dx*dx + dy*dy);

    double R = (fabs(angle) < 1e-8) ? (d / (2 * sin(angle/2))) : 0.0; // circle radius

    double theta0 = prev_angle * M_PI / 180.0;
    double ICC_x = prev_x - R * sin(theta0);
    double ICC_y = prev_y + R * cos(theta0);
}

#endif // COLLISION_FUNCTIONS_HPP