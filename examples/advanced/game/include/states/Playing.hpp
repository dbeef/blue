#pragma once

#include "jobs/MapIntersectionJob.h"
#include "BaseState.hpp"

class Playing : public BaseState
{
public:
    ~Playing();
    std::shared_ptr<BaseState> update() override;
    void on_entry() override;
private:
        MapIntersectionJob _intersection_job;
};
