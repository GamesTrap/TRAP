#ifndef TRAP_TRAPPCH_H
#define TRAP_TRAPPCH_H

#include "Core/PlatformDetection.h"

//C++ STL
#include <iosfwd>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <set>
#include <filesystem>
#include <optional>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <thread>
#include <chrono>
#include <ctime>
#include <cstdio>
#include <iomanip>
#include <deque>
#include <future>
#include <cctype>
#include <any>
#include <climits>
#include <limits>
#include <forward_list>
#include <random>
#include <queue>
#include <tuple>
#include <cassert>
#include <numeric>
#include <variant>
#include <mutex>
#include <shared_mutex>
#include <bit>
#include <source_location>
#include <csignal>
#include <ranges>
#include <regex>
#include <version>
#include <concepts>

#include "Core/Backports.h"
#include "Core/Types.h"

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/

//Tracy - Profiler
#include <tracy/Tracy.hpp>

//TRAP
#include "Core/Base.h"
#include "TRAP_Assert.h"
#include "Log/Log.h"
#include "Utils/NumericCasts.h"

#include "Utils/UniqueResource.h"
#include "Utils/Optional.h"
#include "Utils/Expected.h"
#include "Utils/Enum.h"
#include "Utils/Bit.h"
#include "Utils/SmartPtr.h"

#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

#endif /*TRAP_TRAPPCH_H*/
