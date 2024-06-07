#include "solution.h"
#include <iostream>
#include <algorithm>
// 插入算子,从每条路径中选出一个节点插入到另一条路径中，并且插入的位置要在前后的时间窗开始之内，并计算f(s)的大小，选出最小的f(s)作为局部最优解
using namespace std;


void Solution::Reloacation()
{
    // 将当前的解复制一份,以当前的解为基准局部搜索
    Solution current_sol = *this;

    Solution local_best_sol = *this;
    double local_best_f = INF_DOUBLE;
    double local_best_c = distance_all;
    int node_id_local_best = 0;
    int k_from_local_best = 0;
    int n_from_local_best = 0;
    int k_to_local_best = 0;
    int n_to_local_best = 0;
    bool local_best_is_new_route = false; // 判断当前局部最优是不是添加新路径产生的
    int new_route_create_customer_id = 0; // 记录被插入到空路径的结点的id

    for (int k_from = 0; k_from < current_sol.route_vector.size(); k_from++) // 取出节点的路径
    {                                                                        // 如果是空路径，不用考虑取出来节点插入
        if (current_sol.route_vector[k_from].nodes.size() == 2)
        {
            continue;
        }
        for (int n_from = 1; n_from < current_sol.route_vector[k_from].nodes.size() - 1; n_from++) // 取出节点的索引
        {
            int node_id = current_sol.route_vector[k_from].nodes[n_from].id;
            for (int k_to = 1; k_to < current_sol.route_vector.size(); k_to++) // 插入节点的路径
            {
                // 如果只考虑在不同的路径之间插入的话
                // if (k_from == k_to)
                // {
                //     continue;
                // }
                for (int n_to = 1; n_to < current_sol.route_vector[k_to].nodes.size(); n_to++) // 插入节点的索引，插入在n_to前面
                {
                    // 判断ei <= ej <= ej+1
                    if (k_from == k_to) // 如果是在同一条路径内插入
                    {
                        if (n_to == n_from || n_to == n_from + 1) // 同一个位置，没有意义，塞到后一个节点前面也同样没有意义
                        {
                            continue;
                        }
                    }
                    if (n_to == current_sol.route_vector[k_to].nodes.size() - 1) // 被插入的位置是最后一个节点
                    {
                        if (current_sol.route_vector[k_from].nodes[n_from].e < current_sol.route_vector[k_to].nodes[n_to - 1].e)
                        {
                            continue;
                        }
                    }
                    else
                    {
                        if (current_sol.route_vector[k_from].nodes[n_from].e < current_sol.route_vector[k_to].nodes[n_to - 1].e || current_sol.route_vector[k_to].nodes[n_to].e < current_sol.route_vector[k_from].nodes[n_from].e)
                        {
                            continue;
                        }
                    }
                    //判断插入后新路径会不会超过duration
                    //  double delta_distance = distance_matrix[current_sol.route_vector[k_from].nodes[n_from].id][current_sol.route_vector[k_to].nodes[n_to - 1].id] + distance_matrix[current_sol.route_vector[k_from].nodes[n_from].id][current_sol.route_vector[k_to].nodes[n_to].id] - distance_matrix[current_sol.route_vector[k_to].nodes[n_to - 1].id][current_sol.route_vector[k_to].nodes[n_to].id];
                    //  if (current_sol.route_vector[k_to].total_distance + delta_distance > DURATION-(current_sol.route_vector[k_to].nodes.size()-1)*10)
                    //  {
                    //      continue;
                    //  }

                    // 判断插入后新路径会不会超过capacity
                    double delta_demand = current_sol.route_vector[k_from].nodes[n_from].demand;
                    if (current_sol.route_vector[k_to].total_demand + delta_demand > VEHICLE_CAPACITY)
                    {
                        continue;
                    }

                    // 判断插入的是不是新路径，采用禁忌表判断，并考虑到存在藐视准则
                    bool is_new_route_tabu = false;
                    bool is_new_route = false;
                    if (current_sol.route_vector[k_to].nodes.size() == 2)
                    {
                        is_new_route = true;
                        is_new_route_tabu = new_route_tabu_list[node_id] > iter_count;
                    }

                    current_sol.insert_node(k_from, n_from, k_to, n_to);
                    current_sol.relocation_update_info(k_from, k_to);
                    // 检查禁忌表，并考虑藐视准则
                    bool is_tabu = insert_tabu_list[node_id][k_to] > iter_count;
                    bool aspiration_criterion = current_sol.calculate_score_c() < Best_sol_c && current_sol.check_feasible();

                    if ((is_tabu || is_new_route_tabu) && !aspiration_criterion)
                    { // 被禁忌但是没有比最优解好
                        current_sol = *this;
                        // current_sol.update_solution_info();
                        continue;
                    }
                    if (aspiration_criterion)
                    {
                        Best_sol = current_sol;
                        Best_sol_c = current_sol.calculate_score_c();
                    }
                    double f_temp = current_sol.f;
                    if (f_temp < local_best_f) // 更新局部最优解
                    {
                        local_best_sol = current_sol;
                        local_best_f = f_temp;
                        local_best_c = current_sol.distance_all;
                        node_id_local_best = node_id;
                        k_from_local_best = k_from;
                        k_to_local_best = k_to;
                        n_from_local_best = n_from;
                        n_to_local_best = n_to;

                        // 每次迭代找局部最优解，时候发现的当前候选的解是否是插入新路径，更新一下是否是新路径
                        if (is_new_route)
                        {
                            local_best_is_new_route = true;
                            new_route_create_customer_id = node_id;
                        }
                        else
                        {
                            local_best_is_new_route = false;
                        }
                    }

                    current_sol = *this;
                    // current_sol.update_solution_info();

                } // end n_to
            } // end k_to
        } // end n_from
    } // end k_from

    local_best_sol.update_solution_info();
    update_parameters(local_best_sol);
    update_visited_node_route_matrix(node_id_local_best, k_to_local_best);
    // 更新禁忌表
    insert_tabu_list[node_id_local_best][k_to_local_best] = iter_count + TABU_TENURE;
    insert_tabu_list[node_id_local_best][k_from_local_best] = iter_count + TABU_TENURE;
    if (local_best_is_new_route)
    {
        new_route_tabu_list[new_route_create_customer_id] = iter_count + TABU_TENURE;
    }

    /* 搜索结束，把当前的局部最优解给自身，用于下一次算子操作。
    // if(*this==local_best_sol)
    // {
    //     cout<<"++++++++++++ error relocation no find local best    +++++++++++++++++++++++++"<<endl;
    // }*/

    // 如果找到了可行解，就更新最好的可行解
    if (local_best_sol.calculate_score_c() < Best_sol_c && local_best_sol.check_feasible()) // 如果当前的解比最好的可行解还好就给最好的可行解
    {
        // 打印发现可行解
       // cout << "find feasible solution" << endl;
        Best_sol = local_best_sol;
        Best_sol_c = local_best_sol.calculate_score_c();
    }

    *this = local_best_sol;

    // 打印一下本次的操作，把那个节点插入哪
    //std::cout << "Insert  (Route, n_from):(" << k_from_local_best << "," << n_from_local_best << ")\t" << "  Node: " << node_id_local_best << "\t to (Route,n_to): (" << k_to_local_best << "," << n_to_local_best << ")\t";
}

