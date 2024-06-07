#pragma once

#include "param.h"
#include <cstdlib> // Include the <cstdlib> header for the rand() function

// 定义一个解的表达
class Solution
{
public:
    vector<Route> route_vector;
    double distance_all;       // 所有路径的行驶距离
    double capacity_violation; // 违反的容量约束
    double time_violation;     // 违反的时间窗约束
    int route_used_num;        // 使用的车辆路径数量

    int available_route_num; //所有的可用车辆数==VEHICLE_NUMBER
    int visited_customer_num; //被服务的客户数量

    double f_s;//作为评价的函数值
    double f_p;//作为评价的函数值
    double f;//作为评价的函数值
    double execution_time;


public:
    void init_solution();
    void init_solution_over_time();//生成的初始解有违反时间窗约束
    void init_solution_no_time_over();//生成的初始解没有违反时间窗约束
    void init_solution_over_time_random();
    
    void add_new_route();        // 添加一条空路径
    
    bool check_feasible(); // 检查解是否可行 有无违反时间窗，容量约束
    
    void update_visited_node_route_matrix(int node_id,int route_id);//更新节点使用矩阵
    
    double calculate_score_c();//计算消耗的时间
    double calculate_score_f_s(); // 计算评价的目标函数值
    double calculate_score_p(); // 计算评价当前解diversity的目标函数值

    void update_solution_info(); // 更新解的信息，总的路程 车辆数等
    void print_solution_info(); // 打印解的信息


    void tabu_search_run();//执行禁忌搜索
    void apply_operator(void (Solution::*operator_func)(), double &improvement);
    int choose_operator();
    void insert_node(int k_from,int n_from,int k_to,int n_to);//定义插入操作
    
    void Reloacation();//定义重新插入算子
    void relocation_update_info(int k_from, int k_to);

    void exchange_1();//定义路线内的交换节点操作
    void exchange_1_update_info(int route_k,int node_index_1,int node_index_2);
    void exchange_2();//两条路线之间节点的交换
    void exchange_2_update_info(int k_from, int k_to);

    void save_output(string out_filename);//把当前解保存到文件


public:
    Solution()
    {
        distance_all = 0;
        capacity_violation = 0;
        time_violation = 0;
        route_used_num = 0;
        available_route_num = VEHICLE_NUMBER;

        // 添加一条空路径，至少要有一条路径吧
        add_new_route();
    }
    // 深拷贝构造函数
    Solution(const Solution& other)
        : distance_all(other.distance_all),
          capacity_violation(other.capacity_violation),
          time_violation(other.time_violation),
          route_used_num(other.route_used_num),
          available_route_num(other.available_route_num),
          visited_customer_num(other.visited_customer_num),
          f_s(other.f_s),
          f_p(other.f_p), 
          f(other.f){
        // 深拷贝 route_vector
        route_vector.reserve(other.route_vector.size());
        for (const auto& route : other.route_vector) {
            route_vector.push_back(Route(route)); // 使用 Route 的深拷贝构造函数
        }
    }

    // 深拷贝赋值运算符
    Solution& operator=(const Solution& other) {
        if (this == &other) return *this;

        distance_all = other.distance_all;
        capacity_violation = other.capacity_violation;
        time_violation = other.time_violation;
        available_route_num = other.available_route_num;
        route_used_num = other.route_used_num;
        visited_customer_num = other.visited_customer_num;
        f_s = other.f_s;
        f_p = other.f_p;
        f = other.f;

        // 深拷贝 route_vector
        route_vector.clear();
        route_vector.reserve(other.route_vector.size());
        for (const auto& route : other.route_vector) {
            route_vector.push_back(Route(route)); // 使用 Route 的深拷贝构造函数
        }

        return *this;
    }

    //重载==运算符用于比较两个solution是否相等
    bool operator==(const Solution& other) const {
        if (route_vector.size() != other.route_vector.size())
            return false;
        for (int i = 0; i < route_vector.size(); i++) {
            if (!(route_vector[i] == other.route_vector[i]))
                return false;
        }
        return true;
    }
};

//声明一个全局最好的解和他的评价函数
extern Solution Best_sol;
extern double Best_sol_c;
extern int iter_count;

// 定义禁忌表，行表示节点 ID，列表示路线索引
extern std::vector<std::vector<int>> insert_tabu_list;
// 定义新增路线产生的禁忌表，节点插入新路线之后再禁忌步长内不能取出来
extern std::vector<int> new_route_tabu_list;

// 定义exchange 禁忌表 保存交换的两个节点的id
extern std::vector<std::vector<int>> exchange_tabu_list;


void update_parameters(const Solution &sol);//更新参数alpha b_beta

void save_result(const Solution &res_sol,string instance_name);