#ifndef TRAP_CONTENTIONFREESHAREDMUTEX_H
#define TRAP_CONTENTIONFREESHAREDMUTEX_H

#include "Core/Types.h"
#include "Core/Backports.h"

#include <array>
#include <atomic>
#include <memory>
#include <new>
#include <thread>
#include <unordered_map>
#include <cassert>

namespace TRAP::Utils
{
    /// @brief Contention free shared mutex (same-lock-type is recursive for X->X, X->S or S->S locks), but (S->X - is UB).
    ///
    ///        This Mutex type mostly behaves like std::shared_mutex with the difference
    ///        that it is guaranteed lock/wait/contention free up to ContentionFreeCount threads.
    /// @tparam ContentionFreeCount Max number of threads to support contention free.
    ///         Falls back to exclusive locking when max is reached.
    /// @note This type supports Tracy via TracySharedLockable.
    template<u32 ContentionFreeCount = 32>
    class ContentionFreeSharedMutex
    {
    public:
        ContentionFreeSharedMutex()
            : m_sharedLocksArrayPtr(std::make_shared<ArraySlockT>()),
              m_sharedLocksArray(*m_sharedLocksArrayPtr)
        {
        }

        ~ContentionFreeSharedMutex()
        {
            for(auto& i : m_sharedLocksArray)
                i.Value = -1;
        }

        ContentionFreeSharedMutex(const ContentionFreeSharedMutex&) = delete;
        ContentionFreeSharedMutex(ContentionFreeSharedMutex&&) = delete;

        ContentionFreeSharedMutex& operator=(const ContentionFreeSharedMutex&) = delete;
        ContentionFreeSharedMutex& operator=(ContentionFreeSharedMutex&&) = delete;

        [[nodiscard]] bool try_lock_shared()
        {
            const i32 registerIndex = RegisterThread();

            if(registerIndex >= 0)
            {
                const i32 recursionDepth = m_sharedLocksArray[registerIndex].Value.load(std::memory_order_acquire);
                assert(recursionDepth >= 1);

                if(recursionDepth > 1)
                    m_sharedLocksArray[registerIndex].Value.store(recursionDepth + 1, std::memory_order_release); //if recursive -> release
                else
                {
                    m_sharedLocksArray[registerIndex].Value.store(recursionDepth + 1, std::memory_order_seq_cst); //If first -> sequential
                    while(m_wantXLock.load(std::memory_order_seq_cst))
                    {
                        m_sharedLocksArray[registerIndex].Value.store(recursionDepth, std::memory_order_seq_cst);
                        if(m_wantXLock.load(std::memory_order_seq_cst))
                            return false;
                        m_sharedLocksArray[registerIndex].Value.store(recursionDepth + 1, std::memory_order_seq_cst);
                    }
                }

                return true;
            }

            if(m_ownerThreadID.load(std::memory_order_acquire) != GetFastThisThreadID())
            {
                for(bool flag = false; !m_wantXLock.compare_exchange_weak(flag, true, std::memory_order_seq_cst); flag = false)
                    return false;
                m_ownerThreadID.store(GetFastThisThreadID(), std::memory_order_release);
            }
            ++m_recursiveXLockCount;
            return true;
        }

        void lock_shared()
        {
            const i32 registerIndex = RegisterThread();

            if(registerIndex >= 0)
            {
                const i32 recursionDepth = m_sharedLocksArray[registerIndex].Value.load(std::memory_order_acquire);
                assert(recursionDepth >= 1);

                if(recursionDepth > 1)
                    m_sharedLocksArray[registerIndex].Value.store(recursionDepth + 1, std::memory_order_release); //if recursive -> release
                else
                {
                    m_sharedLocksArray[registerIndex].Value.store(recursionDepth + 1, std::memory_order_seq_cst); //If first -> sequential
                    while(m_wantXLock.load(std::memory_order_seq_cst))
                    {
                        m_sharedLocksArray[registerIndex].Value.store(recursionDepth, std::memory_order_seq_cst);
                        for(/*volatile*/ usize i = 0; m_wantXLock.load(std::memory_order_seq_cst); ++i)
                        {
                            if(i % 100000 == 0)
                                std::this_thread::yield();
                        }
                        m_sharedLocksArray[registerIndex].Value.store(recursionDepth + 1, std::memory_order_seq_cst);
                    }
                }
            }
            else
            {
                if(m_ownerThreadID.load(std::memory_order_acquire) != GetFastThisThreadID())
                {
                    usize i = 0;
                    for(bool flag = false; !m_wantXLock.compare_exchange_weak(flag, true, std::memory_order_seq_cst); flag = false)
                        if(++i % 100000 == 0)
                            std::this_thread::yield();
                    m_ownerThreadID.store(GetFastThisThreadID(), std::memory_order_release);
                }
                ++m_recursiveXLockCount;
            }
        }

