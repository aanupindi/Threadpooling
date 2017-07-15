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
					std::cout << "Waiting...." << std::endl;
		while(1){

			//lock on the queue

			std::unique_lock<std::mutex> tasklocker(workerQMutex);
			std::cout << "Waiting for Task : Thread Id" << std::this_thread::get_id() << std::endl;
			//Now check if there are any tasks in the Queue
			workerQueueCV.wait(tasklocker, [&](){return !workerQueue.empty() || terminate.load() == true;});
			std::cout<<"Working on task "<<std::endl;
			//Exit condition
			if(terminate.load() == true) 
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

public:
	ThreadPool(){
		numThreads = thread::hardware_concurrency(); //number of threads supported in the system
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
		//WaitForAllTasks();
		//workerThreads.clear();
	}

	void AddTask(std::function<void()> threadFunction){

		std::cout <<"Adding task " << std::endl;
		std::unique_lock<std::mutex> taskLocker(workerQMutex);
		//function <void (int)> f = std::bind(threadFunction, x);
		workerQueue.push(threadFunction);	 // bind returns a type of std::function<>

		taskLocker.unlock();
		workerQueueCV.notify_one();	

	}
	void WaitForAllTasks(){
		std::cout << "numThreads is " <<numThreads <<std::endl;
		for(int i = 0;i<numThreads; ++i){
			workerThreads[i]->join();
		}
	}	

	void End(){		
		terminate.store(true);
		workerQueueCV.notify_all();
	}
		
};

void Printfunction(){
	std::cout << "Printing this function from " << std::this_thread::get_id() << std::endl;
}

int main(int argc, char** argv){

	ThreadPool pool1;
	pool1.Init();
	pool1.AddTask(Printfunction);
	std::cout<<"before the end " <<std::endl;
		pool1.End();
	pool1.WaitForAllTasks();

}
