#define STB_IMAGE_IMPLEMENTATION

#include "include/imgui/imgui.h"
//#include "include/imgui/imgui.cpp"
#include "include/imgui/imgui_impl_glfw.h"
//#include "include/imgui/imgui_impl_glfw.cpp"
#include "include/imgui/imgui_impl_opengl3.h"
//#include "include/imgui/imgui_impl_opengl3.cpp"
#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h>            // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <include/tri_s.h>
#include <include/shader_s.h>
#include <include/camera.h>
#include <include/mesh.h>
#include <include/model.h>
#include <include/stb_image.h>
#include<include/particle_generator.cpp>

//#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
//#pragma comment(lib, "legacy_stdio_definitions")
//#endif

//#include <map>
#include <list>
#include <vector>

#include <iostream>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN64_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif
static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);
glm::vec3 bezier_tri(float t);



// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
//Camera fix_camera(glm::vec3(0.0f, 0.5f, 2.0f));
Camera fix_camera(glm::vec3(0.0f, 0.1f, 2.0f));
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

Tri tri_c(1.0);

bool fix = false;
bool press = false;
bool bezier = false;
bool bezier_mode = false;
bool track_cut = false;
bool press_f = false;
bool press_left = false;
int cut_num = 0;
bool show_menu = true;
bool press_tab = false;

glm::vec3 KP(-0.5f, 1.1f, 0.0f);//knive position
glm::vec3 PP(-0.5f, 0.6f, 0.5f);
int bezier_node = 0;
GLfloat node[4][3] = { -1.0,0.0,0.5 , { -0.5,0.5,0.0 },
{ 0.0,0.5,0.0 }, { 0.5,0.5,0.0 }};

#define BNUM 800
glm::vec3 curve_b[BNUM];
const float SENS = 2.0;
# define NUM 400


ParticleGenerator* Particles;

glm::mat4 projection;
glm::mat4 view;
glm::mat4 old_projection;
glm::mat4 old_view;
bool collision=false;

