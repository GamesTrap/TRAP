#include <catch2/catch_test_macros.hpp>

#include "Utils/Concurrency/Safe.h"

TEST_CASE("TRAP::Utils::AccessTraits", "[utils][concurrency][accesstraits]")
{
    STATIC_REQUIRE_FALSE(TRAP::Utils::AccessTraits<i32>::IsReadOnly);
    STATIC_REQUIRE_FALSE(TRAP::Utils::AccessTraits<std::lock_guard<std::mutex>>::IsReadOnly);
    STATIC_REQUIRE_FALSE(TRAP::Utils::AccessTraits<std::scoped_lock<std::mutex>>::IsReadOnly);
    STATIC_REQUIRE_FALSE(TRAP::Utils::AccessTraits<std::unique_lock<std::mutex>>::IsReadOnly);
    STATIC_REQUIRE(TRAP::Utils::AccessTraits<std::shared_lock<std::mutex>>::IsReadOnly);
}

TEST_CASE("TRAP::Utils::Safe", "[utils][concurrency][safe]")
{
    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Utils::Safe<std::string>>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Utils::Safe<std::string>>);
        STATIC_REQUIRE_FALSE(std::movable<TRAP::Utils::Safe<std::string>>);
    }

    SECTION("Constructors")
    {
        //Default constructor
        const TRAP::Utils::Safe<std::string> s{};

        //Copy constructor
        // const TRAP::Utils::Safe<std::string> sCopy(s); //Deleted

        //Move constructor
        // TRAP::Utils::Safe<std::string> sMoveSource{};
        // const TRAP::Utils::Safe<std::string> sMoveTarget(std::move(sMoveSource)); //Deleted

        const TRAP::Utils::Safe<std::string> s1(TRAP::Utils::DefaultConstructMutexTag, 100, 'a');

        const TRAP::Utils::Safe<std::string> s2({}, 100, 'a');
    }

    SECTION("Assignments")
    {
        TRAP::Utils::Safe<std::string> s{};

        //Copy assignment operator
        // const TRAP::Utils::Safe<std::string> sCopy = s; //Deleted

        //Move assignment operator
        // const TRAP::Utils::Safe<std::string> sMove = std::move(s); //Deleted
    }

    SECTION("ReadLock()")
    {
        TRAP::Utils::Safe<std::string> s{};
        s.Assign("HelloWorld!");

        const TRAP::Utils::Safe<std::string>::ReadAccess<> read = s.ReadLock();

        REQUIRE(*read == "HelloWorld!");
    }

    SECTION("ReadLock(LockArgs)")
    {
        TRAP::Utils::Safe<std::string> s{};
        s.Assign("HelloWorld!");

        const TRAP::Utils::Safe<std::string>::ReadAccess<> read1 = s.ReadLock(std::try_to_lock_t{});

        REQUIRE(*read1 == "HelloWorld!");
    }

    SECTION("WriteLock()")
    {
        TRAP::Utils::Safe<std::string> s{};
        s.Assign("HelloWorld!");

        TRAP::Utils::Safe<std::string>::WriteAccess<> write = s.WriteLock();

        REQUIRE(*write == "HelloWorld!");

        *write = "Some other stuff";

        REQUIRE(*write == "Some other stuff");
    }

    SECTION("WriteLock(LockArgs)")
    {
        TRAP::Utils::Safe<std::string> s{};
        s.Assign("HelloWorld!");

        TRAP::Utils::Safe<std::string>::WriteAccess<> write = s.WriteLock(std::try_to_lock_t{});

        REQUIRE(*write == "HelloWorld!");

        *write = "Some other stuff";

        REQUIRE(*write == "Some other stuff");
    }

    SECTION("Copy()")
    {
        TRAP::Utils::Safe<std::string> s{};
        s.Assign("Hello World!");

        const std::string str = s.Copy();
        REQUIRE(str == "Hello World!");
    }

    SECTION("Copy(LockArgs)")
    {
        TRAP::Utils::Safe<std::string> s{};
        s.Assign("Hello World!");

        const std::string str = s.Copy(std::try_to_lock_t{});
        REQUIRE(str == "Hello World!");
    }

    SECTION("Assign()")
    {
        TRAP::Utils::Safe<std::string> s{};

        REQUIRE(s.ReadLock()->empty());

        s.Assign("HelloWorld!");

        REQUIRE(*s.ReadLock() == "HelloWorld!");

        const std::string str = "Some other stuff";

        s.Assign(str);

        REQUIRE(*s.ReadLock() == str);
    }

    SECTION("GetUnsafe()")
    {
        TRAP::Utils::Safe<std::string> s{};

        s.Assign("HelloWorld!");

        REQUIRE(s.GetUnsafe() == "HelloWorld!");

        const TRAP::Utils::Safe<std::string> s1{};
        REQUIRE(s1.GetUnsafe().empty());
    }

    SECTION("GetMutex()")
    {
        TRAP::Utils::Safe<std::string> s{};

        const auto& mtx = s.GetMutex();
    }
}

