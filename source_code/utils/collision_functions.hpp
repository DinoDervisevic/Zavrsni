#ifndef COLLISION_FUNCTIONS_HPP
#define COLLISION_FUNCTIONS_HPP

#include "../robot/robot.hpp"
#include "obstacle.hpp"

#define M_PI 3.14159265358979323846
#define K_FACTOR 0.025

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
    vector<Point> obstacle_corners = get_rectangle_corners(obstacle.x, obstacle.y, obstacle.length, obstacle.width, obstacle.angle);

    // Define the axes to test (normals of the edges)
    Point axes[4];
    Point normal = {0, 0};
    double min_overlap = 1e9;
    // Robot edges
    for(int i = 0; i < 2; ++i) {
        Point edge = {robot_corners[(i + 1) % 4].x - robot_corners[i].x,
                      robot_corners[(i + 1) % 4].y - robot_corners[i].y};
        axes[i] = normalize({-edge.y, edge.x}); // Perpendicular vector
    }
    // Obstacle edges
    for(int i = 0; i < 2; ++i) {
        Point edge = {obstacle_corners[(i + 1) % 4].x - obstacle_corners[i].x,
                      obstacle_corners[(i + 1) % 4].y - obstacle_corners[i].y};
        axes[i + 2] = normalize({-edge.y, edge.x}); // Perpendicular vector
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

        double overlap = min(robot_max - obstacle_min, obstacle_max - robot_min);

        if(overlap <= 0) return {false, {0, 0}, 0}; 

        if (overlap < min_overlap) {
            min_overlap = overlap;
            Point d = {robot.x - obstacle.x, robot.y - obstacle.y};
            if ((d.x * axis.x + d.y * axis.y) < 0) 
                normal = {-axis.x, -axis.y};
            else 
                normal = axis;
        }
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
        axes[i] = normalize({-edge.y, edge.x}); // Perpendicular vector
    }
    // Wall edge
    Point wall_edge = {wall.x2 - wall.x1, wall.y2 - wall.y1};
    axes[2] = normalize({-wall_edge.y, wall_edge.x}); // Perpendicular vector

    axes[3] = normalize(wall_edge);

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

        double overlap = min(robot_max - wall_min, wall_max - robot_min);

        // Check for gap
        if(overlap <= 0) {
            return {false, {0, 0}, 0}; // Found a separating axis --> no collision
        }
        if (overlap < min_overlap) {
            min_overlap = overlap;
            normal = axis;
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

Point find_collision_point_obstacle(Robot& robot, Point normal, Obstacle& obstacle){
    vector<Point> robot_corners = get_rectangle_corners(robot.x, robot.y, robot.robot_width, robot.robot_length, robot.angle);
    vector<Point> obstacle_corners = get_rectangle_corners(obstacle.x, obstacle.y, obstacle.length, obstacle.width, obstacle.angle);

    Point tangent = {-normal.y, normal.x};

    double obs_tan_min = 1e9, obs_tan_max = -1e9;
    for (const auto& p : obstacle_corners) {
        double proj = p.x * tangent.x + p.y * tangent.y;
        obs_tan_min = min(obs_tan_min, proj);
        obs_tan_max = max(obs_tan_max, proj);
    }

    double min_proj_norm = 1e9;
    vector<Point> valid_points;

    for (const auto& p : robot_corners) {
        double proj_norm = p.x * normal.x + p.y * normal.y;
        double proj_tan = p.x * tangent.x + p.y * tangent.y;

        // Provjera: Je li točka unutar tangentnih granica prepreke?
        // Dodajemo mali 'epsilon' (0.1) radi numeričke stabilnosti
        if (proj_tan >= obs_tan_min - 1e-5 && proj_tan <= obs_tan_max + 1e-5) {
            if (proj_norm < min_proj_norm - 1e-5) {
                min_proj_norm = proj_norm;
                valid_points.clear();
                valid_points.push_back(p);
            } else if (abs(proj_norm - min_proj_norm) <= 1e-5) {
                valid_points.push_back(p);
            }
        }
    }

   if (valid_points.empty()) {
        double robot_norm_min = 1e9;
        Point best_obs_corner = obstacle_corners[0];
        // Tražimo kut prepreke koji je najdublje u robotu
        for (const auto& p : obstacle_corners) {
            double proj_norm = p.x * (-normal.x) + p.y * (-normal.y);
            if (proj_norm < robot_norm_min) {
                robot_norm_min = proj_norm;
                best_obs_corner = p;
            }
        }
        return best_obs_corner;
    }

    if (valid_points.size() >= 2) {
        double sum_x = 0, sum_y = 0;
        for (const auto& p : valid_points) {
            sum_x += p.x;
            sum_y += p.y;
        }
        return { sum_x / (double)valid_points.size(), sum_y / (double)valid_points.size() };
    }

    return valid_points[0];
}

Point find_collision_point_wall(Robot& robot, Point normal, Wall& wall) {
    vector<Point> robot_corners = get_rectangle_corners(robot.x, robot.y, robot.robot_width, robot.robot_length, robot.angle);

    // 1. Vektor zida i njegova dužina (to nam je tangenta)
    double wall_dx = wall.x2 - wall.x1;
    double wall_dy = wall.y2 - wall.y1;
    double wall_len_sq = wall_dx * wall_dx + wall_dy * wall_dy;
    double wall_len = sqrt(wall_len_sq);
    
    // Normalizirana tangenta
    Point tangent = { wall_dx / wall_len, wall_dy / wall_len };

    // 2. Projekcije krajeva zida na tangentu
    // Postavljamo x1, y1 kao ishodište (0), a x2, y2 je na udaljenosti wall_len
    double wall_min = 0;
    double wall_max = wall_len;

    double min_proj_norm = 1e9;
    vector<Point> valid_points;

    // 3. Provjeri koji kutovi robota su "iznad" segmenta zida
    for (const auto& p : robot_corners) {
        // Vektor od početka zida do kuta robota
        double rel_x = p.x - wall.x1;
        double rel_y = p.y - wall.y1;

        // Projekcija na tangentu zida
        double proj_tan = rel_x * tangent.x + rel_y * tangent.y;
        // Projekcija na normalu
        double proj_norm = p.x * normal.x + p.y * normal.y;

        // Clipping: je li točka unutar [x1, y1] i [x2, y2]?
        if (proj_tan >= wall_min - 0.01 && proj_tan <= wall_max + 0.01) {
            if (proj_norm < min_proj_norm - 1e-5) {
                min_proj_norm = proj_norm;
                valid_points.clear();
                valid_points.push_back(p);
            } else if (abs(proj_norm - min_proj_norm) <= 1e-5) {
                valid_points.push_back(p);
            }
        }
    }

    // 4. Ako niti jedan kut robota nije "iznad" zida, udario si u "vrh" zida
    if (valid_points.empty()) {
        // Točka sudara je onaj kraj zida koji je bliži centru robota
        double d1 = pow(robot.x - wall.x1, 2) + pow(robot.y - wall.y1, 2);
        double d2 = pow(robot.x - wall.x2, 2) + pow(robot.y - wall.y2, 2);
        
        return (d1 < d2) ? Point{wall.x1, wall.y1} : Point{wall.x2, wall.y2};
    }

    // 5. Ako cijeli rub robota naliježe na zid, uzmi točku bližu kraju zida za max torque
    if (valid_points.size() >= 2) {
        double sum_x = 0, sum_y = 0;
        for (const auto& p : valid_points) {
            sum_x += p.x;
            sum_y += p.y;
        }
        return { sum_x / (double)valid_points.size(), sum_y / (double)valid_points.size() };
    }
    return valid_points[0];
}

void handle_collision_obstacle(Robot& robot, double prev_x, double prev_y, 
    CollisionData& collision_data, Obstacle& obstacle) {
    double dx = robot.x - prev_x;
    double dy = robot.y - prev_y;

    Point cp = find_collision_point_obstacle(robot, collision_data.normal, obstacle);
    robot.robot_states.back().cp_x = cp.x;
    robot.robot_states.back().cp_y = cp.y;
    robot.robot_states.back().normal_x = collision_data.normal.x;
    robot.robot_states.back().normal_y = collision_data.normal.y;

    double slop = 0.001; 
    // Faktor izbacivanja (ne izbacuj sve u jednom frameu, nego postepeno kako bi se snizio šok)
    double correction = max(collision_data.depth - slop, 0.0) * 0.3; 
    Point r_to_cp = {cp.x - robot.x, cp.y - robot.y};
    robot.x += collision_data.normal.x * correction;
    robot.y += collision_data.normal.y * correction;

    double dot = dx * collision_data.normal.x + dy * collision_data.normal.y;
    Point v_blocked = {dot * collision_data.normal.x, dot * collision_data.normal.y};

    
    double torque = r_to_cp.x * v_blocked.y - r_to_cp.y * v_blocked.x;

    double dtheta_deg = -(K_FACTOR * torque) * 180.0 / M_PI;
    double dtheta_rad  = dtheta_deg * M_PI / 180.0;

    // Rotate the robot CENTER around the contact point
    Point r_from_cp = {robot.x - cp.x, robot.y - cp.y};
    double cos_dt = cos(dtheta_rad);
    double sin_dt = sin(dtheta_rad);
    robot.x = cp.x + r_from_cp.x * cos_dt - r_from_cp.y * sin_dt;
    robot.y = cp.y + r_from_cp.x * sin_dt + r_from_cp.y * cos_dt;
    robot.angle += dtheta_deg;
}

void handle_collision_wall(Robot& robot, double prev_x, double prev_y, 
    CollisionData& collision_data, Wall& wall) {
    double dx = robot.x - prev_x;
    double dy = robot.y - prev_y;

    Point cp = find_collision_point_wall(robot, collision_data.normal, wall);
    robot.robot_states.back().cp_x = cp.x;
    robot.robot_states.back().cp_y = cp.y;
    robot.robot_states.back().normal_x = collision_data.normal.x;
    robot.robot_states.back().normal_y = collision_data.normal.y;

    double slop = 0.001; 
    // Faktor izbacivanja (ne izbacuj sve u jednom frameu, nego postepeno kako bi se snizio šok)
    double correction = max(collision_data.depth - slop, 0.0) * 0.3; 

    robot.x += collision_data.normal.x * correction;
    robot.y += collision_data.normal.y * correction;

    double dot = dx * collision_data.normal.x + dy * collision_data.normal.y;
    Point v_blocked = {dot * collision_data.normal.x, dot * collision_data.normal.y};

    Point r_to_cp = {cp.x - robot.x, cp.y - robot.y};
    double torque = r_to_cp.x * v_blocked.y - r_to_cp.y * v_blocked.x;

    double dtheta_deg = -(K_FACTOR * torque) * 180.0 / M_PI;
    double dtheta_rad  = dtheta_deg * M_PI / 180.0;

    // Rotate the robot CENTER around the contact point
    Point r_from_cp = {robot.x - cp.x, robot.y - cp.y};
    double cos_dt = cos(dtheta_rad);
    double sin_dt = sin(dtheta_rad);
    robot.x = cp.x + r_from_cp.x * cos_dt - r_from_cp.y * sin_dt;
    robot.y = cp.y + r_from_cp.x * sin_dt + r_from_cp.y * cos_dt;
    robot.angle += dtheta_deg;
}

void collision_response(Robot& robot, map<string, Obstacle>& obstacles, map<string, Wall>& walls){
    double prev_x = robot.previous_x;
    double prev_y = robot.previous_y;

    CollisionData collision_data;
    for (auto& obstacle : obstacles) {
        collision_data = sat_collision_check_obstacle(robot, obstacle.second);
        if (collision_data.is_collision) {
            handle_collision_obstacle(robot, prev_x, prev_y, collision_data, obstacle.second);
        }
    }
    for (auto& wall : walls) {
        collision_data = sat_collision_check_wall(robot, wall.second);
        if (collision_data.is_collision) {
            handle_collision_wall(robot, prev_x, prev_y, collision_data, wall.second);
        }
    }
}

#endif // COLLISION_FUNCTIONS_HPP