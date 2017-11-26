#ifndef SCENE_H_GUARD
#define SCENE_H_GUARD

#include <vector>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <random>
#include "Object.hpp"
#include "Cube.hpp"
#include "Sphere.hpp"
#include "Slime.hpp"
#include "Structs.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "AI.hpp"

#pragma warning(push, 0)
#pragma warning(disable: 4055)
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#pragma warning(disable: 4201)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

enum shaderNames {
	TEXTURE,
	FLAT,
	LIGHT,
	TEST
};

class Scene {
public:
	Scene(unsigned seed = 0);
	virtual ~Scene();

	void addEnemyObject(float r, float x, float y, float z);
    void addEnemyObject(float r, glm::vec3 p);
	void addWall(float h, float x1, float y1, float x2, float y2); 
    void addWall(float h, float w, float l, glm::vec3 center);
	void addCylinder(float h, float r, float x, float y);
	void setPlayerColor(float r, float g, float b);
	void setPlayerColor(glm::vec3 rgb);
    void fillEnemyVector();
    void fillEnemyVector(int start, int end, bool colsFlag);
    void fillStaticObjVector();
    void automatonSimulate();
    void initMaze();
    void generateMoreMaze();
	void updatePRM();

	void setBounds(glm::vec3 min, glm::vec3 max);
	
	void setupTestingObjects();

	void simulate(GLfloat dt);

	void render();

    void toggle_flashlight();
    void toggle_noclip();

	void enableTextureShader(glm::mat4 proj, glm::mat4 view);
	void enableFlatShader(glm::mat4 proj, glm::mat4 view);
	void enableLightShader(glm::mat4 proj, glm::mat4 view);
	void enableTestShader(glm::mat4 proj, glm::mat4 view);

    void reset_proj_view();
    glm::mat4 proj, view;

	Object *playerObject;
	float maxEnemies;
    Cube *floor;
	std::vector<Object *> enemyObjects;
	float maxObstacles;
	std::vector<Object *> staticObjects;
    mazeData mazeInfo;
    std::vector<std::vector<mazeCell>> maze;
	//PRM prm;

	bufferContainer bc;
    glm::vec3 snapToGrid(glm::vec3 position);
    int mod(int num1, int num2);
    //wouldn't it be better to attach a shader to each object?
	Shader * curShader;
	std::unordered_map<shaderNames, Shader *> shaders;

	Camera * camera;
	/* lighting */
	glm::vec3 light_diffuse = glm::vec3(0.3f, 0.3f, 0.3f); // Decrease the influence
	glm::vec3 light_ambient = glm::vec3(0.05f, 0.05f, 0.05f); // Low influence
	glm::vec3 light_specular = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 point_light_positions[4] = {
		glm::vec3(-8.f,  1.5f,  -8.f),
		glm::vec3(8.f,  1.5f, 8.f),
		glm::vec3(0.f,  1.f, 4.f),
		glm::vec3(5.f,  2.f, -5.f)
	};
	glm::vec3 dir_light_dir = glm::vec3(-1.0f, -0.3f, -0.6f);

	bool is_flashlight_on = true;
    bool is_noclip_on = false;

    void reset();
    Slime * test;

private:
    typedef std::uniform_int_distribution<int> uint_dist;
    typedef std::uniform_real_distribution<float> float_dist;
    float_dist simple_float = float_dist(0, 1);
    uint_dist width_rand;
    uint_dist height_rand;
    std::default_random_engine gen;
};

#endif // SCENE_H_GUARD