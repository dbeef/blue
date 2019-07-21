//
// Created by dbeef on 6/9/19.
//

#include "blue/Logger.hpp"

#if defined BLUE_ANDROID
#	include "spdlog/sinks/android_sink.h"
#else
#	include "spdlog/sinks/stdout_sinks.h"
#endif

Logger::Logger()
{
#if defined BLUE_ANDROID
	_logger = spdlog::android_logger_mt("normal logger", "blue");
#else
	_logger = spdlog::stdout_logger_mt("normal logger");
#endif
}
