#pragma once

#include <atomic>
#include <utility>

namespace zq {


// C++ implementation of Dmitry Vyukov's lock free MPSC queue
// http://www.1024cores.net/home/lock-free-algorithms/queues/non-intrusive-mpsc-node-based-queue
template<typename T>
class MPSCQueue
{
public:
    MPSCQueue() : _head(new Node()), _tail(_head.load(std::memory_order_relaxed))
    {
        Node* front = _head.load(std::memory_order_relaxed);
        front->Next.store(nullptr, std::memory_order_relaxed);
    }

    ~MPSCQueue()
    {
        T* output;
        while (this->Dequeue(output))
            ;

        Node* front = _head.load(std::memory_order_relaxed);
        delete front;
    }

    void Enqueue(T* input)
    {
        Node* node = new Node(input);
        Node* prevHead = _head.exchange(node, std::memory_order_acq_rel);
        prevHead->Next.store(node, std::memory_order_release);
    }

    bool Dequeue(T*& result)
    {
        Node* tail = _tail.load(std::memory_order_relaxed);
        Node* next = tail->Next.load(std::memory_order_acquire);
        if (!next)
            return false;

        result = next->Data;
        _tail.store(next, std::memory_order_release);
        delete tail;
        return true;
    }

private:
    struct Node
    {
        Node() = default;
        explicit Node(T* data) : Data(data) { Next.store(nullptr, std::memory_order_relaxed); }

        T* Data;
        std::atomic<Node*> Next;
    };

    std::atomic<Node*> _head;
    std::atomic<Node*> _tail;

    MPSCQueue(MPSCQueue const&) = delete;
    MPSCQueue& operator=(MPSCQueue const&) = delete;
};

}