int main()
{
	
	// glfw: initialize and configure
	// ------------------------------
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;//初始化窗口
	//--------------------配置窗口
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	const char* glsl_version = "#version 130";

	// glfw window creation创建窗口对象
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "this is the name of the window (seems doesn't support chinese)", NULL, NULL);//后两个参数暂时忽略
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);


	// glad: load all OpenGL function pointers//初始化glad
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
	bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
	bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
	bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
	bool err = gladLoadGL(glfwGetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
	bool err = false;
	glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
	bool err = false;
	glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
	bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
///////////
	// Load Fonts
		// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
		// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
		// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
		// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
		// - Read 'docs/FONTS.md' for more instructions and details.
		// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
		//io.Fonts->AddFontDefault();
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
		//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
		//IM_ASSERT(font != NULL);

		// Our state
		bool show_demo_window = true;
	bool show_another_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
//////////


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	Shader ParticleShader("particle.vs", "particle.fs");
	Particles = new ParticleGenerator(ParticleShader, 800);
	//Shader ourShader("4.1.texture.vs", "4.1.texture.fs");
	
	Shader lightingShader("1.colors.vs", "1.colors.fs");
	Shader lightCubeShader("1.light_cube.vs", "1.light_cube.fs");
	Shader TriShader("1.tri.vs", "1.tri.fs");
	Shader shader("6.2.cubemaps.vs", "6.2.cubemaps.fs");
	Shader skyboxShader("6.2.skybox.vs", "6.2.skybox.fs");

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	GLfloat ctrlpoints[4][3] = {
	{-4.0,0.0,0.0},{-2.0,3.0,0.0},
	{0.0,4.0,0.0},{2.0,3.0,0.0}
	};
	GLfloat ctrlcolor[8][4] = {
	{1.0,0.0,0.0,1.0},{0.0,1.0,0.0,1.0},
	{1.0,0.0,0.0,1.0},{0.0,1.0,0.0,1.0},
	{0.0,0.0,1.0,1.0},{0.0,0.0,0.0,1.0},
	{1.0,0.0,0.0,1.0},{0.0,0.0,0.0,1.0} };

	

	glm::vec3 vertex[] =
	{
		glm::vec3(-0.25f,  -0.25f,  0.75f),
		glm::vec3(0.25f,  -0.25f,   0.75f),
		glm::vec3(0.25f,  0.25f,   0.75f),
		glm::vec3(-0.25f,  0.25f,   0.75f),
		glm::vec3(-0.25f,  -0.25f,  0.5f),
		glm::vec3(0.25f,  -0.25f,  0.5f),
		glm::vec3(0.25f,  0.25f,  0.5f),
		glm::vec3(-0.25f,  0.25f,  0.5f),

	};

	float vertices[] = {
		// positions							 // normals           // texture coords
		 vertex[3].x,  vertex[3].y, vertex[3].z,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 vertex[0].x,  vertex[0].y, vertex[0].z,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 vertex[1].x,  vertex[1].y, vertex[1].z,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 vertex[1].x,  vertex[1].y, vertex[1].z,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 vertex[2].x,  vertex[2].y, vertex[2].z,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		 vertex[3].x,  vertex[3].y, vertex[3].z,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		 vertex[7].x,  vertex[7].y, vertex[7].z,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 vertex[4].x,  vertex[4].y, vertex[4].z,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 vertex[5].x,  vertex[5].y, vertex[5].z,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 vertex[5].x,  vertex[5].y, vertex[5].z,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 vertex[6].x,  vertex[6].y, vertex[6].z,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		 vertex[7].x,  vertex[7].y, vertex[7].z,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		 vertex[2].x,  vertex[2].y, vertex[2].z, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 vertex[6].x,  vertex[6].y, vertex[6].z, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 vertex[7].x,  vertex[7].y, vertex[7].z, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 vertex[7].x,  vertex[7].y, vertex[7].z, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 vertex[3].x,  vertex[3].y, vertex[3].z, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 vertex[2].x,  vertex[2].y, vertex[2].z, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

		 vertex[1].x,  vertex[1].y, vertex[1].z,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 vertex[5].x,  vertex[5].y, vertex[5].z,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 vertex[4].x,  vertex[4].y, vertex[4].z,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 vertex[4].x,  vertex[4].y, vertex[4].z,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 vertex[0].x,  vertex[0].y, vertex[0].z,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 vertex[1].x,  vertex[1].y, vertex[1].z,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

		 vertex[7].x,  vertex[7].y, vertex[7].z,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 vertex[4].x,  vertex[4].y, vertex[4].z,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 vertex[0].x,  vertex[0].y, vertex[0].z,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 vertex[0].x,  vertex[0].y, vertex[0].z,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 vertex[3].x,  vertex[3].y, vertex[3].z,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		 vertex[7].x,  vertex[7].y, vertex[7].z,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		 vertex[6].x,  vertex[6].y, vertex[6].z,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 vertex[5].x,  vertex[5].y, vertex[5].z,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 vertex[1].x,  vertex[1].y, vertex[1].z,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 vertex[1].x,  vertex[1].y, vertex[1].z,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 vertex[2].x,  vertex[2].y, vertex[2].z,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		 vertex[6].x,  vertex[6].y, vertex[6].z,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	//刀具的数据
	glm::vec3 knife[] =
	{
		/*glm::vec3(0.0f,  -0.5f,  0.1f),
		glm::vec3(0.05f,  -0.2f,  0.1f),
		glm::vec3(0.05f,  0.5f,  0.1f),
		glm::vec3(0.0f,  0.5f,  0.1f),
		glm::vec3(0.0f,  -0.5f,  -0.1f),
		glm::vec3(0.05f,  -0.2f,  -0.1f),
		glm::vec3(0.05f,  0.5f,  -0.1f),
		glm::vec3(0.0f,  0.5f,  -0.1f),*/
		glm::vec3(0.0f,  -0.5f,  0.1f),
		glm::vec3(0.05f,  -0.2f,  0.1f),
		glm::vec3(0.05f,  0.5f,  0.1f),
		glm::vec3(0.0f,  0.5f,  0.1f),
		glm::vec3(0.0f,  -0.5f,  -0.1f),
		glm::vec3(0.05f,  -0.2f,  -0.1f),
		glm::vec3(0.05f,  0.5f,  -0.1f),
		glm::vec3(0.0f,  0.5f,  -0.1f),

	};
	float knives[] = {
		// positions							 // normals           // texture coords
		 knife[3].x,  knife[3].y, knife[3].z,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 knife[0].x,  knife[0].y, knife[0].z,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 knife[1].x,  knife[1].y, knife[1].z,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 knife[1].x,  knife[1].y, knife[1].z,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 knife[2].x,  knife[2].y, knife[2].z,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		 knife[3].x,  knife[3].y, knife[3].z,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		 knife[7].x,  knife[7].y, knife[7].z,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 knife[4].x,  knife[4].y, knife[4].z,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 knife[5].x,  knife[5].y, knife[5].z,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 knife[5].x,  knife[5].y, knife[5].z,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 knife[6].x,  knife[6].y, knife[6].z,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		 knife[7].x,  knife[7].y, knife[7].z,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		 knife[2].x,  knife[2].y, knife[2].z, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 knife[6].x,  knife[6].y, knife[6].z, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 knife[7].x,  knife[7].y, knife[7].z, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 knife[7].x,  knife[7].y, knife[7].z, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 knife[3].x,  knife[3].y, knife[3].z, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 knife[2].x,  knife[2].y, knife[2].z, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 knife[1].x,  knife[1].y, knife[1].z,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 knife[5].x,  knife[5].y, knife[5].z,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 knife[4].x,  knife[4].y, knife[4].z,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 knife[4].x,  knife[4].y, knife[4].z,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 knife[0].x,  knife[0].y, knife[0].z,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 knife[1].x,  knife[1].y, knife[1].z,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 knife[7].x,  knife[7].y, knife[7].z,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 knife[4].x,  knife[4].y, knife[4].z,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 knife[0].x,  knife[0].y, knife[0].z,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 knife[0].x,  knife[0].y, knife[0].z,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 knife[3].x,  knife[3].y, knife[3].z,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		 knife[7].x,  knife[7].y, knife[7].z,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		 knife[6].x,  knife[6].y, knife[6].z,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 knife[5].x,  knife[5].y, knife[5].z,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 knife[1].x,  knife[1].y, knife[1].z,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 knife[1].x,  knife[1].y, knife[1].z,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 knife[2].x,  knife[2].y, knife[2].z,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		 knife[6].x,  knife[6].y, knife[6].z,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::vec3 pointLightPositions[] = {
	   glm::vec3(1.7f,  0.2f,  2.0f),
	   glm::vec3(-1.3f, -3.0f, -3.0f),
	   glm::vec3(-4.0f,  2.0f, -6.0f),
	   glm::vec3(-2.0f,  -1.0f, 0.0f)
	};

	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	unsigned int KnifeVAO,KnifeVBO;
	glGenVertexArrays(1, &KnifeVAO);
	glGenBuffers(1, &KnifeVBO);

	glBindVertexArray(KnifeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, KnifeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(knives), knives, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	

	unsigned int NewVAO, NewVBO;
	glGenVertexArrays(1, &NewVAO);
	glGenBuffers(1, &NewVBO);

	glBindVertexArray(NewVAO);
	glBindBuffer(GL_ARRAY_BUFFER, NewVBO);
	/*glBufferData(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_STATIC_DRAW);*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(tri_c.tri), tri_c.tri, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);



	unsigned int diffuseMap = loadTexture("include/metal1.jpg");
	unsigned int specularMap = loadTexture("include/metal1.jpg");
	//glMatrixMode(GL_PROJECTION);
	////glOrtho(-SCR_WIDTH/2, SCR_WIDTH/2,-SCR_HEIGHT/2, SCR_HEIGHT/2, - 1,1);
	//glOrtho(-SCR_WIDTH / 2, SCR_WIDTH / 2,0, SCR_HEIGHT / 2, -10, 10);
	//画图的事前准备
	//————————-
	
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);

	//贝塞尔
	//glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);//这里的3是因为一个点有三个坐标
	//glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &node[0][0]);
	//glMap1f(GL_MAP1_COLOR_4, 0.0, 1.0, 4, 4, &ctrlcolor[0][0]);

	/*激活一维顶点求值器*//*
	glEnable(GL_MAP1_VERTEX_3);*/
	//glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &node[0][0]);
	//glShadeModel(GL_FLAT);

	//glDisable(GL_MAP1_VERTEX_3);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, diffuseMap);
	//// bind specular map
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, specularMap);
	
	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// load textures
	// -------------
	vector<std::string> faces
	{
		"include/skybox/right.jpg",
		"include/skybox/left.jpg",
		"include/skybox/top.jpg",
		"include/skybox/bottom.jpg",
		"include/skybox/front.jpg",
		"include/skybox/back.jpg",
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	// shader configuration
	// --------------------
	shader.use();
	shader.setInt("skybox", 0);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);


	while (!glfwWindowShouldClose(window))
	{
		// Poll and handle events (inputs, window resize, etc.)
	   // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
	   // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
	   // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
	   // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		//if (show_demo_window)
		//	//ImGui::ShowDemoWindow(&show_demo_window);
		//// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		//{
		//	static float f = 0.0f;
		//	static int counter = 0;
		//	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
		//	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		//	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		//	ImGui::Checkbox("Another Window", &show_another_window);
		//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		//	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
		//	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		//		counter++;
		//	ImGui::SameLine();
		//	ImGui::Text("counter = %d", counter);
		//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//	ImGui::End();
		//}
		show_another_window = show_menu;
		if (show_another_window)
		{
			ImGui::Begin("Floating Menu", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Welcome to use this imgui menu");

			if (ImGui::Button("fix view"))
				fix = !fix;
			if (fix)
			{
				if (ImGui::Button("free model"))
					press = !press;
				if (ImGui::Button("Bezier model"))
					bezier_mode = !bezier_mode;
				
				if (bezier_node == 4)
				{
					if (ImGui::Button("track_cut"))
					{
						track_cut = true;
					}
				}
				if (ImGui::Button("Reset"))
				{
					Tri tri_n(1.0);
					tri_c = tri_n;

					//fix = false;
					press = false;
					bezier = false;
					bezier_mode = false;
					track_cut = false;
					press_f = false;
					press_left = false;
					cut_num = 0;
					bezier_node = 0;
					KP = glm::vec3(-0.5f, 1.1f, 0.0f);
					PP = glm::vec3(-0.5f, 0.6f, 0.5f);
				}		
			}
			ImGui::End();
		}
////


		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// input
		// -----
		processInput(window);

		

		int i;
		glClearColor(0.75f, 0.52f, 0.3f, 1.0f);
		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
		//glFlush();

		

		//track cut
		if (track_cut&& bezier_node==4)
		{
			float x = curve_b[cut_num].x;
			float y = curve_b[cut_num].y;
			KP.x = x;
			KP.y = y+0.5;
			cut_num++;
			if (cut_num >= BNUM)
			{
				track_cut = false;
				KP = glm::vec3(-0.5f, 1.1f, 0.0f);
			}
			if (KP.x >= -0.5f && KP.x <= 0.5f)
			{
				if (tri_c.get_height(KP.x) > (KP.y - 0.5f))
				{
					//cout << "colli" << endl;
					//碰撞，需要修改
					if ((KP.y - 0.5f) <= 0.0)
					{
						KP.y = 0.5f;
					}
					tri_c.change_at_pos(KP.x, KP.y - 0.5f);
					collision = true;
				}
			}
		}
		//track_cut = false;
		Particles->Update(0.03, KP - glm::vec3(0.0, 0.48, 0.0), glm::vec3(0.0, -1.0, 0.0), 3, collision, glm::vec3(0.0, 0.0, 0.0));
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		Particles->shader.use();
		Particles->shader.setMat4("projection", projection);
		Particles->shader.setMat4("view", view);
		Particles->Draw();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		collision = false;


		lightingShader.use();
		if (fix)
		{
			lightingShader.setVec3("viewPos", fix_camera.Position);
		}
		else
		{
			lightingShader.setVec3("viewPos", camera.Position);
		}
		lightingShader.setFloat("material.shininess", 32.0f);

		glm::vec3 pointLightColors[] = {
			glm::vec3(1.0f, 1.0f, 240.0/255.0),
			/*glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0, 0.0),
			glm::vec3(0.2f, 0.2f, 1.0f)*/
			glm::vec3(1.0f, 1.0f, 240.0 / 255.0),
			glm::vec3(1.0f, 1.0f, 240.0 / 255.0),
			glm::vec3(1.0f, 1.0f, 240.0 / 255.0),
		};

		
		glm::vec3 ambientLight = glm::vec3(1.0f, 165.0 / 255.0, 0.0f);
		
		lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		lightingShader.setVec3("dirLight.ambient", ambientLight.x*0.1f, ambientLight.y*0.1f, ambientLight.z*0.1f); //白光
		lightingShader.setVec3("dirLight.diffuse", ambientLight.x * 0.4f, ambientLight.y* 0.4f, ambientLight.z * 0.4f);
		lightingShader.setVec3("dirLight.specular", ambientLight.x * 0.5f, ambientLight.y * 0.5f, ambientLight.z * 0.5f);
		// point light 1
		lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		lightingShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1);
		lightingShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
		lightingShader.setVec3("pointLights[0].specular", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
		lightingShader.setFloat("pointLights[0].constant", 1.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.1);
		lightingShader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		lightingShader.setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1, pointLightColors[1].z * 0.1);
		lightingShader.setVec3("pointLights[1].diffuse", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
		lightingShader.setVec3("pointLights[1].specular", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
		lightingShader.setFloat("pointLights[1].constant", 1.0f);
		lightingShader.setFloat("pointLights[1].linear", 0.1);
		lightingShader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		lightingShader.setVec3("pointLights[2].ambient", pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1, pointLightColors[2].z * 0.1);
		lightingShader.setVec3("pointLights[2].diffuse", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
		lightingShader.setVec3("pointLights[2].specular", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
		lightingShader.setFloat("pointLights[2].constant", 1.0f);
		lightingShader.setFloat("pointLights[2].linear", 0.1);
		lightingShader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		lightingShader.setVec3("pointLights[3].ambient", pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1, pointLightColors[3].z * 0.1);
		lightingShader.setVec3("pointLights[3].diffuse", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
		lightingShader.setVec3("pointLights[3].specular", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
		lightingShader.setFloat("pointLights[3].constant", 1.0f);
		lightingShader.setFloat("pointLights[3].linear", 0.1);
		lightingShader.setFloat("pointLights[3].quadratic", 0.032);

		// view/projection transformations
		
		if (fix)
		{
			projection = glm::perspective(glm::radians(fix_camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			//实际上fix之后这里需要变化，变成固定的正视图
			//projection = glm::mat4(1.0f);
			//projection =glm::ortho(0.0f, 640.0f, 480.0f, 0.0f);
			//projection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 200.0f);
			/*projection = glm::ortho(-(float)SCR_WIDTH / 2, (float)SCR_WIDTH / 2, -(float)SCR_HEIGHT / 2, (float)SCR_HEIGHT / 2, 50.0f, 100.0f);
			projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.0f, 100.0f);*/
			projection = glm::ortho(-(float)SCR_WIDTH / 500.0f, (float)SCR_WIDTH / 500.0f, -(float)SCR_HEIGHT / 500.0f, (float)SCR_HEIGHT / 500.0f, 0.1f, 100.0f);
			view = fix_camera.GetViewMatrix();
			
			//lightingShader.setFloat("fixxx", 1.0f);
			//projection = glm::mat4(1.0f);
			
			//glMatrixMode(GL_PROJECTION); //设置当前操作的是投影矩阵队列
			//glLoadIdentity(); //将队列最上层的一个投影矩阵清空成单位矩阵
			//glOrtho(0, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0, -100, 100);
		}
		else
		{
			//lightingShader.setFloat("fixxx", 0.0f);
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			
			//projection = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, 0.1f, 100.0f);
			view = camera.GetViewMatrix();
		}
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);


		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("model", model);

		glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &node[0][0]);
		//// render the cube
		//glBindVertexArray(cubeVAO);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		 // render containers
		float Gtime = (float)glfwGetTime();

		//render knife with
		glBindVertexArray(KnifeVAO);
		model = glm::mat4(1.0f);
		/*model = glm::translate(model, glm::vec3(0.0f,2.0f,0.0f));*/
		model = glm::translate(model, KP);
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		
		
		// also draw the lamp object
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, lightPos);
		//model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		//lightCubeShader.setMat4("model", model);
		/*glBindVertexArray(lightCubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);*/
		glBindVertexArray(lightCubeVAO);
		for (unsigned int i = 0; i < 4; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			lightCubeShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));
		lightCubeShader.setMat4("model", model);
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPointSize(20.0);
		glBegin(GL_POINTS);
		//glColor3f(1.0, 1.0, 1.0);
		/*glVertex3f(0.6, 0.6, 0.6);*/
		glVertex3f(PP.x, PP.y, PP.z);
		glEnd();
		

		
		//画曲线
		if (bezier_node == 4)
		{
			/*激活一维顶点求值器*/
			//glEnable(GL_MAP1_VERTEX_3);

			glLineWidth(5.0);
			/*计算曲线上60个点的坐标，并顺序连接*/
			glBegin(GL_LINE_STRIP);
			for (i = 0;i < BNUM;i++)
			{//glEvalCoord1f((GLfloat)i / 60.0);
				glm::vec3 p = bezier_tri(i / (BNUM*1.0));
				glVertex3f(p.x,p.y,p.z);
				//然后可以在这里存下来后面用（先200个看看光不光滑）
				curve_b[i] = p;
			}
			glEnd();
			glFlush();
			//glDisable(GL_MAP1_VERTEX_3);
		}
		//ctrlpoints = node;


		glPointSize(6.0);
		glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_POINTS);
		for (i = 0;i < bezier_node;i++)
			glVertex3fv(&node[i][0]);
		glEnd();
		/*显示控制多边形*///把控制顶点用线连接起来。
		glLineWidth(2.0);
		glBegin(GL_LINE_STRIP);
		for (i = 0;i < bezier_node;i++)
			glVertex3fv(&node[i][0]);
		glEnd();
		glFlush();


		TriShader.use();
		if (fix)
		{
			TriShader.setVec3("viewPos", fix_camera.Position);
		}
		else
		{
			TriShader.setVec3("viewPos", camera.Position);
		}
		TriShader.setFloat("material.shininess", 32.0f);
		TriShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		TriShader.setVec3("dirLight.ambient", ambientLight.x * 0.05f, ambientLight.y * 0.05f, ambientLight.z * 0.05f); //白光
		TriShader.setVec3("dirLight.diffuse", ambientLight.x * 0.4f, ambientLight.y * 0.4f, ambientLight.z * 0.4f);
		TriShader.setVec3("dirLight.specular", ambientLight.x * 0.5f, ambientLight.y * 0.5f, ambientLight.z * 0.5f);
		// point light 1
		TriShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		TriShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1);
		TriShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
		TriShader.setVec3("pointLights[0].specular", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
		TriShader.setFloat("pointLights[0].constant", 1.0f);
		TriShader.setFloat("pointLights[0].linear", 0.1);
		TriShader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		TriShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		TriShader.setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1, pointLightColors[1].z * 0.1);
		TriShader.setVec3("pointLights[1].diffuse", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
		TriShader.setVec3("pointLights[1].specular", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
		TriShader.setFloat("pointLights[1].constant", 1.0f);
		TriShader.setFloat("pointLights[1].linear", 0.1);
		TriShader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		TriShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		TriShader.setVec3("pointLights[2].ambient", pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1, pointLightColors[2].z * 0.1);
		TriShader.setVec3("pointLights[2].diffuse", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
		TriShader.setVec3("pointLights[2].specular", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
		TriShader.setFloat("pointLights[2].constant", 1.0f);
		TriShader.setFloat("pointLights[2].linear", 0.1);
		TriShader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		TriShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		TriShader.setVec3("pointLights[3].ambient", pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1, pointLightColors[3].z * 0.1);
		TriShader.setVec3("pointLights[3].diffuse", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
		TriShader.setVec3("pointLights[3].specular", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
		TriShader.setFloat("pointLights[3].constant", 1.0f);
		TriShader.setFloat("pointLights[3].linear", 0.1);
		TriShader.setFloat("pointLights[3].quadratic", 0.032);

		TriShader.setMat4("projection", projection);
		TriShader.setMat4("view", view);

		model = glm::mat4(1.0f);
		TriShader.setMat4("model", model);
		//之后可能需要每次循环都来做一次这个——因为数据发生了变化——只在数据发生变化之后去bind
		glBindVertexArray(NewVAO);
		glBindBuffer(GL_ARRAY_BUFFER, NewVBO);
		/*glBufferData(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_STATIC_DRAW);*/
		glBufferData(GL_ARRAY_BUFFER, sizeof(tri_c.tri), tri_c.tri, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		

		glBindVertexArray(NewVAO);
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(2.0f, 2.0f, 0.0f));
		for (unsigned int i = 0; i < 360; i++)//360
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			//model = glm::translate(model, cubePositions[0]);
			float angle = 1.0f * i;
			//model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
			
			
			model = glm::rotate(model, Gtime * glm::radians(50.0f), glm::vec3(1.0f, 0.0f, 0.0f));//旋转
			TriShader.setMat4("model", model);

			//glDrawArrays(GL_TRIANGLES, 0, 36);
			/*glDrawArrays(GL_TRIANGLES, 0, 3*4 * NUM);*/
			glDrawArrays(GL_TRIANGLES, 0, 3 * 2 * NUM);//实际上这里的NUM应该统一
		}
		glBindVertexArray(cubeVAO);
		for (unsigned int i = 0; i < 3; i++)//360
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			//model = glm::translate(model, cubePositions[0]);
			float angle = 120.0f * i;

			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, Gtime * glm::radians(50.0f), glm::vec3(1.0f, 0.0f, 0.0f));//旋转
			model = glm::translate(model, glm::vec3(0.74f, 0.0f, -0.05f));
			//model = glm::scale(model, glm::vec3(0.5f));

			TriShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		old_projection = projection;
		old_view = view;
		if (fix)
		{
			view = glm::mat4(glm::mat3(fix_camera.GetViewMatrix()));
			projection = glm::perspective(glm::radians(fix_camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		}
		else {
			view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		}
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		view = old_view;
		projection = old_projection;


			// Rendering
			ImGui::Render();
			int display_w, display_h;
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
			//glClear(GL_COLOR_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		//glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteBuffers(1, &VBO);
	// glfw: terminate, clearing all previously allocated GLFW resources.释放资源
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		//reset
		Tri tri_n(1.0);
		tri_c = tri_n;

		//fix = false;
		press = false;
		bezier = false;
		bezier_mode = false;
		track_cut = false;
		press_f = false;
		press_left = false;
		cut_num = 0;
		bezier_node = 0;
		KP=glm::vec3(-0.5f, 1.1f, 0.0f);
		PP = glm::vec3(-0.5f, 0.6f, 0.5f);

	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		bezier = true;
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
	{
		if (bezier)
		{
			bezier_mode = !bezier_mode;
			bezier = false;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		press_f = true;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
	{
		if (press_f)
		{
			if (fix)
				press = !press;
			press_f = false;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		press_tab = true;
	}
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
	{
		if (press_tab)
		{
			show_menu = !show_menu;
			press_tab = false;
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);//设置视口的属性



}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	float oldx = KP.x;
	float xdis = xoffset / SCR_WIDTH * SENS;
	float ydis = yoffset / SCR_HEIGHT * SENS;
	if (fix && press&&!bezier_mode)//自由模式
	//if (fix && press)
	{
		KP.x += xdis;
		KP.y += ydis;
	}
	if (fix && bezier_mode)//贝塞尔模式
	//if (fix && !press)
	{
		PP.x += xdis;
		PP.y += ydis;
	}
	if (fix && press_left && bezier_node==4)//&&
	{
		//cout << "yeah" << endl;
		for (int i = 0;i < bezier_node;i++)
		{
			node[i][0] += xdis;
			node[i][1] += ydis;
		}
		for (int i = 0;i < BNUM;i++)
		{
			curve_b[i].x += xdis;
			curve_b[i].y += ydis;
		}
	}

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
	float tmpx;
	for (tmpx = min(oldx, KP.x);tmpx <= max(oldx, KP.x);tmpx += 0.001f)//——这里的来回都要检查，所以需要max（不能先入为主地判断大小）
	{
		if (tmpx >= -0.5f && tmpx <= 0.5f)
		{
			if (tri_c.get_height(tmpx) > (KP.y - 0.5f))
			{
				//cout << "colli" << endl;
				//碰撞，需要修改
				if ((KP.y - 0.5f) <= 0.0)
				{
					KP.y = 0.5f;
				}
				tri_c.change_at_pos(tmpx, KP.y - 0.5f);
				collision = true;

			}
		}
	}
	
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

//鼠标点击控制
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		//start_or_pause = !start_or_pause;
		//std::cout << "bool:" << start_or_pause << std::endl;
		/*state = (state + 1) % 3;
		std::cout << "state:" << state << std::endl;*/
		//if(fix)
		//	press = !press;
		if (fix && bezier_mode&&bezier_node<4)
		{ 

			node[bezier_node][0] =PP.x;
			node[bezier_node][1] = PP.y;
			node[bezier_node][2] = PP.z;
			//node[bezier_node] = { PP.x,PP.y,PP.z };
			bezier_node++;
		}
		if (fix)
		{
			press_left = true;
		}
		
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		cout << "start to cut" << endl;
		if (bezier_node == 4)
		{
			track_cut = true;
		}		
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		fix = !fix;//固定以及解除固定
		if (!fix)
		{
			cout << "unbind" << endl;
			Camera new_camera(glm::vec3(0.0f, 0.0f, 3.0f));
			camera = new_camera;
		}
		break;
	
	default:
		/*press = false;*/
		return;
	}
	
	if (action == GLFW_RELEASE) switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		//放开鼠标左键
		if (fix)
		{
			press_left = false;
		}
		break;
	default:
		/*press = false;*/
		return;
	}
	return;
}

glm::vec3 bezier_tri(float t)
{
	glm::vec3 point,p1,p2,p3,p0;
	float B1 = (1 - t) * (1 - t) * (1 - t);
	float B2 = 3*t * (1 - t) * (1 - t);
	float B3 = 3*t*t * (1 - t);
	float B4 = t * t * t;
	point.x = B1 * node[0][0] + B2 * node[1][0] + B3 * node[2][0] + B4 * node[3][0];
	point.y = B1 * node[0][1] + B2 * node[1][1] + B3 * node[2][1] + B4 * node[3][1];
	point.z = B1 * node[0][2] + B2 * node[1][2] + B3 * node[2][2] + B4 * node[3][2];
	return point;
}
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}