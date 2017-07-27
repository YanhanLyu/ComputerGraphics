
/* Author: Yanhan Lyu
    clang 560mainSpot.c -lglfw -framework OpenGL
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <GLFW/glfw3.h>

#include "500shader.c"
#include "530vector.c"
#include "510mesh.c"
#include "520matrix.c"
#include "520camera.c"
#include "540texture.c"
#include "540scene.c"
#include "560light.c"

GLdouble alpha = 0.0;
GLuint program;
GLint attrLocs[3];
GLint viewingLoc, modelingLoc;
GLint unifLocs[2];
GLint texCoordsLoc[1], textureLoc[1];
GLint lightPosLoc, lightColLoc, lightAttLoc, lightDirLoc, lightCosLoc;
camCamera cam;
lightLight myLight;

/* Allocate three textures, three meshes and three scene graph nodes. */
texTexture rootTex, childTex, siblingTex;
texTexture *rootTexP[1] = {&rootTex};
texTexture *childTexP[1] = {&childTex};
texTexture *siblingTexP[1] = {&siblingTex};
meshGLMesh rootMesh, childMesh, siblingMesh;
sceneNode rootNode, childNode, siblingNode;

/*declare*/

GLdouble lightCol[3] = {1.0,1.0,1.0};
GLdouble lightDir[3] = {1.0,0.0,0.0};
GLdouble lightCos[3] = {1.0,0.0,0.0};
GLdouble lightPosition[3] = {0.0, 0.0, 0.0};
GLdouble lightAngle = 1;


/*handle if there is error*/
void handleError(int error, const char *description) {
	fprintf(stderr, "handleError: %d\n%s\n", error, description);
}

/* When the window changes shape, we update not just the viewport but also the 
projection, so that our view of the world is not distorted. Because we're using 
fovy (not fovx), the vertical extent of our view is fixed, and the horizontal 
extent changes to compensate. */
void handleResize(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
	camSetWidthHeight(&cam, width, height);
}

/*handle the key to move the camera. This part is credit to Yijun*/
void handleKey(GLFWwindow *window, int key, int scancode, int action,
		int mods) {
	int shiftIsDown = mods & GLFW_MOD_SHIFT;
	int controlIsDown = mods & GLFW_MOD_CONTROL;
	int altOptionIsDown = mods & GLFW_MOD_ALT;
	int superCommandIsDown = mods & GLFW_MOD_SUPER;
	if (action == GLFW_PRESS && key == GLFW_KEY_L) {
		camSwitchProjectionType(&cam);
	} else if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_W)
			camAddTheta(&cam, -0.1);
		else if (key == GLFW_KEY_S)
			camAddTheta(&cam, 0.1);
		else if (key == GLFW_KEY_A)
			camAddPhi(&cam, -0.1);
		else if (key == GLFW_KEY_D)
			camAddPhi(&cam, 0.1);
		else if (key == GLFW_KEY_Q)
			camAddDistance(&cam, -0.1);
		else if (key == GLFW_KEY_E)
			camAddDistance(&cam, 0.1);
	}
}

/* Returns 0 on success, non-zero on failure. Warning: If initialization fails 
midway through, then does not properly deallocate all resources. But that's 
okay, because the program terminates almost immediately after this function 
returns. */
int initializeScene(void) {
    /* Initialize light. */
    lightSetColor(&myLight, lightCol);
    lightSetSpotAngle(&myLight, lightAngle);
    lightSetType(&myLight, lightSPOT);
    lightShineFrom(&myLight, lightPosition, 0.0, 0.0);    
	/* Initialize meshes. */
	meshMesh mesh;
	if (meshInitializeCapsule(&mesh, 0.5, 2.0, 16, 32) != 0)
		return 1;
	meshGLInitialize(&rootMesh, &mesh);
	meshDestroy(&mesh);
	if (meshInitializeBox(&mesh, -0.5, 0.5, -0.5, 0.5, -0.5, 0.5) != 0)
		return 2;
	meshGLInitialize(&childMesh, &mesh);
	meshDestroy(&mesh);
	if (meshInitializeSphere(&mesh, 0.5, 16, 32) != 0)
		return 3;
	meshGLInitialize(&siblingMesh, &mesh);
	meshDestroy(&mesh);
	/* Initialize scene graph nodes. */
	if (sceneInitialize(&siblingNode, 6, 1, &siblingMesh, NULL, NULL) != 0)
		return 4;
	if (sceneInitialize(&childNode, 6, 1, &childMesh, NULL, NULL) != 0)
		return 5;
	if (sceneInitialize(&rootNode, 6, 1, &rootMesh, &childNode, &siblingNode) != 0)
		return 6;
    /* Initialize textures. */
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    if (texInitializeFile(&rootTex, "granite.jpg", GL_LINEAR, GL_LINEAR, 
        GL_CLAMP, GL_CLAMP) != 0) {
        return 7;
    }
    if (texInitializeFile(&childTex, "grass.jpg", GL_LINEAR, GL_LINEAR, 
        GL_CLAMP, GL_CLAMP) != 0) {
        return 8;
    }
    if (texInitializeFile(&siblingTex, "tree.jpeg", GL_LINEAR, GL_LINEAR, 
        GL_CLAMP, GL_CLAMP) != 0) {
        return 9;
    }
	/* Customize the uniforms. */
	GLdouble trans[3] = {1.0, 0.0, 0.0};
	sceneSetTranslation(&childNode, trans);
	vecSet(3, trans, 0.0, 1.0, 0.0);
	sceneSetTranslation(&siblingNode, trans);
	GLdouble unif[6] = {1.0,1.0,1.0, 1.0,1.0,1.0};
	sceneSetTexture(&siblingNode, siblingTexP);
	sceneSetTexture(&childNode, childTexP);
	sceneSetTexture(&rootNode, rootTexP);
	sceneSetUniform(&siblingNode, unif);
	sceneSetUniform(&childNode, unif);
	sceneSetUniform(&rootNode, unif);
	return 0;
}
/*destroy the meshGL, node and tex to free the memory*/
void destroyScene(void) {
	meshGLDestroy(&siblingMesh);
	meshGLDestroy(&childMesh);
	meshGLDestroy(&rootMesh);
	sceneDestroyRecursively(&rootNode);
	texDestroy(&rootTex);
	texDestroy(&childTex);
	texDestroy(&siblingTex);
}

