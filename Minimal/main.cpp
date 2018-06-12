/************************************************************************************

Authors     :   Bradley Austin Davis <bdavis@saintandreas.org>
Copyright   :   Copyright Brad Davis. All Rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/

#include <iostream>
#include <memory>
#include <exception>
#include <algorithm>

#include <Windows.h>

#define __STDC_FORMAT_MACROS 1

#define FAIL(X) throw std::runtime_error(X)

///////////////////////////////////////////////////////////////////////////////
//
// GLM is a C++ math library meant to mirror the syntax of GLSL 
//

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// Import the most commonly used types into the default namespace
using glm::ivec3;
using glm::ivec2;
using glm::uvec2;
using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::quat;

///////////////////////////////////////////////////////////////////////////////
//
// GLEW gives cross platform access to OpenGL 3.x+ functionality.  
//

#include <GL/glew.h>

/** Define any preprocessing directives here **/
// Sound buffer indices
#define STAGE1_BGM 0
#define STAGE2_BGM 1
#define MON_DEATH1 2
#define MON_DEATH2 3
#define GAME_OVER 4
#define GAME_WIN 5
#define CAT_HIT 6
#define CAT_LOW_HEALTH 7
#define GAME_START 8

// Limits
#define GAME_TIME_LIMIT 60.0
#define HP_LIMIT 10
#define LOW_HEALTH_LIMIT 4

#define SERVER 1
#define CLIENT 2

/** Define our file inclusions here **/
#include <chrono>
#include <ctime>
#include "Model.h"
#include "Audio.h"
#include "Skybox.h"
#include "Shader.h"
#include "Treasure.h"
#include "Player.h"
#include "Enemy.h"
#include "Curve.h"


bool checkFramebufferStatus(GLenum target = GL_FRAMEBUFFER) {
	GLuint status = glCheckFramebufferStatus(target);
	switch (status) {
	case GL_FRAMEBUFFER_COMPLETE:
		return true;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		std::cerr << "framebuffer incomplete attachment" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cerr << "framebuffer missing attachment" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		std::cerr << "framebuffer incomplete draw buffer" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		std::cerr << "framebuffer incomplete read buffer" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		std::cerr << "framebuffer incomplete multisample" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		std::cerr << "framebuffer incomplete layer targets" << std::endl;
		break;

	case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cerr << "framebuffer unsupported internal format or image" << std::endl;
		break;

	default:
		std::cerr << "other framebuffer error" << std::endl;
		break;
	}

	return false;
}

bool checkGlError() {
	GLenum error = glGetError();
	if (!error) {
		return false;
	}
	else {
		switch (error) {
		case GL_INVALID_ENUM:
			std::cerr << ": An unacceptable value is specified for an enumerated argument.The offending command is ignored and has no other side effect than to set the error flag.";
			break;
		case GL_INVALID_VALUE:
			std::cerr << ": A numeric argument is out of range.The offending command is ignored and has no other side effect than to set the error flag";
			break;
		case GL_INVALID_OPERATION:
			std::cerr << ": The specified operation is not allowed in the current state.The offending command is ignored and has no other side effect than to set the error flag..";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cerr << ": The framebuffer object is not complete.The offending command is ignored and has no other side effect than to set the error flag.";
			break;
		case GL_OUT_OF_MEMORY:
			std::cerr << ": There is not enough memory left to execute the command.The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";
			break;
		case GL_STACK_UNDERFLOW:
			std::cerr << ": An attempt has been made to perform an operation that would cause an internal stack to underflow.";
			break;
		case GL_STACK_OVERFLOW:
			std::cerr << ": An attempt has been made to perform an operation that would cause an internal stack to overflow.";
			break;
		}
		return true;
	}
}

void glDebugCallbackHandler(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, GLvoid* data) {
	OutputDebugStringA(msg);
	std::cout << "debug call: " << msg << std::endl;
}

//////////////////////////////////////////////////////////////////////
//
// GLFW provides cross platform window creation
//

#include <GLFW/glfw3.h>

namespace glfw {
	inline GLFWwindow * createWindow(const uvec2 & size, const ivec2 & position = ivec2(INT_MIN)) {
		GLFWwindow * window = glfwCreateWindow(size.x, size.y, "glfw", nullptr, nullptr);
		if (!window) {
			FAIL("Unable to create rendering window");
		}
		if ((position.x > INT_MIN) && (position.y > INT_MIN)) {
			glfwSetWindowPos(window, position.x, position.y);
		}
		return window;
	}
}

