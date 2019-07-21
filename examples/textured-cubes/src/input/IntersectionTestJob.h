#include <utility>

//
// Created by dbeef on 4/22/19.
//

#ifndef OPENGL_PLAYGROUND_INTERSECTIONTESTJOB_H
#define OPENGL_PLAYGROUND_INTERSECTIONTESTJOB_H
#include "blue/jobs/JobSystem.hpp"

class IntersectionTestJob : public Job {

public:

    void execute() override;
};

#endif //OPENGL_PLAYGROUND_INTERSECTIONTESTJOB_H
