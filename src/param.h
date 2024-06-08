#pragma once
#ifndef PARAM_H
#define PARAM_H

#include <fstream>
#include <vector>
#include <string>

constexpr int INF_int = 0x3ffffff; // Infinity
extern int ALL_CUSTOMER_NUMBER; // Customer number
extern int VEHICLE_CAPACITY; // Vehicle capacity
extern int VEHICLE_NUMBER; // Vehicle number
constexpr int DURATION = 230; // Maximum operating duration
constexpr int ITER_MAX = 2000; // Number of search iterations
constexpr int TABU_TENURE = 20; // Tabu tenure
extern int RANDOM_SEED; // Random seed
extern std::string file_name_no_suffix;
constexpr double INF_DOUBLE = double(0x1fffffffffffff);

extern std::string instance_name;


using namespace std;

class Node {
public:
    int id;
    double x_coord;
    double y_coord;
    double demand;
    double e;
    double l;
    double service_time;
    double actual_arrive_time; // Vehicle actual arrival time
    double actual_start_time;//节点实际被服务的开始时间
    int route_id; // Vehicle ID the customer belongs to

    Node()
        : id(0), x_coord(0), y_coord(0), demand(0), e(0), l(0),
          service_time(0), actual_arrive_time(0), actual_start_time(0),route_id(-1) {}

    Node(int _id, double _x_coord, double _y_coord, int _demand,
         double _e, double _l, double _service_time)
        : id(_id), x_coord(_x_coord), y_coord(_y_coord), demand(_demand),
          e(_e), l(_l), service_time(_service_time), actual_arrive_time(0),actual_start_time(0), route_id(-1) {}


    //重载==运算符用于比较两个node是否相等
    bool operator==(const Node& other) const {
        return id == other.id;
    }
};

class Route {
public:
    vector<Node> nodes; // Nodes served by the route
    double capacity; // Vehicle capacity
    double total_distance; // Total distance traveled by the vehicle
    double total_demand; // Total demand served by the vehicle
    double time_violation; // Total time window violation
    double max_distance; // Maximum allowed distance
    double max_capacity; // Maximum allowed capacity
    int customer_number; // Number of customers served

public:
    void update_route_info(); // Update route information
    Route(Node depot)
        : capacity(VEHICLE_CAPACITY), total_distance(0), total_demand(0),
          time_violation(0), max_distance(DURATION), max_capacity(VEHICLE_CAPACITY),
          customer_number(0){
        // Initialize the route with the depot node at both ends
        nodes.push_back(depot);
        nodes.push_back(depot);
        nodes[0].actual_arrive_time = 0;
        nodes[0].actual_start_time = 0;
        
        nodes[1].actual_arrive_time = 0;
        nodes[1].actual_start_time = 0;
    }
    // 深拷贝构造函数
    Route(const Route& other)
        : nodes(other.nodes), capacity(other.capacity), total_distance(other.total_distance),
          total_demand(other.total_demand), time_violation(other.time_violation),
          max_distance(other.max_distance), max_capacity(other.max_capacity),
          customer_number(other.customer_number) {}

    // 深拷贝赋值运算符
    Route& operator=(const Route& other) {
        if (this == &other) return *this;
        nodes = other.nodes;
        capacity = other.capacity;
        total_distance = other.total_distance;
        total_demand = other.total_demand;
        time_violation = other.time_violation;
        max_distance = other.max_distance;
        max_capacity = other.max_capacity;
        customer_number = other.customer_number;
        return *this;
    }

    //重载==运算符用于比较两个route是否相等
    bool operator==(const Route& other) const {
        if (nodes.size() != other.nodes.size())
            return false;
        for (int i = 0; i < nodes.size(); i++) {
            if (nodes[i].id != other.nodes[i].id)
                return false;
        }
        return true;
    }
};

// Declare depot and customer nodes
extern Node depot;
extern vector<Node> customer_all_vector;
// Define a matrix to store distances between nodes of different IDs
extern vector<vector<double>> distance_matrix;

//f(s) = c(s) + alpha*q(s) + beta*t(s) 
//c(s)表示路径的行驶距离
//q(s)表示路径的违反容量约束
//t(s)表示路径的违反时间窗约束
//alpha和belta是权重系数
//theta是调整参数
extern double alpha;
extern double b_beta;
extern double g_gamma;
extern double theta;

//p(s) = lambda*c(s)*sqrt(n*m)*sum(visited_node_route_matrix(s))
//lambda是权重系数
//n是节点数
//m是车辆数
//visited_node_route_matrix是一个n*m的矩阵，表示第i个节点被第j辆车访问过的次数
extern double lambda;
//声明一个n*m的矩阵，表示第i个节点被第j辆车访问过的次数，车辆数取最大的车辆数,用id索引
extern vector<vector<double>> visited_node_route_matrix;

// Function declarations
double calculateDistance(const Node& node1, const Node& node2); // Calculate distance between nodes
void parseTestInstance(const string& filename); // Read test instance file
void printNodes(); // Print node information
void init_distance_matrix(); // Initialize distance matrix



#endif // PARAM_H