#ifndef TRAP_CONCURRENCY_QUEUE_H
#define TRAP_CONCURRENCY_QUEUE_H

#include <atomic>

#include "Core/Types.h"
#include "Utils/Optional.h"

#ifdef TRAP_PLATFORM_WINDOWS
#pragma warning(push)
#pragma warning(disable: 4324)
#endif /*TRAP_PLATFORM_WINDOWS*/

namespace TRAP::Utils
{
    /// @brief A FIFO Queue implementation without locking.
    ///        Internally a single linked list is used.
    template<typename T>
    class LockFreeQueue
    {
    public:
        /// @brief Constructor.
		/// @threadsafe
        LockFreeQueue()
        {
            Node* dummy = new Node();
            m_head.store(dummy, std::memory_order::relaxed);
            m_tail.store(dummy, std::memory_order::relaxed);
        }

        /// @brief Destructor.
		/// @threadsafe
        ~LockFreeQueue()
        {
            //Clean up queue nodes
            while(Node* const node = m_head.load(std::memory_order::relaxed))
            {
                m_head.store(node->Next.load(std::memory_order::relaxed), std::memory_order::relaxed);
                delete node;
            }
        }

        /// @brief Copy constructor.
		/// @threadsafe
        LockFreeQueue(const LockFreeQueue&) = delete;

        /// @brief Move constructor.
        /// @param other Queue to move from.
		/// @threadsafe
        LockFreeQueue(LockFreeQueue&& other) noexcept
        {
            m_head.store(other.m_head.load(std::memory_order::relaxed), std::memory_order::relaxed);
            m_tail.store(other.m_tail.load(std::memory_order::relaxed), std::memory_order::relaxed);

            other.m_head.store(nullptr, std::memory_order::relaxed);
            other.m_tail.store(nullptr, std::memory_order::relaxed);
        }

        /// @brief Copy assignment operator.
		/// @threadsafe
        LockFreeQueue& operator=(const LockFreeQueue&) = delete;

        /// @brief Move assignment operator.
        /// @param other Queue to move from.
        /// @return *this.
		/// @threadsafe
        LockFreeQueue& operator=(LockFreeQueue&& other) noexcept
        {
            if(this == &other)
                return *this;

            //Clean up current queue
            while(Node* const node = m_head.load(std::memory_order::relaxed))
            {
                m_head.store(node->Next.load(std::memory_order::relaxed), std::memory_order::relaxed);
                delete node;
            }

            m_head.store(other.m_head.load(std::memory_order::relaxed), std::memory_order::relaxed);
            m_tail.store(other.m_tail.load(std::memory_order::relaxed), std::memory_order::relaxed);

            other.m_head.store(nullptr, std::memory_order::relaxed);
            other.m_tail.store(nullptr, std::memory_order::relaxed);

            return *this;
        }

        /// @brief Push a value onto the queue.
        /// @param value Value to add to the back of the queue.
		/// @threadsafe
        void Push(T value)
        {
            Node* const newNode = new Node(std::move(value));
            Node* const prevTail = m_tail.exchange(newNode, std::memory_order::acq_rel); //Atomically swap the tail
            prevTail->Next.store(newNode, std::memory_order::release); //Link the previous tail to the new node
        }

        /// @brief Try to pop a value from the front of the queue.
        /// @return Popped value or TRAP::NullOpt when queue is empty.
		/// @threadsafe
        [[nodiscard]] TRAP::Optional<T> TryPop()
        {
            const Node* const oldHead = m_head.load(std::memory_order::acquire);

            Node* const nextNode = oldHead->Next.load(std::memory_order::acquire);
            if(nextNode == nullptr)
                return TRAP::NullOpt; //Queue is empty

            //Move the head forward
            m_head.store(nextNode, std::memory_order::release);

            TRAP::Optional<T> result = std::move(nextNode->Data); //Retrieve the value from the next node

            delete oldHead;
            return result;
        }

    private:
        struct Node
        {
            constexpr explicit Node(T data)
                : Data(std::move(data))
            {}

            constexpr Node() = default;

            TRAP::Optional<T> Data = TRAP::NullOpt;
            std::atomic<Node*> Next = nullptr;
        };

        alignas(std::hardware_destructive_interference_size) std::atomic<Node*> m_head = nullptr;
        u8 m_avoidFalseSharing1[std::hardware_destructive_interference_size]{};
        alignas(std::hardware_destructive_interference_size) std::atomic<Node*> m_tail = nullptr;
        u8 m_avoidFalseSharing2[std::hardware_destructive_interference_size]{};
    };
}

#ifdef TRAP_PLATFORM_WINDOWS
#pragma warning(pop)
#endif /*TRAP_PLATFORM_WINDOWS*/

#endif /*TRAP_CONCURRENCY_QUEUE_H*/
