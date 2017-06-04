#include <mutex>
#include <future>

class ConcurrentBuffer{

	int *buffer;
	int size;
	int capacity;
	int front;
	int rear;

	//locking constructs
	std::mutex bufferMutex;
	std::condition_variable notEmpty, notFull;

public:
	ConcurrentBuffer(int t_size){
		capacity = t_size;
		buffer = new int[capacity];
		size = 0;
		front = 0;
		rear = 0;
	}

	void PushData(int data){
		std::unique_lock<std::mutex> locker(bufferMutex);

		notFull.wait(locker, [&](){return (size < capacity);});

		buffer[rear++] = data;
		size++;

		notEmpty.notify_one();
	}

	int FetchData(){
		std::unique_lock<std::mutex> locker(bufferMutex);

		notEmpty.wait(locker, [&](){ return (size > 0); });

		int current = buffer[front++];
		--size;

		notFull.notify_one();
	}

};

int main(int argc, char const *argv[])
{
	
	return 0;
}