/* Returns 0 on success, non-zero on failure. */
int initializeShaderProgram(void) {
	GLchar vertexCode[] = "\
        uniform mat4 viewing;\
        uniform mat4 modeling;\
        attribute vec3 position;\
        attribute vec2 texCoords;\
        attribute vec3 normal;\
        varying vec3 fragPos;\
        varying vec3 normalDir;\
        varying vec2 st;\
        void main() {\
            vec4 worldPos = modeling * vec4(position, 1.0);\
            gl_Position = viewing * worldPos;\
            fragPos = vec3(worldPos);\
            normalDir = vec3(modeling * vec4(normal, 0.0));\
            st = texCoords;\
        }";
    GLchar fragmentCode[] = "\
        uniform sampler2D texture0;\
        uniform vec3 lightPos;\
        uniform vec3 lightCol;\
        uniform float lightCos;\
        uniform vec3 lightDir;\
        varying vec3 fragPos;\
        varying vec3 normalDir;\
        varying vec2 st;\
        void main() {\
            vec3 surfCol = vec3(texture2D(texture0, st));\
            vec3 spotDir = normalize(lightPos - fragPos);\
            float lamda = dot(spotDir, -lightDir);\
            if (lamda >= lightCos){\
                gl_FragColor = vec4(surfCol*lightCol, 1.0);\
            }else{\
                gl_FragColor = vec4(surfCol*lightCol*0.2, 1.0);\
            }\
        }";
	program = makeProgram(vertexCode, fragmentCode);
	if (program != 0) {
		glUseProgram(program);
		attrLocs[0] = glGetAttribLocation(program, "position");
		attrLocs[1] = glGetAttribLocation(program, "texCoords");
		attrLocs[2] = glGetAttribLocation(program, "normal");

		viewingLoc = glGetUniformLocation(program, "viewing");
		modelingLoc = glGetUniformLocation(program, "modeling");

		lightPosLoc = glGetUniformLocation(program, "lightPos");
		lightColLoc = glGetUniformLocation(program, "lightCol");
        lightDirLoc = glGetUniformLocation(program, "lightDir");
        lightCosLoc = glGetUniformLocation(program, "lightCos");

		textureLoc[0] = glGetUniformLocation(program, "texture0");
	}
	return (program == 0);
}


/*do the rendering part. Update cameraPosLoc here.*/
void render(void) {
	/* This part is the same as in 520mainCamera.c. */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	camRender(&cam, viewingLoc);

    lightRender(&myLight, lightPosLoc, lightColLoc, lightAttLoc, lightDirLoc, lightCosLoc);
    
	GLdouble rot[3][3], identity[4][4], axis[3] = {1.0, 1.0, 1.0};
	vecUnit(3, axis, axis);
	alpha += 0.01;
	mat33AngleAxisRotation(alpha, axis, rot);
	sceneSetRotation(&rootNode, rot);

	mat44Identity(identity);
	GLuint unifDims[2] = {3, 3};
	GLuint attrDims[3] = {3, 2, 3};
	sceneRender(&rootNode, identity, modelingLoc, 3, unifDims, unifLocs, 3, 
		attrDims, attrLocs, textureLoc);
}

/*The main for this demo. Initialize window, set camera and do other initializations.*/
int main(void) {
    glfwSetErrorCallback(handleError);
    if (glfwInit() == 0)
        return 1;
    GLFWwindow *window;
    window = glfwCreateWindow(512, 512, "Scene Graph", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return 2;
    }
    glfwSetWindowSizeCallback(window, handleResize);
    glfwSetKeyCallback(window, handleKey);
    glfwMakeContextCurrent(window);
    fprintf(stderr, "main: OpenGL %s, GLSL %s.\n", 
		glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
    glEnable(GL_DEPTH_TEST);
    glDepthRange(1.0, 0.0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    /* Initialize a whole scene, rather than just one mesh. */
    if (initializeScene() != 0)
    	return 3;
    if (initializeShaderProgram() != 0)
    	return 4;
    GLdouble target[3] = {0.0, 0.0, 0.0};
	camSetControls(&cam, camPERSPECTIVE, M_PI / 6.0, 10.0, 512.0, 512.0, 10.0, 
		M_PI / 4.0, M_PI / 4.0, target);
    while (glfwWindowShouldClose(window) == 0) {
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteProgram(program);
    /* Don't forget to destroy the whole scene. */
    destroyScene();
	glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


