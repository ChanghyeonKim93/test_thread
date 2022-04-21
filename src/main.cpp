#include <iostream>
#include <chrono>

#include "thread_pool.h"

void work(int t, int id){
    printf("  work   %d start \n", id);
    std::this_thread::sleep_for(std::chrono::seconds(t));
    std::cout <<"        " <<  std::this_thread::get_id() << " cpu num: " << sched_getcpu() << " " << id << " end after " << t << " seconds.\n";
};
void work2(int t, int id, int a){
    printf("  work2  %d start \n", id);
    std::this_thread::sleep_for(std::chrono::seconds(t));
    std::cout <<"        " <<  std::this_thread::get_id() << " " << id << " end after " << t << " seconds.\n";
};

int main(){
    printf("!!!!! Program starts.\n");
    unsigned num_max_threads = std::thread::hardware_concurrency();
    std::cout << "Launching " << num_max_threads << " threads\n";

    ThreadPool pool(4);

    for(int i = 0; i < 20; ++i) {
        pool.enqueueJob([i] () { work(i % 3 + 1, i); });
    }


    while(1){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "!!!!! program ends...\n";
    
    return 0;
}