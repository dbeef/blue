#ifndef BLUE_CALLBACKS_H
#define BLUE_CALLBACKS_H

class Callbacks
{
public:

    static Callbacks& instance();

    static void init();

    static void dispose();

    void register_callbacks();

private:

    static Callbacks* _instance;
};

#endif //BLUE_CALLBACKS_H