// A class to encapsulate using GLFW to handle input and render a scene
class GlfwApp {

protected:
	uvec2 windowSize;
	ivec2 windowPosition;
	GLFWwindow * window{ nullptr };
	unsigned int frame{ 0 };

public:
	GlfwApp() {
		// Initialize the GLFW system for creating and positioning windows
		if (!glfwInit()) {
			FAIL("Failed to initialize GLFW");
		}
		glfwSetErrorCallback(ErrorCallback);
	}

	virtual ~GlfwApp() {
		if (nullptr != window) {
			glfwDestroyWindow(window);
		}
		glfwTerminate();
	}

	virtual int run() {
		preCreate();

		window = createRenderingTarget(windowSize, windowPosition);

		if (!window) {
			std::cout << "Unable to create OpenGL window" << std::endl;
			return -1;
		}

		postCreate();

		initGl();

		while (!glfwWindowShouldClose(window)) {
			++frame;
			glfwPollEvents();
			update();
			draw();
			finishFrame();
		}

		shutdownGl();

		return 0;
	}


protected:
	virtual GLFWwindow * createRenderingTarget(uvec2 & size, ivec2 & pos) = 0;

	virtual void draw() = 0;

	void preCreate() {
		glfwWindowHint(GLFW_DEPTH_BITS, 16);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	}


	void postCreate() {
		glfwSetWindowUserPointer(window, this);
		glfwSetKeyCallback(window, KeyCallback);
		glfwSetMouseButtonCallback(window, MouseButtonCallback);
		glfwMakeContextCurrent(window);

		// Initialize the OpenGL bindings
		// For some reason we have to set this experminetal flag to properly
		// init GLEW if we use a core context.
		glewExperimental = GL_TRUE;
		if (0 != glewInit()) {
			FAIL("Failed to initialize GLEW");
		}
		glGetError();

		if (GLEW_KHR_debug) {
			GLint v;
			glGetIntegerv(GL_CONTEXT_FLAGS, &v);
			if (v & GL_CONTEXT_FLAG_DEBUG_BIT) {
				//glDebugMessageCallback(glDebugCallbackHandler, this);
			}
		}
	}

	virtual void initGl() {
	}

	virtual void shutdownGl() {
	}

	virtual void finishFrame() {
		glfwSwapBuffers(window);
	}

	virtual void destroyWindow() {
		glfwSetKeyCallback(window, nullptr);
		glfwSetMouseButtonCallback(window, nullptr);
		glfwDestroyWindow(window);
	}

	virtual void onKey(int key, int scancode, int action, int mods) {
		if (GLFW_PRESS != action) {
			return;
		}

		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, 1);
			return;
		}
	}

	virtual void update() {}

	virtual void onMouseButton(int button, int action, int mods) {}

protected:
	virtual void viewport(const ivec2 & pos, const uvec2 & size) {
		glViewport(pos.x, pos.y, size.x, size.y);
	}

private:

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		GlfwApp * instance = (GlfwApp *)glfwGetWindowUserPointer(window);
		instance->onKey(key, scancode, action, mods);
	}

	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		GlfwApp * instance = (GlfwApp *)glfwGetWindowUserPointer(window);
		instance->onMouseButton(button, action, mods);
	}

	static void ErrorCallback(int error, const char* description) {
		FAIL(description);
	}
};

//////////////////////////////////////////////////////////////////////
//
// The Oculus VR C API provides access to information about the HMD
//

#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>

namespace ovr {

	// Convenience method for looping over each eye with a lambda
	template <typename Function>
	inline void for_each_eye(Function function) {
		for (ovrEyeType eye = ovrEyeType::ovrEye_Left;
			eye < ovrEyeType::ovrEye_Count;
			eye = static_cast<ovrEyeType>(eye + 1)) {
			function(eye);
		}
	}

	inline mat4 toGlm(const ovrMatrix4f & om) {
		return glm::transpose(glm::make_mat4(&om.M[0][0]));
	}

	inline mat4 toGlm(const ovrFovPort & fovport, float nearPlane = 0.01f, float farPlane = 10000.0f) {
		return toGlm(ovrMatrix4f_Projection(fovport, nearPlane, farPlane, true));
	}