        void unlock_shared()
        {
            const i32 registerIndex = GetOrSetIndex();

            if(registerIndex >= 0)
            {
                const i32 recursionDepth = m_sharedLocksArray[registerIndex].Value.load(std::memory_order_acquire);
                assert(recursionDepth > 1);

                m_sharedLocksArray[registerIndex].Value.store(recursionDepth - 1, std::memory_order_release);
            }
            else
            {
                if(--m_recursiveXLockCount == 0)
                {
                    m_ownerThreadID.store(decltype(m_ownerThreadID)(), std::memory_order_release);
                    m_wantXLock.store(false, std::memory_order_release);
                }
            }
        }

        [[nodiscard]] bool try_lock()
        {
            //Forbidden upgrade S-lock to X-lock - This is an excellent opportunity to get a deadlock
            const i32 registerIndex = GetOrSetIndex();
            if(registerIndex >= 0)
                assert(m_sharedLocksArray[registerIndex].Value.load(std::memory_order_acquire) == 1);

            if(m_ownerThreadID.load(std::memory_order_acquire) != GetFastThisThreadID())
            {
                for(bool flag = false; !m_wantXLock.compare_exchange_weak(flag, true, std::memory_order_seq_cst); flag = false)
                {
                    return false;
                }

                m_ownerThreadID.store(GetFastThisThreadID(), std::memory_order_release);

                for(auto& i : m_sharedLocksArray)
                {
                    while(i.Value.load(std::memory_order_seq_cst) > 1);
                }
            }

            ++m_recursiveXLockCount;

            return true;
        }

        void lock()
        {
            //Forbidden upgrade S-lock to X-lock - This is an excellent opportunity to get a deadlock
            const i32 registerIndex = GetOrSetIndex();
            if(registerIndex >= 0)
                assert(m_sharedLocksArray[registerIndex].Value.load(std::memory_order_acquire) == 1);

            if(m_ownerThreadID.load(std::memory_order_acquire) != GetFastThisThreadID())
            {
                usize i = 0;
                for(bool flag = false; !m_wantXLock.compare_exchange_weak(flag, true, std::memory_order_seq_cst); flag = false)
                {
                    if(++i % 100000 == 0)
                        std::this_thread::yield();
                }

                m_ownerThreadID.store(GetFastThisThreadID(), std::memory_order_release);

                for(auto& slock : m_sharedLocksArray)
                {
                    while(slock.Value.load(std::memory_order_seq_cst) > 1);
                }
            }

            ++m_recursiveXLockCount;
        }

        void unlock()
        {
            assert(m_recursiveXLockCount > 0);
            if(--m_recursiveXLockCount == 0)
            {
                m_ownerThreadID.store(decltype(m_ownerThreadID)(), std::memory_order_release);
                m_wantXLock.store(false, std::memory_order_release);
            }
        }

    private:
        std::atomic<bool> m_wantXLock = false;
        struct ContentionFreeFlagT
        {
            alignas(std::hardware_destructive_interference_size) //Avoid false sharing
            std::atomic<i32> Value;
            constexpr ContentionFreeFlagT()
                : Value(0)
            {
            }
        };
        using ArraySlockT = std::array<ContentionFreeFlagT, ContentionFreeCount>;