// 尝试再同一条路线内交换两个用户节点
void Solution::exchange_1()
{
    // 将当前的解复制一份,以当前的解为基准局部搜索
    Solution current_sol = *this;

    Solution local_best_sol = *this;
    double local_best_f = INF_DOUBLE;
    double local_best_c = distance_all;
    int exchange_node_id_1=-1;
    int exchange_node_id_2=-1;
    int k_exchange_local_best = 0;

    // 遍历每条路径
    for (int k = 0; k < current_sol.route_vector.size(); k++)
    {
        // 如果路线只有1个用户没有交换的必要
        if (current_sol.route_vector[k].nodes.size() <= 3)
        {
            continue;
        }
        // 获取当前路径上的节点数量
        int node_count = current_sol.route_vector[k].nodes.size();

        // 遍历每对节点
        for (int i = 1; i < node_count - 1; i++)
        {
            int node_id_1 = current_sol.route_vector[k].nodes[i].id;
            for (int j = i + 1; j < node_count - 1; j++)
            {
                int node_id_2 = current_sol.route_vector[k].nodes[j].id;
                // 交换节点i和节点j的位置
                std::swap(current_sol.route_vector[k].nodes[i], current_sol.route_vector[k].nodes[j]);

                // 更新解的信息,因为只涉及到路线k，且只涉及到时间窗违反时间
                current_sol.exchange_1_update_info(k,i,j);

                // 判断当前交换操作是否被禁忌，并考虑藐视准则
                bool is_tabu = exchange_tabu_list[node_id_1][node_id_2] > iter_count;
                bool aspiration_criterion = current_sol.calculate_score_c() < Best_sol_c && current_sol.check_feasible();

                if ((is_tabu || aspiration_criterion) && !aspiration_criterion)
                { // 被禁忌但是没有比最优解好
                    current_sol = *this;
                    continue;
                }
                if (aspiration_criterion)
                {
                    Best_sol = current_sol;
                    Best_sol_c = current_sol.calculate_score_c();
                }
                double f_temp = current_sol.f;
                if (f_temp < local_best_f) // 更新局部最优解
                {
                    local_best_sol = current_sol;
                    local_best_f = f_temp;
                    local_best_c = current_sol.distance_all;
                    exchange_node_id_1 = node_id_1;
                    exchange_node_id_2 = node_id_2;
                    k_exchange_local_best = k;
                }

                current_sol = *this;
                // current_sol.update_solution_info();
            }
        }
    }
    if(exchange_node_id_1==-1)
    {
        return;
        //这地方有bug出现存在一条路径找不到交换的...
    }
    local_best_sol.update_solution_info();
    update_parameters(local_best_sol);
    update_visited_node_route_matrix(exchange_node_id_1,k_exchange_local_best);
    update_visited_node_route_matrix(exchange_node_id_2,k_exchange_local_best);
    
    // 检查是否找到更好的解
    if (local_best_c < Best_sol_c && local_best_sol.check_feasible())
    {
        // 更新最好的解
        Best_sol = local_best_sol;
        Best_sol_c = local_best_c;
    }

    // 根据最好的交换操作更新禁忌表
    exchange_tabu_list[exchange_node_id_1][exchange_node_id_2] = iter_count + TABU_TENURE;
    exchange_tabu_list[exchange_node_id_2][exchange_node_id_1] = iter_count + TABU_TENURE;
    *this = local_best_sol;
    // this->update_solution_info();
}

