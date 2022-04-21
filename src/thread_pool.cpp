#include "thread_pool.h"

// Intel Celeron J4125: L2 cache (4 MB) (1 MB per core?) only.
// Intel Core i7-10700K: L1 cache 64 KB (per core), L2 256 KB (per core) L3 16 MB (shared)
// L1 cache cycle : 4
// L2 cache cycle : 12
// L3 cache cycle : 36 (for i7-4770)
// LRU - Least Recently Used) cache bye-bye
// add, sub cycle : 1
// mul cycle : 4
// div cycle : 20~26


ThreadPool::ThreadPool(size_t num_thread)
: num_threads_(num_thread), flag_stop_all_(false)
{
    worker_threads_.reserve(num_threads_);
    for(size_t i = 0; i < num_threads_; ++i) {
        worker_threads_.emplace_back([this]() { this->workerThread(); });
        this->setThreadCPUAffinity(worker_threads_[i], 2*i+1);
    }
};

ThreadPool::~ThreadPool() {
    flag_stop_all_ = true;
    cond_var_.notify_all();

    for(auto& t : worker_threads_) {
        std::cout << "--- JOINING THE THREAD [" << t.get_id() << "]...";
        t.join();
        std::cout << " DONE!\n";
    }
};


void ThreadPool::workerThread() {

    {
        std::unique_lock<std::mutex> lock(mut_);
        std::cout << "The thread id [" << std::this_thread::get_id() << "] is made.\n";
    }

    while( true ) {
        std::unique_lock<std::mutex> lock(mut_);
        cond_var_.wait(lock, [this]() { return !this->jobs_.empty() || flag_stop_all_;}); 
        // 조건문을 검사해보고 false이면 notify가 올 때 까지 무한슬립한다.
        // notify가 오면 조건을 다시 검사한다. 그래도 false이면 다시 잔다.
        // Spurious wake 를 생각하자. mutex를 공유하는 다른 thread가 wakeup 하면 notify된다? 
        std::cout << "The thread id [" << std::this_thread::get_id() << "] woke up!\n";
        std::cout << "flag : "<< flag_stop_all_ << std::endl;
        if( flag_stop_all_ && this->jobs_.empty()){
            break;
        }

        // Get a job
        std::function<void()> job = std::move(jobs_.front());
        jobs_.pop();
        lock.unlock();

        // Do the job! 
        std::cout << "The thread id [" << std::this_thread::get_id() << "] get the job.\n";
        job();
    }
    std::cout << "The thread id [" << std::this_thread::get_id() << "] is end.\n";
};

void ThreadPool::enqueueJob(std::function<void()> job){
    if( flag_stop_all_ ){
        throw std::runtime_error("Thread pool 사용 중지.");
    }

    {
        std::lock_guard<std::mutex> lock(mut_);
        jobs_.push(std::move(job));
        std::cout <<"push job. job queue size : " << jobs_.size() <<std::endl;
    }
    cond_var_.notify_one();
    std::cout << "   notified.\n";
};

bool ThreadPool::setThreadCPUAffinity(std::thread& th, const int& cpu_num){

    unsigned int num_max_threads = std::thread::hardware_concurrency();
    if(cpu_num >= num_max_threads) {
        std::cerr << "Exceed the maximum logical CPU number!\n";
        return false;
    }
    
    cpu_set_t cpuSet;
    CPU_ZERO(&cpuSet );
    CPU_SET(cpu_num, &cpuSet);
    int rc = pthread_setaffinity_np(th.native_handle(), sizeof(cpu_set_t), &cpuSet);
    if (rc != 0) {
        std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
        return false;
    }

    return true;
};
