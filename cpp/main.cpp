#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#pragma comment(lib, "Winmm.lib")
#include <Windows.h>
#include <mmsystem.h>


#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"
    
#include <iostream>

float sensitivity = 0.05f;
float pitch, yaw;
bool firstMouse = true;

float lastX, lastY;

bool isPreviewActive = false;
float angleForRotation = 0.4f;

bool isDay = true;

bool startCar = false;


// window
gps::Window myWindow;

//skybox
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;
GLuint skyboxDay, skyboxNight;


//shade
const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;
glm::mat4 lightRotation;

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 10.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, -20.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.5f;

GLboolean pressedKeys[1024];

// models
gps::Model3D teapot;
gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D bird;
gps::Model3D car;


GLfloat angle;
GLfloat lightAngle;

// shaders
gps::Shader myBasicShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;
bool showDepthMap;
const GLfloat near_plane = 0.1f, far_plane = 50.0f;
std::vector<const GLchar*> faces;

//cond fog
GLint fogCond;
GLint fogCondLoc;

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xDiff = (float)xpos - lastX;
    float yDiff = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;

    xDiff *= sensitivity;
    yDiff *= sensitivity;

    yaw -= xDiff;
    pitch -= yDiff;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    myCamera.rotate(pitch, yaw);

    myBasicShader.useShaderProgram();
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
}

void initSkyBox() {
    std::vector<const GLchar*> dayFaces = {
        "skybox/posx.jpg",
        "skybox/negx.jpg",
        "skybox/posy.jpg",
        "skybox/negy.jpg",
        "skybox/posz.jpg",
        "skybox/negz.jpg"
    };
    mySkyBox.Load(dayFaces);

    view = myCamera.getViewMatrix();
    skyboxShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
        glm::value_ptr(view));

    projection = glm::perspective(glm::radians(45.0f), (float)1920 / (float)1080, 0.1f, 1000.0f);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
        glm::value_ptr(projection));
}

void initSkyBoxDark() {
    std::vector<const GLchar*> nightFaces = {
        "skybox/posx_dark.jpg",
        "skybox/negx_dark.jpg",
        "skybox/posy_dark.jpg",
        "skybox/negy_dark.jpg",
        "skybox/posz_dark.jpg",
        "skybox/negz_dark.jpg"
    };
    mySkyBox.Load(nightFaces);

    view = myCamera.getViewMatrix();
    skyboxShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
        glm::value_ptr(view));

    projection = glm::perspective(glm::radians(45.0f), (float)1920 / (float)1080, 0.1f, 1000.0f);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
        glm::value_ptr(projection));
}

void processMovement() {
	if (pressedKeys[GLFW_KEY_W] && (myCamera.getPosition().y > 1)) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		//update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

    if (pressedKeys[GLFW_KEY_Q]) {
        myCamera.rotateAroundY(1.0f); 
    }

    if (pressedKeys[GLFW_KEY_E]) {
        myCamera.rotateAroundY(-1.0f); 
    }

    if (pressedKeys[GLFW_KEY_Z]) {
        isPreviewActive = true;
    }

    // stop preview
    if (pressedKeys[GLFW_KEY_X]) {
        isPreviewActive = false;
    }

    if (pressedKeys[GLFW_KEY_J]) { //SOLID
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (pressedKeys[GLFW_KEY_K]) { //WIREFRAME
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (pressedKeys[GLFW_KEY_L]) { //POLIGONAL
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    if (pressedKeys[ GLFW_KEY_F]) {
        fogCond = 0;
        myBasicShader.useShaderProgram();
        glUniform1i(fogCondLoc, fogCond);

    }
    if (pressedKeys[ GLFW_KEY_G]) {
        fogCond = 1;
        myBasicShader.useShaderProgram();
        glUniform1i(fogCondLoc, fogCond);
    }

    if (pressedKeys[GLFW_KEY_N]) {
        isDay = true;
        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "isDay"), isDay);

     
                initSkyBox(); // Load the day skybox

                skyboxShader.useShaderProgram();
                glUniform1i(glGetUniformLocation(skyboxShader.shaderProgram, "skybox"), 0);
                   
    }

    if (pressedKeys[GLFW_KEY_M]) {
        isDay = false;
        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "isDay"), isDay);


            initSkyBoxDark(); // Load the night skybox

            skyboxShader.useShaderProgram();
            glUniform1i(glGetUniformLocation(skyboxShader.shaderProgram, "skybox"), 0);
   
    }

    if (pressedKeys[GLFW_KEY_0]) {
        PlaySoundA("miaw.wav", nullptr, SND_ASYNC | SND_FILENAME);
    }


    if (pressedKeys[GLFW_KEY_9]) {
        PlaySound(nullptr, nullptr, SND_ASYNC);
    }

    if (pressedKeys[GLFW_KEY_H]) { //start car move
        startCar = !startCar;
    }
}