void Solution::exchange_1_update_info(int route_k,int node_index_1,int node_index_2)
{
    
    this->distance_all -= this->route_vector[route_k].total_distance;
    this->time_violation -= this->route_vector[route_k].time_violation;

    this->route_vector[route_k].update_route_info();

    this->time_violation += this->route_vector[route_k].time_violation ;
    this->distance_all += this->route_vector[route_k].total_distance ;
    
    
    f_s = calculate_score_f_s();
    f_p = calculate_score_p();
    f = f_p + f_s;


}

// 在两条路线之中交换节点的算子操作
void Solution::exchange_2()
{
    // 将当前的解复制一份,以当前的解为基准局部搜索
    Solution current_sol = *this;

    Solution local_best_sol = *this;
    double local_best_f = INF_DOUBLE;
    double local_best_c = distance_all;
    int exchange_node_id_1=-1;
    int exchange_node_id_2=-1;
    int k_from_local_best = 0;
    int n_from_local_best = 0;
    int k_to_local_best = 0;
    int n_to_local_best = 0;

    // 遍历每对路径
    for (int k_from = 0; k_from < current_sol.route_vector.size(); k_from++)
    {
        for (int k_to = k_from + 1; k_to < current_sol.route_vector.size(); k_to++)
        {
            // 获取当前路径上的节点数量
            int node_count_from = current_sol.route_vector[k_from].nodes.size();
            int node_count_to = current_sol.route_vector[k_to].nodes.size();

            // 遍历每对节点
            for (int n_from = 1; n_from < node_count_from - 1; n_from++)
            {
                int node_id_1 = current_sol.route_vector[k_from].nodes[n_from].id;
                for (int n_to = 1; n_to < node_count_to - 1; n_to++)
                {
                    int node_id_2 = current_sol.route_vector[k_to].nodes[n_to].id;
                    // 交换节点n_from和节点n_to的位置
                    std::swap(current_sol.route_vector[k_from].nodes[n_from], current_sol.route_vector[k_to].nodes[n_to]);

                    // 更新解的信息
                    current_sol.exchange_2_update_info(k_from,k_to);

                    // 判断当前交换操作是否被禁忌，并考虑藐视准则
                    bool is_tabu = exchange_tabu_list[node_id_1][node_id_2] > iter_count;
                    bool aspiration_criterion = current_sol.calculate_score_c() < Best_sol_c && current_sol.check_feasible();

                    if ((is_tabu || aspiration_criterion) && !aspiration_criterion)
                    { // 被禁忌但是没有比最优解好
                        current_sol = *this;
                        continue;
                    }
                    if (aspiration_criterion)
                    {
                        Best_sol = current_sol;
                        Best_sol_c = current_sol.calculate_score_c();
                    }
                    double f_temp = current_sol.f;
                    if (f_temp < local_best_f) // 更新局部最优解
                    {
                        local_best_sol = current_sol;
                        local_best_f = f_temp;
                        local_best_c = current_sol.distance_all;
                        exchange_node_id_1 = node_id_1;
                        exchange_node_id_2 = node_id_2;
                        k_from_local_best = k_from;
                        n_from_local_best = n_from;
                        k_to_local_best = k_to;
                        n_to_local_best = n_to;
                    }

                    current_sol = *this;
                    // current_sol.update_solution_info();
                }
            }
        }
    }
    if(exchange_node_id_1==-1){
        return;
    }
    local_best_sol.update_solution_info();
    update_parameters(local_best_sol);

    //更新节点使用状态
    update_visited_node_route_matrix(exchange_node_id_1,k_to_local_best);
    update_visited_node_route_matrix(exchange_node_id_2,k_from_local_best);
    // 检查是否找到更好的解
    if (local_best_c < Best_sol_c && local_best_sol.check_feasible())
    {
        // 更新最好的解
        Best_sol = local_best_sol;
        Best_sol_c = local_best_c;
    }

    // 根据最好的交换操作更新禁忌表
    exchange_tabu_list[exchange_node_id_1][exchange_node_id_2] = iter_count + TABU_TENURE;
    exchange_tabu_list[exchange_node_id_2][exchange_node_id_1] = iter_count + TABU_TENURE;

    *this = local_best_sol;
    
}


