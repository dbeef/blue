#pragma once

#include <blue/gui/Button.hpp>
#include "BaseState.hpp"

#include <atomic>

class MainMenu : public BaseState
{
public:

    ~MainMenu();
    std::shared_ptr<BaseState> update() override;
    void on_entry() override;

private:

    std::atomic<bool> _start_requested{false};
    std::atomic<bool> _options_requested{false};
    std::atomic<bool> _exit_requested{false};

    struct
    {
        Button start;
        Button options;
        Button exit;
    } controls;
};
