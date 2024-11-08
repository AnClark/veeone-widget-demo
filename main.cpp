// Dear ImGui: standalone example application for GLFW + OpenGL2, using legacy fixed pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// **DO NOT USE THIS CODE IF YOUR CODE/ENGINE IS USING MODERN OPENGL (SHADERS, VBO, VAO, etc.)**
// **Prefer using the code in the example_glfw_opengl2/ folder**
// See imgui_impl_glfw.cpp for details.

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include "synthv1_wave.h"
#include "veeone_adsr_widget.hpp"
#include "veeone_filter_widget.hpp"
#include "veeone_wave_widget.hpp"
#include <stdio.h>
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GLFW/glfw3.h>

#include <memory>   // For unique pointer

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL2 example", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // To avoid performance issue, define and init 
    std::unique_ptr<synthv1_wave_lf> test_wavedata = std::make_unique<synthv1_wave_lf>();

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Vee-One Filter Envelope Demo");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("Filter envelope:");

            static float cutoff = 50.0f;
            static float resonance = 0.0f;
            static int filter_type = 0;
            static int filter_slope = 0;

            {
                VeeOneWidgets::DCFCurve("test_curve", &cutoff, &resonance, &filter_type, &filter_slope, 200, 80);
                VeeOneWidgets::DCFCurve("test_curve_larger", &cutoff, &resonance, &filter_type, &filter_slope, 400, 80);
            }


            ImGui::SliderFloat("Cutoff", &cutoff, 0, 100);
            ImGui::SliderFloat("Resonance", &resonance, 0, 100);

            ImGui::BeginGroup();
            {

                ImGui::Text("Filter Type:");

                ImGui::RadioButton("LPF", &filter_type, 0); ImGui::SameLine();
                ImGui::RadioButton("BPF", &filter_type, 1); ImGui::SameLine();
                ImGui::RadioButton("HPF", &filter_type, 2); ImGui::SameLine();
                ImGui::RadioButton("BRF", &filter_type, 3);
            }
            ImGui::EndGroup();

            ImGui::BeginGroup();
            {
                ImGui::Text("Filter Slope:");

                ImGui::RadioButton("12db/oct", &filter_slope, 0); ImGui::SameLine();
                ImGui::RadioButton("24db/oct", &filter_slope, 1); ImGui::SameLine();
                ImGui::RadioButton("Biquad", &filter_slope, 2); ImGui::SameLine();
                ImGui::RadioButton("Formant", &filter_slope, 3);
            }
            ImGui::EndGroup();



            ImGui::End();
        }

        {
            ImGui::Begin("Vee-One ADSR Envelope Demo");

            static float attack = 100.0f;
            static float decay = 100.0f;
            static float sustain = 50.0f;
            static float release = 100.0f;

            {
                VeeOneWidgets::ADSREnvelope("test_adsr", &attack, &decay, &sustain, &release, 400, 80);
            }


            ImGui::SliderFloat("Attack", &attack, 0, 100);
            ImGui::SliderFloat("Decay", &decay, 0, 100);
            ImGui::SliderFloat("Sustain", &sustain, 0, 100);
            ImGui::SliderFloat("Release", &release, 0, 100);


            ImGui::End();
        }

        {
            ImGui::Begin("Draggable Button Test");

            {
                static bool is_pressed = false;

                if (ImGui::Button("Push Me!"))
                {
                    // 不进行任何动作
                }

                // 
                if (ImGui::IsItemActive() && ImGui::IsMouseDown(0))      // 如果要求鼠标放在按钮上，则还要检测 IsItemActive()
                {
                    is_pressed = true;
                } else {
                    is_pressed = false;
                }
                
                ImGui::SameLine(0, 20);

                if (is_pressed) {
                    ImGui::Text("Mouse pressed on the button!");

                    ImVec2 drag_delta = ImGui::GetMouseDragDelta(0);
                    ImGui::Text("Drag delta: (%.0f, %.0f)", drag_delta.x, drag_delta.y);
                }
            }

            ImGui::End();
        }

        {
            ImGui::Begin("Waveform view test");

            static int waveform = synthv1_wave_lf::Sine;
            static float wave_width = 100.0f;

            {
                VeeOneWidgets::Wave("osc1", &*test_wavedata, waveform, wave_width, 100, 100);
            }

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
        // you may need to backup/reset/restore other state, e.g. for current shader using the commented lines below.
        //GLint last_program;
        //glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        //glUseProgram(0);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        //glUseProgram(last_program);

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
