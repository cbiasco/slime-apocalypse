#include "main.hpp"

int main() {
    GLFWwindow * window = init_window_context();
    if (window == nullptr)
        return kill_app(EXIT_FAILURE);

	/* Management */
    Gtime::init_stack(1);
    Gtime::Timer * game_loop_clock = &(Gtime::stack[0]);

	// initialize the handle_scene
	scene = new Scene();

	/* Shaders */
	Shader * cube_shader = new Shader();
	scene->shaders[TEXTURE] = cube_shader;
    cube_shader->init_from_files(
        ((std::string)PROJECT_SOURCE_DIR + "/Shaders/cube.vert").c_str(),
        ((std::string)PROJECT_SOURCE_DIR + "/Shaders/cube.frag").c_str());
	Shader * flat_shader = new Shader();
	scene->shaders[FLAT] = flat_shader;
    flat_shader->init_from_files(
        ((std::string)PROJECT_SOURCE_DIR + "/Shaders/flat.vert").c_str(),
        ((std::string)PROJECT_SOURCE_DIR + "/Shaders/flat.frag").c_str());
	Shader * lamp_shader = new Shader();
	scene->shaders[LIGHT] = lamp_shader;
    lamp_shader->init_from_files(
        ((std::string)PROJECT_SOURCE_DIR + "/Shaders/lamp.vert").c_str(),
        ((std::string)PROJECT_SOURCE_DIR + "/Shaders/lamp.frag").c_str());
	Shader * test_shader = new Shader();
	scene->shaders[TEST] = test_shader;
	test_shader->init_from_files(
		((std::string)PROJECT_SOURCE_DIR + "/Shaders/test.vert").c_str(),
		((std::string)PROJECT_SOURCE_DIR + "/Shaders/test.frag").c_str());

	/* Load textures */
	/*GLenum tex_format;
	GLint tex_width, tex_height, tex_channels;
	std::string RESOURCES_DIR = (std::string)PROJECT_SOURCE_DIR + "/Resources/";
	GLubyte * image = stbi_load((RESOURCES_DIR + "container2.png").c_str(), &tex_width, &tex_height, &tex_channels, 0);
	if (!image)
		std::cerr << "Failed to load texture " + RESOURCES_DIR + "container2.png" << std::endl;
	switch (tex_channels) {
		case 1: tex_format = GL_ALPHA;     break;
		case 2: tex_format = GL_LUMINANCE; break;
		case 4: tex_format = GL_RGBA;      break;
        default:
        case 3: tex_format = GL_RGB;       break;
	}

	GLuint tex_container;
	glGenTextures(1, &tex_container);
	glBindTexture(GL_TEXTURE_2D, tex_container);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, tex_format, tex_width, tex_height, 0, tex_format, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	glBindTexture(GL_TEXTURE_2D, 0);*/

	scene->setupTestingObjects();

    /* Path Planning */
    ai::init(scene->enemyObjects, scene->staticObjects, scene->mazeInfo);
    scene->playerObject->ai.method = ai_comp::Planner::NONE;
    scene->enemyObjects.push_back(scene->playerObject);
    //game_loop_clock->pause();
    Gtime::add_now();
    GMP::replan(scene->enemyObjects);
    std::cout << "plan for all:" << Gtime::del_top() << std::endl;
    //game_loop_clock->play();

    /* Game Loop */
	scene->toggle_noclip();

	GLfloat fpsTimer = 0;
	int frameCounter = 0;
	std::string title = "Slime Apocalypse - FPS: ";
    game_loop_clock->frame();
	D(std::cout << std::endl << "Entering Game Loop..." << std::endl << std::endl);	
	while (!glfwWindowShouldClose(window)) {
		// Timer
        game_loop_clock->frame();
		frameCounter++;
		fpsTimer += game_loop_clock->delta();
		if (fpsTimer >= 1) {
			glfwSetWindowTitle(window, (title + std::to_string(frameCounter)).c_str());
			frameCounter = 0;
			fpsTimer -= 1;
		}

        // Callbacks 
        glfwPollEvents();
        handle_input(game_loop_clock, scene);

        //AI
        //game_loop_clock->pause();//catch jerks
        ai::update_agents(scene->staticObjects, scene->enemyObjects, scene->playerObject);
        //game_loop_clock->play();
        
        //Physics
        scene->simulate(game_loop_clock->delta());

		// Render 
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        scene->reset_proj_view();

		// TODO: currently must enable shader every frame to update view matrix; easily fixed
        /*
        scene->reset_proj_view();
		scene->enableTestShader(scene->proj, scene->camera->getView());
        */
		scene->enableTestShader(scene->proj, scene->view);
        
		scene->render();

		// Swap the Double Buffer
		glfwSwapBuffers(window);
    }
	
	D(std::cout << std::endl << "Exiting Game Loop..." << std::endl << std::endl);

	/* Exit */
    return kill_app(EXIT_SUCCESS);
}