TEST_CASE("TRAP::Utils::Safe::ReadAccess", "[utils][concurrency][safe][readaccess]")
{
    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Utils::Safe<std::string>::ReadAccess<>>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Utils::Safe<std::string>::ReadAccess<>>);
        STATIC_REQUIRE_FALSE(std::movable<TRAP::Utils::Safe<std::string>::ReadAccess<>>);
    }

    SECTION("Constructors")
    {
        TRAP::Utils::INTERNAL::DefaultMutex mtx{};

        //Value, mutex constructor
        {
            const std::string str = "Hello World!";
            TRAP::Utils::Safe<std::string>::ReadAccess<> access{str, mtx};
        }

        //Safe constructor
        {
            const TRAP::Utils::Safe<std::string> s{};
            TRAP::Utils::Safe<std::string>::ReadAccess<> access(s);
        }

        //Copy constructor
        // {
        //     const std::string str = "Hello World!";
        //     TRAP::Utils::Safe<std::string>::ReadAccess<> access{str, mtx};

        //     const TRAP::Utils::Safe<std::string>::ReadAccess<> accessCopy(access); //Not allowed
        // }

        //Copy constructor (read->write access)
        // {
        //     std::string str = "Hello World!";
        //     TRAP::Utils::Safe<std::string>::ReadAccess<> access{str, mtx};

        //     const TRAP::Utils::Safe<std::string>::WriteAccess<> accessCopy(access); //Not allowed
        // }

        //Move constructor
        {
            const std::string str = "Hello World!";
            TRAP::Utils::Safe<std::string>::ReadAccess<> access(str, mtx);

            const TRAP::Utils::Safe<std::string>::ReadAccess<> accessMove(std::move(access));
        }

        //Move constructor (read->write access)
        // {
        //     std::string str = "Hello World!";
        //     TRAP::Utils::Safe<std::string>::ReadAccess<> access(str, mtx);

        //     const TRAP::Utils::Safe<std::string>::WriteAccess<> accessMove(std::move(access)); //Not allowed
        // }
    }

    SECTION("Assignments")
    {
        TRAP::Utils::INTERNAL::DefaultMutex mtx{};

        //Copy assignment operator
        // {
        //     const std::string str = "Hello World!";
        //     TRAP::Utils::Safe<std::string>::ReadAccess<> access{str, mtx};

        //     const TRAP::Utils::Safe<std::string>::ReadAccess<> accessCopy = access; //Deleted
        // }

        //Copy assignment operator (read->write access)
        // {
        //     const std::string str = "Hello World!";
        //     TRAP::Utils::Safe<std::string>::ReadAccess<> access{str, mtx};

        //     const TRAP::Utils::Safe<std::string>::WriteAccess<> accessCopy = access; //Deleted
        // }

        //Move assignment operator
        {
            const std::string str = "Hello World!";
            TRAP::Utils::Safe<std::string>::ReadAccess<> access{str, mtx};

            const TRAP::Utils::Safe<std::string>::ReadAccess<> accessMove = std::move(access);
        }

        //Move assignment operator (read->write access)
        // {
        //     const std::string str = "Hello World!";
        //     TRAP::Utils::Safe<std::string>::ReadAccess<> access{str, mtx};

        //     const TRAP::Utils::Safe<std::string>::WriteAccess<> accessMove = std::move(access);
        // }
    }

    SECTION("operator->()")
    {
        TRAP::Utils::INTERNAL::DefaultMutex mtx{};
        const std::string str = "Hello World!";

        {
            const TRAP::Utils::Safe<std::string>::ReadAccess<> access{str, mtx};

            REQUIRE_FALSE(access->empty());
        }
        {
            TRAP::Utils::Safe<std::string>::ReadAccess<> access{str, mtx};

            REQUIRE_FALSE(access->empty());
        }
    }

    SECTION("operator*()")
    {
        TRAP::Utils::INTERNAL::DefaultMutex mtx{};
        const std::string str = "Hello World!";

        {
            const TRAP::Utils::Safe<std::string>::ReadAccess<> access{str, mtx};

            REQUIRE_FALSE((*access).empty());
        }
        {
            TRAP::Utils::Safe<std::string>::ReadAccess<> access{str, mtx};

            REQUIRE_FALSE((*access).empty());
        }
    }
}

