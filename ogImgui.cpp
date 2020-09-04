#include "libs.h"

#include "ogImgui.h"

ogImgui::ogImgui(GLFWwindow *window) {
        
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    this->m_window = window;
    this->m_clear_color = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
    this->m_show_demo_window = false;
    this->m_show_another_window = false;
};

ogImgui::~ogImgui() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
void ogImgui::mPushCallback(ogImguiRenderer* aImguiRenderer)
{
    this->m_callback_stack.push(aImguiRenderer);
}

void ogImgui::mRender() {

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &this->m_show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &this->m_show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&this->m_clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    if (this->m_show_another_window) {
        static float f2 = 0.0f;
        ImGui::Begin("Another Window !", &this->m_show_another_window);                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &this->m_show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &this->m_show_another_window);

        ImGui::SliderFloat("float", &f2, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

        ImGui::End();
    }

    {
        while (!this->m_callback_stack.empty()) {
            ogImguiRenderer *callback_ptr = this->m_callback_stack.top();
            callback_ptr->renderer();
            this->m_callback_stack.pop();
        }
    }

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(this->m_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    //glClearColor(this->m_clear_color.x, this->m_clear_color.y, this->m_clear_color.z, this->m_clear_color.w);
   // glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
};
