//
// Created by dbeef on 5/26/19.
//

#ifndef OPENGL_PLAYGROUND_APPLICATIONSTATE_HPP
#define OPENGL_PLAYGROUND_APPLICATIONSTATE_HPP

class ApplicationState {

public:

    virtual void draw_imgui() = 0;

    virtual void on_entry() = 0;

};

#endif //OPENGL_PLAYGROUND_APPLICATIONSTATE_HPP
