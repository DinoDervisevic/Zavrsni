#ifndef COLLISION_FUNCTIONS_HPP
#define COLLISION_FUNCTIONS_HPP

#include "../robot/robot.hpp"
#include "obstacle.hpp"

#define M_PI 3.14159265358979323846
#define K_FACTOR 0.2

struct Point { double x, y; };

struct CollisionData {
    bool is_collision;
    Point normal;
    double depth;
};

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

CollisionData sat_collision_check_obstacle(Robot& robot, Obstacle& obstacle){
    // Get the corners of the robot
    vector<Point> robot_corners = get_rectangle_corners(robot.x, robot.y, robot.robot_width, robot.robot_length, robot.angle);

    // Get the corners of the obstacle
    vector<Point> obstacle_corners = get_rectangle_corners(obstacle.x, obstacle.y, obstacle.width, obstacle.length, obstacle.angle);

    // Define the axes to test (normals of the edges)
    Point axes[4];
    Point normal = {0, 0};
    double min_overlap = 1e9;
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
        if(robot_max < obstacle_min || obstacle_max < robot_min) {
            return {false, {0, 0}, 0}; // Found a separating axis --> no collision
        }
        else {
            double overlap = min(robot_max, obstacle_max) - max(robot_min, obstacle_min);
            if (overlap < 1e-12) overlap = 0;
            if (overlap > 0 && overlap < min_overlap) {
                min_overlap = overlap;
                normal = normalize(axis);
            }
        }
    }

    Point center_diff = {robot.x - obstacle.x, robot.y - obstacle.y};
    if ((center_diff.x * normal.x + center_diff.y * normal.y) < 0) {
        normal.x = -normal.x;
        normal.y = -normal.y;
    }

    return {true, normal, min_overlap}; // No separating axis found --> collision detected
}

CollisionData sat_collision_check_wall(Robot& robot, Wall& wall){
    // Get the corners of the robot
    vector<Point> robot_corners = get_rectangle_corners(robot.x, robot.y, robot.robot_width, robot.robot_length, robot.angle);

    // Define the wall corners
    vector<Point> wall_corners(2);
    wall_corners[0] = {wall.x1, wall.y1};
    wall_corners[1] = {wall.x2, wall.y2};

    // Define the axes to test (normals of the edges)
    Point axes[4];
    Point normal = {0, 0};
    double min_overlap = 1e9;
    // Robot edges
    for(int i = 0; i < 2; ++i) {
        Point edge = {robot_corners[(i + 1) % 4].x - robot_corners[i].x,
                      robot_corners[(i + 1) % 4].y - robot_corners[i].y};
        axes[i] = {-edge.y, edge.x}; // Perpendicular vector
    }
    // Wall edge
    Point wall_edge = {wall.x2 - wall.x1, wall.y2 - wall.y1};
    axes[2] = {-wall_edge.y, wall_edge.x}; // Perpendicular vector

    Point wall_normal = normalize(wall_edge);
    axes[3] = {-wall_normal.y, wall_normal.x};

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
        if(robot_max < wall_min || wall_max < robot_min) {
            return {false, {0, 0}, 0}; // Found a separating axis --> no collision
        }
        else {
            double overlap = min(robot_max, wall_max) - max(robot_min, wall_min);
            if (overlap < 1e-12) overlap = 0;
            if (overlap > 0 && overlap < min_overlap) {
                min_overlap = overlap;
                normal = normalize(axis);
            }
        }
    }

    double wall_mid_x = (wall.x1 + wall.x2) / 2.0;
    double wall_mid_y = (wall.y1 + wall.y2) / 2.0;
    Point center_diff = {robot.x - wall_mid_x, robot.y - wall_mid_y};
    if ((center_diff.x * normal.x + center_diff.y * normal.y) < 0) {
        normal.x = -normal.x;
        normal.y = -normal.y;
    }

    return {true, normal, min_overlap}; // No separating axis found --> collision detected
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

Point find_collision_point(Robot& robot, Point normal, double depth){
    vector<Point> robot_corners = get_rectangle_corners(robot.x, robot.y, robot.robot_width, robot.robot_length, robot.angle);

    double min_projection = 1e9;
    Point deepest_point = {0, 0};
    vector<Point> contact_points;

    for(int i = 0; i < 4; ++i) {
        Point p1 = robot_corners[i];
        
        double projection = (p1.x * normal.x) + (p1.y * normal.y);
        
       if (projection < min_projection - 1e-5) {
            min_projection = projection;
            contact_points.clear();      
            contact_points.push_back(p1);
        }
        else if (abs(projection - min_projection) <= 1e-5) {
            contact_points.push_back(p1);
        }
    }
    
    if (contact_points.size() == 1) {
        return contact_points[0];
    }
    else if (contact_points.size() == 2) {
        return { (contact_points[0].x + contact_points[1].x) / 2.0, 
                 (contact_points[0].y + contact_points[1].y) / 2.0 };
    }

   return contact_points[0];
}

void handle_collision(Robot& robot, double prev_x, double prev_y, CollisionData& collision_data){
    double dx = robot.x - prev_x;
    double dy = robot.y - prev_y;

    Point cp = find_collision_point(robot, collision_data.normal, collision_data.depth);

    robot.x += collision_data.normal.x * collision_data.depth;
    robot.y += collision_data.normal.y * collision_data.depth;

    double dot = dx * collision_data.normal.x + dy * collision_data.normal.y;
    Point v_blocked = {dot * collision_data.normal.x, dot * collision_data.normal.y};

    Point r_to_cp = {cp.x - robot.x, cp.y - robot.y};
    double torque = r_to_cp.x * v_blocked.y - r_to_cp.y * v_blocked.x;

    robot.angle += (K_FACTOR * torque) * 180.0 / M_PI;
}

#endif // COLLISION_FUNCTIONS_HPP