void initOpenGLWindow() {
    myWindow.Create(1024, 768, "OpenGL Project Core");
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);

}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    teapot.LoadModel("models/teapot/final.obj");
    lightCube.LoadModel("models/cube/cube.obj");
    screenQuad.LoadModel("models/quad/quad.obj");
    car.LoadModel("models/car/car.obj");
    bird.LoadModel("models/bird/bird.obj");
}

void initShaders() {
    myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
    lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
    lightShader.useShaderProgram();
    depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
    depthMapShader.useShaderProgram();
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();
}

void initUniforms() {
	myBasicShader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.1f, 200.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

    //set fogCond
    fogCond = 1;
    fogCondLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogCond");
    glUniform1i(fogCondLoc, fogCond);

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
}

void initFBO() {
    //TODO - Create the FBO, the depth texture and attach the depth texture to the FBO

    //generate FBO ID
    glGenFramebuffers(1, &shadowMapFBO);

    //create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    //unbind our frame buffer, otherwise it would be the only frame buffer used by OpenGl
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix

    glm::mat4 lightView = glm::lookAt((glm::mat3(lightRotation) * 25.0f * lightDir), glm::vec3(0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    const GLfloat near_plane = 0.1f, far_plane = 100.0f;
    glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);

    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

    return lightSpaceTrMatrix;

}

void renderTeapot(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    // draw teapot

    //cube.Draw(shader);
   // quad.Draw(shader);

  //  model = glm::mat4(1.0f); 
   // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
   // glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    teapot.Draw(shader);

    //car x= 0.13764 y=36.677 z=0.10738

    static glm::vec3 carPosition = glm::vec3(0.13764f, -0.10738f, 36.677f); 
    const glm::vec3 forwardDirection = glm::vec3(0.0f, 0.0f, 0.1f);     

    if (startCar) {
    carPosition += forwardDirection;
}

    model = glm::mat4(1.0f);
    model = glm::translate(model, carPosition);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    car.Draw(shader);

    model = glm::mat4(1.0f);

    //bird x= -7.0813, y= -3.0496, z= 4.4969
    //spinAround x=-6.1501, y=-1.9767, z= 2.3055

    static float angle = 0.0f; 
    const glm::vec3 birdInitialPosition = glm::vec3(-7.0813f, 4.4969f, -3.0496f); 
    const glm::vec3 spinAroundPoint = glm::vec3(-6.1501f, 2.3055f, 1.9767f);      

    angle += 0.1f; 

    glm::mat4 birdModel = glm::mat4(1.0f);

    birdModel = glm::translate(birdModel, spinAroundPoint);

    birdModel = glm::rotate(birdModel, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

    birdModel = glm::translate(birdModel, birdInitialPosition - spinAroundPoint);

    shader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(birdModel));

    bird.Draw(shader);

    model = glm::mat4(1.0f);
    glDepthMask(GL_FALSE);
    skyboxShader.useShaderProgram();
    mySkyBox.Draw(skyboxShader, view, projection);
    glDepthMask(GL_TRUE);
}


void renderScene() {

    depthMapShader.useShaderProgram();
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

    glClear(GL_DEPTH_BUFFER_BIT);

    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
        1,
        GL_FALSE,
        glm::value_ptr(computeLightSpaceTrMatrix()));


    renderTeapot(depthMapShader, true);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // final scene rendering pass (with shadows)
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myBasicShader.useShaderProgram();
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

    //bind the shadow map
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
        1,
        GL_FALSE,
        glm::value_ptr(computeLightSpaceTrMatrix()));

    renderTeapot(myBasicShader, false);

    //draw a white cube around the light
    lightShader.useShaderProgram();

    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    model = lightRotation;
    model = glm::translate(model, 1.0f * lightDir);
    model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    lightCube.Draw(lightShader);

    skyboxShader.useShaderProgram();
    mySkyBox.Draw(skyboxShader, view, projection);


    if (isPreviewActive) {
        angleForRotation += 0.5f;
        myCamera.scenePreview(angleForRotation);
    }
}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char * argv[]) {
    PlaySoundA("amb.wav", nullptr, SND_ASYNC | SND_FILENAME);

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    initFBO();

    initOpenGLState();
	initModels();
	initShaders();
	initUniforms();


    if (!isDay) {
        initSkyBox(); // Load the day skybox
    }
    else {
        initSkyBoxDark(); // Load the night skybox
    }


	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
	    renderScene();

		glfwPollEvents();

        setWindowCallbacks();

		glfwSwapBuffers(myWindow.getWindow());


		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
