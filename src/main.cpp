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

#include <boost/asio/read_until.hpp>
#include <boost/asio.hpp>

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
#include "actor.h"
#include "body.h"
#include "projectile.h"
#include "player.h"

using namespace std;

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

// raycasting probly doesnt need to be recursive but oh well
glm::vec2 size0(0,0);
void raycast(const glm::vec2& direction, glm::vec2& ray, const glm::vec2& initialPos, const vector<shared_ptr<PhysicsBody>>& space, int maxMag){
    while (true){
        if (world.checkCollisions(space, initialPos + ray, size0)) {
            return;
        }
        if (glm::length(ray) > maxMag) return;
        ray.x += direction.x;
        ray.y += direction.y;
    }
}
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void aStar(ImDrawList* list, const vector<shared_ptr<PhysicsBody>>& space, const glm::vec2& Camera, const glm::vec2& pos, const glm::vec2& finalPos, const float& step, const float& maxSize){
    if (world.checkCollisions(space, finalPos, glm::vec2(step / 2, step / 2))) return;
    struct Node {
        glm::vec2 position;
        float gCost, hCost, fCost;
        shared_ptr<Node> parent = nullptr;
        string toString() const{
            return to_string(round(position.x * 10)) + " " + to_string(round(position.y * 10));
        }
    };
    shared_ptr<Node> start = shared_ptr<Node>(new Node(pos, 0, 0, 0));
    auto cmp = [](shared_ptr<Node> left, shared_ptr<Node> right){
        return tie(left->fCost, left->hCost, left->gCost) > tie(right->fCost, right->hCost, right->gCost);
    };
    priority_queue<shared_ptr<Node>, vector<shared_ptr<Node>>, decltype(cmp)> open;
    unordered_set<string> openSet;
    unordered_set<string> closed;

    open.push(start);
    openSet.insert(start->toString());
    float shortestDistance = numeric_limits<float>::max();
    shared_ptr<Node> lastNode;
    while (open.size()){
        shared_ptr<Node> current = open.top();
        open.pop();
        openSet.erase(current->toString());
        closed.insert(current->toString());

        if (glm::length(current->position - finalPos) <= step * 1.1) {lastNode = current;break;}

        static auto checkPos = [&start, &current, &shortestDistance, &open, &closed, &openSet, &step](shared_ptr<Node>& newNode, const vector<shared_ptr<PhysicsBody>>& space, const glm::vec2& finalPos)
        {
            if (world.checkCollisions((world.staticBodies), newNode->position, size0) || closed.count(newNode->toString())) return;
            
            // erm what da sigma

            float cost = current->gCost + glm::length(newNode->position - current->position);
            if (!openSet.count(newNode->toString())){
                newNode->gCost = cost;
                newNode->hCost = glm::length(newNode->position - finalPos);
                newNode->fCost = newNode->gCost + newNode->hCost;
                newNode->parent = current;
                openSet.insert(newNode->toString());
                open.push(newNode);
            }
        };

        
        for (int i = -1; i <= 1; i++){
            for (int j = -1; j <= 1; j++){
                if (!i && !j) continue;
                shared_ptr<Node> temp = shared_ptr<Node>(new Node(current->position + glm::vec2(i * step, j * step), 0, 0, 0));
                checkPos(temp, world.staticBodies, finalPos);
            }
        }
    }
    int count = 0;
    for (shared_ptr<Node> tmp = lastNode; tmp; tmp = tmp->parent){
        count++;

        list->AddRectFilled(ImVec2(tmp->position.x - Camera.x + step / 2, tmp->position.y - Camera.y + step / 2) , ImVec2(tmp->position.x - Camera.x - step / 2, tmp->position.y - Camera.y - step / 2), IM_COL32(255, 255, 255, 100));
    }
}

