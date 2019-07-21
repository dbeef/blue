//
// Created by dbeef on 6/9/19.
//

#ifndef OPENGL_PLAYGROUND_LOGGER_HPP
#define OPENGL_PLAYGROUND_LOGGER_HPP

#include "spdlog/spdlog.h"
#include "blue/Assertions.h"

class Logger 
{
public:

	Logger();

	inline spdlog::logger& get() 
	{
		BLUE_ASSERT(_logger);
		return *_logger;
	}

private:

    std::shared_ptr<spdlog::logger> _logger = nullptr;
};


#endif //OPENGL_PLAYGROUND_LOGGER_HPP
