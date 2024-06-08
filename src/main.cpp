#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <time.h>
#include <fstream>
#include <filesystem>

// 加入头文件param.h,里面存放参数和类型定义
#include "solution.h"
#include "param.h"
using namespace std;

int main()
{

    // 取出../data/下所有数据文件的文件名
    vector<string> file_names = {"R101.txt", "R102.txt", "R103.txt", "R104.txt", "R105.txt", "R106.txt", "R107.txt", "R108.txt", "R109.txt", "R110.txt", "R111.txt", "R112.txt",
                                 "R201.txt", "R202.txt", "R203.txt", "R204.txt", "R205.txt", "R206.txt", "R207.txt", "R208.txt", "R209.txt", "R210.txt", "R211.txt"};
    
    string filename;
    vector<int> customer_num_vec = {100};
    vector<int> RANDOM_SEED_VECTOR = {110, 111, 112, 113, 114, 115, 116, 117, 118, 119,120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135};
    for (auto rd_seed : RANDOM_SEED_VECTOR)
    {
        RANDOM_SEED = rd_seed;//更新随机数种子

        for (int i = 0; i < file_names.size(); i++)
        {
            filename = file_names[i];
            file_name_no_suffix = filename.substr(0, filename.find(".txt"));
            // 打印当前正在处理的问题的信息
            cout << "--------------------------------------------------------------------------" << endl;
            cout << "read data from :" << filename << endl;
            // 重置保存用户节点的数组customer_all_vector
            customer_all_vector.clear();
            parseTestInstance("../data/" + filename);

            // 处理不同规模的问题
            for (auto customer_num : customer_num_vec)
            {

                ALL_CUSTOMER_NUMBER = customer_num;
                cout << "current problem:" << file_names[i] << "\t" << "customer number:" << customer_num << endl;

                //-------------------------重置由于实例规模变化后影响的参数-------------------------
                // 重置禁忌表
                insert_tabu_list = std::vector<std::vector<int>>(ALL_CUSTOMER_NUMBER + 10, std::vector<int>(VEHICLE_NUMBER, 0));
                new_route_tabu_list = std::vector<int>(ALL_CUSTOMER_NUMBER + 10, 0);
                exchange_tabu_list = std::vector<std::vector<int>>(ALL_CUSTOMER_NUMBER + 10, std::vector<int>(ALL_CUSTOMER_NUMBER + 10, 0));
                // 重置节点路线访问矩阵
                visited_node_route_matrix = vector<vector<double>>(ALL_CUSTOMER_NUMBER + 2, vector<double>(VEHICLE_NUMBER + 1, 0.0));
                // 重置全局最好解
                Best_sol = Solution();
                Best_sol_c = double(0xffffffffff);
                // 重置迭代次数
                iter_count = 0;
                // 重置参数
                alpha = 1;   // 容量
                b_beta = 1;  // 时间
                g_gamma = 1; // duration
                theta = 0.5;

                lambda = 0.0002; // diversity
                //----------------------------------------------------------------------------------

                // 记录程序执行时间
                clock_t start, end;
                start = clock();
                // Your code here
                //cout << "init_distance_matrix" << endl;
                // 初始化距离矩阵
                init_distance_matrix();
                cout << "init_distance_matrix over" << endl;

                // 初始化解
                Solution solution;

                // solution.init_solution_no_time_over();
                solution.init_solution_over_time_random();
                cout << "init_solution over" << endl;
                solution.tabu_search_run();
                cout << "search over" << endl;
                end = clock();

                double execution_time = double(end - start) / CLOCKS_PER_SEC;
                Best_sol.execution_time = execution_time;

                cout << "output and save the result" << endl;
                string output_name = "output_" + file_name_no_suffix + "_customer number_" + to_string(ALL_CUSTOMER_NUMBER) + "_Random seed_" + to_string(RANDOM_SEED) + ".txt";
                Best_sol.save_output("../output/" + output_name);
                save_result(Best_sol, file_name_no_suffix);
                cout << "------------------------------------------------------------------------" << endl;
            }
        }
    }

    return 0;
}

// 将当前的解的结果保存到csv中，用作统计比较分析
// csv文件列名为 instance,customer_num,routes_used_number,distance_all,execution_time,random_seed
void save_result(const Solution &res_sol, string instance_name)
{
    ofstream out_file;
    string file_path = "../output/result.csv";
    // 检查文件是否存在
    if (!std::filesystem::exists(file_path))
    {
        out_file.open(file_path);
        // 写入列名
        out_file << "instance,customer_num,routes_used_number,distance_all,execution_time,random_seed" << endl;
    }
    else
    {
        out_file.open(file_path, ios::app);
    }
    out_file << instance_name << "," << res_sol.visited_customer_num << "," << res_sol.route_used_num << "," << res_sol.distance_all << "," << res_sol.execution_time << "," << RANDOM_SEED << endl;
    out_file.close();
}