#pragma once
#include "libs.h"
#include <stack>

class ogImguiRenderer {
public:
    virtual void renderer(void) = 0;
};

class ogImgui {

public:
    ogImgui(GLFWwindow* window);
    ~ogImgui();

    void mPushCallback(ogImguiRenderer *aImguiRenderer);

    void mRender();

private:
    GLFWwindow* m_window;
    bool m_show_demo_window;
    bool m_show_another_window;
    glm::vec4 m_clear_color;

    std::stack<ogImguiRenderer*> m_callback_stack;
};
