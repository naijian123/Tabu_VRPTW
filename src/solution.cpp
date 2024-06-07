#include "solution.h"
#include <iostream>
#include <cmath> // Include the <cmath> header file
#include <random>
#include <cstdlib>
#include <algorithm>

using namespace std;

bool can_insert_node(Node &node_temp, Route &route, int position);

void Solution::add_new_route()
{
    if (route_vector.size() < VEHICLE_NUMBER)
    {
        this->route_vector.push_back(Route(depot));
    }
    else
    {
        cout << "over vehicle num" << endl;
        // return;
    }
}

void Solution::update_solution_info()
{
    // 首先更新每条路径的信息
    for (int i = 0; i < this->route_vector.size(); i++)
    {
        this->route_vector[i].update_route_info();
    }

    // 更新解的信息
    this->distance_all = 0;
    this->capacity_violation = 0;
    this->time_violation = 0;
    this->route_used_num = 0;
    this->visited_customer_num = 0;
    for (int i = 0; i < this->route_vector.size(); i++)
    {
        this->distance_all += this->route_vector[i].total_distance;
        this->capacity_violation += max(0.0, this->route_vector[i].total_demand - this->route_vector[i].max_capacity);
        this->time_violation += this->route_vector[i].time_violation;
        this->visited_customer_num += this->route_vector[i].customer_number;
        if (this->route_vector[i].customer_number > 0)
        {
            this->route_used_num++;
        }
    }
    f_s = calculate_score_f_s();
    f_p = calculate_score_p();
    f = f_s + f_p;
    // this->route_used_num = this->route_vector.size();
}

// 打印解的信息
void Solution::print_solution_info()
{
    cout << "Distance All: " << this->distance_all << endl;
    cout << "Capacity Violation: " << this->capacity_violation << endl;
    cout << "Time Violation: " << this->time_violation << endl;
    cout << "Route Used Num: " << this->route_used_num << endl;
    cout << endl;
    // 格式化输出每条路径的具体信息 ,每条路径占用2行
    for (int i = 0; i < this->route_vector.size(); i++)
    {
        cout << "Route " << i << " : " << endl;
        cout << "Total Distance: " << this->route_vector[i].total_distance << "\t";
        cout << "Total Demand: " << this->route_vector[i].total_demand << "\t";
        cout << "Time Violation: " << this->route_vector[i].time_violation << "\t";
        cout << "Customer Number: " << this->route_vector[i].customer_number << "\t";
        cout << "Nodes: ";
        for (int j = 0; j < this->route_vector[i].nodes.size(); j++)
        {
            cout << this->route_vector[i].nodes[j].id << " ";
        }
        cout << endl;
    }
}

