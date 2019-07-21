//
// Created by dbeef on 4/21/19.
//

#include "gmock/gmock.h"
#include "jobs/JobSystem.hpp"

using ::testing::AtLeast;

class CounterJob : public Job {
public:

    int counter = 0;

    void execute() override {
        counter++;
    }

};

class ExpensiveJob : public Job {
public:

    void execute() override {
        for (volatile int a = 0; a < std::numeric_limits<uint16_t >::max(); a++) {
            volatile int i = i % 123456789;
        }
    }

};

TEST(JobSystemTest, StartStopTest) {
    JobSystem job_system;
    job_system.start();
    EXPECT_EQ(job_system.is_working(), true);
    job_system.shutdown();
    EXPECT_EQ(job_system.is_working(), false);
}

TEST(JobSystemTest, LongAndInexpensiveTest_DispatchAndWait) {

    JobSystem job_system;

    for(int x =0 ; x < 1000;x++) {
        job_system.start();

        for (int index = 0; index < 100; index++) {
            auto counter_job = std::make_shared<CounterJob>();
            job_system.dispatch(counter_job);
            job_system.wait_for_done();
            EXPECT_EQ(counter_job->counter, 1);
        }

        job_system.shutdown();
    }
}

TEST(JobSystemTest, LongAndExpensiveTest_MultipleDispatchThenWait) {
    JobSystem job_system;
    job_system.start();

    // create jobs
    std::vector<std::shared_ptr<Job>> expensive_jobs;

    for (int a = 0; a < 50000; a++) {
        auto job = std::make_shared<ExpensiveJob>();
        expensive_jobs.push_back(job);
        EXPECT_EQ(job->done.load(), false);
        job_system.dispatch(job);
    }

    job_system.wait_for_done();

    for(const auto& job : expensive_jobs) EXPECT_EQ(job->done.load(), true);

    job_system.shutdown();
}

TEST(JobSystemTest, LongAndExpensiveTest_WaitForSpecificJob) {
    JobSystem job_system;
    job_system.start();

    // create jobs
    std::vector<std::shared_ptr<Job>> expensive_jobs;

    for (int a = 0; a < 50000; a++) {
        auto job = std::make_shared<ExpensiveJob>();
        expensive_jobs.push_back(job);
        EXPECT_EQ(job->done.load(), false);
        job_system.dispatch(job);
    }

    auto special_job = std::make_shared<ExpensiveJob>();
    EXPECT_EQ(special_job->done.load(), false);
    job_system.dispatch(special_job);

    special_job->wait_for_done();

    for(const auto& job : expensive_jobs) EXPECT_EQ(job->done.load(), true);

    job_system.shutdown();
}

TEST(JobSystemTest, LongAndExpensiveTest_ShutdownWithoutWaiting) {
    JobSystem job_system;
    job_system.start();

    // create jobs
    std::vector<std::shared_ptr<Job>> expensive_jobs;

    for (int a = 0; a < 50000; a++) {
        auto job = std::make_shared<ExpensiveJob>();
        expensive_jobs.push_back(job);
        EXPECT_EQ(job->done.load(), false);
        job_system.dispatch(job);
    }

    job_system.shutdown();
}

TEST(JobSystemTest, LongAndExpensiveTest_MultipleSystemsInParallel) {

    JobSystem job_system_1;
    JobSystem job_system_2;

    job_system_1.start();
    job_system_2.start();

    // create jobs
    std::vector<std::shared_ptr<Job>> expensive_jobs_1;
    std::vector<std::shared_ptr<Job>> expensive_jobs_2;

    for (int a = 0; a < 50000; a++) {
        auto job = std::make_shared<ExpensiveJob>();
        EXPECT_EQ(job->done.load(), false);
        if(a % 2) {
            expensive_jobs_1.push_back(job);
            job_system_1.dispatch(job);
        } else {
            expensive_jobs_2.push_back(job);
            job_system_2.dispatch(job);
        }
    }

    job_system_1.wait_for_done();
    job_system_2.wait_for_done();

    for(const auto& job : expensive_jobs_1) EXPECT_EQ(job->done.load(), true);
    for(const auto& job : expensive_jobs_2) EXPECT_EQ(job->done.load(), true);

    job_system_1.shutdown();
    job_system_2.shutdown();
}

int main(int argc, char **argv) {
    // The following line must be executed to initialize Google Mock
    // and Google Test before running tests.
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