void Solution::exchange_2_update_info(int k_from, int k_to)
{
    
    this->distance_all -= (this->route_vector[k_from].total_distance + this->route_vector[k_to].total_distance);
    this->time_violation -= (this->route_vector[k_from].time_violation + this->route_vector[k_to].time_violation);
    this->capacity_violation -= (max(this->route_vector[k_from].total_demand - route_vector[k_from].max_capacity, 0.0) + max(this->route_vector[k_to].total_demand - route_vector[k_to].max_capacity, 0.0) );
    
    this->route_vector[k_from].update_route_info();
    this->route_vector[k_to].update_route_info();

    this->time_violation += (this->route_vector[k_from].time_violation + this->route_vector[k_to].time_violation);
    this->capacity_violation += (max(this->route_vector[k_from].total_demand - route_vector[k_from].max_capacity, 0.0) + max(this->route_vector[k_to].total_demand - route_vector[k_to].max_capacity, 0.0) );
    this->distance_all += (this->route_vector[k_from].total_distance + this->route_vector[k_to].total_distance);
    
    
    f_s = calculate_score_f_s();
    f_p = calculate_score_p();
    f = f_s + f_p;

}

void Solution::insert_node(int k_from, int n_from, int k_to, int n_to)
{
    // 首先判断有没有越界
    bool over_index = false;
    if (k_from >= route_vector.size() || k_to >= route_vector.size() || n_from >= route_vector[k_from].nodes.size() || n_to >= route_vector[k_to].nodes.size())
    {
        over_index = true;
        std::cout << "insert function meet error, index over the limit" << std::endl; // Use 'std::cout' instead of 'cout'
        return;
    }
    // 开始把路线k_from 上n_from位置的结点，插入到k_to路线，位置n_to 上。
    if (k_from == k_to)
    {
        if (n_from == n_to)
        {
            return;
        }

        // 先插入到n_to,后删除n_from
        route_vector[k_to].nodes.insert(route_vector[k_to].nodes.begin() + n_to, route_vector[k_from].nodes[n_from]);
        if (n_from < n_to)
        {
            route_vector[k_from].nodes.erase(route_vector[k_from].nodes.begin() + n_from);
        }
        else
        {
            route_vector[k_from].nodes.erase(route_vector[k_from].nodes.begin() + n_from + 1);
        }
    }
    else
    {
        // 插入到不同路径

        // 插入到n_to位置
        route_vector[k_to].nodes.insert(route_vector[k_to].nodes.begin() + n_to, route_vector[k_from].nodes[n_from]);
        // 删除n_from位置
        route_vector[k_from].nodes.erase(route_vector[k_from].nodes.begin() + n_from);
    }
}


