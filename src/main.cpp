#include <iostream>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <random>
#include <chrono>
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

#include <glm/glm.hpp>
#include "actor.h"
#include "square.h"
#include "projectile.h"

using namespace std;

/* 
STOLEN CODEE !!!
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

void raycast(const glm::vec2& direction, glm::vec2& ray, const glm::vec2& initialPos, const vector<Square>& space, int maxMag){
    if (glm::length(ray) > maxMag) return;
    for (const Square& s: space) {
       if (((ray.x + initialPos.x > s.start.x && ray.x + initialPos.x < s.end.x) && 
           (ray.y + initialPos.y > s.start.y && ray.y + initialPos.y < s.end.y))) {
            return;
        }
    }
    ray.x += direction.x;
    ray.y += direction.y;
    raycast(direction, ray, initialPos, space, maxMag);
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
    glewInit();

    // Set Window Icon
    int width, height, channels;
    unsigned char* pixels = stbi_load("./assets/images/noelle.jpg", &width, &height, &channels, 4);

    GLFWimage images[2];
    images[0].width = width;
    images[0].height = height;
    images[0].pixels = pixels;

    int width1, height1, channels1;
    unsigned char* pixels1 = stbi_load("./assets/images/gun.png", &width1, &height1, &channels1, 4);
    images[1].width = width1;
    images[1].height = height1;
    images[1].pixels = pixels1;
    glfwSetWindowIcon(window, 1, images);
    vector<Projectile> projectiles;
// uhh idk what this does i just do copy paste haha
    GLuint image_texture1, image_texture2;
    glGenTextures(1, &image_texture1);
    glGenTextures(1, &image_texture2);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, image_texture2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width1, height1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels1);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, image_texture1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture

    vector<Square> space;

    space.push_back(Square{glm::vec2{500, 500}, glm::vec2{600, 600}});
    space.push_back(Square{glm::vec2{0, 0}, glm::vec2{250, 250}});
    space.push_back(Square{glm::vec2{80, 80}, glm::vec2{90, 90}});
    space.push_back(Square{glm::vec2{0, 700}, glm::vec2{1000, 1000}});
    
// Main loop
    double characterX = 1280 / 2, characterY = 720 / 2;
    float timer = 50;
    double deltaTime = 0;
    double fireRate = 100;
    int mag = 32;
    auto start = std::chrono::system_clock::now(); 
    auto end = start;
    while (!glfwWindowShouldClose(window))
    {
        end = start;
        start = std::chrono::system_clock::now(); 
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        timer = timer + deltaTime;
        timer = (timer > fireRate)? fireRate : timer;
        deltaTime = std::chrono::duration<double, std::milli>(start - end).count();
        static float f = 0.1f;
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
        ImGui::GetForegroundDrawList()->AddImage((void*) image_texture1, ImVec2(width * scale + xPos - width * scale /2, height * scale + yPos - height * scale /2) , 
                                        ImVec2(0 + xPos - width * scale / 2, 0 + yPos - height * scale/ 2), ImVec2(1,1) , ImVec2(0, 0) , IM_COL32(255, 255, 255, 255));


        glm::vec2 velocity = {0, 0};
        int W= glfwGetKey(window, GLFW_KEY_W);
        int A= glfwGetKey(window, GLFW_KEY_A);
        int S= glfwGetKey(window, GLFW_KEY_S);
        int D= glfwGetKey(window, GLFW_KEY_D);
        int SHIFT= glfwGetKey(window, GLFW_KEY_Q);
        int F = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        int R = glfwGetKey(window, GLFW_KEY_R);

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
        if (R == GLFW_PRESS){
            cout << "Reloading" << endl;
            mag = 32;
        }
        velocity = glm::normalize(velocity);

        bool moveX = true;
        bool moveY = true;
        float woffset = width * scale / 2;
        float hoffset = height * scale / 2;
        for (const Square& s: space) {
            if (((characterX + velocity.x / 3 * deltaTime + woffset > s.start.x && characterX + velocity.x / 3 * deltaTime - woffset < s.end.x) && 
                (characterY + hoffset> s.start.y && characterY - hoffset < s.end.y))){
                   moveX = false;
            }
            if (((characterX + woffset> s.start.x && characterX - woffset < s.end.x) && 
                (characterY + velocity.y / 3 * deltaTime + hoffset> s.start.y && characterY + velocity.y / 3 * deltaTime - hoffset < s.end.y))){
                   moveY = false;
            }
        }

        if (moveX){
            characterX += velocity.x / 3 * deltaTime;
        }
        if (moveY){
            characterY += velocity.y / 3* deltaTime;
        }
        
        glm::vec2 Camera = {characterX - windowWidth / 2, characterY - windowHeight / 2};
        for (const Square& x: space){
            ImGui::GetBackgroundDrawList()->AddImage((void*) image_texture1, ImVec2(x.start.x - Camera.x, x.start.y - Camera.y) , 
                                                ImVec2(x.end.x - Camera.x, x.end.y - Camera.y), ImVec2(0,0) , ImVec2(1, 1) , IM_COL32(255, 255, 255, 255));
        }

        glm::vec2 direction = {xPos - windowWidth / 2, yPos - windowHeight /2};
        direction = glm::normalize(direction);

        if (F == GLFW_PRESS && timer >= fireRate && mag){
            glm::vec2 initial = {characterX, characterY};
            glm::vec2 end {0.0};
            glm::vec2 dir = {direction.x * 100.0 / (rand() % 20 + 80), direction.y * 100.0 / (rand() % 20 + 90)};
            dir = glm::normalize(dir);
            raycast(dir, end, initial, space, 1000);
            cout << end.x << " " << end.y << endl;
            end.x += initial.x;
            end.y += initial.y;
            Projectile proj = {{characterX, characterY}, dir, end, true};
            proj.vel = glm::normalize(proj.vel);
            projectiles.push_back(proj);
            mag--;
            cout << "Ammo: " << mag << " / 32" << endl;
            timer = 0.0;
        }
        scale = .05;
        for (Projectile& a: projectiles){
            if (!a.render) continue;
            for (int i = 0; i < 50 * deltaTime / 6; i++){
                if (sqrt(pow(a.pos.x - a.end.x, 2) + pow(a.pos.y - a.end.y, 2)) <= 25) {a.render = false; break;}
                a.pos.x += a.vel.x;
                a.pos.y += a.vel.y;
            }
            if (a.render)
            ImGui::GetBackgroundDrawList()->AddImage((void*) image_texture1, ImVec2(width * scale + a.pos.x - width * scale / 2 - Camera.x, height * scale + a.pos.y - height * scale /2 - Camera.y) , 
                                                    ImVec2(a.pos.x - width * scale / 2 - Camera.x, a.pos.y - height * scale / 2 - Camera.y), ImVec2(1,1) , ImVec2(0, 0) , IM_COL32(255, 255, 255, 255));
        }
        scale = .1;
        for (float i = -3.14/5; i <= 3.14/5; i += .005){
            glm::vec2 rotatedDir = {direction.x * cos(i) - direction.y * sin(i), direction.x * sin(i) + direction.y * cos(i)};
            glm::vec2 ray = {0, 0};
            glm::vec2 initial = {characterX, characterY};
            raycast(rotatedDir, ray, initial, space, 250);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(initial.x - Camera.x, initial.y + i - Camera.y), ImVec2(initial.x + ray.x - Camera.x, initial.y + ray.y + i - Camera.y) , IM_COL32(200, 200, 200, 10), 25);
        }
        ImGui::GetBackgroundDrawList()->AddImage((void*) image_texture1, ImVec2(width * scale + characterX - width * scale / 2 - Camera.x, height * scale + characterY - height * scale /2 - Camera.y) , 
                                        ImVec2(characterX - width * scale / 2 - Camera.x, characterY - height * scale / 2 - Camera.y ) , ImVec2(1,1) , ImVec2(0, 0) , IM_COL32(255, 255, 255, 255));
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        // if (show_demo_window)
        //    ImGui::ShowDemoWindow(&show_demo_window);
        static float angle = 0.0f;
        angle += deltaTime / 1000  * 1.0f;
        ImDrawList* list = ImGui::GetBackgroundDrawList();

        int flip = 1;
        float angul = atan(direction.y/direction.x);
        cout << direction.x << " " << direction.y << endl;
        if (direction.x <= 0) {angul += 3.14; flip = -1;}
        ImageRotated((void*) image_texture2, ImVec2(width * scale + characterX - width * scale - Camera.x + direction.x * 70, height * scale + characterY - height * scale - Camera.y + direction.y * 70), ImVec2(width * scale, flip * height * scale), angul, list); 
        ImageRotated((void*) image_texture1, ImVec2(1500 - Camera.x, 900 - Camera.y), ImVec2(200, 200.0f), angle, list); 
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.001f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
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

        ActorManager::update(deltaTime);

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

        // Make sure this is commented out in the final build
        //ActorManager::checkForDanglingActors();
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
