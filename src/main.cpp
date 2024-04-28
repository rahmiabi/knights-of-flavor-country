#include <iostream>
#include <stdio.h>
#include <cmath>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

using namespace std;

struct Vec2{
    float x, y;

    void normalize(){
        if (!x && !y) return;
        int mag = sqrt(x * x + y * y);
        x /= mag;
        y /= mag;
    }
    float getMag() const {
       return sqrt(x * x + y * y);
    }
};

struct Projectile{
    Vec2 pos, vel;
};

void raycast(const Vec2& direction, Vec2& ray, const Vec2& initialPos){
    if (ray.getMag() > 1000 || ray.x  + initialPos.x > 1000 || ray.x + initialPos.x < 100) return;
    ray.x += direction.x;
    ray.y += direction.y;
    raycast(direction, ray, initialPos);
}
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
}

// Main code
int main(int, char**)
{
// SETUP STUFF
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Knights of Flavor Country", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    glfwSetCursorPosCallback(window, cursor_position_callback);
    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.;
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
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

    // Set Window Icon
    int width, height, channels;
    unsigned char* pixels = stbi_load("./assets/images/noelle.jpg", &width, &height, &channels, 4);

    GLFWimage images[1];
    images[0].width = width;
    images[0].height = height;
    images[0].pixels = pixels;

    glfwSetWindowIcon(window, 1, images);
    vector<Projectile> projectiles;

// uhh idk what this does i just do copy paste haha
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    
// Main loop
    double characterX = 1280 / 2, characterY = 720 / 2;
    while (!glfwWindowShouldClose(window))
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_None);

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        double scale = 0.1;
        ImGui::GetForegroundDrawList()->AddImage((void*) image_texture, ImVec2(width * scale + xPos - width * scale /2, height * scale + yPos - height * scale /2) , 
                                        ImVec2(0 + xPos - width * scale / 2, 0 + yPos - height * scale/ 2), ImVec2(1,1) , ImVec2(0, 0) , IM_COL32(255, 255, 255, 255));

        Vec2 velocity = {0, 0};
        int W= glfwGetKey(window, GLFW_KEY_W);
        int A= glfwGetKey(window, GLFW_KEY_A);
        int S= glfwGetKey(window, GLFW_KEY_S);
        int D= glfwGetKey(window, GLFW_KEY_D);
        int SHIFT= glfwGetKey(window, GLFW_KEY_Q);
        int F = glfwGetKey(window, GLFW_KEY_F);

        if (W == GLFW_PRESS){
            velocity.y -= 50;
        }
        if (A == GLFW_PRESS){
            velocity.x -= 50;
        }
        if (S == GLFW_PRESS){
            velocity.y += 50;
        }
        if (D == GLFW_PRESS){
            velocity.x += 50;
        }
        velocity.normalize();
        characterX += velocity.x * 6;
        characterY += velocity.y * 6;

        Vec2 direction = {xPos - characterX, yPos - characterY};
        direction.normalize();

        if (F == GLFW_PRESS){
            Projectile proj = {{characterX, characterY}, {direction.x * 25, direction.y * 25}};
            projectiles.push_back(Projectile(proj));
        }
        scale = .05;
        for (Projectile& a: projectiles){
            a.pos.x += a.vel.x;
            a.pos.y += a.vel.y;
            ImGui::GetBackgroundDrawList()->AddImage((void*) image_texture, ImVec2(width * scale + a.pos.x - width * scale / 2, height * scale + a.pos.y - height * scale /2) , 
                                             ImVec2(a.pos.x - width * scale / 2, a.pos.y - height * scale / 2), ImVec2(1,1) , ImVec2(0, 0) , IM_COL32(255, 255, 255, 255));
        }
        if (SHIFT == GLFW_PRESS){
            characterX += direction.x * 10; 
            characterY += direction.y * 10; 
        }
        scale = .1;
        for (float i = -3.14/4; i <= 3.14/4; i += .01){
            Vec2 rotatedDir = {direction.x * cos(i) - direction.y * sin(i), direction.x * sin(i) + direction.y * cos(i)};
            Vec2 ray = {0, 0};
            Vec2 initial = {characterX, characterY};
            raycast(rotatedDir, ray, initial);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(initial.x, initial.y + i), ImVec2(initial.x + ray.x, initial.y + ray.y + i) , IM_COL32(255, 255, 255, 255), 0.3);
        }
        ImGui::GetBackgroundDrawList()->AddImage((void*) image_texture, ImVec2(width * scale + characterX - width * scale / 2, height * scale + characterY - height * scale /2) , 
                                        ImVec2(characterX - width * scale / 2, characterY - height * scale / 2), ImVec2(1,1) , ImVec2(0, 0) , IM_COL32(255, 255, 255, 255));
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        // if (show_demo_window)
        //    ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnd();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    delete pixels;

    return 0;
}