bool Solution::check_feasible()
{
    // 首先刷新信息
    // update_solution_info();
    // 检查是否有违反时间窗约束和容量约束
    if (this->capacity_violation == 0 && this->time_violation == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// 更新节点使用状态
void Solution::update_visited_node_route_matrix(int node_id, int route_id)
{
    visited_node_route_matrix[node_id][route_id] += 1;
}

double Solution::calculate_score_c()
{
    return this->distance_all;
}
double Solution::calculate_score_f_s()
{
    double duration_over = 0;
    for(auto route:route_vector)
    {
        if(route.total_distance > DURATION)
        {
            duration_over += route.total_distance - DURATION;
        }
    }
    double f_s_temp = distance_all + alpha * this->capacity_violation + b_beta * this->time_violation + g_gamma * duration_over;
    // 加入以timeviolation 为评价标准
    // f = time_violation;
    f_s = f_s_temp;

    return f_s_temp;
}

// 计算评价当前解diversity的目标函数值,暂时先不用这个评价指标
double Solution::calculate_score_p()
{
    //return 0;
    double node_visited = 0;
    for (int k = 0; k < route_vector.size(); k++)
    {
        for (int i = 1; i < route_vector[k].nodes.size() - 1; i++)
        {
            node_visited += visited_node_route_matrix[route_vector[k].nodes[i].id][k];
        }
    }
   // cout<<node_visited<<endl;
    double p = lambda * distance_all * sqrt(route_used_num * visited_customer_num) * node_visited;
    f_p = p;
    return p;
}

void Solution::init_solution()
{
}
void Solution::init_solution_no_time_over()
{
    srand(RANDOM_SEED);

    // 打乱客户索引顺序
    std::vector<int> customer_indices(ALL_CUSTOMER_NUMBER);
    std::iota(customer_indices.begin(), customer_indices.end(), 0);
    std::shuffle(customer_indices.begin(), customer_indices.end(), std::default_random_engine(RANDOM_SEED));

    for (int customer_index : customer_indices)
    { 
        bool customer_assigned = false;
        for (int k = 0; k < VEHICLE_NUMBER && !customer_assigned; k++)
        {
            if (k >= this->route_vector.size())
            {
                add_new_route();
            }

            Route &current_route = this->route_vector[k];

            // 检查是否超过载重
            if (current_route.total_demand + customer_all_vector[customer_index].demand > current_route.max_capacity)
            {
                continue;
            }

            for (int i = 1; i < current_route.nodes.size(); i++)
            {

                // 计算插入后新的总距离
                double new_distance = distance_matrix[current_route.nodes[i - 1].id][customer_all_vector[customer_index].id] +
                                      distance_matrix[customer_all_vector[customer_index].id][current_route.nodes[i].id] -
                                      distance_matrix[current_route.nodes[i - 1].id][current_route.nodes[i].id];
                if (current_route.total_distance + new_distance > current_route.max_distance)
                {
                    continue;
                }

                // 尝试在临时路线中插入新的客户节点
                Route temp_route = current_route;
                temp_route.nodes.insert(temp_route.nodes.begin() + i, customer_all_vector[customer_index]);
                temp_route.update_route_info();

                // 检查插入后是否违反时间窗
                if (temp_route.time_violation == 0)
                {
                    // 更新正式路线信息
                    customer_all_vector[customer_index].route_id = k;
                    this->route_vector[k] = std::move(temp_route); // 使用移动语义提高效率
                    customer_assigned = true;
                    break;
                }
            }

            // 如果没有找到合适的插入位置，且当前路径是最后一条路径，则新建一条路径
            if (!customer_assigned && k == this->route_vector.size() - 1 && this->route_vector.size() < VEHICLE_NUMBER)
            {
                add_new_route();
                customer_all_vector[customer_index].route_id = route_vector.size() - 1;
                // 插入到新添加的路径的第二个位置
                route_vector.back().nodes.insert(route_vector.back().nodes.begin() + 1, customer_all_vector[customer_index]);
                route_vector.back().update_route_info();
                customer_assigned = true;
                break;
            }
            // 如果已经达到最大路径限制了，就把他插入到最后一条路径上，即便会违反约束
            else if ((!customer_assigned && k == this->route_vector.size() - 1) && this->route_vector.size() == VEHICLE_NUMBER)
            {
                customer_all_vector[customer_index].route_id = k;
                // 插入到倒数最后一个位置之前
                route_vector[k].nodes.insert(route_vector[k].nodes.end() - 1, customer_all_vector[customer_index]);
                route_vector[k].update_route_info();
                customer_assigned = true;
                break;
            }
   
        }
    }
    // 更新并打印解的信息
    while (route_vector.size() < VEHICLE_NUMBER)
    {
        add_new_route();
    }
    update_solution_info();
    print_solution_info();
}

void Solution::init_solution_over_time()
{
    // 固定随机数种子
    srand(RANDOM_SEED); // Replace 123 with your desired seed value

    // 获取所有客户节点数量
    int all_customer_num = ALL_CUSTOMER_NUMBER; // customer_all_vector.size();

    // 随机选取一个节点的索引作为起始点，向路径中开始插入
    int start_index = rand() % all_customer_num;

    // 遍历所有客户节点并构造路径
    for (int index = 0; index < all_customer_num; ++index)
    {
        // 计算当前客户索引
        int customer_index = (start_index + index) % all_customer_num;

        // 尝试将客户插入路径
        bool inserted = false;
        for (int k = 0; k < VEHICLE_NUMBER; ++k)
        {
            if (k >= this->route_vector.size())
            {
                add_new_route();
            }
            // 判断容量和里程是否超过路径限制
            if (route_vector[k].total_demand + customer_all_vector[customer_index].demand > route_vector[k].max_capacity)
            {
                continue;
            }

            // 遍历路径中的节点，寻找插入点
            for (int i = 1; i < route_vector[k].nodes.size(); i++)
            {

                // 首先检查如果插入到i的位置总里程会不会超过上限
                double new_distance = distance_matrix[route_vector[k].nodes[i - 1].id][customer_all_vector[customer_index].id] + distance_matrix[customer_all_vector[customer_index].id][route_vector[k].nodes[i].id] - distance_matrix[route_vector[k].nodes[i - 1].id][route_vector[k].nodes[i].id];
                if (route_vector[k].total_distance + new_distance > route_vector[k].max_distance)
                {
                    continue;
                }
                // 计算插入后的时间
                double arrive_time = route_vector[k].nodes[i - 1].actual_start_time + route_vector[k].nodes[i - 1].service_time + distance_matrix[route_vector[k].nodes[i - 1].id][customer_all_vector[customer_index].id];
                double start_time = max(arrive_time, customer_all_vector[customer_index].e);

                // 检查时间窗约束
                // if (start_time > customer_all_vector[customer_index].l ||
                //     start_time + customer_all_vector[customer_index].service_time + distance_matrix[route_vector[k].nodes[i].id][customer_all_vector[customer_index].id] > route_vector[k].nodes[i].l)
                //     continue;
                if (i == route_vector[k].nodes.size() - 1)
                {
                    if (route_vector[k].nodes[i - 1].e + route_vector[k].nodes[i - 1].service_time > customer_all_vector[customer_index].e || customer_all_vector[customer_index].e + customer_all_vector[customer_index].service_time > route_vector[k].nodes[i].l)
                    {
                        continue;
                    }
                }
                else
                {
                    if (route_vector[k].nodes[i - 1].e + route_vector[k].nodes[i - 1].service_time > customer_all_vector[customer_index].e || customer_all_vector[customer_index].e + customer_all_vector[customer_index].service_time > route_vector[k].nodes[i].e)
                    {
                        continue;
                    }
                }

                // 插入节点
                customer_all_vector[customer_index].route_id = k;
                route_vector[k].nodes.insert(route_vector[k].nodes.begin() + i, customer_all_vector[customer_index]);
                route_vector[k].update_route_info();
                inserted = true;
                break;
            }

            if (inserted)
            {
                break;
            }
        }

        // 如果无法插入到已有路径中，则新建一条路径
        if (!inserted)
        {
            if (route_vector.size() < VEHICLE_NUMBER)
            {
                add_new_route();
            }
            else
            {
                cout << "无法插入到已有路径中,且已达到最大车辆数目" << endl;
                break;
            }
        }
    }
    while (route_vector.size() < VEHICLE_NUMBER)
    {
        add_new_route();
    }

    // 更新解的信息
    update_solution_info();
    // 打印解的信息
    // print_solution_info();
}

void Solution::init_solution_over_time_random()
{
    // 初始化随机数种子
    std::srand(RANDOM_SEED);

    // 初始化客户节点列表
    std::vector<int> customer_nodes_index_vector;
    for (int i = 0; i < ALL_CUSTOMER_NUMBER; ++i)
    {
        customer_nodes_index_vector.push_back(i);
    }

    std::default_random_engine rng(RANDOM_SEED);
    // 打乱客户节点列表，实现随机选择节点
    std::shuffle(customer_nodes_index_vector.begin(), customer_nodes_index_vector.end(), rng);

    // 遍历每个客户节点，尝试随机插入到某条路径中
    for (int i = 0; i < customer_nodes_index_vector.size(); ++i)
    {
        int node_index = customer_nodes_index_vector[i];
        // cout << customer_all_vector[node_index].id << "\t";

        bool inserted = false;

        // 获取当前解的路径数量，并生成一个包含所有路径索引的列表
        int route_count = route_vector.size();
        std::vector<int> route_indices(route_count);
        for (int j = 0; j < route_count; j++)
        {
            route_indices[j] = j;
        }

        // 打乱路径索引列表，实现随机选择路径
        std::shuffle(route_indices.begin(), route_indices.end(), rng);

        // 尝试将节点插入到随机选择的路径中
        for (int j = 0; j < route_count; j++)
        {
            int route_index = route_indices[j];
            Route &route = route_vector[route_index];

            // 尝试在该路径中的随机位置插入节点
            bool inserted_in_route = false;
            int node_count = route.nodes.size();
            for (int k = 1; k < node_count; ++k)
            {
                // 这里假设插入节点不会破坏时间窗约束和容量约束
                if (can_insert_node(customer_all_vector[node_index], route, k))
                {
                    route.nodes.insert(route.nodes.begin() + k, customer_all_vector[node_index]);
                    update_solution_info();
                    inserted_in_route = true;
                    inserted = true;
                    break;
                }
            }

            if (inserted_in_route)
            {
                break;
            }
        }

        // 如果所有现有路径都无法插入，则新建一条路径
        if (!inserted)
        {
            add_new_route();
            // 在最后添加的路径中插入节点
            route_vector.back().nodes.insert(route_vector.back().nodes.begin() + 1, customer_all_vector[node_index]);
            update_solution_info();
        }
    }

    // 如果生成的解使用的路径数量没有达到可用的数量时，补充空路径到可用数量。
    while (route_vector.size() < VEHICLE_NUMBER)
    {
        add_new_route();
    }

    // 更新解的信息
    update_solution_info();
    // print_solution_info();
}
// 把当前最优解的信息保存到文件
void Solution::save_output(string out_filename)
{
    ofstream outfile(out_filename);
    if (!outfile)
    {
        cout << "Cannot open output file." << endl;
        return;
    }

    // Write solution information to the output file
    outfile << "Distance All: " << this->distance_all << endl;
    outfile << "Capacity Violation: " << this->capacity_violation << endl;
    outfile << "Time Violation: " << this->time_violation << endl;
    outfile << "Route Used Num: " << this->route_used_num << endl;
    // 服务的用户数量
    outfile << "Visited Customer Num: " << ALL_CUSTOMER_NUMBER << endl;
    outfile << endl;

    // Write each route's information to the output file
    for (int i = 0; i < this->route_vector.size(); i++)
    {
        outfile << "Route " << i << " : " << endl;
        outfile << "Total Distance: " << this->route_vector[i].total_distance << "\t";
        outfile << "Total Demand: " << this->route_vector[i].total_demand << "\t";
        outfile << "Time Violation: " << this->route_vector[i].time_violation << "\t";
        outfile << "Customer Number: " << this->route_vector[i].customer_number << "\t";
        outfile << "Nodes: ";
        for (int j = 0; j < this->route_vector[i].nodes.size(); j++)
        {
            outfile << this->route_vector[i].nodes[j].id << " ";
        }
        outfile << endl;
    }

    // 输出执行时间
    outfile << "================================================================================= " << endl;
    outfile << "Execution Time: " << this->execution_time << " s " << endl;

    outfile.close();
}

bool can_insert_node(Node &node_temp, Route &route, int position)
{
    // 如果一条路径没有用户节点则一定可以插入
    if (route.customer_number == 0)
    {
        return true;
    }

    // 假设Node类有时间窗属性 e 和 l，和需求量 demand
    // 检查插入节点是否符合时间窗和容量约束
    Node &prev_node = route.nodes[position - 1];
    Node &next_node = route.nodes[position];

    // 计算插入导致路径长度增加量
    double delta_distance = distance_matrix[node_temp.id][route.nodes[position].id] + distance_matrix[node_temp.id][route.nodes[position - 1].id] - distance_matrix[route.nodes[position].id][route.nodes[position - 1].id];

    // 这里简化了时间窗和容量约束的判断逻辑
    // 可以根据具体问题进行详细实现
    if (position == route.customer_number - 1)
    {
        if (prev_node.e <= node_temp.e && node_temp.e <= next_node.l &&
            delta_distance + route.total_distance <= DURATION - (route.nodes.size() - 1) * 10 &&
            route.total_demand + node_temp.demand <= VEHICLE_CAPACITY)
        {
            return true;
        }
    }
    else
    {
        if (prev_node.e <= node_temp.e && node_temp.e <= next_node.e &&
            delta_distance + route.total_distance <= DURATION - (route.nodes.size() - 1) * 10 &&
            route.total_demand + node_temp.demand <= VEHICLE_CAPACITY)
        {
            return true;
        }
    }

    return false;
}