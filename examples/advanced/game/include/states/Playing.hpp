#pragma once

#include "BaseState.hpp"

class Playing : public BaseState
{
public:
    std::shared_ptr<BaseState> update() override;
    void on_entry() override;
private:
};