	inline vec3 toGlm(const ovrVector3f & ov) {
		return glm::make_vec3(&ov.x);
	}

	inline vec2 toGlm(const ovrVector2f & ov) {
		return glm::make_vec2(&ov.x);
	}

	inline uvec2 toGlm(const ovrSizei & ov) {
		return uvec2(ov.w, ov.h);
	}

	inline quat toGlm(const ovrQuatf & oq) {
		return glm::make_quat(&oq.x);
	}

	inline mat4 toGlm(const ovrPosef & op) {
		mat4 orientation = glm::mat4_cast(toGlm(op.Orientation));
		mat4 translation = glm::translate(mat4(), ovr::toGlm(op.Position));
		return translation * orientation;
	}

	inline ovrMatrix4f fromGlm(const mat4 & m) {
		ovrMatrix4f result;
		mat4 transposed(glm::transpose(m));
		memcpy(result.M, &(transposed[0][0]), sizeof(float) * 16);
		return result;
	}

	inline ovrVector3f fromGlm(const vec3 & v) {
		ovrVector3f result;
		result.x = v.x;
		result.y = v.y;
		result.z = v.z;
		return result;
	}

	inline ovrVector2f fromGlm(const vec2 & v) {
		ovrVector2f result;
		result.x = v.x;
		result.y = v.y;
		return result;
	}

	inline ovrSizei fromGlm(const uvec2 & v) {
		ovrSizei result;
		result.w = v.x;
		result.h = v.y;
		return result;
	}

	inline ovrQuatf fromGlm(const quat & q) {
		ovrQuatf result;
		result.x = q.x;
		result.y = q.y;
		result.z = q.z;
		result.w = q.w;
		return result;
	}
}

class RiftManagerApp {
protected:
	ovrSession _session;
	ovrHmdDesc _hmdDesc;
	ovrGraphicsLuid _luid;

public:
	RiftManagerApp() {
		if (!OVR_SUCCESS(ovr_Create(&_session, &_luid))) {
			FAIL("Unable to create HMD session");
		}

		_hmdDesc = ovr_GetHmdDesc(_session);
	}

	~RiftManagerApp() {
		ovr_Destroy(_session);
		_session = nullptr;
	}
};

class RiftApp : public GlfwApp, public RiftManagerApp {
public:

private:
	GLuint _fbo{ 0 };
	GLuint _depthBuffer{ 0 };
	ovrTextureSwapChain _eyeTexture;

	GLuint _mirrorFbo{ 0 };
	ovrMirrorTexture _mirrorTexture;

	ovrEyeRenderDesc _eyeRenderDescs[2];

	mat4 _eyeProjections[2];

	ovrLayerEyeFov _sceneLayer;
	ovrViewScaleDesc _viewScaleDesc;

	uvec2 _renderTargetSize;
	uvec2 _mirrorSize;

public:

	RiftApp() {
		using namespace ovr;
		_viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;

		memset(&_sceneLayer, 0, sizeof(ovrLayerEyeFov));
		_sceneLayer.Header.Type = ovrLayerType_EyeFov;
		_sceneLayer.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;

		ovr::for_each_eye([&](ovrEyeType eye) {
			ovrEyeRenderDesc& erd = _eyeRenderDescs[eye] = ovr_GetRenderDesc(_session, eye, _hmdDesc.DefaultEyeFov[eye]);
			ovrMatrix4f ovrPerspectiveProjection =
				ovrMatrix4f_Projection(erd.Fov, 0.01f, 1000.0f, ovrProjection_ClipRangeOpenGL);
			_eyeProjections[eye] = ovr::toGlm(ovrPerspectiveProjection);
			_viewScaleDesc.HmdToEyePose[eye] = erd.HmdToEyePose;

			ovrFovPort & fov = _sceneLayer.Fov[eye] = _eyeRenderDescs[eye].Fov;
			auto eyeSize = ovr_GetFovTextureSize(_session, eye, fov, 1.0f);
			_sceneLayer.Viewport[eye].Size = eyeSize;
			_sceneLayer.Viewport[eye].Pos = { (int)_renderTargetSize.x, 0 };

			_renderTargetSize.y = std::max(_renderTargetSize.y, (uint32_t)eyeSize.h);
			_renderTargetSize.x += eyeSize.w;
		});
		// Make the on screen window 1/4 the resolution of the render target
		_mirrorSize = _renderTargetSize;
		_mirrorSize /= 4;
	}

protected:
	GLFWwindow * createRenderingTarget(uvec2 & outSize, ivec2 & outPosition) override {
		return glfw::createWindow(_mirrorSize);
	}

