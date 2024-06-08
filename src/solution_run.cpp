#include "solution.h"
#include <iostream>
#include <vector>
#include <fstream>

// 定义全局最好的解和它的评价函数
Solution Best_sol;
double Best_sol_c = double(0xfffff);

// 获取当前迭代次数
int iter_count = 0;

// 定义禁忌表
std::vector<std::vector<int>> insert_tabu_list(ALL_CUSTOMER_NUMBER + 10, std::vector<int>(VEHICLE_NUMBER, 0));
std::vector<int> new_route_tabu_list(ALL_CUSTOMER_NUMBER + 10, 0);
std::vector<std::vector<int>> exchange_tabu_list(ALL_CUSTOMER_NUMBER + 10, std::vector<int>(ALL_CUSTOMER_NUMBER + 2, 0));

// 改进记录
double relocation_improvement = 0.5;
double exchange1_improvement = 0.25;
double exchange2_improvement = 0.25;

// 算子的权重
std::vector<double> operator_weights = {relocation_improvement, exchange1_improvement, exchange2_improvement};

// 记录数据
std::vector<double> f_values;
std::vector<double> f_s_values;
std::vector<double> f_p_values;
std::vector<double> distance_values;
std::vector<double> time_violation_values;
std::vector<double> capacity_violation_values;


void Solution::tabu_search_run()
{
    if (check_feasible())
    {
        Best_sol = *this;
        Best_sol_c = distance_all;
    }

    for (int iter = 0; iter < ITER_MAX; ++iter)
    {
        iter_count = iter;
        int operator_choice = choose_operator();

        switch (operator_choice)
        {
        case 0:
            Reloacation();
            break;
        case 1:
            exchange_1();
            break;
        case 2:
            exchange_2();
            break;
        default:
            break;
        }

        // 记录数据
        f_values.push_back(f);
        f_s_values.push_back(f_s);
        f_p_values.push_back(f_p);
        distance_values.push_back(distance_all);
        time_violation_values.push_back(time_violation);
        capacity_violation_values.push_back(capacity_violation);

       // std::cout << "Iter: " << iter << ", f: " << f << ", f_s: " << f_s << ", f_p: " << f_p << ", distance: " << distance_all << ", time_violation: " << time_violation << ", capacity_violation: " << capacity_violation << std::endl;
    }

    // 保存数据到文件
    std::ofstream outfile("tabu_search_data_2.csv", std::ios_base::app);
    if (outfile.tellp() == 0) {
        // 只有在文件为空时才写入标题行
        outfile << "RANDOM_SEED,CUSTOMER_NUM,filename,Iter,f,f_s,f_p,distance,time_violation,capacity_violation\n";
    }
    for (int i = 0; i < ITER_MAX; ++i)
    {
        outfile << RANDOM_SEED << "," << ALL_CUSTOMER_NUMBER << "," << file_name_no_suffix << "," << i << "," << f_values[i] << "," << f_s_values[i] << "," << f_p_values[i] << "," << distance_values[i] << "," << time_violation_values[i] << "," << capacity_violation_values[i] << "\n";
    }
    outfile.close();
}

int Solution::choose_operator()
{
    if (iter_count < 800)
    {
        return 0; // 前400次迭代选择Relocation算子
    }
    else if (iter_count < 1000)
    {
        return 1; // 400到800次迭代选择Exchange_1算子
    }
    else if (iter_count < 1400)
    {
        return 2; // 800到1200次迭代选择Exchange_2算子
    }
    else if (iter_count < 1800)
    {
        int range = (iter_count - 1200) / 40;
        return range % 3; // 1200到1600次迭代每隔50次切换算子
    }
    else
    {
        // 动态调整权重，基于算子表现选择算子
        double total_weight = relocation_improvement + exchange1_improvement + exchange2_improvement;
        double rand_val = (double)rand() / RAND_MAX * total_weight;
        if (rand_val < relocation_improvement)
        {
            return 0;
        }
        else if (rand_val < relocation_improvement + exchange1_improvement)
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }
}
