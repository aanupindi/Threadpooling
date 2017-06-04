#include <iostream>
#include <thread>
#include <deque>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <future>
#include <vector>

using namespace std;

class Buffer{

std::deque<int> _buffer;
std::mutex bufferMutex;
std::condition_variable BufferCV;
std::mutex printMutex;
static const int totalSize = 10;

public:
	Buffer(){

	}

	void PrintData(const std::string& action, int value){
		std::unique_lock<std::mutex> printlocker(printMutex);
		cout << action << value << endl;
	}

	void AddValue(int value = 0){
		std::string action("Adding value: ");
		std::unique_lock<std::mutex> locker(bufferMutex); // lock the buffer
		BufferCV.wait(locker, [&](){return (_buffer.size() < totalSize) ;}); // wait until there is space in the buffer.
		int val;
		if(value == 0)
			val = rand() % 100;
		else
			val = -1;
		_buffer.push_back(val);
		locker.unlock();
		BufferCV.notify_all();
		PrintData(action,val);
	}

	int GetValue(){
		std::string action("Removing value ");
		std::unique_lock<std::mutex> locker(bufferMutex);
		BufferCV.wait(locker, [&](){return (_buffer.size() > 0) ;});
		int current = _buffer.front();
		if(current != -1)
			_buffer.pop_front();
		locker.unlock();
		BufferCV.notify_all();
		PrintData(action,current);
		return current;
	}

	void TerminateProduction(){
		AddValue(-1);
	}

};

class Producer{
	Buffer &buffer;
	int limit = 10;
	std::thread *prodThread;

public:
	Producer(Buffer& b):buffer(b){
	}

	void Start(){
		prodThread = new std::thread(&Producer::PushData, this);
	}

	void PushData(){
		int current = 0;
		//cout << "Producer::PushData"<<endl;
		while(current < limit){
			buffer.AddValue();
			++current;
		}	
		buffer.TerminateProduction();
	}
	void WaitUntilDone(){
		prodThread->join();
	}

};

class Consumer {
	Buffer& buffer;
	std::thread *consumerThread;

public:
	Consumer(Buffer& b):buffer(b){

	}

	void Start(){
		cout << "Consumer:: GetData"<<endl;
		consumerThread = new std::thread(&Consumer::GetData, this);
	}

	void GetData(){
		int value ;
		while(value != -1){
			value = buffer.GetValue();
			cout <<"Thread Number: " << std::this_thread::get_id() <<" consumer value :" << value <<endl;
		}
		cout << "Done executing thread " << std::this_thread::get_id() << std::endl;
		return;	
	}

	void WaitUntilDone(){
		consumerThread->join();
	}
};

int main(int argc, char* argv[]){
	Buffer b;
	Producer p(b);

	cout << "Starting all threads.\n";

	std::vector<Consumer*> consumerThreads(5);
	for(auto &consumerThread : consumerThreads){
		consumerThread = new Consumer(b);
		consumerThread->Start();
	}
	
	p.Start();

	p.WaitUntilDone();

	for(const auto &consumerThread : consumerThreads){
		consumerThread->WaitUntilDone();
	}

	return 0;

}

