#include <vector>
#include "param.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>


Node depot;
vector<Node> customer_all_vector;
// Define a matrix to store distances between nodes of different IDs
vector<vector<double>> distance_matrix;

double alpha = 1;

double b_beta = 1;

double g_gamma = 1;

double theta = 0.5;

double lambda = 0.001;

vector<vector<double>> visited_node_route_matrix(ALL_CUSTOMER_NUMBER+2, vector<double>(VEHICLE_NUMBER+1,0.0));


int ALL_CUSTOMER_NUMBER = 100; // Customer number
int VEHICLE_CAPACITY ; // Vehicle capacity
int VEHICLE_NUMBER = 25 ; // Vehicle number
int RANDOM_SEED = 123;
std::string instance_name;

string file_name_no_suffix;



// Function to calculate Euclidean distance between two nodes
double calculateDistance(const Node& node1, const Node& node2) {
    double dx = node1.x_coord - node2.x_coord;
    double dy = node1.y_coord - node2.y_coord;
    return sqrt(dx * dx + dy * dy);
}


void parseTestInstance(const string& filename) {
    customer_all_vector.clear();
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Unable to open file: print()" << filename << endl;
        exit(1);
    }
    //注意前9行时这些信息
    /*
        R101

    VEHICLE
    NUMBER     CAPACITY
    25         200

    CUSTOMER
    CUST NO.   XCOORD.   YCOORD.    DEMAND   READY TIME   DUE DATE   SERVICE TIME
    
    */
    //第10行的节点是起始点depot单独保存
     // Skip the first 9 lines (metadata)
    file>>instance_name;
    string line;
    // 跳过剩余的第一行
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // 跳过接下来的3行
    for (int i = 0; i < 3; ++i) {
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    int vehicle_number,vehicle_capacity;
    
    // Read the vehicle number and capacity from the 5th line
    file >> vehicle_number >> vehicle_capacity;
    // Skip the next 4 lines (metadata)
    for (int i = 0; i < 4; ++i) {
        getline(file, line);
    }
    file >> depot.id >> depot.x_coord >> depot.y_coord >> depot.demand >> depot.e >> depot.l >> depot.service_time;

    Node node;
    while (file >> node.id >> node.x_coord >> node.y_coord >> node.demand >> node.e >> node.l >> node.service_time) {
        customer_all_vector.push_back(node);
    }
    file.close();

    VEHICLE_NUMBER = vehicle_number;
    VEHICLE_CAPACITY = vehicle_capacity;
}
// Function to print nodes


void printNodes() {
    cout << "Nodes:" << endl;
    for (const auto& node : customer_all_vector) {
        cout << "ID: " << node.id << ", X: " << node.x_coord << ", Y: " << node.y_coord << ", Demand: " << node.demand << ", Earliest Start Time: " << node.e << ", Latest Start Time: " << node.l << ", Service Time: " << node.service_time << endl;
    }
    cout << endl;
}

void init_distance_matrix() {
    //首先把distance_matrix矩阵清空
    distance_matrix.clear();

    // Find the maximum ID to determine the size of the distance matrix
    int max_id = customer_all_vector.size()+1;
    // Resize the distance matrix to accommodate all IDs
    distance_matrix.resize(max_id + 1, vector<double>(max_id + 1, 0.0));
    
    // Create a vector to hold all nodes, with the depot as the last element
    vector<Node*> nodes = { &depot };
    for (auto& customer : customer_all_vector) {
        nodes.push_back(&customer);
    }

    // Calculate the distance between each pair of nodes
    for (size_t i = 0; i < nodes.size(); ++i) {
        for (size_t j = i + 1; j < nodes.size(); ++j) {
            int id1 = nodes[i]->id;
            int id2 = nodes[j]->id;
            double distance = calculateDistance(*nodes[i], *nodes[j]);
            distance_matrix[id1][id2] = distance;
            distance_matrix[id2][id1] = distance; // Symmetric property
        }
    }
}

void Route::update_route_info()
{
    total_distance = 0;
    total_demand = 0;
    time_violation = 0;
    customer_number = nodes.size() - 2;
    

    if (nodes.size() == 2)
        return; // 处理空路线的情况

    for (int i = 0; i < nodes.size() - 1; i++)
    {
        total_distance += distance_matrix[nodes[i].id][nodes[i + 1].id];
        total_demand += nodes[i].demand;

        // 更新节点的实际到达时间和开始服务时间，以统计违反时间窗的总时长
        if (i == 0)
        {
            nodes[i].actual_arrive_time = 0;
            nodes[i].actual_start_time = nodes[i].e;
        }
        else
        {
            nodes[i].actual_arrive_time = nodes[i - 1].actual_start_time + nodes[i - 1].service_time + distance_matrix[nodes[i - 1].id][nodes[i].id];
            nodes[i].actual_start_time = std::max(nodes[i].actual_arrive_time, nodes[i].e);
        }

        if (nodes[i].actual_start_time > nodes[i].l)
        {
            time_violation += (nodes[i].actual_start_time - nodes[i].l);
        }
    }

    // 累加最后一个节点的需求量, 更新最后一个节点状态
    total_demand += nodes.back().demand;
    nodes.back().actual_arrive_time = nodes[nodes.size() - 2].actual_start_time + nodes[nodes.size() - 2].service_time + distance_matrix[nodes[nodes.size() - 2].id][nodes.back().id];
    nodes.back().actual_start_time = std::max(nodes.back().actual_arrive_time, nodes.back().e);
    if (nodes.back().actual_start_time > nodes.back().l)
    {
        time_violation += (nodes.back().actual_start_time - nodes.back().l);
    }
}