TEST_CASE("TRAP::Utils::Safe::WriteAccess", "[utils][concurrency][safe][writeaccess]")
{
    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Utils::Safe<std::string>::WriteAccess<>>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Utils::Safe<std::string>::WriteAccess<>>);
        STATIC_REQUIRE_FALSE(std::movable<TRAP::Utils::Safe<std::string>::WriteAccess<>>);
    }

    SECTION("Constructors")
    {
        TRAP::Utils::INTERNAL::DefaultMutex mtx{};

        //Value, mutex constructor
        {
            std::string str = "Hello World!";
            TRAP::Utils::Safe<std::string>::WriteAccess<> access{str, mtx};
        }

        //Safe constructor
        {
            TRAP::Utils::Safe<std::string> s{};
            TRAP::Utils::Safe<std::string>::WriteAccess<> access(s);
        }

        //Copy constructor
        // {
        //     std::string str = "Hello World!";
        //     TRAP::Utils::Safe<std::string>::WriteAccess<> access{str, mtx};

        //     const TRAP::Utils::Safe<std::string>::WriteAccess<> accessCopy(access); //Not allowed
        // }

        //Copy constructor (write->read access)
        // {
        //     std::string str = "Hello World!";
        //     TRAP::Utils::Safe<std::string>::WriteAccess<> access{str, mtx};

        //     const TRAP::Utils::Safe<std::string>::ReadAccess<> accessCopy(access); //Not allowed
        // }

        //Move constructor
        {
            std::string str = "Hello World!";
            TRAP::Utils::Safe<std::string>::WriteAccess<> access(str, mtx);

            const TRAP::Utils::Safe<std::string>::WriteAccess<> accessMove(std::move(access));
        }

        //Move constructor (write->read access)
        // {
        //     std::string str = "Hello World!";
        //     TRAP::Utils::Safe<std::string>::WriteAccess<> access(str, mtx);

        //     const TRAP::Utils::Safe<std::string>::ReadAccess<> accessMove(std::move(access)); //Not allowed
        // }
    }

    SECTION("Assignments")
    {
        TRAP::Utils::INTERNAL::DefaultMutex mtx{};

        //Copy assignment operator (write->read access)
        // {
        //     std::string str = "Hello World!";
        //     TRAP::Utils::Safe<std::string>::WriteAccess<> access{str, mtx};

        //     const TRAP::Utils::Safe<std::string>::ReadAccess<> accessCopy = access; //Deleted
        // }

        //Move assignment operator (write->read access)
        // {
        //     std::string str = "Hello World!";
        //     TRAP::Utils::Safe<std::string>::WriteAccess<> access{str, mtx};

        //     const TRAP::Utils::Safe<std::string>::ReadAccess<> accessMove = std::move(access);
        // }
    }

    SECTION("operator->()")
    {
        TRAP::Utils::INTERNAL::DefaultMutex mtx{};
        std::string str = "Hello World!";

        {
            const TRAP::Utils::Safe<std::string>::WriteAccess<> access{str, mtx};

            REQUIRE_FALSE(access->empty());
        }
        {
            TRAP::Utils::Safe<std::string>::WriteAccess<> access{str, mtx};

            REQUIRE_FALSE(access->empty());
        }
    }

    SECTION("operator*()")
    {
        TRAP::Utils::INTERNAL::DefaultMutex mtx{};
        std::string str = "Hello World!";

        {
            const TRAP::Utils::Safe<std::string>::WriteAccess<> access{str, mtx};

            REQUIRE_FALSE((*access).empty());
        }
        {
            TRAP::Utils::Safe<std::string>::WriteAccess<> access{str, mtx};

            REQUIRE_FALSE((*access).empty());
        }
    }
}