// normalizes vector
void normalize(glm::vec2 &thing){
	if(thing.x == 0 && thing.y == 0) return;
	float mag = sqrt(pow(thing.x, 2) + pow(thing.y,2));
	thing.x /= mag;
	thing.y /= mag;
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
    glfwSwapInterval(0); // Enable vsync
    srand(time(0));

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
    unsigned char* pixels1 = stbi_load("./assets/images/run.png", &width1, &height1, &channels1, 4);
    images[1].width = width1;
    images[1].height = height1;
    images[1].pixels = pixels1;
    
    int width2, height2, channels2;
    unsigned char* pixels2 = stbi_load("./assets/images/edge.png", &width2, &height2, &channels2, 4);
    images[1].width = width2;
    images[1].height = height2;
    images[1].pixels = pixels2;
    glfwSetWindowIcon(window, 1, images);
    vector<Projectile> projectiles;
// uhh idk what this does i just do copy paste haha
    GLuint image_texture1, image_texture2, image_texture3;
    glGenTextures(1, &image_texture1);
    glGenTextures(1, &image_texture2);
    glGenTextures(1, &image_texture3);
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

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture

    world.staticBodies.push_back(shared_ptr<PhysicsBody>(new Rectangle(glm::vec2{500, 500}, glm::vec2{100, 100})));
    world.staticBodies.push_back(shared_ptr<PhysicsBody>(new Rectangle(glm::vec2{800, 532}, glm::vec2{250, 250})));
    world.staticBodies.push_back(shared_ptr<PhysicsBody>(new Rectangle(glm::vec2{430, 80}, glm::vec2{90, 90})));
    world.staticBodies.push_back(shared_ptr<PhysicsBody>(new Rectangle(glm::vec2{590, 700}, glm::vec2{100, 100})));
    world.staticBodies.push_back(shared_ptr<PhysicsBody>(new Rectangle(glm::vec2{1320, 5335}, glm::vec2{169, 169})));
    
// Main loop
    float timer = 50;
    float rtimer = 1000;
    double deltaTime = 0;
    double fireRate = 100;
    double reloadTime = 250;
    int mag = 1;
    auto start = std::chrono::system_clock::now(); 
    auto end = start;

    float angler = 3.14/4;
    Player player(glm::vec2(0, 0), glm::vec2(width, height), image_texture1);
    char clear[250] = "";
    char inputText[250] = "";
    string log = "";
    bool pressed = true;
    while (!glfwWindowShouldClose(window))
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_None);
        end = start;
        start = std::chrono::system_clock::now(); 
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        timer = timer + deltaTime;
        rtimer = rtimer + deltaTime;
        timer = (timer > fireRate)? fireRate : timer;
        rtimer = (rtimer > reloadTime)? reloadTime : rtimer;
        deltaTime = std::chrono::duration<double, std::milli>(start - end).count();
        static float f = 0.1f;

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

        glm::vec2 velocity{0, 0};
        int W= glfwGetKey(window, GLFW_KEY_W);
        int A= glfwGetKey(window, GLFW_KEY_A);
        int S= glfwGetKey(window, GLFW_KEY_S);
        int D= glfwGetKey(window, GLFW_KEY_D);
        int F = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        int R = glfwGetKey(window, GLFW_KEY_R);

        if (W == GLFW_PRESS){
            velocity.y -= 100;
        }
        if (A == GLFW_PRESS){
            velocity.x -= 100;
        }
        if (S == GLFW_PRESS){
            velocity.y += 100;
        }
        if (D == GLFW_PRESS){
            velocity.x += 100;
        }
//        if (R == GLFW_PRESS && rtimer == reloadTime){
//            cout << "Reloading" << endl;
//            rtimer = 0;
//            mag = 32;
//        }
         if (rtimer == reloadTime && !mag){
            //cout << "Reloading" << endl;
            rtimer = 0;
            mag = 1;
        }
        normalize(velocity);

	  // collision checkin
	  // eventually put this in player update
        player += glm::vec2(velocity.x / 3 * deltaTime, 0);
        if (glm::length(velocity) && world.checkCollisions(world.staticBodies, player.getRect())){
        	player += glm::vec2(-1 * velocity.x / 3 * deltaTime, 0);
        }

        player += glm::vec2(0, velocity.y / 3 * deltaTime);
        if (glm::length(velocity) && world.checkCollisions(world.staticBodies, player.getRect())){
        	player += glm::vec2(0, -1 * velocity.y / 3 * deltaTime);
        }

        
        glm::vec2 Camera = {player.pos().x - windowWidth / 2, player.pos().y - windowHeight / 2};
	// draws squaes
        for (const shared_ptr<PhysicsBody>& x: world.staticBodies){
            ImGui::GetBackgroundDrawList()->AddImage((void*) image_texture1, ImVec2(x->start().x - Camera.x, x->start().y - Camera.y) , 
                                                ImVec2(x->end().x - Camera.x, x->end().y - Camera.y), ImVec2(0,0) , ImVec2(1, 1) , IM_COL32(255, 255, 255, 255));
        }


        glm::vec2 direction = {xPos - windowWidth / 2, yPos - windowHeight /2};
        normalize(direction);
        aStar(ImGui::GetForegroundDrawList(), world.staticBodies, Camera, player.pos(), glm::vec2(player.pos().x + xPos - windowWidth / 2, player.pos().y + yPos - windowHeight / 2), 50.0f, 10.0f);

        // SPAWN PROJECTILES
        if (F == GLFW_PRESS && timer >= fireRate && mag && rtimer == reloadTime){
            // for projectiles;
//            glm::vec2 initial = player.pos();
//            glm::vec2 end {0.0};
//            float angle = 3.14 / 50 - 3.14 / 25 * (rand() % 100 + 1) / 100;
//            cout << angle << endl;
//            glm::vec2 rotatedDir = {direction.x * cos(angle) - direction.y * sin(angle), direction.x * sin(angle) + direction.y * cos(angle)};
//            glm::vec2 dir = {rotatedDir.x * 100.0 , rotatedDir.y * 100.0};
//	        if(dir.x && dir.y)
//            normalize(dir);
//            raycast(dir, end, initial, space, 1000);
//            end.x += initial.x;
//            end.y += initial.y;
//            Projectile proj = {player.pos(), dir, end, true};
//            normalize(proj.vel);
//            projectiles.push_back(proj);
//            mag--;
//            cout << "Ammo: " << mag << " / 32" << endl;
//            timer = 0.0;

            // for melee
            mag--;
        }
        scale = .05;

        // DO PROJECTILE THINGS
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