	void initGl() override {
		GlfwApp::initGl();

		// Disable the v-sync for buffer swap
		glfwSwapInterval(0);

		ovrTextureSwapChainDesc desc = {};
		desc.Type = ovrTexture_2D;
		desc.ArraySize = 1;
		desc.Width = _renderTargetSize.x;
		desc.Height = _renderTargetSize.y;
		desc.MipLevels = 1;
		desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.SampleCount = 1;
		desc.StaticImage = ovrFalse;
		ovrResult result = ovr_CreateTextureSwapChainGL(_session, &desc, &_eyeTexture);
		_sceneLayer.ColorTexture[0] = _eyeTexture;
		if (!OVR_SUCCESS(result)) {
			FAIL("Failed to create swap textures");
		}

		int length = 0;
		result = ovr_GetTextureSwapChainLength(_session, _eyeTexture, &length);
		if (!OVR_SUCCESS(result) || !length) {
			FAIL("Unable to count swap chain textures");
		}
		for (int i = 0; i < length; ++i) {
			GLuint chainTexId;
			ovr_GetTextureSwapChainBufferGL(_session, _eyeTexture, i, &chainTexId);
			glBindTexture(GL_TEXTURE_2D, chainTexId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		// Set up the framebuffer object
		glGenFramebuffers(1, &_fbo);
		glGenRenderbuffers(1, &_depthBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, _renderTargetSize.x, _renderTargetSize.y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		ovrMirrorTextureDesc mirrorDesc;
		memset(&mirrorDesc, 0, sizeof(mirrorDesc));
		mirrorDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
		mirrorDesc.Width = _mirrorSize.x;
		mirrorDesc.Height = _mirrorSize.y;
		if (!OVR_SUCCESS(ovr_CreateMirrorTextureGL(_session, &mirrorDesc, &_mirrorTexture))) {
			FAIL("Could not create mirror texture");
		}
		glGenFramebuffers(1, &_mirrorFbo);
	}

	void onKey(int key, int scancode, int action, int mods) override {
		if (GLFW_PRESS == action) switch (key) {
		case GLFW_KEY_R:
			ovr_RecenterTrackingOrigin(_session);
			return;
		}

		GlfwApp::onKey(key, scancode, action, mods);
	}

	void draw() final override {
		ovrPosef eyePoses[2];
		ovr_GetEyePoses(_session, frame, true, _viewScaleDesc.HmdToEyePose, eyePoses, &_sceneLayer.SensorSampleTime);

		int curIndex;
		ovr_GetTextureSwapChainCurrentIndex(_session, _eyeTexture, &curIndex);
		GLuint curTexId;
		ovr_GetTextureSwapChainBufferGL(_session, _eyeTexture, curIndex, &curTexId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curTexId, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ovr::for_each_eye([&](ovrEyeType eye) {
			const auto& vp = _sceneLayer.Viewport[eye];
			glViewport(vp.Pos.x, vp.Pos.y, vp.Size.w, vp.Size.h);
			_sceneLayer.RenderPose[eye] = eyePoses[eye];
			renderScene(_eyeProjections[eye], ovr::toGlm(eyePoses[eye]));
		});
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		ovr_CommitTextureSwapChain(_session, _eyeTexture);
		ovrLayerHeader* headerList = &_sceneLayer.Header;
		ovr_SubmitFrame(_session, frame, &_viewScaleDesc, &headerList, 1);

		GLuint mirrorTextureId;
		ovr_GetMirrorTextureBufferGL(_session, _mirrorTexture, &mirrorTextureId);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, _mirrorFbo);
		glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTextureId, 0);
		glBlitFramebuffer(0, 0, _mirrorSize.x, _mirrorSize.y, 0, _mirrorSize.y, _mirrorSize.x, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}

	virtual void renderScene(const glm::mat4 & projection, const glm::mat4 & headPose) = 0;
};

/*-------------------RENDER MODELS USING THE EXAMPLE APP -------------------*/

// An example application that renders a simple cube
class ExampleApp : public RiftApp {

public:
	ExampleApp() {}

	/** Data variables **/
	/* 3D Models */
	Model * head, *sphere, *sword, *treasure, *pedestal;	// Player models, treasure models
	Model * str_mons;										// Enemy models
	Skybox * stage1, *stage2;								// Skyboxes represent different stages
	// TODO: MAYBE ADD A TERRAIN?
	Treasure * treasure_unit;								// Treasure object taken as one unit
	Player * player_1, * player_2;							// Players
	Enemy * test_enemy;										// Enemy

	/* Enemy Path testers */
	Curve * curve1, *curve2, *curve3, *curve4;				// Enemy paths
	vector<Curve *> path_container;							// Contains pointers to enemy paths
	
	/* Shaders */
	Shader * obj_shader, *sky_shader;						// Shaders for objects and skybox
	Shader * treasure_shader, *player_shader;				// Shaders for the treasure and the player
	Shader * enemy_shader, *bound_shader;					// Shader for the enemy

	/* Audio */
	Audio * sounds;											// Holds sounds (bgm/sound fx)

	/* State indicators */
	unsigned int stage_type = 1;							// Stage to load (NOT ENOUGH TIME TO IMPLEMENT)
	unsigned int server_or_client = 1;						//
	int HP = HP_LIMIT;										// HP of the cat
	bool game_win = false;									// Game win state
	bool game_lose = false;									// Game lose state
	bool start_game = false;								// Start game start
	bool button_down = false;								// Button press state
	std::chrono::system_clock::time_point start_time;		// Keep track of time at each starting call

	/* Position/Transformation indicators */
	mat4 rHandTransform1, rHandTransform2;					// Right hand transformation (translation * rotation)
	mat4 headTransform1, headTransform2;					// Head transformation matrix (translation * rotation)
	
	/* Path indices */
	unsigned int path_ind1 = 0;
	unsigned int path_ind2 = 0;
	unsigned int path_ind3 = 0;
	unsigned int path_ind4 = 0;
	vector<unsigned int *> path_ind_container;

	/** Private Functions **/
	/*----------------- INITIALIZER FUNCTIONS -----------------*/
	void initialize_models() {
		/* Initialize models here */
		cout << "Loading models..." << endl;
		// Load up each Model obj
		head = new Model(string("assets/models/obj/male_head.obj"), false);
		sphere = new Model(string("assets/models/obj/sphere2.obj"), false);
		sword = new Model(string("assets/models/obj/sword_obj.obj"), false);
		treasure = new Model(string("assets/models/obj/Morgana3D.obj"), false);
		pedestal = new Model(string("assets/models/obj/001_Pedestal_high_poly.obj"), false);
		str_mons = new Model(string("assets/models/obj/cacodemon.obj"), false);
		// Set up stage here
		treasure_unit = new Treasure(pedestal, treasure);	// Pedestal and treasure treated as one whole unit
		player_1 = new Player(head, sphere, sword, true);
		player_2 = new Player(head, sphere, sword, false);
		test_enemy = new Enemy(str_mons, false, 1.5f);
		cout << "Finished loading models!" << std::endl;
	}

	void initialize_stages() {
		// Skybox image path locations (pz, px, nx, py, ny, nz). Will need to rotate the skybox faces
		cout << "Loading stages..." << endl;
		std::vector<char *> stage1Faces = {
			"assets/skybox/nuke/nuke_ft.ppm",
			"assets/skybox/nuke/nuke_lf.ppm",
			"assets/skybox/nuke/nuke_rt.ppm",
			"assets/skybox/nuke/nuke_up.ppm",
			"assets/skybox/nuke/nuke_dn.ppm",
			"assets/skybox/nuke/nuke_bk.ppm"
		};
		std::vector<char *> stage2Faces = {
			"assets/skybox/blood/blood_ft.ppm",
			"assets/skybox/blood/blood_lf.ppm",
			"assets/skybox/blood/blood_rt.ppm",
			"assets/skybox/blood/blood_up.ppm",
			"assets/skybox/blood/blood_dn.ppm",
			"assets/skybox/blood/blood_bk.ppm"
		};
		stage1 = new Skybox(stage1Faces);
		stage2 = new Skybox(stage2Faces);
		cout << "Finished loading stages!" << endl;
	}

	void initialize_audio() {
		cout << "Loading audio..." << endl;
		std::vector<std::string> soundFiles;
		soundFiles.push_back("assets/sounds/VHS Glitch - Jaw Breaker.wav");
		soundFiles.push_back("assets/sounds/taiko_bgm.wav");
		soundFiles.push_back("assets/sounds/monster_death1.wav");
		soundFiles.push_back("assets/sounds/monster_death2.wav");
		soundFiles.push_back("assets/sounds/game_over.wav");
		soundFiles.push_back("assets/sounds/game_win.wav");
		soundFiles.push_back("assets/sounds/treasur_shield_hit.wav");
		soundFiles.push_back("assets/sounds/treasur_HP_low.wav");
		soundFiles.push_back("assets/sounds/game_start.wav");
		sounds = new Audio(soundFiles);
		cout << "Finished loading audio!" << endl;
	}

	void initialize_shaders() {
		obj_shader = new Shader("obj_shader.vert", "obj_shader.frag");
		sky_shader = new Shader("skybox.vert", "skybox.frag");
		player_shader = new Shader("player.vert", "player.frag");
		enemy_shader = new Shader("enemy_shader.vert", "enemy_shader.frag");
		bound_shader = new Shader("bounds.vert", "bounds.frag");
	}

	void initialize_enemy_paths() {
		// Front path
		vec4 p0 = vec4(0, -0.2f, -10.0f, 1.0f);
		vec4 p1 = vec4(-7.0f, 0.9f, -2.8f, 1.0f);
		vec4 p2 = vec4(7.0f, -0.2f, -1.4f, 1.0f);
		vec4 p3 = vec4(0, -0.2f, 1.0f, 1.0f);
		curve1 = new Curve(mat4(p0, p1, p2, p3), 1300);

		// Back path
		p0 = vec4( 0.0f, 8.0f, 10.0f, 1.0f);
		p1 = vec4(7.0f, 0.9f, 7.0f, 1.0f);
		p2 = vec4(-4.0f, 5.0f, 4.0f, 1.0f);
		curve2 = new Curve(mat4(p0, p1, p2, p3), 800);

		// Left path
		p0 = vec4(-7.0f, 1.0f, 0.0f, 1.0f);
		p1 = vec4(-5.5f, -1.0f, 1.0f, 1.0f);
		p2 = vec4(-2.0f, -0.2f, 1.0f, 1.0f);
		curve3 = new Curve(mat4(p0, p1, p2, p3), 700);

		// Right path
		p0 = vec4(9.0f, 2.0f, -3.0f, 1.0f);
		p1 = vec4(6.4f, 0.0f, 1.0f, 1.0f);
		p2 = vec4(4.5f, 0.0f, 1.2f, 1.0f);
		curve4 = new Curve(mat4(p0, p1, p2, p3), 900);

		// Populate containers
		path_container.push_back(curve1);
		path_container.push_back(curve2);
		path_container.push_back(curve3);
		path_container.push_back(curve4);
		path_ind_container.push_back(&path_ind1);
		path_ind_container.push_back(&path_ind2);
		path_ind_container.push_back(&path_ind3);
		path_ind_container.push_back(&path_ind4);

	}

	/*------------------ UPDATE FUNCTIONS -------------------*/
	void updateHeadAndHandTransforms() {
		// Query Touch controllers. Query their parameters:
		double displayMidpointSeconds = ovr_GetPredictedDisplayTime(_session, 0);
		// GET TRACKING STATE
		ovrTrackingState trackState = ovr_GetTrackingState(_session, displayMidpointSeconds, ovrTrue);

		// Updating hand transformation
		mat4 translate_hand = glm::translate(ovr::toGlm(trackState.HandPoses[ovrHand_Right].ThePose.Position));						// Get hand position
		mat4 controllerRotationMat = glm::toMat4(ovr::toGlm(ovrQuatf(trackState.HandPoses[ovrHand_Right].ThePose.Orientation)));	// Get hand orientation
		rHandTransform1 = translate_hand * controllerRotationMat;

		// Updating head transformation
		mat4 headPosMat = glm::translate(ovr::toGlm(trackState.HeadPose.ThePose.Position));				// Get head position
		mat4 headRotMat = glm::toMat4(ovr::toGlm(ovrQuatf(trackState.HeadPose.ThePose.Orientation)));	// Get head orientation
		headTransform1 = headPosMat * headRotMat;
	}
	
	void handleGameState(bool wonGame) {
		// Did you win?
		if (wonGame) {
			game_win = true;
			// Play win game sound
			sounds->play(GAME_WIN);
			cout << "YOU WIN!" << endl;
		}
		// Did you lose?
		else {
			game_win = false;
			// Play lose game sound
			sounds->play(GAME_OVER);
			cout << "YOU LOSE!" << endl;
		}

		// Reset states
		start_game = false;
		start_time = std::chrono::system_clock::now();
		HP = HP_LIMIT;
		path_ind1 = 0;
		path_ind2 = 0;
		path_ind3 = 0;
		path_ind4 = 0;
	}

	void handleMainGameLogic() {
		// Update sword bounding box
		player_1->updateBoundingBox(rHandTransform1);

		// Go through each path (1 monster is on each path at a time)
		for (unsigned int i = 0; i < path_container.size(); i++) {
			// Update monster hitbox
			test_enemy->updateHitBox(glm::translate(path_container[i]->getVertices()[*(path_ind_container[i])]));
			// Check if box is hit
			if (player_1->checkHit(test_enemy->getHitBox())) {
				*(path_ind_container[i]) = 0;
				sounds->play(MON_DEATH1);
			}
			// Update monster movement
			else {
				(*(path_ind_container[i]))++;
			}
			// Check if enemy has reached the cat
			if (*(path_ind_container[i]) == path_container[i]->getVertices().size()) {
				HP--;
				sounds->play(CAT_HIT);
			}
		}
		// Check if low HP
		if (HP <= LOW_HEALTH_LIMIT) {
			sounds->play(CAT_LOW_HEALTH);
		}

		// Reset index
		for (unsigned int i = 0; i < path_container.size(); i++) {
			(*path_ind_container[i]) = *(path_ind_container[i]) % path_container[i]->getVertices().size();
		}
	}

protected:
	void initGl() override {
		RiftApp::initGl();
		glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
		glEnable(GL_DEPTH_TEST);
		ovr_RecenterTrackingOrigin(_session);

		// Initialize 3D models
		initialize_models();
		// Load skyboxes
		initialize_stages();
		// Initialize audio here
		initialize_audio();
		// Initialize shaders here
		initialize_shaders();
		// Initialize paths here
		initialize_enemy_paths();

		/*
		cout << "Size of mat4 " << sizeof(mat4) << endl;
		cout << "Size of char " << sizeof(char) << endl;
		cout << "Size of unsigned int " << sizeof(unsigned int) << endl;
		*/

		/* Pick server or client here 
		do {
			cout << "PICK A TYPE: SERVER = 1 | CLIENT = 2..." << endl;
			cin >> server_or_client;
			if (server_or_client != 1 && server_or_client != 2) {
				cout << "Invalid option! Try again!" << endl;
			}
		} while (server_or_client != 1 && server_or_client != 2);
		
		cout << "Waiting on other player..." << endl;
		// Stall until both server and client players are ready TODO
		*/

		glEnable(GL_CULL_FACE);			// Enable backface culling
		cout << "Please wait 5 seconds..." << endl;
		// Start countdown timer by getting current time
		start_time = std::chrono::system_clock::now();
	}

	void shutdownGl() override {
		/** TODO: DEAL WITH CLEANUP HERE **/
		delete head, sphere, sword, treasure, pedestal;
		delete str_mons;
		delete treasure_unit, player_1, player_2;
		delete test_enemy;
		delete stage1, stage2;
		delete sounds;
		delete obj_shader, sky_shader, treasure_shader, player_shader, bound_shader;
	}

	void update() {
		/** Deal with idle_callbacks here **/
		// Update head and hand transformation matrices
		updateHeadAndHandTransforms();

		// Play stage bgm
		if (stage_type == 1) {
			sounds->play(STAGE1_BGM);
		}
		else {
			sounds->play(STAGE2_BGM);
		}

		// Update timer
		std::chrono::system_clock::time_point current_time = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = current_time - start_time;
		// Do not go through main game logic until 3 seconds after start
		if (!start_game) {
			if (elapsed_seconds.count() > 5.0) {
				start_game = true;
				cout << "GAME START!" << endl;
				// Play sound
				sounds->play(GAME_START);
				// Start time for actual game
				start_time = std::chrono::system_clock::now();
			}
		}
		else {
			// Check if players won
			if (elapsed_seconds.count() >= GAME_TIME_LIMIT) {
				handleGameState(true);
			}
			// Check if players lost
			else if (HP == 0) {
				handleGameState(false);
			}
			// Continue main game updates
			else {
				handleMainGameLogic();
			}
		}
	}

	// RENDER MODELS HERE
	void renderScene(const glm::mat4 & projection, const glm::mat4 & headPose) override {
		// Skybox (Stage) Rendering
		glFrontFace(GL_CW);	// Treat counterclockwise denotation as back face
		sky_shader->use();
		switch (stage_type) {
		case 1:
			stage1->draw(sky_shader->ID, projection, glm::inverse(headPose));
			break;
		case 2:
			stage2->draw(sky_shader->ID, projection, glm::inverse(headPose));
		}

		// Model Rendering
		glFrontFace(GL_CCW);	// Treat clockwise orientation as back face (default)
		// Cat rendering
		obj_shader->use();
		treasure_unit->draw(*obj_shader, projection, glm::inverse(headPose));
		// Player rendering
		player_shader->use();
		player_1->drawPlayer(*player_shader, projection, glm::inverse(headPose), rHandTransform1, headTransform1);
		player_2->drawPlayer(*player_shader, projection, glm::inverse(headPose), mat4(1.0f), mat4(1.0f));

		// Render enemies when game properly starts
		if (start_game) {
			/**/
			// Enemy rendering
			enemy_shader->use();
			test_enemy->draw(*enemy_shader, projection, glm::inverse(headPose), glm::translate(curve1->getVertices()[path_ind1]));
			test_enemy->draw(*enemy_shader, projection, glm::inverse(headPose), glm::translate(curve2->getVertices()[path_ind2]) * glm::rotate(glm::pi<float>(), vec3(0, 1, 0)));
			test_enemy->draw(*enemy_shader, projection, glm::inverse(headPose), glm::translate(curve3->getVertices()[path_ind3]) * glm::rotate(glm::pi<float>() / 2, vec3(0, 1, 0)));
			test_enemy->draw(*enemy_shader, projection, glm::inverse(headPose), glm::translate(curve4->getVertices()[path_ind4]) * glm::rotate(-glm::pi<float>() / 2, vec3(0, 1, 0)));

			/* DEAL WITH DEBUG CODE HERE */
			// Path rendering
			curve1->draw(enemy_shader->ID, projection, glm::inverse(headPose));
			curve2->draw(enemy_shader->ID, projection, glm::inverse(headPose));
			curve3->draw(enemy_shader->ID, projection, glm::inverse(headPose));
			curve4->draw(enemy_shader->ID, projection, glm::inverse(headPose));
			
			// Bounding box rendering
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			bound_shader->use();
			player_1->drawBoundingBox(*bound_shader, projection, glm::inverse(headPose), rHandTransform1);
			test_enemy->drawHitBox(*bound_shader, projection, glm::inverse(headPose), glm::translate(curve1->getVertices()[path_ind1]));
			test_enemy->drawHitBox(*bound_shader, projection, glm::inverse(headPose), glm::translate(curve2->getVertices()[path_ind2]) * glm::rotate(glm::pi<float>(), vec3(0, 1, 0)));
			test_enemy->drawHitBox(*bound_shader, projection, glm::inverse(headPose), glm::translate(curve3->getVertices()[path_ind3]) * glm::rotate(glm::pi<float>() / 2, vec3(0, 1, 0)));
			test_enemy->drawHitBox(*bound_shader, projection, glm::inverse(headPose), glm::translate(curve4->getVertices()[path_ind4]) * glm::rotate(-glm::pi<float>() / 2, vec3(0, 1, 0)));
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			
		}
	}
};

// Execute our example class
//int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
int main(int argc, char** argv) {
	int result = -1;
	try {
		if (!OVR_SUCCESS(ovr_Initialize(nullptr))) {
			FAIL("Failed to initialize the Oculus SDK");
		}
		result = ExampleApp().run();
	}
	catch (std::exception & error) {
		OutputDebugStringA(error.what());
		std::cerr << error.what() << std::endl;
	}
	ovr_Shutdown();
	return result;
}