GLFWwindow * init_window_context() {
    /* Load GLFW  */
    D(std::cout << "Initializing GLFW for OpenGL 3.3...");
    glfwInit();
    // Set required hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //For macs
    D(OK());

    /* Create Window Context */
    D(std::cout << "Creating GLFW Window Context...");
    //handle fullscreen
    GLFWmonitor* monitor = nullptr;
    if (G::WIN_FULLSCREEN)
        monitor = glfwGetPrimaryMonitor();
    //Make a window
    std::string title = "Slime Apocalypse - FPS: 0";
    GLFWwindow* window = glfwCreateWindow(G::WIN_WIDTH, G::WIN_HEIGHT, title.c_str(), monitor, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create OpenGL Context" << std::endl;
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    D(OK());

    /* Define callbacks */
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    D(OK());

    /* load all OpenGL functions */
    D(std::cout << "Loading OpenGL with glad...");
    // via glad using the glfw loader function
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        return nullptr;
    }
    D(std::cout << "OK ::: OpenGL " << glGetString(GL_VERSION) << std::endl);

    /* Handle Viewport */
    D(std::cout << "Creating viewport...");
    GLint width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    D(OK());

    glEnable(GL_DEPTH_TEST);
    return window;
}

void handle_input(Gtime::Timer * clock, Scene * handle_scene) {
	static int slices = scene->test->slices;
	static int stacks = scene->test->stacks;
	static float reverse = 1;
    handle_scene->camera->mouse_rotate_camera(UI::cursor_edx, UI::cursor_edy);
	UI::cursor_edx = 0;
	UI::cursor_edy = 0;
    handle_scene->camera->scroll_zoom_camera(UI::d_scroll);
	UI::d_scroll = 0;

	glm::vec3 v;
    if (UI::keys[GLFW_KEY_W] || UI::keys[GLFW_KEY_S] || UI::keys[GLFW_KEY_A] || UI::keys[GLFW_KEY_D] || UI::keys[GLFW_KEY_Q] || UI::keys[GLFW_KEY_E]
        || UI::keys[GLFW_KEY_UP] || UI::keys[GLFW_KEY_DOWN] || UI::keys[GLFW_KEY_LEFT] || UI::keys[GLFW_KEY_RIGHT]) {
        if (!handle_scene->is_noclip_on) {
            if (UI::keys[GLFW_KEY_W]) {
                v += handle_scene->camera->dir;
            }
            if (UI::keys[GLFW_KEY_A]) {
                v -= handle_scene->camera->right;
            }
            if (UI::keys[GLFW_KEY_S]) {
                v -= handle_scene->camera->dir;
            }
            if (UI::keys[GLFW_KEY_D]) {
                v += handle_scene->camera->right;
            }
            v[1] = 0;
        }
        else {
            if (UI::keys[GLFW_KEY_W])
                handle_scene->camera->translate_camera(G::CAMERA::FORWARD, clock->delta()*5);
            if (UI::keys[GLFW_KEY_A])
                handle_scene->camera->translate_camera(G::CAMERA::LEFT, clock->delta()*5);
            if (UI::keys[GLFW_KEY_S])
                handle_scene->camera->translate_camera(G::CAMERA::BACKWARD, clock->delta()*5);
            if (UI::keys[GLFW_KEY_D])
                handle_scene->camera->translate_camera(G::CAMERA::RIGHT, clock->delta()*5);
			if (UI::keys[GLFW_KEY_Q])
				handle_scene->camera->translate_camera(G::CAMERA::DOWNWARD, clock->delta()*5);
			if (UI::keys[GLFW_KEY_E])
				handle_scene->camera->translate_camera(G::CAMERA::UPWARD, clock->delta()*5);

            if (UI::keys[GLFW_KEY_UP])
                v += glm::vec3(0, 0, 1);
            if (UI::keys[GLFW_KEY_LEFT])
                v += glm::vec3(1, 0, 0);
            if (UI::keys[GLFW_KEY_DOWN])
                v += glm::vec3(0, 0, -1);
            if (UI::keys[GLFW_KEY_RIGHT])
                v += glm::vec3(-1, 0, 0);
        }
        float len = glm::length(v);
        if (len > 0)
            v *= 5.f/len;
        handle_scene->playerObject->dyn.vel = v; 
        handle_scene->generateMoreMaze();
    }
	else {
		v = glm::vec3(0);
		handle_scene->playerObject->dyn.vel = v;
	}

    if (UI::keys[GLFW_KEY_LEFT_SHIFT])
        handle_scene->camera->sprint *= static_cast<GLfloat>(1.0 + 0.5*clock->delta());
	else
		handle_scene->camera->sprint = 1.0f;

    if (UI::keys[GLFW_KEY_F]) {
        UI::keys[GLFW_KEY_F] = false;
        handle_scene->toggle_flashlight();
    }

    if (UI::keys[GLFW_KEY_N]) {
        UI::keys[GLFW_KEY_N] = false;
        handle_scene->toggle_noclip();
    }
}

int kill_app(int retVal) {
    glfwTerminate();
    std::cout << std::endl << "Application Terminated. With exit value: " << retVal << std::endl;
    //D(slow_print(50, 300, "\n\nGoodbye..."));
    //D(slow_print(150, 500, "OK"));
    return retVal;
}