//        for (float i = -3.14/5; i <= 3.14/5; i += .005){
//            glm::vec2 rotatedDir = {direction.x * cos(i) - direction.y * sin(i), direction.x * sin(i) + direction.y * cos(i)};
//            glm::vec2 ray = {0, 0};
//            glm::vec2 initial(characterX, characterY);
//            raycast(rotatedDir, ray, initial, space, 250);
//            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(initial.x - Camera.x, initial.y + i - Camera.y), ImVec2(initial.x + ray.x - Camera.x, initial.y + ray.y + i - Camera.y) , IM_COL32(200, 200, 200, 10), 25);
//        }
        ImGui::PushFont(font2);
        string skib = to_string(mag) + "/32";
        ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(windowWidth - 110 - 10, windowHeight - 110 - 25) , ImVec2(windowWidth - 10, windowHeight - 25), IM_COL32(255, 255, 255, 100));
        ImGui::GetBackgroundDrawList()->AddRect(ImVec2(windowWidth - 110 - 10, windowHeight - 110 - 25) , ImVec2(windowWidth - 10, windowHeight - 25), IM_COL32(255, 255, 255, 255));
        ImGui::GetBackgroundDrawList()->AddImage((void*) image_texture3, ImVec2(windowWidth - 110 - 10, windowHeight - 110 - 25) , 
                                                ImVec2(windowWidth - 10, windowHeight - 25), ImVec2(0,0) , ImVec2(1, 1) , IM_COL32(255, 255, 255, 255));
        ImGui::GetForegroundDrawList()->AddText(ImVec2(windowWidth - ImGui::CalcTextSize(skib.c_str()).x - 10, windowHeight - ImGui::CalcTextSize(skib.c_str()).y - 140), IM_COL32_WHITE, (skib).c_str());

        if (rtimer < reloadTime){
            angler = 3.14/4 - 3.14/2 * rtimer/reloadTime;
            ImGui::GetForegroundDrawList()->AddLine(ImVec2(0, windowHeight - 25 / 2), ImVec2(windowWidth * rtimer / reloadTime, windowHeight - 25 / 2) , IM_COL32(200, 200, 200, 100), 25);
        }
        //ImGui::GetBackgroundDrawList()->AddImage((void*) image_texture1, ImVec2(width * scale + characterX - width * scale / 2 - Camera.x, height * scale + characterY - height * scale /2 - Camera.y) , 
        //                                ImVec2(characterX - width * scale / 2 - Camera.x, characterY - height * scale / 2 - Camera.y) , ImVec2(1,1) , ImVec2(0, 0) , IM_COL32(255, 255, 255, 255));
        player.render(ImGui::GetBackgroundDrawList(), Camera);
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        // if (show_demo_window)
        //    ImGui::ShowDemoWindow(&show_demo_window);
        static float angle = 0.0f;
        angle += deltaTime / 1000  * 1.0f;
        ImDrawList* list = ImGui::GetBackgroundDrawList();
        ImGui::PopFont();

        int flip = 1;
        glm::vec2 rotatedDir;
        if (rtimer < reloadTime)
            rotatedDir = {direction.x * cos(angler) - direction.y * sin(angler), direction.x * sin(angler) + direction.y * cos(angler)};
        else
            rotatedDir = direction;
        float angul = atan(rotatedDir.y/rotatedDir.x);
        float flippy = 1;
        if (rotatedDir.x < 0) {angul += 3.14/2 /*- 3.14 / 2*/; flip = -1; flippy = -1;}
        else angul += 3.14/2;
        ImageRotated((void*) image_texture2, ImVec2(width * scale + player.pos().x - width * scale - Camera.x + rotatedDir.x * 80, height * scale + player.pos().y - height * scale - Camera.y + rotatedDir.y * 100), ImVec2(width * flippy * .125, flip * height * .125), angul, list); 
        ImageRotated((void*) image_texture1, ImVec2(1500 - Camera.x, 900 - Camera.y), ImVec2(200, 200.0f), angle, list); 
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        ImGui::PushFont(font1);
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

        // Chat Window
        // TODO - Kiyoshi
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
                    for (int i = 0; i < 250; i++){
                        inputText[i] = '\0';
                    }
                }
                pressed = false;
            } else pressed = true;
              //log += client.messageBuffer_.front();
              //client.messageBuffer_.pop_front();
            ImGui::End();
        }
        ImGui::PopFont();

        // TODO: uncomment when world object is available
        // for (auto& actor : world.actors) {
        //     actor.update(deltaTime);
        // }

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