void update_parameters(const Solution &sol)
{
    if (sol.time_violation > 0 && b_beta < 2000)
        b_beta *= (1 + theta);
    else if (sol.time_violation == 0 && b_beta > 0.01)
        b_beta /= (1 + theta);

    if (sol.capacity_violation > 0 && alpha < 2000)
        alpha *= (1 + theta);
    else if (sol.capacity_violation == 0 && alpha > 0.01)
        alpha /= (1 + theta);
    double duration_over = 0;
    for(auto route:sol.route_vector)
    {
        if(route.total_distance>route.max_distance)
        {
            duration_over+=route.total_distance-route.max_distance;
        }
    }
   // cout<<"duration over: "<<duration_over<<endl;
    if (duration_over > 0 && g_gamma < 200)
        g_gamma *= (1 + theta);
    else if (duration_over == 0 && g_gamma > 0.01)
        g_gamma /= (1 + theta);
}
void Solution::relocation_update_info(int k_from, int k_to)
{
    if (this->route_vector[k_from].nodes.size() == 3)
    {
        this->route_used_num--;
    }
    if (this->route_vector[k_to].nodes.size() == 2)
    {
        this->route_used_num++;
    }

    this->distance_all -= (this->route_vector[k_from].total_distance + this->route_vector[k_to].total_distance);
    this->time_violation -= (this->route_vector[k_from].time_violation + this->route_vector[k_to].time_violation);
    this->capacity_violation -= (max(this->route_vector[k_from].total_demand - route_vector[k_from].max_capacity, 0.0) + max(this->route_vector[k_to].total_demand - route_vector[k_to].max_capacity, 0.0));

    this->route_vector[k_from].update_route_info();
    this->route_vector[k_to].update_route_info();

    this->time_violation += (this->route_vector[k_from].time_violation + this->route_vector[k_to].time_violation);
    this->capacity_violation += (max(this->route_vector[k_from].total_demand - route_vector[k_from].max_capacity, 0.0) + max(this->route_vector[k_to].total_demand - route_vector[k_to].max_capacity, 0.0));
    this->distance_all += (this->route_vector[k_from].total_distance + this->route_vector[k_to].total_distance);

    f_s = calculate_score_f_s();
    f_p = calculate_score_p();
    f = f_s + f_p;
    //this->route_used_num = this->route_vector.size();
}
