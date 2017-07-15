#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <fstream>
#include <queue>
#include <future>


using namespace std;
using namespace std::placeholders;

class ThreadPool{
	int numThreads;
	std::queue<std::function<void()>> workerQueue;
	std::vector<std::unique_ptr<std::thread>> workerThreads;
	std::mutex workerQMutex;
	std::condition_variable workerQueueCV;
	std::atomic_bool terminate;

	void GetTask(){

		while(1){

			//lock on the queue

			std::unique_lock<std::mutex> tasklocker(workerQMutex);
			std::cout << "Waiting for Task : Thread Id" << std::this_thread::get_id() << std::endl;
			//Now check if there are any tasks in the Queue
			workerQueueCV.wait(tasklocker, [&](){return !workerQueue.empty() || terminate.load() == true;});
			std::cout<<"Working on task "<<std::endl;
			//Exit condition
			if(workerQueue.empty() && terminate.load() == true) 
			{
				std::cout << "terminate called. Exiting!" << std::endl;
				return;
			}
			//comes here only if queue is not empty
			std::function<void()> currentJob = workerQueue.front();
			workerQueue.pop();
			tasklocker.unlock();

			currentJob(); //do the job

		}
	}

	void WaitForAllTasks(){
		for(int i = 0;i<numThreads; ++i){
			workerThreads[i]->join();
		}
	}	

	void End(){		
		terminate.store(true);
		workerQueueCV.notify_all();
	}

public:
	ThreadPool(uint8_t t_numThreads){
		uint8_t threadsSupported = thread::hardware_concurrency();
		numThreads = (t_numThreads > threadsSupported)? t_numThreads: threadsSupported; //number of threads supported in the system
		workerThreads.resize(numThreads);
		terminate.store(false);
	}

	void Init(){
		std::cout <<"Calling init on all threads: "<<numThreads<<std::endl;
		for(int i = 0;i<numThreads; ++i){
			workerThreads[i] = std::unique_ptr<std::thread>(new std::thread(&ThreadPool::GetTask,this));
		}
	}

	~ThreadPool(){
			End();
			WaitForAllTasks();	
		}

	void AddTask(std::function<void()> threadFunction){

		std::cout <<"Adding task " << std::endl;
		std::unique_lock<std::mutex> taskLocker(workerQMutex);
		//function <void (int)> f = std::bind(threadFunction, x);
		workerQueue.push(threadFunction);	 // bind returns a type of std::function<>

		taskLocker.unlock();
		workerQueueCV.notify_one();	

	}
		
};

void Printfunction(){
	std::cout << "Printing this function from " << std::this_thread::get_id() << std::endl;
}

int main(int argc, char** argv){

	ThreadPool pool1(5);
	pool1.Init();

	for (int i = 0;i<5;++i)
		pool1.AddTask([](){std::cout<<"Print this from "<<std::this_thread::get_id()<<std::endl;});
}
