#include <iostream>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <random>
#include <chrono>
#include <memory>
#include <limits>
#include <thread>
#include <unordered_set>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>

//#include <boost/asio/read_until.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#define GLM_SWIZZLE_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include "vecmath.h"
#include "actor.h"
#include "npc.h"
#include "player.h"
#include "enemy.h"
#include "body.h"
#include "projectile.h"
#include "client.h"
using namespace std;

World world;
/* 
STOLEN CODEE !!! i dont know how matrices work :c
*/
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) 
{ 
    return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); 
}
static inline ImVec2 ImRotate(const ImVec2& v, float cos_a, float sin_a) 
{ 
    return ImVec2(v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a);
}
void ImageRotated(ImTextureID tex_id, ImVec2 center, ImVec2 size, float angle, ImDrawList* draw_list)
{
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    ImVec2 pos[4] =
    {
        center + ImRotate(ImVec2(-size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a),
        center + ImRotate(ImVec2(+size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a),
        center + ImRotate(ImVec2(+size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a),
        center + ImRotate(ImVec2(-size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a)
    };
    ImVec2 uvs[4] = 
    { 
        ImVec2(0.0f, 0.0f), 
        ImVec2(1.0f, 0.0f), 
        ImVec2(1.0f, 1.0f), 
        ImVec2(0.0f, 1.0f) 
    };

    draw_list->AddImageQuad(tex_id, pos[0], pos[1], pos[2], pos[3], uvs[0], uvs[1], uvs[2], uvs[3], IM_COL32_WHITE);
}
float jujutsu(float x){
    float val = (1 / (1 + pow(exp(1), -10 * (x / 100 - 0.5))));
    return val;
}

// callbacks
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}
bool pressed = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    pressed = true;
}
enum class State {
    INTRO,
    MAINMENU,
    SETTINGS,
    SINGLEPLAYER,
    MULTIPLAYER,
    SINGLEGAME,
    MULTIGAME
};
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
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Taskforce Redmond", nullptr, nullptr);

    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    // 
//    boost::asio::io_context io_context;
//    tcp::resolver resolver(io_context);
//    auto endpoints = resolver.resolve("localhost","6969");
//    ChatClient client(io_context, endpoints);
//    client.startChat();
    unique_ptr<ChatClient> client;
    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("localhost","6969");
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
    ImFont* font1 = io.Fonts->AddFontFromFileTTF("./assets/fonts/roboto.ttf", 10.0f);
    ImFont* font2 = io.Fonts->AddFontFromFileTTF("./assets/fonts/roboto.ttf", 40.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    State clientState = State::INTRO;

    ImVec4 clear_color = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    glewInit();

    // Set Window Icon
    int width, height, channels;
    unsigned char* pixels = stbi_load("./assets/images/noelle.jpg", &width, &height, &channels, 4);

    GLFWimage images[5];
    images[0].width = width;
    images[0].height = height;
    images[0].pixels = pixels;

    int width1, height1, channels1;
    unsigned char* pixels1 = stbi_load("./assets/images/run.png", &width1, &height1, &channels1, 4);
    images[1].width = width1;
    images[1].height = height1;
    images[1].pixels = pixels1;
    
    int width2, height2, channels2;
    unsigned char* pixels2 = stbi_load("./assets/images/edge.png", &width2, &height2, &channels2, 4);
    images[2].width = width2;
    images[2].height = height2;
    images[2].pixels = pixels2;
    glfwSetWindowIcon(window, 1, images);

    int width3, height3, channels3;
    unsigned char* pixels3 = stbi_load("./assets/images/undertale.jpg", &width3, &height3, &channels3, 4);
    images[3].width = width3;
    images[3].height = height3;
    images[3].pixels = pixels3;
    
    int width4, height4, channels4;
    unsigned char* pixels4 = stbi_load("./assets/images/logo.png", &width4, &height4, &channels4, 4);
    images[4].width = width3;
    images[4].height = height3;
    images[4].pixels = pixels3;

    // uhh idk what this does i just do copy paste haha
    GLuint image_texture1, image_texture2, image_texture3, map, logo;
    glGenTextures(1, &image_texture1);
    glGenTextures(1, &image_texture2);
    glGenTextures(1, &image_texture3);
    glGenTextures(1, &map);
    glGenTextures(1, &logo);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, image_texture2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width1, height1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels1);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, image_texture1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, image_texture3);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels2);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width3, height3, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels3);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, logo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width4, height4, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels4);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
    
