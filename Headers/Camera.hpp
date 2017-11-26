#ifndef CAMERA_H_GUARD
#define CAMERA_H_GUARD

// GL
#pragma warning(push, 0)
#pragma warning(disable: 4055)
#include <glad/glad.h>
#pragma warning(disable: 4201)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#pragma warning(pop)

namespace G {
	namespace CAMERA {
		// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
		enum Movement {
			FORWARD,
			BACKWARD,
			LEFT,
			RIGHT,
			UPWARD,
			DOWNWARD
		};

		// Default camera values
		const GLfloat YAW = -90.0f;
		const GLfloat PITCH = 0.0f;
		const GLfloat SPEED = 6.0f;
		const GLfloat SENSITIVTY = 0.1f;
		const GLfloat FOV = 90.0f;
	}
}

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
/* modified from learnopengl.com */
class Camera
{
public:
	// Camera Attributes
	glm::vec3 pos;
	glm::vec3 dir;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	// Eular Angles
	GLfloat yaw;
	GLfloat pitch;
	// Camera options
	GLfloat speed;
	GLfloat sprint;
	GLfloat sensitivity;
	GLfloat fov;

	// Constructor with vectors
	Camera(
			glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
			GLfloat yaw = G::CAMERA::YAW,
			GLfloat pitch = G::CAMERA::PITCH):
		dir(glm::vec3(0.0f, 0.0f, -1.0f)),
		speed(G::CAMERA::SPEED),
		sprint(1),
		sensitivity(G::CAMERA::SENSITIVTY),
		fov(G::CAMERA::FOV) {
		this->pos = pos;
		this->worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();
	}
	// Constructor with scalar values
	Camera(
			GLfloat posX, GLfloat posY, GLfloat posZ,
			GLfloat upX = 0.0f, GLfloat upY = 1.0f, GLfloat upZ = 0.0f,
			GLfloat yaw = G::CAMERA::YAW, 
			GLfloat pitch = G::CAMERA::PITCH):
		dir(glm::vec3(0.0f, 0.0f, -1.0f)),
		speed(G::CAMERA::SPEED),
		sprint(1),
		sensitivity(G::CAMERA::SENSITIVTY),
		fov(G::CAMERA::FOV) {
		this->pos = glm::vec3(posX, posY, posZ);
		this->worldUp = glm::vec3(upX, upY, upZ);
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();
	}

	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	glm::mat4 getView() {
		return glm::lookAt(this->pos, this->pos + this->dir, this->up);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void translate_camera(G::CAMERA::Movement direction, GLfloat deltaTime) {
		// frame speed
		GLfloat fspeed = this->speed * this->sprint * deltaTime;
		if (direction == G::CAMERA::FORWARD)
			this->pos += this->dir * fspeed;
		if (direction == G::CAMERA::BACKWARD)
			this->pos -= this->dir * fspeed;
		if (direction == G::CAMERA::LEFT)
			this->pos -= this->right * fspeed;
		if (direction == G::CAMERA::RIGHT)
			this->pos += this->right * fspeed;
		if (direction == G::CAMERA::UPWARD)
			this->pos += this->up * fspeed;
		if (direction == G::CAMERA::DOWNWARD)
			this->pos -= this->up *fspeed;
	}
    bool isFacing(glm::vec3 p) {
        glm::vec3 vec = p - this->pos;
        glm::normalize(vec);
        if (glm::dot(vec, this->dir) < cos(PI*fov/180.0)) {
            return true;
        }
        else {
            return false;
        }

    }
	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void mouse_rotate_camera(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true) {
		xoffset *= this->sensitivity;
		yoffset *= this->sensitivity;

		this->yaw -= xoffset;
		this->pitch += yoffset; // += causes an inverted mouse

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
			this->pitch = glm::max(-89.0f, glm::min(89.0f, this->pitch));

		// Update Front, Right and Up Vectors using the updated Euler angles
		this->updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void scroll_zoom_camera(GLfloat yoffset) {
		this->fov = glm::max(1.0f, glm::min(90.0f, this->fov - yoffset));
	}

private:
	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors() {
		// Calculate the new Front vector
		glm::vec3 newDir;
		newDir.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		newDir.y = sin(glm::radians(this->pitch));
		newDir.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->dir = glm::normalize(newDir);
		// Also re-calculate the Right and Up vector
		this->right = glm::normalize(glm::cross(this->dir, this->worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		this->up = glm::normalize(glm::cross(this->right, this->dir));
	}
};

#endif // CAMERA_H_GUARD