#ifndef __has_feature
    #define __has_feature(x) 0
#endif /*__has_feature*/

#if defined(__SANITIZE_ADDRESS__) || __has_feature(address_sanitizer) || defined(TRAP_ASAN) || \
    defined(__SANITIZE_UNDEFINED__) || __has_feature(undefined_sanitizer) || defined(TRAP_UBSAN) || \
    defined(__SANITIZE_LEAK__) || __has_feature(leak_sanitizer) || defined(TRAP_LSAN) || \
    defined(__SANITIZE_THREAD__) || __has_feature(thread_sanitizer) || defined(TRAP_TSAN)

//The callbacks we define here will be called from the sanitizer runtime,
//but aren't referenced from the executable itself. We must ensure that those
//callbacks are not sanitizer-instrumented, and that they aren't stripped by
//the linker.
#define SANITIZER_HOOK_ATTRIBUTE            \
    extern "C"                              \
    __attribute__((no_sanitize("address", "undefined", "leak", "thread"))) \
    __attribute__((visibility("default")))  \
    __attribute__((used))
#endif

#if defined(__SANITIZE_ADDRESS__) || __has_feature(address_sanitizer) || defined(TRAP_ASAN)

//Default options for AddressSanitizer:
//    strip_path_prefix=/../../ - prefixes up to and including this
//        substring will be stripped from source file path in symbolized reports
//    fast_unwind_on_fatal=1 - use the fast (frame-pointer-based) stack unwinder
//        to print error reports.
//    detect_stack_use_after_return=1 - use fake stack to delay the reuse of
//        stack allocations and detect stack-use-after-return errors.
//    symbolize=1 - enable in-process symbolization.
//    external_symbolizer_path=/usr/bin/llvm-symbolizer - path to llvm-symbolizer.

SANITIZER_HOOK_ATTRIBUTE const char* __asan_default_options()
{
    return "strip_path_prefix=/../../ "
           "fast_unwind_on_fatal=1 "
           "detect_stack_use_after_return=1 "
           "symbolize=1 "
           "detect_leaks=0 "
           "protect_shadow_gap=0 "
           "external_symbolizer_path=/usr/bin/llvm-symbolizer-15";
}

#endif

#if defined(__SANITIZE_UNDEFINED__) || __has_feature(undefined_sanitizer) || defined(TRAP_UBSAN)

//Default options for UndefinedBehaviorSanitizer:
//    print_stacktrace=1 - print the stacktrace when UBSan reports an error.

SANITIZER_HOOK_ATTRIBUTE const char* __ubsan_default_options()
{
    return "print_stacktrace=1 "
           "strip_path_prefix=/../../ "
           "external_symbolizer_path=/usr/bin/llvm-symbolizer-15";
}

#endif

#if defined(__SANITIZE_LEAK__) || __has_feature(leak_sanitizer) || defined(TRAP_LSAN)

//Default options for LeakSanitizer:
//    strip_path_prefix=/../../ - prefixes up to and including this
//        substring will be stripped from source file paths in symbolized reports.
//    external_symbolizer_path=/usr/bin/llvm-symbolizer - path to llvm-symbolizer.
//    use_poisoned=1 - Scan poisoned memory.

SANITIZER_HOOK_ATTRIBUTE const char* __lsan_default_options()
{
    return "strip_path_prefix=/../../ "
           "use_poisoned=1 "
           "external_symbolizer_path=/usr/bin/llvm-symbolizer-15";
}

SANITIZER_HOOK_ATTRIBUTE const char* __lsan_default_suppressions()
{
    return "# Add suppressions here\n"

            //Leaks in NVIDIAs GL and Vulkan drivers.
            "leak:libGL*\n"
            "leak:libGLX_nvidia*\n"
            "leak:libnvidia-glcore*\n"
            "leak:libnvidia-cbl*\n"
            "leak:libnvidia-fatbinaryloader*\n"
            "leak:libnvidia-rtcore*\n"
            "leak:nvidia0*\n"
            "leak:nvidiactl*\n"

            "leak:libvulkan*\n"

            "leak:libxcb*\n"

            //Leaks in X11 (and related libraries)
            "leak:libX11*\n"
            "leak:libXrandr*\n"
            "leak:libXi*\n"
            "leak:libXcursor*\n"
            "leak:libXinerama*\n"
            "leak:libX11-xcb*\n"
            "leak:libXrender*\n"
            "leak:libXext*\n"
            "leak:libXxf86vm*\n"

            //Leaks in Wayland (and related libraries)
            "leak:libwayland-client*\n"
            "leak:libwayland-cursor*\n"
            "leak:libwayland-egl*\n"
            "leak:libxkbcommon*\n"

            //Leaks in DBus
            "leak:libdbus*\n"

            //Leaks in Discord Game SDK
            "leak:libdiscord_game_sdk*\n"

            //Leaks in NVIDIA Nsight Aftermath
            "leak:libGFSDK_Aftermath_Lib*\n"

            //Leaks in Valve's Steamworks API
            "leak:libsteam_api*\n"

            //Suppress leaks from unknown third party modules.
            "leak:<unknown module>\n"

            // ================ Leaks in TRAP code ================
            // PLEASE DO NOT ADD SUPPRESSIONS FOR NEW LEAKS.
            // Instead, commits that introduce memory leaks should be fixed.

        //End of suppressions.
        ; //Please keep this semicolon.
}

#endif

#if defined(__SANITIZE_THREAD__) || __has_feature(thread_sanitizer) || defined(TRAP_TSAN)

//Default options for ThreadSanitizer:
//    second_deadlock_stack=1 - more verbose deadlock reports.
//    report_signal_unsafe=0 - do not report async-signal-unsafe functions called from
//        signal handlers.
//    report_thread_leaks=0 - do not report unjoined threads at the end of the
//        program execution.
//    print_suppressions=1 - print the list of matched suppressions.
//    history_size=7 - make the history buffer proportional to 2^7 (the maximum
//        value) to keep more stack traces.
//    strip_path_prefix=/../../ - prefixes up to and including this
//        substring will be stripped from source file paths in symbolized reports.
//    external_symbolizer_path=/usr/bin/llvm-symbolizer - path to llvm-symbolizer.

SANITIZER_HOOK_ATTRIBUTE const char* __tsan_default_options()
{
    return "second_deadlock_stack=1 "
           "report_signal_unsafe=0 "
           "report_thread_leaks=0 "
           "print_suppressions=1 "
           "history_size=7 "
           "strip_path_prefix=/../../ "
           "external_symbolizer_path=/usr/bin/llvm-symbolizer-15 "
           //"log_path=./tsan.log"
           ;
}

SANITIZER_HOOK_ATTRIBUTE const char* __tsan_default_suppressions()
{
    //The suppressions probably need to be adjusted to hardware, OS and driver combination

    return "# Add suppressions here\n"

        //    "race:*\n"

        //    "deadlock:*\n"

            "called_from_lib:libvulkan_radeon*\n"

        //End of suppressions.
        ; //Please keep this semicolon.
}

#endif
