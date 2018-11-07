#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <atomic>
#include <type_traits>

namespace zq {

template <typename T>
class ProducerConsumerQueue
{
public:

	ProducerConsumerQueue<T>() : shutdown_(false) { }

	void push(const T& value)
	{
		std::lock_guard<std::mutex> lock(queueLock_);
		queue_.push(std::move(value));

		// 向正在等待的线程发出通知，唤醒那个等待的线程
		condition_.notify_one();
	}

	bool empty()
	{
		std::lock_guard<std::mutex> lock(queueLock_);

		return queue_.empty();
	}

	bool pop(T& value)
	{
		std::lock_guard<std::mutex> lock(queueLock_);

		if (queue_.empty() || shutdown_)
			return false;

		value = queue_.front();

		queue_.pop();

		return true;
	}

	void waitAndPop(T& value)
	{
		std::unique_lock<std::mutex> lock(queueLock_);

		// https://connect.microsoft.com/VisualStudio/feedback/details/1098841
		// 当前线程在调用wait过后会一直被阻塞,会自动调用lock.unlock()释放锁
		// 在收到notify后，该线程又重新对mutex进行加锁
		// 在之前，我这里一直不理解，原因是该线程被notify了过后，理应mutex是被释放了，
		// 但是如果mutex被释放了，其他线程就会和这个线程就可能会出现竞争资源的情况，
		// 其实，这里真正的过程是这样:在这里,wait函数中会释放mutex(unlock),同时进行等待
		// 在被notify过后,该线程又被唤醒，先唤醒之前，会先获得mutex(lock),工作流程就是这样
		// 而这里用std::unique_lock而不用std::lock_guard是为了语义上更加准确,
		// 如果用的是std::lock_guard，因为std::lock_guard只有在它析构的时候才会被释放，
		// 但是这里出现了锁被释放又被获得的情况，虽然这样不会出问题，但是语义上有点矛盾，而std::unique_lock
		// 可以随时释放锁，所以这里用std::unique_lock语义上更加准确
		while (queue_.empty() && !shutdown_)
			condition_.wait(lock);

		if (queue_.empty() || shutdown_)
			return;

		value = queue_.front();

		queue_.pop();
	}

	void cancel()
	{
		std::unique_lock<std::mutex> lock(queueLock_);

		while (!queue_.empty())
		{
			T& value = queue_.front();

			deleteQueuedObject(value);

			queue_.pop();
		}

		shutdown_ = true;

		condition_.notify_all();
	}

private:
	std::mutex queueLock_;
	std::queue<T> queue_;
	std::condition_variable condition_;
	std::atomic<bool> shutdown_;

private:
	template<typename E = T>
	typename std::enable_if<std::is_pointer<E>::value>::type deleteQueuedObject(E& obj) { delete obj; }

	template<typename E = T>
	typename std::enable_if<!std::is_pointer<E>::value>::type deleteQueuedObject(E const& /*packet*/) { }
};

}


