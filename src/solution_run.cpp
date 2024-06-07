#include "solution.h"
#include <iostream> // Include the necessary header file for 'cout'

// 定义全局最好的解和他的评价函数
Solution Best_sol;
double Best_sol_c = double(0xfffff);

// 获取当前迭代次数
int iter_count = 0;

// 定义禁忌表，行表示节点 ID，列表示路线索引
std::vector<std::vector<int>> insert_tabu_list(ALL_CUSTOMER_NUMBER + 10, std::vector<int>(VEHICLE_NUMBER, 0));
// 定义新增路线产生的禁忌表，节点插入新路线之后再禁忌步长内不能取出来
std::vector<int> new_route_tabu_list(ALL_CUSTOMER_NUMBER + 10, 0);

// 定义exchange 禁忌表 保存交换的两个节点的id
std::vector<std::vector<int>> exchange_tabu_list(ALL_CUSTOMER_NUMBER + 10, std::vector<int>(ALL_CUSTOMER_NUMBER + 2, 0));



double relocation_improvement = 0.5;
double exchange1_improvement = 0.25;
double exchange2_improvement = 0.25;


void Solution::tabu_search_run()
{
    // 首先判断当前的解是不是可行解，如果是则先保存
    if (check_feasible())
    {
        // 把当前的解复制给Best_sol
        Best_sol = *this;
        Best_sol_c = distance_all;
    }

    // 开始迭代执行禁忌搜索
   for (int iter = 0; iter < ITER_MAX; ++iter)
    {
        iter_count = iter;
        int operator_choice = choose_operator();

        switch (operator_choice)
        {
        case 0:
            Reloacation();
            //cout<<"relocation \t";
            break;
        case 1:
            exchange_1();
           // cout<<"exchange_1 \t";
            break;
        case 2:
            exchange_2();
           // cout<<"exchange_2 \t";
            break;
        default:
            break;
        }
        // if(iter>1740)
        // {
        //     this->print_solution_info();
        // }
        //打印当前解的 f F_s f_p distance 和time_violation capacity_violation
        std::cout  <<"Iter: " << iter<< ", f: " << f << ", f_s: " << f_s << ", f_p: " << f_p << ", distance: " << distance_all << ", time_violation: " << time_violation <<", capacity_ciolation: "<<capacity_violation<< std::endl;
        
        

        
        
        
    }
    // Output current solution and best solution
    // std::cout << std::endl
    //           << "Current Solution: " << std::endl;
    // this->print_solution_info();
    // std::cout << endl
    //           << "Best Solution: " << std::endl;
    // Best_sol.print_solution_info();
}

int Solution::choose_operator()
{
    int rand_val = rand() % 3;

    // Choose the operator based on the iteration count
    if (iter_count < 800)
    {
        return 0; // Always choose Relocation operator for the first 500 iterations
    }
    else if (iter_count < 1000)
    {
        return 1; // Always choose Exchange_1 operator for the next 500 iterations
    }
    else if (iter_count < 1300)
    {
        return 2; // Always choose Exchange_2 operator for the next 500 iterations
    }
    else
    {
        return rand_val; // Randomly choose an operator for the remaining iterations
    }
}
