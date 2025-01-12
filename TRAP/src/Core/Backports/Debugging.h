#ifndef TRAP_BACKPORTS_H
#define TRAP_BACKPORTS_H

#ifndef __cpp_lib_debugging

namespace std
{
    /// @brief Attempts to determine if the program is being executed with the debugger present.
    ///        Formally, the behaviour of this function is completely implementation-defined.
    /// @return True if, to the best knowledge, the program is executed under a debugger.
    /// @note The intent of this function is allowing printing out extra output to help diagnose
    ///       problems, executing extra test code, displaying an extra user interface to help in
    ///       debugging, etc.
    [[nodiscard]] bool is_debugger_present() noexcept;

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Unconditional breakpoint: attempts to temporarily halt the execution
    ///        of the program and transfer control to the debugger whether or not it's
    ///        not possible to determine if the debugger is present. Formally, the behaviour
    ///        of this function is completely implementation-defined.
    /// @note The intent of this function is allowing for runtime control of breakpoints
    ///       beyond what might be available from a debugger while not causing the program
    ///       to exit. For example, breaking when an infrequent non-critical condition is
    ///       detected, allowing programmatic control with complex runtime sensitive conditions,
    ///       breaking on user input to inspect context in interactive programs without needing
    ///       to switch to the debugger application, etc.
    void breakpoint() noexcept;

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Conditional breakpoint: attempts to temporarily halt
    ///        the execution of the program and transfer control to the
    ///        debugger if it were able to determine that the debugger is present.
    ///        Acts as a no-op otherwise. Formally, the behaviour of this function is
    ///        completely implementation-defined.
    void breakpoint_if_debugging() noexcept;
}

#else

#if __cplusplus >= 202311L
#warning "Backported std::breakpoint() should be removed!"
#warning "Backported std::breakpoint_if_debugging() should be removed!"
#warning "Backported std::is_debugger_present() should be removed!"
#endif /*__cplusplus >= 202311L*/

#endif /*__cpp_lib_debugging*/

#endif /*TRAP_BACKPORTS_H*/
