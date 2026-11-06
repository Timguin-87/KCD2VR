#pragma once
namespace DetourModKit {struct HostLogger {template<class...T>void info(const char*,T...){} template<class...T>void warning(const char*,T...){} template<class...T>void error(const char*,T...){} };inline HostLogger& log(){static HostLogger l;return l;}}