        const std::shared_ptr<ArraySlockT> m_sharedLocksArrayPtr; //0 - unregistered, 1 registered & free, 2... - busy
        char m_avoidFalseSharing1[std::hardware_destructive_interference_size]{};

        ArraySlockT& m_sharedLocksArray;
        char m_avoidFalseSharing2[std::hardware_destructive_interference_size]{};

        i32 m_recursiveXLockCount = 0;

        enum class IndexOp
        {
            UnregisterThreadOp,
            GetIndexOp,
            RegisterThreadOp
        };

        using ThreadID = std::thread::id;
        std::atomic<std::thread::id> m_ownerThreadID = ThreadID();
        [[nodiscard]] std::thread::id GetFastThisThreadID() noexcept
        {
            return std::this_thread::get_id();
        }

        struct Unregister
        {
            Unregister(const i32 index, const std::shared_ptr<ArraySlockT>& ptr)
                : ThreadIndex(index), ArraySlockPtr(ptr)
            {
            }
            Unregister(Unregister&& other) noexcept
                : ThreadIndex(other.ThreadIndex), ArraySlockPtr(std::move(other.ArraySlockPtr))
            {
            }
            ~Unregister()
            {
                if(ArraySlockPtr.use_count() > 0)
                    --(*ArraySlockPtr)[ThreadIndex].Value;
            }

            Unregister(const Unregister&) = delete;

            Unregister& operator=(const Unregister&) = delete;
            Unregister& operator=(Unregister&&) = delete;

            i32 ThreadIndex;
            std::shared_ptr<ArraySlockT> ArraySlockPtr;
        };

        i32 GetOrSetIndex(const IndexOp indexOp = IndexOp::GetIndexOp, i32 setIndex = -1)
        {
            thread_local static std::unordered_map<void*, Unregister> threadLocalIndexHashmap{};

            //Get thread index - in any cases. Use &m_sharedLocksArray as key ("this" may get recycled)
            if(const auto it = threadLocalIndexHashmap.find(&m_sharedLocksArray); it != threadLocalIndexHashmap.cend())
                setIndex = it->second.ThreadIndex;

            if(indexOp == IndexOp::UnregisterThreadOp) //Unregister thread
            {
                if(m_sharedLocksArray[setIndex].Value == 1) //If isn't shared_lock now
                    threadLocalIndexHashmap.erase(&m_sharedLocksArray);
                else
                    return -1;
            }
            else if(indexOp == IndexOp::RegisterThreadOp) //Register thread
            {
                threadLocalIndexHashmap.emplace(&m_sharedLocksArray, Unregister(setIndex, m_sharedLocksArrayPtr));

                //Remove info about deleted contentionfree-mutexes
                for(auto it = threadLocalIndexHashmap.begin(), ite = threadLocalIndexHashmap.end(); it != ite;)
                {
                    if((*it->second.ArraySlockPtr)[it->second.ThreadIndex].Value < 0) //If contentionfree-mutex was deleted
                        it = threadLocalIndexHashmap.erase(it);
                    else
                        ++it;
                }
            }

            return setIndex;
        }

        [[nodiscard]] bool UnregisterThread()
        {
            return GetOrSetIndex(IndexOp::UnregisterThreadOp) >= 0;
        }

        [[nodiscard]] i32 RegisterThread()
        {
            i32 currentIndex = GetOrSetIndex();

            if(currentIndex != -1)
                return currentIndex;

            if(m_sharedLocksArrayPtr.use_count() <= static_cast<i32>(m_sharedLocksArray.size())) //Try once to register thread
            {
                for(usize i = 0; i < m_sharedLocksArray.size(); ++i)
                {
                    i32 unregisteredValue = 0;
                    if(m_sharedLocksArray[i].Value == 0)
                    {
                        if(m_sharedLocksArray[i].Value.compare_exchange_strong(unregisteredValue, 1))
                        {
                            currentIndex = static_cast<i32>(i);
                            GetOrSetIndex(IndexOp::RegisterThreadOp, currentIndex); //Thread registered successfully
                            break;
                        }
                    }
                }
            }

            return currentIndex;
        }
    };
}

#endif /*TRAP_CONTENTIONFREESHAREDMUTEX_H*/
