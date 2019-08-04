#pragma once

#include <memory>

class BaseState
{
public:
	virtual std::shared_ptr<BaseState> update() = 0;
	virtual void on_entry() = 0;
};
