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

deque<double> scrollBuffer;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    scrollBuffer.push_back(yoffset);
}


void load(const string& filename, World& world){
	  ifstream ins2(filename);
    string test;
     getline(ins2,test);
     
     while (ins2) {
     getline(ins2,test);
     istringstream iss(test);
     string type;
     string tempp;
     getline(iss,type,',');
     getline(iss,tempp,',');
     if(tempp== "") break;
     int tempx = stoi(tempp);
     getline(iss,tempp,',');
     int tempy= stoi(tempp);
     getline(iss,tempp,',');
     int sizex = stoi(tempp);
     getline(iss,tempp,',');
     int sizey = stoi(tempp);
     world.staticBodies.push_back(shared_ptr<PhysicsBody>(new Rect(glm::vec2{tempx, tempy}, glm::vec2{sizex, sizey})));
  }
}
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

struct Image{
    int width, height, channels;
    unsigned char* pixels;
    GLuint texture;
    Image(){}
    Image(string filename, int index, GLFWimage images[]){
       pixels = stbi_load((filename).c_str(), &width, &height, &channels, 4);
       images[index].width = width;
       images[index].height = height;
       images[index].pixels = pixels;
       glGenTextures(1, &texture);

       glBindTexture(GL_TEXTURE_2D, texture);
       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
       glGenerateMipmap(GL_TEXTURE_2D);
    }
};
struct Stratagem {
    vector<unsigned char> arrows;
    Stratagem(){}
    Stratagem(int size){
        for (int i = 0; i < size; i++){
            arrows.push_back(rand() % 8 + 1);
        } 
    }
    void reset(int size){
        arrows.clear();
        for (int i = 0; i < size; i++){
            arrows.push_back(rand() % 8 + 1);
        } 
    }
};

// callbacks
static void glfw_error_callback(int error, const char* description)
{
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}
bool pressed = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  pressed = true;
  ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
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
  auto endpoints = resolver.resolve("localhost", "6969");
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
  ImFont* font1 = io.Fonts->AddFontFromFileTTF("./assets/fonts/roboto.ttf", 25.0f);
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
  unsigned char* pixels1 = stbi_load("./assets/images/cursor.png", &width1, &height1, &channels1, 4);
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

  bool hover = false;
    static World world;
    static bool loading = true;
    if (loading){
      load("assets/images/mapfile/map3.csv", world);
      loading = false;
    }
    static shared_ptr<World> worldptr = shared_ptr<World>(&world);
    // Upload pixels into texture
   ItemRegistry::init();
    Weapon weap = ItemRegistry::getWeapon("Edge Blade");
    vector<shared_ptr<Image>> ims;