// Main loop
    auto start = std::chrono::system_clock::now(); 
    auto end = start;
    float deltaTime = 0;
    char clear[250] = "";
    char inputText[250] = "";
    float f = 0;
    string log = "";
    while (!glfwWindowShouldClose(window))
    {
        glfwSetKeyCallback(window, key_callback);
        ImGui::SetMouseCursor(ImGuiMouseCursor_None);
        end = start;
        start = std::chrono::system_clock::now(); 
        deltaTime = std::chrono::duration<double, std::milli>(start - end).count();

        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

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

        // custom cursor
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        double scale = 0.1;
        ImGui::GetForegroundDrawList()->AddImage((void*) image_texture1, ImVec2(width * scale + xPos - width * scale /2, height * scale + yPos - height * scale /2) , 
                                        ImVec2(0 + xPos - width * scale / 2, 0 + yPos - height * scale/ 2), ImVec2(1,1) , ImVec2(0, 0) , IM_COL32(255, 255, 255, 255));
        int W= glfwGetKey(window, GLFW_KEY_W);
        int A= glfwGetKey(window, GLFW_KEY_A);
        int S= glfwGetKey(window, GLFW_KEY_S);
        int D= glfwGetKey(window, GLFW_KEY_D);
        int F= glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        int R= glfwGetKey(window, GLFW_KEY_R);

        switch (clientState){
           case (State::INTRO): {
                static float timer = 0;
                static int alpha; 
                if (timer < 3000){
                    alpha = timer / 1500 * 255;
                    alpha = min(alpha, 255);
                }
                else if (timer < 5000){
                    alpha = 255 + (timer - 3000) / 1000 * -255;
                    alpha = max(alpha, 0);
                } else 
                    clientState = State::MAINMENU;
                if (pressed)
                    clientState = State::MAINMENU;
                ImGui::GetBackgroundDrawList()->AddImage((void*) map, ImVec2((width3 / -2)  + windowWidth / 2, (height3/ -2) + windowHeight / 2), 
                                                ImVec2((width3 / 2)  + windowWidth / 2, height3 / 2 + windowHeight / 2), ImVec2(0,0) , ImVec2(1, 1) , IM_COL32(255, 255, 255, alpha));
                timer += deltaTime;
           } break;
           case (State::MAINMENU): {
                static float timer =0;
                static float size = 0;
                timer += deltaTime;
                if (timer < 1000){
                    size = jujutsu(timer / 10);
                    size = min(size, 1.0f); 
                }
                ImGui::GetBackgroundDrawList()->AddImage((void*) logo, ImVec2((width4 / -2) * size  + windowWidth / 2, (height4/ -2) * size  + windowHeight / 5), 
                                                ImVec2((width4 / 2) * size + windowWidth / 2, height4 / 2 * size + windowHeight / 5), ImVec2(0,0) , ImVec2(1, 1) , IM_COL32(255, 255, 255, 255));
                {
                ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
                ImGui::SetNextWindowPos(ImVec2(0, 0));
                ImGui::Begin("Another Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings  );   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
                ImGui::SetCursorPos(ImVec2(windowWidth / 4, windowHeight * 9/ 20));
                if (ImGui::Button("Singleplayer", ImVec2(windowWidth / 2, windowHeight / 10)))
                    clientState = State::SINGLEPLAYER;
                ImGui::SetCursorPos(ImVec2(windowWidth / 4, windowHeight * (1.0f / 2.0f + 1.0f / 8.0f - 1.0f / 20.0f)));
                if (ImGui::Button("Multiplayer", ImVec2(windowWidth / 2, windowHeight / 10)))
                    clientState = State::MULTIPLAYER;
                ImGui::SetCursorPos(ImVec2(windowWidth / 4, windowHeight * 7 / 10));
                if (ImGui::Button("Settings", ImVec2(windowWidth / 2, windowHeight / 10)))
                    clientState = State::SETTINGS;
                ImGui::End();
                }
           } break;
           case (State::SETTINGS): {
                float size = 1.0f;
                ImGui::GetBackgroundDrawList()->AddImage((void*) logo, ImVec2((width4 / -2) * size  + windowWidth / 2, (height4/ -2) * size  + windowHeight / 5), 
                                                ImVec2((width4 / 2) * size + windowWidth / 2, height4 / 2 * size + windowHeight / 5), ImVec2(0,0) , ImVec2(1, 1) , IM_COL32(255, 255, 255, 255));
           } break;
           case (State::SINGLEPLAYER):{
                float size = 1.0f;
                ImGui::GetBackgroundDrawList()->AddImage((void*) logo, ImVec2((width4 / -2) * size  + windowWidth / 2, (height4/ -2) * size  + windowHeight / 5), 
                                                ImVec2((width4 / 2) * size + windowWidth / 2, height4 / 2 * size + windowHeight / 5), ImVec2(0,0) , ImVec2(1, 1) , IM_COL32(255, 255, 255, 255));

           } break;
           case (State::MULTIPLAYER):{
                float size = 1.0f;
                ImGui::GetBackgroundDrawList()->AddImage((void*) logo, ImVec2((width4 / -2) * size  + windowWidth / 2, (height4/ -2) * size  + windowHeight / 5), 
                                                ImVec2((width4 / 2) * size + windowWidth / 2, height4 / 2 * size + windowHeight / 5), ImVec2(0,0) , ImVec2(1, 1) , IM_COL32(255, 255, 255, 255));
                client = unique_ptr<ChatClient>(new ChatClient(io_context, endpoints));
                client->startChat();
                clientState = State::SETTINGS;
           } break;
        }


        /*
        {
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

            ImGui::SliderFloat("float", &f, 0.25f, 5.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }*/


        // Chat Window
        // TODO - Kiyoshi
        /*
        {
            ImGui::SetNextWindowSize(ImVec2(windowWidth * 0.35, windowHeight * 0.35));
            ImGui::SetNextWindowPos(ImVec2(10, windowHeight - windowHeight * 0.35 - 30));
            ImGui::SetNextWindowBgAlpha(0.35f);
            ImGui::Begin("Chat Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::SetCursorPos(ImVec2(5, 0));
            ImGui::Text(log.c_str());

            int enter = glfwGetKey(window, GLFW_KEY_ENTER);
            ImGui::SetCursorPos(ImVec2(0, windowHeight * 0.35));
            ImGui::PushItemWidth(windowWidth * 0.35 - 70);
            ImGui::InputText("Message", inputText, IM_ARRAYSIZE(inputText));
            if (enter == GLFW_PRESS){
                if (pressed){
                    log += string(inputText) + '\n';
                    string inp(inputText);
                    client.write(inp);
                    for (int i = 0; i < 250; i++){
                        inputText[i] = '\0';
                    }
                }
                pressed = false;
            } else pressed = true;
            if (client.messageBuffer_.size()){
                log += client.messageBuffer_.front() + '\n';
                client.messageBuffer_.pop_front();
            }
              //log += client.messageBuffer_.front();
              //client.messageBuffer_.pop_front();
            ImGui::End();
        }
        */
        

        // TODO: uncomment when world object is available
        // for (auto& actor : world.actors) {
        //     actor.update(deltaTime);
        // }
        //cout << enemy.getPos().x << " " << enemy.getPos().y << endl;

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnd();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        pressed = false;
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    delete pixels;
    delete pixels1;
    delete pixels2;
    delete pixels3;

    return 0;
}