// Main loop
    double deltaTime = 0;
    int mag = 1;

    float angler = 3.14/4;
    static float f = 1.0f;

    Player player(glm::vec2(0, 0), glm::vec2(width *0.25, height* 0.25), image_texture1);
    player.setName("jon");
    world.players.push_back(shared_ptr<Player>(&player));

    Npc npc("john", shared_ptr<PhysicsBody>(new Rect(glm::vec2(500, 0), glm::vec2(width * .1, height * .1))));

    // TODO MATTHEW - DIALOGUE
    npc.planned.push_back("Crazy? I was crazy once.");
    npc.planned.push_back("They locked me in a room.");
    npc.planned.push_back("A rubber room.");
    npc.planned.push_back("A rubber room with rats.");
    npc.planned.push_back("And rats make me crazy.");

    npc.dialogue.push_back("i love noelle silva and mai sakurajima!");

    world.addActor(shared_ptr<Actor>(&npc));
    world.addActor(shared_ptr<Actor>(&player));

    for (int i =0; i < 7; i++){
       shared_ptr<Image> image = shared_ptr<Image>(new Image);
       image->pixels = stbi_load(("./assets/images/weapons/im" + to_string(i) + ".jpg").c_str(), &image->width, &image->height, &image->channels, 4);
       images[3 + i].width = image->width;
       images[3 + i].height = image->height;
       images[3 + i].pixels = image->pixels;
       glGenTextures(1, &image->texture);

        glBindTexture(GL_TEXTURE_2D, image->texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        ims.push_back(image);
    }

    Image back("./assets/images/arrows/b.png", 10, images);
    Image down("./assets/images/arrows/d.png", 11, images);
    Image downBack("./assets/images/arrows/db.png", 12, images);
    Image downForward("./assets/images/arrows/df.png", 13, images);
    Image forward("./assets/images/arrows/f.png", 14, images);
    Image up("./assets/images/arrows/u.png", 15, images);
    Image upBack("./assets/images/arrows/ub.png", 16, images);
    Image upForward("./assets/images/arrows/uf.png", 17, images);

    unordered_map<int, Image*> arrowMap = {
        {1, &up},
        {2, &forward},
        {3, &down},
        {4, &back},
        {5, &upForward},
        {6, &downForward},
        {7, &downBack},
        {8, &upBack}
    };
    player.addWeapon("Edge Blade");
    //player.addWeapon("Edge Blade");
    //player.addWeapon("Sniper");
    //player.addWeapon("Z-Zip");

    char clear[250] = "";
    char inputText[250] = "";
    string log = "";
    bool pressed = true;
    float xSize = 1.0, ySize = 1.0;
    float skibid = 50;

    Stratagem stratagem(6);
  while (!glfwWindowShouldClose(window))
  {
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
    float cursorScale = windowHeight * windowWidth / (1280.0f * 720.0f);

    if (!hover)
      ImGui::GetForegroundDrawList()->AddImage((void*) image_texture2, ImVec2(xPos, yPos), 
        ImVec2(xPos + width1 * 0.4 * 0.075 * cursorScale,  yPos + height1 * 0.075 * cursorScale), ImVec2(0.1,0) , ImVec2(0.5, 1) , IM_COL32(255, 255, 255, 255));
    else
      ImGui::GetForegroundDrawList()->AddImage((void*) image_texture2, ImVec2(xPos, yPos), 
        ImVec2(xPos + width1 * 0.5 * 0.075 * cursorScale,  yPos + height1 * 0.075 * cursorScale), ImVec2(0.5,0) , ImVec2(1, 1) , IM_COL32(255, 255, 255, 255));
    hover = false;
    int W= glfwGetKey(window, GLFW_KEY_W);
    int A= glfwGetKey(window, GLFW_KEY_A);
    int S= glfwGetKey(window, GLFW_KEY_S);
    int D= glfwGetKey(window, GLFW_KEY_D);
    int F= glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    int R= glfwGetKey(window, GLFW_KEY_R);

    switch (clientState){
      case (State::INTRO): 
        {
          glfwSetKeyCallback(window, key_callback);
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
      case (State::MAINMENU): 
        {
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
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,255,255,255));
            ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::Begin("Another Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings  );   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::SetCursorPos(ImVec2(windowWidth / 4, windowHeight * 9/ 20));
            if (ImGui::Button("Singleplayer", ImVec2(windowWidth / 2, windowHeight / 10)))
              clientState = State::SINGLEPLAYER;
            ImGui::SetCursorPos(ImVec2(windowWidth / 4, windowHeight * (1.0f / 2.0f + 1.0f / 8.0f - 1.0f / 20.0f)));
            if (ImGui::IsItemHovered()) hover = true;
            if (ImGui::Button("Multiplayer", ImVec2(windowWidth / 2, windowHeight / 10)))
              clientState = State::MULTIPLAYER;
            if (ImGui::IsItemHovered()) hover = true;
            ImGui::SetCursorPos(ImVec2(windowWidth / 4, windowHeight * 7 / 10));
            if (ImGui::Button("Settings", ImVec2(windowWidth / 2, windowHeight / 10)))
              clientState = State::SETTINGS;
            if (ImGui::IsItemHovered()) hover = true;
            ImGui::PopStyleColor();
            ImGui::End();
          }
        } break;
      case (State::SETTINGS): 
        {
          float size = 1.0f;
          ImGui::GetBackgroundDrawList()->AddImage((void*) logo, ImVec2((width4 / -2) * size  + windowWidth / 2, (height4/ -2) * size  + windowHeight / 5), 
              ImVec2((width4 / 2) * size + windowWidth / 2, height4 / 2 * size + windowHeight / 5), ImVec2(0,0) , ImVec2(1, 1) , IM_COL32(255, 255, 255, 255));
        } break;
      case (State::SINGLEPLAYER):
        {
          float size = 1.0f;
          ImGui::GetBackgroundDrawList()->AddImage((void*) logo, ImVec2((width4 / -2) * size  + windowWidth / 2, (height4/ -2) * size  + windowHeight / 5), 
              ImVec2((width4 / 2) * size + windowWidth / 2, height4 / 2 * size + windowHeight / 5), ImVec2(0,0) , ImVec2(1, 1) , IM_COL32(255, 255, 255, 255));

            string prompt = "Long ago there was a man named Surin.";
            ImGui::GetForegroundDrawList()->AddText(ImVec2(windowWidth / 2 - (ImGui::CalcTextSize(prompt.c_str()).x) / 2, windowHeight * 2 / 3 + ImGui::CalcTextSize(prompt.c_str()).y / 2), IM_COL32_WHITE, (prompt).c_str());

            if (pressed == true){
              clientState = State::SINGLEGAME;
            }

        } break;
      case (State::MULTIPLAYER):
        {
          float size = 1.0f;
          ImGui::GetBackgroundDrawList()->AddImage((void*) logo, ImVec2((width4 / -2) * size  + windowWidth / 2, (height4/ -2) * size  + windowHeight / 5), 
              ImVec2((width4 / 2) * size + windowWidth / 2, height4 / 2 * size + windowHeight / 5), ImVec2(0,0) , ImVec2(1, 1) , IM_COL32(255, 255, 255, 255));
          {
            static char address[50] = "localhost";
            static char port[50] = "6969";
            static char username[50] = "A2AxPhoenix";
            int backspace = glfwGetKey(window, GLFW_KEY_BACKSPACE);
            ImGui::SetNextWindowSize(ImVec2(windowWidth / 3, windowHeight / 5));
            ImGui::SetNextWindowPos(ImVec2(windowWidth * (1.0f / 2.0f - 1.0f / 6.0f), windowHeight * (1.0f / 2.0f - 1.0f / 10.0f)));
            ImGui::Begin("Connect", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings );
            ImGui::GetIO().FontGlobalScale = windowWidth * windowHeight / 2138400.0f;
            ImGui::Text("IP: "); 
            ImGui::SameLine();
            ImGui::InputText("##address", address, IM_ARRAYSIZE(address));
            if (ImGui::IsItemHovered()) hover = true;
            ImGui::Text("PORT: "); 
            ImGui::SameLine();
            ImGui::InputText("##port", port, IM_ARRAYSIZE(port));
            if (ImGui::IsItemHovered()) hover = true;
            ImGui::NewLine();
            ImGui::Text("Username: "); 
            ImGui::SameLine();
            ImGui::InputText("##user", username, IM_ARRAYSIZE(username));
            if (ImGui::IsItemHovered()) hover = true;

            ImGuiStyle& style = ImGui::GetStyle();
            float size = ImGui::CalcTextSize("Connect").x + style.FramePadding.x * 2.0f;

            float avail = ImGui::GetContentRegionAvail().x;
            float off = (avail - size) * 0.5f;
            if (off > 0.0f){
              ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
            }
            if (ImGui::Button("Connect")){
              endpoints = resolver.resolve(address, port);
              try{
                string user(username);
                client = unique_ptr<ChatClient>(new ChatClient(io_context, endpoints, user));
                client->startChat();
                clientState = State::MULTIGAME;
              }
              catch (boost::wrapexcept<boost::system::system_error> error){
                cout << "Unable to connect to port" << endl;
              }
            }
            if (ImGui::IsItemHovered()) hover = true;
            size = ImGui::CalcTextSize("Back").x + style.FramePadding.x * 2.0f;
            avail = ImGui::GetContentRegionAvail().x;
            off = (avail - size) * 0.5f;
            if (off > 0.0f){
              ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
            }
            if (ImGui::Button("Back")){
              clientState = State::MAINMENU;
            }
            if (ImGui::IsItemHovered()) hover = true;
            ImGui::End();


          }
        } break;
      case (State::MULTIGAME):
        {

        }break;
      case (State::SINGLEGAME):
      {

    static jthread t1(&World::update, &world);

    bool brugs = false;
        if (player.getHp() < 0 || world.deaths > 50) brugs = true;
        end = start;
        start = std::chrono::system_clock::now(); 
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        deltaTime = std::chrono::duration<double, std::milli>(start - end).count();

        if (scrollBuffer.size()){
            skibid += scrollBuffer.back();
            skibid = min(100.0f, skibid);
            skibid = max(1.0f, skibid);
            scrollBuffer.pop_back();
        }
        f = (jujutsu(skibid) * 2.5 + 0.001);

        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        double scale = 0.1;
        //ImGui::GetForegroundDrawList()->AddImage((void*) image_texture1, ImVec2(width *// scale + xPos - width * scale /2, height * scale + yPos - height * scale /2) , 
//                                        ImVec2(0 + xPos - width * scale / 2, 0 + yPos -// height * scale/ 2), ImVec2(1,1) , ImVec2(0, 0) , IM_COL32(255, 255, 255, 255));

        glm::vec2 velocity{0, 0};
        int W= glfwGetKey(window, GLFW_KEY_W);
        int A= glfwGetKey(window, GLFW_KEY_A);
        int S= glfwGetKey(window, GLFW_KEY_S);
        int D= glfwGetKey(window, GLFW_KEY_D);
        int num1= glfwGetKey(window, GLFW_KEY_1);
        int num2= glfwGetKey(window, GLFW_KEY_2);
        int num3= glfwGetKey(window, GLFW_KEY_3);
        int F = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        int R = glfwGetKey(window, GLFW_KEY_R);
        int E = glfwGetKey(window, GLFW_KEY_E);

        int Q = glfwGetKey(window, GLFW_KEY_Q);

        if (num1 == GLFW_PRESS && player.weaponNames.size() >= 1){
            player.curWeapon = &player.inventory.getWeapon(player.getWeapInd(0));
        }
        if (num2 == GLFW_PRESS && player.weaponNames.size() >= 2){
            player.curWeapon = &player.inventory.getWeapon(player.getWeapInd(1));
        }
        if (num3 == GLFW_PRESS && player.weaponNames.size() >= 3){
            player.curWeapon = &player.inventory.getWeapon(player.getWeapInd(2));
        }

        {
            // puzzle state dont wanna rename tho
            static bool stratagemState = false;
            static bool justPressed = false;
            static int puzzle = 0;
            if (!stratagemState){
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
                if (Q == GLFW_PRESS){
                    if (justPressed){
                        stratagemState = true;
                        if (!puzzle) puzzle = rand() % 3 + 1; 
                    }
                    justPressed = false;
                } else {
                    justPressed = true;
                }
            } else{
                if (Q == GLFW_PRESS){
                    if (justPressed)
                        stratagemState = false;
                    justPressed = false;
                } else {
                    justPressed = true;
                }

               if (puzzle == 1) {
                {
                    //ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,255,255,255));
                 ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 1.0f, 1.0f, 0.75f)); // Set window background to red
                    ImGui::SetNextWindowSize(ImVec2(windowWidth * 3 / 4.0f, windowHeight * 3 / 4.0f));
                    ImGui::SetNextWindowPos(ImVec2(windowWidth *(1/2.0f - 3/8.0f) , windowHeight * (1/2.0f - 3/8.0f)));
                    ImGui::Begin("Stratagem", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings  );   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
                    ImVec2 curSize = ImGui::GetWindowSize();
                    static int position = 0;

                    ImGui::SetCursorPos(ImVec2(curSize.x /2 - 85, curSize.y * .75 - 85));
                    if (ImGui::ImageButton((void*) upBack.texture, ImVec2(50, 50), ImVec2(0, 0), ImVec2(1,1), 1.0f)){
                        if (stratagem.arrows.at(position) == 8) position++;
                        else position = 0; 
                    }
                    ImGui::SetCursorPos(ImVec2(curSize.x /2 - 25, curSize.y * .75 - 85));
                    if (ImGui::ImageButton((void*) up.texture, ImVec2(50, 50), ImVec2(0, 0), ImVec2(1,1), 1.0f)){

                        if (stratagem.arrows.at(position) == 1) position++;
                        else position = 0; 
                    }
                    ImGui::SetCursorPos(ImVec2(curSize.x /2 + 35, curSize.y * .75 - 85));
                    if (ImGui::ImageButton((void*) upForward.texture, ImVec2(50, 50), ImVec2(0, 0), ImVec2(1,1), 1.0f)){

                        if (stratagem.arrows.at(position) == 5) position++;
                        else position = 0; 
                    }

                    ImGui::SetCursorPos(ImVec2(curSize.x /2 - 85, curSize.y * .75 - 25));
                    if (ImGui::ImageButton((void*) back.texture, ImVec2(50, 50), ImVec2(0, 0), ImVec2(1,1), 1.0f)){

                        if (stratagem.arrows.at(position) == 4) position++;
                        else position = 0; 
                    }

                    ImGui::SetCursorPos(ImVec2(curSize.x /2 + 35, curSize.y * .75 - 25));
                    if (ImGui::ImageButton((void*) forward.texture, ImVec2(50, 50), ImVec2(0, 0), ImVec2(1,1), 1.0f)){
                        if (stratagem.arrows.at(position) == 2) position++;
                        else position = 0; 
                    }

                    ImGui::SetCursorPos(ImVec2(curSize.x /2 - 85, curSize.y * .75 + 35));
                    if (ImGui::ImageButton((void*) downBack.texture, ImVec2(50, 50), ImVec2(0, 0), ImVec2(1,1), 1.0f)){

                        if (stratagem.arrows.at(position) == 7) position++;
                        else position = 0; 
                    }
                    ImGui::SetCursorPos(ImVec2(curSize.x /2 - 25, curSize.y * .75 + 35));
                    if (ImGui::ImageButton((void*) down.texture, ImVec2(50, 50), ImVec2(0, 0), ImVec2(1,1), 1.0f)){

                        if (stratagem.arrows.at(position) == 3) position++;
                        else position = 0; 
                    }
                    ImGui::SetCursorPos(ImVec2(curSize.x /2 + 35, curSize.y * .75 + 35));
                    if (ImGui::ImageButton((void*) downForward.texture, ImVec2(50, 50), ImVec2(0, 0), ImVec2(1,1), 1.0f)){

                        if (stratagem.arrows.at(position) == 6) position++;
                        else position = 0; 
                    }

                    if (position >= stratagem.arrows.size()){
                        stratagem.reset(6);
                        stratagemState = false;
                        position = 0;
                        puzzle = 0;
                    }

                    for (int i = 0; i < stratagem.arrows.size(); i++){
                        if (i <= stratagem.arrows.size() / 2)
                            ImGui::SetCursorPos(ImVec2(curSize.x / 2 - 100 * (stratagem.arrows.size() /2 - i), curSize.y * .25));
                        else
                            ImGui::SetCursorPos(ImVec2(curSize.x / 2 + 100 * (i - stratagem.arrows.size() / 2 + 1) - 100, curSize.y * .25));
                        if (i < position)
                            ImGui::Image((void*) arrowMap.at(stratagem.arrows.at(i))->texture, ImVec2(100, 100), ImVec2(0.0f,0.0f), ImVec2(1.0f,1.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                        else ImGui::Image((void*) arrowMap.at(stratagem.arrows.at(i))->texture, ImVec2(100, 100), ImVec2(0.0f,0.0f), ImVec2(1.0f,1.0f), ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
                    }
                    ImGui::PopStyleColor();
                    ImGui::End();
                    }   
                // PUZZLE NUMBER 2
                } else if (puzzle == 2){ 
                   ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 1.0f, 1.0f, 0.75f)); // Set window background to red
                    ImGui::SetNextWindowSize(ImVec2(windowWidth * 3 / 4.0f, windowHeight * 3 / 4.0f));
                    ImGui::SetNextWindowPos(ImVec2(windowWidth *(1/2.0f - 3/8.0f) , windowHeight * (1/2.0f - 3/8.0f)));
                    ImGui::Begin("Rainbow (sorry if you're colorblind)", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings  );   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

                    ImVec2 curSize = ImGui::GetWindowSize();
                    static int position = 0;
                    static bool reset = true;
                    static vector<pair<ImVec2, int>> positions;
                    static vector<int> values;
                    static const vector<ImVec4> colors = {
                        ImVec4(255 / 255.0f, 68 / 255.0f, 0 / 255.0f, 255 / 255.0f), ImVec4(255 / 255.0f, 145 / 255.0f, 0 / 255.0f, 255 / 255.0f), ImVec4(255 / 255.0f, 230 / 255.0f, 0 / 255.0f, 255 / 255.0f), ImVec4(34 / 255.0f, 255 / 255.0f, 0 / 255.0f, 255 / 255.0f), ImVec4(0 / 255.0f, 183 / 255.0f, 255 / 255.0f, 255 / 255.0f), ImVec4(43 / 255.0f, 0 / 255.0f, 255 / 255.0f, 255 / 255.0f), ImVec4(230 / 255.0f, 0 / 255.0f, 255 / 255.0f, 255 / 255.0f) 
                    };
                    
                    static const int MAX_SIZE = 100;
                    static const int MIN_SIZE = 30;
                    
                    if (reset){
                        positions.clear();
                        for (int i = 0; i < 7; i++){
                            int x = rand() % 2;
                            int y = rand() % 2;
                            if (!x) x = -1;
                            if (!y) y = -1; 
                            positions.push_back(make_pair(ImVec2(curSize.x / 2 - curSize.x * 0.4 * (rand() % 100 + 1) / 100.0f * x, curSize.y / 2 - curSize.y * 0.4 * (rand() % 100 + 1) / 100.0f * y ), rand() % (MAX_SIZE - MIN_SIZE + 1) + MIN_SIZE));
                        }
                        reset = false;
                    }

                    for (int i = position; i < 7; i++){
                        ImGui::PushStyleColor(ImGuiCol_Button, colors.at(i)); // Set window background to red
                        ImGui::SetCursorPos(positions.at(i).first);
                        if (ImGui::Button(to_string(i + 1).c_str(), ImVec2(positions.at(i).second, positions.at(i).second))){
                            if (position == i){
                                position++;
                            } else {
                                position = 0;
                            }
                        }
                        ImGui::PopStyleColor();
                    }

                    if (position == 7){
                        stratagemState = false;
                        position = 0;
                        puzzle = 0;
                        reset = true;
                    }

                    ImGui::PopStyleColor();
                    ImGui::End();
                    }
                    // TODO - THIRD PUZZLE - AMONG US WIRES                   
                else if (puzzle == 3){
                   ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 1.0f, 1.0f, 0.75f)); // Set window background to red
                    ImGui::SetNextWindowSize(ImVec2(windowWidth * 3 / 4.0f, windowHeight * 3 / 4.0f));
                    ImGui::SetNextWindowPos(ImVec2(windowWidth *(1/2.0f - 3/8.0f) , windowHeight * (1/2.0f - 3/8.0f)));
                    ImGui::Begin("Wires from Among Us", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings  );   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
                    ImVec2 curSize = ImGui::GetWindowSize();
                    static bool reset = true;
                    static vector<pair<string, ImVec4>> leftPos;
                    static vector<pair<string, ImVec4>> rightPos;
                    static unordered_map<int, pair<bool, ImVec2>> done;
                    static unordered_map<int, bool> clicked = {{0, 0}, {1, 0}, {2, 0}, {3, 0}};
                    static unordered_map<int, bool> endClicked = {{0, 0}, {1, 0}, {2, 0}, {3, 0}};
                    static unordered_map<string, vector<int>> wires;
                    static unordered_set<int> connected; 

                    if (reset){
                        leftPos.clear();
                        leftPos.clear();
                        wires.clear();
                        connected.clear();
                        leftPos.push_back(make_pair("RED", ImVec4(252 / 255.0f, 94 / 255.0f, 3 / 255.0f, 255 / 255.0f)));
                        leftPos.push_back(make_pair("BLUE", ImVec4(23 / 255.0f, 3 / 255.0f, 252 / 255.0f, 255 / 255.0f)));
                        leftPos.push_back(make_pair("YELLOW", ImVec4(252 / 255.0f, 236 / 255.0f, 3 / 255.0f, 255 / 255.0f)));
                        leftPos.push_back(make_pair("PINK", ImVec4(255 / 255.0f, 181 / 255.0f, 243 / 255.0f, 255 / 255.0f)));
                        rightPos = leftPos;
                        auto rd = std::random_device {};
                        auto rng = default_random_engine{rd()};
                        shuffle(rightPos.begin(), rightPos.end(), rng);
                        shuffle(leftPos.begin(), leftPos.end(), rng);
                        reset = false;
                        for (int i = 0; i < 4; i++){
                            wires[leftPos[i].first].push_back(i);
                        }
                        for (int i = 0; i < 4; i++){
                            wires[rightPos[i].first].push_back(i);
                        }
                    }

                    if (connected.size() == 4){
                        reset = true;
                        stratagemState = false;
                        puzzle = 0;
                        for (auto x: clicked){
                            x.second = false;
                        }
                        for (auto x: endClicked){
                            x.second = false;
                        }
                    }

                    for (int i = 0; i < 4; i++){

                        glm::vec2 direction = {xPos, yPos};


                        ImGui::PushStyleColor(ImGuiCol_Button, rightPos.at(i).second); // Set window background to red
                        ImGui::SetCursorPos(ImVec2(curSize.x * .8, curSize.y * 0.1 + curSize.y * 0.2 * i));
                        ImGui::Button(("##END" + to_string(i)).c_str(), ImVec2(100, 100));

                        if (connected.find(i) != connected.end()) {
                            ImGui::GetWindowDrawList()->AddLine(ImVec2(windowWidth *(1/2.0f - 3/8.0f) + curSize.x * .1 + 50, windowHeight * (1/2.0f - 3/8.0f)  + (curSize.y * 0.1 + curSize.y * 0.2 * wires[rightPos[i].first].front() ) + 50), ImVec2(windowWidth *(1/2.0f - 3/8.0f) + curSize.x * .8+ 50, windowHeight * (1/2.0f - 3/8.0f)  + (curSize.y * 0.1 + curSize.y * 0.2 * i ) + 50), IM_COL32(255 * rightPos.at(i).second.x, 255 * rightPos.at(i).second.y, 255 * rightPos.at(i).second.z, 255 * rightPos.at(i).second.w), 3);
                        }

                        if (ImGui::IsItemHovered() && endClicked.at(i)) {
                            connected.insert(i);
                        } else if (!clicked.at(wires[rightPos[i].first].front())){
                            endClicked.at(i) = false; 
                        }

                        if (clicked.at(i) == true){
                        ImGui::GetWindowDrawList()->AddLine(ImVec2(windowWidth *(1/2.0f - 3/8.0f) + curSize.x * .1 + 50, windowHeight * (1/2.0f - 3/8.0f)  + (curSize.y * 0.1 + curSize.y * 0.2 * i ) + 50), ImVec2(direction.x, direction.y), IM_COL32(255 * leftPos.at(i).second.x, 255 * leftPos.at(i).second.y,255 * leftPos.at(i).second.z,255 * leftPos.at(i).second.w), 3);
                        }

                    }

                    for (int i = 0; i < 4; i++){
                        ImGui::PopStyleColor();
                        ImGui::PushStyleColor(ImGuiCol_Button, leftPos.at(i).second); // Set window background to red
                        ImGui::SetCursorPos(ImVec2(curSize.x * .1, curSize.y * 0.1 + curSize.y * 0.2 * i));
                        ImGui::Button(("##START" + to_string(i)).c_str(), ImVec2(100, 100));
                        if (ImGui::IsItemHovered() && F == GLFW_PRESS) {
                            clicked.at(i) = true; 
                            endClicked.at(wires[leftPos[i].first].back()) = true; 
                        } else if (F == GLFW_RELEASE){
                            clicked.at(i) = false;
                        }
                        ImGui::PopStyleColor();

                    }

                    ImGui::PopStyleColor();
                    ImGui::End();
                }
        } 
    }


        if (player.curWeapon){
            player.curWeapon->update(deltaTime);
        }
        if (player.curWeapon && !player.curWeapon->getAmmo()){
            player.curWeapon->setReloading(true);
        }
        normalize(velocity);
        velocity = glm::vec2{velocity.x * 1.1, velocity.y * 1.1};

        player.setVel(velocity);
        
        glm::vec2 Camera = player.getPos();

        static float mapScale = 3.5;//map1 = 10.502 //map2 = 9.647 //map3 = 3.5 //map4 = same //idk where to put it but map4 camera is shifter up by 250
        static float xChange = -6260;//map1 = 5359 //map2 = 19162 //map3 = -6260 //map4 = same
        static float yChange = -2384;//map1 = 1790.518 //map2 = 10 //map3 = -2385 //map4 = 391
          ImGui::GetBackgroundDrawList()->AddImage((void*) map, ImVec2((width3 * mapScale / -2  - Camera.x - xChange) * f + windowWidth / 2, (height3 * mapScale / -2 - Camera.y - yChange) * f + windowHeight / 2),
                                                ImVec2((width3 * mapScale / 2 - Camera.x - xChange) * f + windowWidth / 2, (height3 * mapScale / 2 - Camera.y - yChange) * f + windowHeight / 2), ImVec2(0,0) , ImVec2(1, 1) , IM_COL32(255, 255, 255, 255));
        for (const shared_ptr<PhysicsBody>& x: world.staticBodies){
            ImGui::GetBackgroundDrawList()->AddImage((void*) image_texture1, ImVec2((x->start().x - Camera.x) * f + windowWidth / 2, (x->start().y - Camera.y) * f + windowHeight / 2),
                                                ImVec2((x->end().x - Camera.x) * f + windowWidth / 2, (x->end().y - Camera.y) *f + windowHeight / 2), ImVec2(0,0) , ImVec2(1, 1) , IM_COL32(255, 255, 255, 255));
        }


        glm::vec2 direction = {xPos - windowWidth / 2, yPos - windowHeight /2};
        normalize(direction);

        // SPAWN PROJECTILES
        static int numby = 0;
        {
            static float time = 0.0;
            if (player.curWeapon){
            time = min((float) player.curWeapon->getFireRate(), time);}
        if (F == GLFW_PRESS && player.getIndex() > -1 && time >= player.curWeapon->getFireRate()  && player.curWeapon->getAmmo() && !player.curWeapon->reloading){
            int bullets = 1;
            if (player.curWeapon->getName() == "Shotgun") bullets = player.curWeapon->getMaxAmmo();
            for (int i = 0; i < bullets; i++){
            glm::vec2 initial = player.getPos();
            float angle = 3.14 / 50 - 3.14 / 25 * (rand() % 100 + 1) / 100;
            glm::vec2 rotatedDir = {direction.x * cos(angle) - direction.y * sin(angle), direction.x * sin(angle) + direction.y * cos(angle)};
            glm::vec2 dir = {rotatedDir.x * 100.0 , rotatedDir.y * 100.0};
	        if(dir.x && dir.y)
            normalize(dir);
            dir = glm::vec2{dir.x * 10, dir.y * 10};

            shared_ptr<Actor> proj(new Projectile(to_string(numby), shared_ptr<PhysicsBody>(new Rect(player.getPos(), glm::vec2{10, 10})), dir, player.curWeapon->getDamage()));
        
            world.addActor(proj);
            player.curWeapon->decAmmo();
            numby++;
            time = 0;
            }
        }
            time += deltaTime;
        }
        scale = .05;

        for (auto& actor: world.actors){
            if (!actor.second->dead){
            ImGui::GetBackgroundDrawList()->AddImage((void*) image_texture1, ImVec2((actor.second->getBody().start().x - Camera.x) * f + windowWidth / 2, (actor.second->getBody().start().y - Camera.y) * f + windowHeight / 2),
                                                ImVec2((actor.second->getBody().end().x - Camera.x) * f + windowWidth / 2, (actor.second->getBody().end().y - Camera.y) *f + windowHeight / 2), ImVec2(0,0) , ImVec2(1, 1) , IM_COL32(255, 255, 255, 255));
            actor.second->physics(deltaTime, worldptr);                      
            }

        }
        scale = .1;

        ImGui::PushFont(font2);
        if (player.curWeapon && player.getIndex() > -1)
            ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(windowWidth - 110 - 10 - (110 + 10) * (player.weaponNames.size() - 1 - player.getIndex()), windowHeight - 110 - 25) , ImVec2(windowWidth - 10 - (110 + 10) * (player.weaponNames.size() - 1 - player.getIndex()), windowHeight - 25), IM_COL32(255, 255, 255, 100));
        for (int i = 0; i < player.weaponNames.size(); i++){
            ImGui::GetBackgroundDrawList()->AddRect(ImVec2(windowWidth - 110 - 10 - (110 + 10) * i, windowHeight - 110 - 25) , ImVec2(windowWidth - 10 - (110 + 10) * i, windowHeight - 25), IM_COL32(255, 255, 255, 255));
            
            try{
            ImGui::GetBackgroundDrawList()->AddImage((void*) ims.at(ItemRegistry::getWeapon(player.getWeapInd(player.weaponNames.size() - 1- i)).getTextureId())->texture, ImVec2(windowWidth - 110 - 10 - (110 + 10) * i, windowHeight - 110 - 25) , 
                                                ImVec2(windowWidth - 10 - (110 + 10) * i, windowHeight - 25), ImVec2(0,0) , ImVec2(1, 1) , IM_COL32(255, 255, 255, 128));
            } catch(...){}
        }

        if (player.curWeapon && player.curWeapon->getType() == WeaponType::GUN){
            string ammoText = (player.curWeapon->getName() + ": " + to_string(player.curWeapon->getAmmo()) + " / " + to_string(player.curWeapon->getMaxAmmo()));
            ImGui::GetForegroundDrawList()->AddText(ImVec2(windowWidth - ImGui::CalcTextSize(ammoText.c_str()).x - 10, windowHeight - ImGui::CalcTextSize(ammoText.c_str()).y - 140), IM_COL32_WHITE, (ammoText).c_str());
        } else if (player.curWeapon && player.curWeapon->getType() == WeaponType::MELEE){
            string ammoText = (player.curWeapon->getName());
            ImGui::GetForegroundDrawList()->AddText(ImVec2(windowWidth - ImGui::CalcTextSize(ammoText.c_str()).x - 10, windowHeight - ImGui::CalcTextSize(ammoText.c_str()).y - 140), IM_COL32_WHITE, (ammoText).c_str());
        }

        if (player.curWeapon && player.curWeapon->time < player.curWeapon->getReloadSpeed()){
            ImGui::GetForegroundDrawList()->AddLine(ImVec2(0, windowHeight - 25 / 2), ImVec2(windowWidth * player.curWeapon->time / player.curWeapon->getReloadSpeed(), windowHeight - 25 / 2) , IM_COL32(200, 200, 200, 100), 25);
        }
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(0, 25 / 2), ImVec2(windowWidth * player.getHp() / 200.0f * .8, 25 / 2) , IM_COL32(194, 47, 47, 255), 25);
        player.render(ImGui::GetBackgroundDrawList(), Camera, f, windowWidth, windowHeight);
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        // if (show_demo_window)
        //    ImGui::ShowDemoWindow(&show_demo_window);

        static float angle = 0.0f;
        angle += deltaTime / 1000  * 1.0f;
        ImDrawList* list = ImGui::GetBackgroundDrawList();
        ImGui::PopFont();

        int flip = 1;
        glm::vec2 rotatedDir;
        rotatedDir = direction;

        float angul = atan(rotatedDir.y/rotatedDir.x);
        float flippy = 1;
         angul += 3.14/2;
        if (player.curWeapon && player.getIndex() > -1)
            ImageRotated((void*) ims.at(player.curWeapon->getTextureId())->texture, ImVec2((width * scale + player.getPos().x - width * scale - Camera.x + rotatedDir.x * 80) * f + windowWidth / 2, (height * scale + player.getPos().y - height * scale - Camera.y + rotatedDir.y * 100) * f + windowHeight / 2), ImVec2(width * flippy * .125 * f, f * flip * height * .125), angul, list); 

        ImageRotated((void*) image_texture1, ImVec2((1500 - Camera.x) * f + windowWidth / 2, (900 - Camera.y) * f + windowHeight / 2), ImVec2(200 * f,f * 200.0f), angle, list); 

        ImGui::PushFont(font1);
        //cout << dick.x << " " << dick.y << endl;
        //cout << player.pos().x << " " << player.pos().y << endl;
 
 //       {
 //           static int counter = 0;
 //           static char weaponN[50] = "";

 //           ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

 //           ImGui::Text("This is some useful text."); 
 //           ImGui::Text("x:%f, y:%f", dick.x, dick.y);       // Display some text (you can use a format strings too)
 //           ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
 //           ImGui::Checkbox("Another Window", &show_another_window);

 //           //ImGui::SliderFloat("float", &f, 0.25f, 5.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
 //           ImGui::SliderFloat("scale", &mapScale, 0.0f, 100.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
 //           ImGui::SliderFloat("xch", &xChange, 0.0f, 10 * width3);            // Edit 1 float using a slider from 0.0f to 1.0f
 //           ImGui::SliderFloat("ych", &yChange, 0.0f, 10 * height3);            // Edit 1 float using a slider from 0.0f to 1.0f
 //           ImGui::SliderFloat("xSize", &xSize, 0.0f, 5000.0f); 
 //           ImGui::SliderFloat("ySize", &ySize, 0.0f, 5000.0f); 
 //           ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

 //           ImGui::InputText("WeaponName", weaponN, IM_ARRAYSIZE(inputText));

 //           if (ImGui::Button("Add Weapon"))  {                          // Buttons return true when clicked (most widgets return true when edited/activated)
 //               try{
 //                   player.addWeapon(string(weaponN));
 //               } catch(...){
 //               }
 //           }
 //           if (ImGui::Button("Remove Weapon")) {                           // Buttons return true when clicked (most widgets return true when edited/activated)
 //               try{
 //                   if (player.weaponNames.count(string(weaponN)))
 //                       player.removeWeapon(string(weaponN));
 //               } catch (...){
 //               }
 //           }

 //           if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
 //               {
 //      // usleep(1);
 //      //         world.muntex.lock();
 //      //         world.staticBodies.clear();
 //      //         world.muntex.unlock();
 //      //         thing();
 //               }
 //           ImGui::SameLine();
 //           ImGui::Text("counter = %d", counter);

 //           ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
 //           ImGui::End();
 //       }
      //  usleep(1);
      //  world.muntex.lock();
      //  world.staticBodies.clear();
      //          thing();
      //          world.muntex.unlock();

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
                    string inp(inputText);

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

        // spawn enemies uh oh the misery
        {
            static float timer = 0;
            timer+= deltaTime;
            static float spawnRate = 10000;
        if (timer >= spawnRate){
          for (auto& player: world.players){
            for (int j = 0; j < 5; j++){
                 static int i = 0;
                 glm::vec2 direction = {1, 1};
                 direction = glm::normalize(direction);
                 float angle = 3.14 * 2 * (rand() % 100 / 100.0f);
                 glm::vec2 rotatedDir = {direction.x * cos(angle) - direction.y * sin(angle), direction.x * sin(angle) + direction.y * cos(angle)};
                 glm::vec2 ray = {0,0}; 
                 world.raycast(rotatedDir, ray, player->getPos(), world.staticBodies, 5000);
                M1ChipEnemy* enmey = new M1ChipEnemy("enemigo" + std::to_string(i), std::shared_ptr<PhysicsBody>(new Rect(glm::vec2(player->getPos().x + ray.x, player->getPos().y + ray.y), glm::vec2(width * .1, height * .1))));
                 world.addActor(std::shared_ptr<Actor>(enmey));
                 world.addEnemy(std::shared_ptr<Enemy>(enmey));
                 i++;
            }
            }
          timer = 0;
          }
        }

        if (F == GLFW_PRESS){
            //cout << "rectangle," << dick.x << ", " << dick.y << ", " << xSize << ", " << ySize << endl;
        }
        // TODO: uncomment when world object is available
        // for (auto& actor : world.actors) {
        //     actor.update(deltaTime);
        // }
        //cout << enemy.getPos().x << " " << enemy.getPos().y << endl;
        ImGui::GetIO().FontGlobalScale = f;
        // TODO - FIX THIS
        if (player.promptGiver && player.prompt) {
            ImGui::PushFont(font2);
            if (E == GLFW_PRESS){
                player.promptGiver->speaking = true;
            }
            string prompt = "Press [E] to interact";
            ImGui::GetForegroundDrawList()->AddText(ImVec2(windowWidth / 2 - (ImGui::CalcTextSize(prompt.c_str()).x) / 2, windowHeight * 2 / 3 + ImGui::CalcTextSize(prompt.c_str()).y / 2), IM_COL32_BLACK, (prompt).c_str());
            ImGui::PopFont();
        } 

  //      if (player.promptGiver && player.promptGiver->speaking){
  //          ImGui::PushFont(font2);
  //          try{
  //              string line = "";
  //              // gets random line from dialogue
  //              if (!npc.planned.size()) line = player.promptGiver->dialogue.at((int)(((rand() % 100 + 1) / 100.0f) * (player.promptGiver->dialogue.size() - 1)));
  //              else line = npc.planned.front();
  //              ImGui::SameLine((windowWidth / 2) - (ImGui::CalcTextSize(line.c_str()).x / 2));
  //              ImGui::GetForegroundDrawList()->AddText(ImVec2((player.promptGiver->getPos().x - (ImGui::CalcTextSize(line.c_str()).x) / 2/ f- Camera.x) * f + windowWidth / 2, (player.promptGiver->getPos().y - player.promptGiver->size().y - 10 - Camera.y)*f+ windowHeight / 2 ), IM_COL32_BLACK, (line).c_str());
  //          } catch(...){}
  //          ImGui::PopFont();
  //      }
        player.prompt = false;
        player.promptGiver = nullptr;


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
        usleep(10);
        if (brugs){
    world.stop = true;
    t1.join();}












      }break;
    }

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

  if (client != nullptr){
    client->read = false;
    client->receiveThread.join();
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
