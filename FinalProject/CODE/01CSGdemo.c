/* Yanhan Lyu and Yijun Wang and original author Brad Grantham.
   This file is a two object demo of rendering CSG models. 
   Intersection(and), Union(or) and Subtraction(sub) are
   allowed here. Only operations between two primitives
   are possible. Two boxes and two spheres are defined
   as primitives in this file. 
   complie command: clang 01CSGdemo.c 000pixel.o -lglfw -framework OpenGL
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <GLFW/glfw3.h>

#include "500shader.c"
#include "510vector.c"
#include "510mesh.c"
#include "520matrix.c"
#include "520camera.c"
#include "530scene.c"

GLdouble alpha = 0.0;
GLuint program;
GLint attrLocs[3];
GLint viewingLoc, modelingLoc;
GLint unifLocs[1];
camCamera cam;
/* Allocate four OpenGL meshes and four scene graph nodes.
   Initialize GLFW window. */
meshMesh mesh;
meshGLMesh box1Mesh, box2Mesh, sphere1Mesh, sphere2Mesh;
sceneNode box1Node, box2Node, sphere1Node, sphere2Node;
GLFWwindow *window;

/* Initialize four pointer to the render functions. */
void (*A)(void);
void (*B)(void);
void (*C)(void);
void (*D)(void);

/* functions */

/* one()
 * draw a single object
 */
void one(void(*a)(void))
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    a();
    glDisable(GL_DEPTH_TEST);
}

/* or()
 * draw the union of two objects.
 * Just draw the objects in a sequence.
 */
void or(void(*a)(void), void(*b)())
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glPushAttrib(GL_ALL_ATTRIB_BITS);  /* TODO - should just push depth */
    glEnable(GL_DEPTH_TEST);
    a(); 
    b();
    glPopAttrib();
}

/* multOr()
 * Following the logic of or(), this function renders the union of multiple objects,
 * by just drawing them consecutively.
 * The length of the objects to be drawn and a list of objects is required.
 */
void multOr(int num, void(*a[])(void))
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glPushAttrib(GL_ALL_ATTRIB_BITS);  /* TODO - should just push depth */
    glEnable(GL_DEPTH_TEST);
	int i;
	for (i = 0; i < num; i ++) {
	    a[i]();
	}
	glPopAttrib();    
}

/* inside()
 * sets stencil buffer to show the part of A
 * (front or back face according to 'face')
 * that is inside of B.
 */
void inside(void(*a)(void), void(*b)(void), GLenum face, GLenum test)
{
    /* draw A into depth buffer, but not into color buffer */
    glEnable(GL_DEPTH_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glCullFace(face);
    a();
    
    /* use stencil buffer to find the parts of A that are inside of B
     * by first incrementing the stencil buffer wherever B's front faces
     * are...
     */
    glDepthMask(GL_FALSE);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
    glCullFace(GL_BACK);
    b();

    /* decrement the stencil buffer wherever B's back faces are */
    glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
    glCullFace(GL_FRONT);
    b();

    /* now draw the part of A that is inside of B */
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(test, 0, 1);
    glDisable(GL_DEPTH_TEST);
    glCullFace(face);
    a();

    /* reset stencil test */
    glDisable(GL_STENCIL_TEST);
}

/* Helper function to fix up the depth buffer after calling inside(). */
void fixup(void(*a)(void))
{
    /* fix up the depth buffer */
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDepthFunc(GL_ALWAYS);
    a();
    
    /* reset depth func */
    glDepthFunc(GL_LESS);
}

/* and()
 * boolean A and B (draw wherever A intersects B)
 * algorithm: find where A is inside B, then find where
 *            B is inside A
 */
void and(void(*a)(void), void(*b)(void))
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    inside(a, b, GL_BACK, GL_NOTEQUAL);
    fixup(b);
    inside(b, a, GL_BACK, GL_NOTEQUAL);
}

/* sub()
 * boolean A subtract B (draw wherever A is and B is NOT)
 * algorithm: find where a is inside B, then find where
 *            the BACK faces of B are NOT in A
 */
void sub(void(*a)(void), void(*b)(void))
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    inside(a, b, GL_FRONT, GL_NOTEQUAL);
    fixup(b);
    inside(b, a, GL_BACK, GL_EQUAL);
}


/* !!!Now we have 2 spheres and 2 boxes which only have different translation. */
/* This function initializes a sphere mesh centered at the origin. */
int initializeSphere2(void){
    if (meshInitializeSphere(&mesh, 1.3, 16, 32) != 0)
        return 1;
    meshGLInitialize(&sphere2Mesh, &mesh);
    meshDestroy(&mesh);
    if (sceneInitialize(&sphere2Node, 2, &sphere2Mesh, NULL, NULL) != 0)
        return 2;
    GLdouble trans[3] = {0.0, 0.0, 0.0};
    sceneSetTranslation(&sphere2Node, trans);
    GLdouble unif[2] = {1.0, 1.0};
    sceneSetUniform(&sphere2Node, unif);
    return 0;
}

/* This function initializes a sphere mesh. */
int initializeSphere1(void){
    if (meshInitializeSphere(&mesh, 1.3, 16, 32) != 0)
        return 1;
    meshGLInitialize(&sphere1Mesh, &mesh);
    meshDestroy(&mesh);
    if (sceneInitialize(&sphere1Node, 2, &sphere1Mesh, NULL, NULL) != 0)
        return 2;
    GLdouble trans[3] = {1.0, 1.0, 0.0};
    sceneSetTranslation(&sphere1Node, trans);
    GLdouble unif[2] = {1.0, 1.0};
    sceneSetUniform(&sphere1Node, unif);
    return 0;
}

/* This function initializes a box mesh centered at the origin. */
int initializeBox1(void){
    if (meshInitializeBox(&mesh, -1, 1, -1, 1, -1, 1) != 0)
        return 3;
    meshGLInitialize(&box1Mesh, &mesh);
    meshDestroy(&mesh);
    if (sceneInitialize(&box1Node, 2, &box1Mesh, NULL, NULL) != 0)
        return 4;
    GLdouble trans[3] = {0.0, 0.0, 0.0};
    sceneSetTranslation(&box1Node, trans);
    GLdouble unif[2] = {1.0, 1.0};
    sceneSetUniform(&box1Node, unif);
    return 0;
}

/* This function initializes a box mesh. */
int initializeBox2(void){
    if (meshInitializeBox(&mesh, -1, 1, -1, 1, -1, 1) != 0)
        return 3;
    meshGLInitialize(&box2Mesh, &mesh);
    meshDestroy(&mesh);
    if (sceneInitialize(&box2Node, 2, &box2Mesh, NULL, NULL) != 0)
        return 4;
    GLdouble trans[3] = {1.0, 1.0, 0.0};
    sceneSetTranslation(&box2Node, trans);
    GLdouble unif[2] = {1.0, 1.0};
    sceneSetUniform(&box2Node, unif);
    return 0;
}

/* This function renders the sphere1Node. */
void renderSphere1(void) {
    glPushMatrix();
    glUseProgram(program);
    camRender(&cam, viewingLoc);   
    GLdouble identity[4][4];
    mat44Identity(identity);
    GLuint unifDims[1] = {2};
    GLuint attrDims[3] = {3, 2, 3};
    sceneRender(&sphere1Node, identity, modelingLoc, 1, unifDims, unifLocs, 3, 
        attrDims, attrLocs);
    glPopMatrix();
}

/* This function renders the sphere2Node. */
void renderSphere2(void) {
    glPushMatrix();
    glUseProgram(program);
    camRender(&cam, viewingLoc);   
    GLdouble identity[4][4];
    mat44Identity(identity);
    GLuint unifDims[1] = {2};
    GLuint attrDims[3] = {3, 2, 3};
    sceneRender(&sphere2Node, identity, modelingLoc, 1, unifDims, unifLocs, 3, 
        attrDims, attrLocs);
    glPopMatrix();
}

/* This function renders the box1Node. */
void renderBox1(void) {
    glPushMatrix();
    glUseProgram(program);
    camRender(&cam, viewingLoc);
    GLdouble identity[4][4];
    mat44Identity(identity);
    GLuint unifDims[1] = {2};
    GLuint attrDims[3] = {3, 2, 3};
    sceneRender(&box1Node, identity, modelingLoc, 1, unifDims, unifLocs, 3, 
        attrDims, attrLocs);
    glPopMatrix();
}

/* This function renders the box2Node. */
void renderBox2(void) {
    glPushMatrix();
    glUseProgram(program);
    camRender(&cam, viewingLoc);
    GLdouble identity[4][4];
    mat44Identity(identity);
    GLuint unifDims[1] = {2};
    GLuint attrDims[3] = {3, 2, 3};
    sceneRender(&box2Node, identity, modelingLoc, 1, unifDims, unifLocs, 3, 
        attrDims, attrLocs);
    glPopMatrix();
}

void handleError(int error, const char *description) {
    fprintf(stderr, "handleError: %d\n%s\n", error, description);
}

/* This function doesn't work right now because it doesn't draw anything. */
void handleResize(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    camSetWidthHeight(&cam, width, height);
}

/*  Z, X, C, V for drawing one A, B, C, D node;
    U, I, O, P for or(A,B), or(C,B), sub(C,B), sub(A,B);
    0 for multOr(4, {A, D, B, C});
    1, 2 for and(A,C), and(A,B);
    W, S, A, D for rotation */
void handleKey(GLFWwindow *window, int key, int scancode, int action,
        int mods) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glPushMatrix();

    if (key == GLFW_KEY_Z){
        one(A);
    } else if (key == GLFW_KEY_X){
        one(B);
    } else if (key == GLFW_KEY_C){
        one(C);
    } else if (key == GLFW_KEY_V){
        one(D);
    } else if (key == GLFW_KEY_U){
        or(A,B);
    } else if (key == GLFW_KEY_I){
        or(C,B);
    } else if (key == GLFW_KEY_O){
        sub(C,B);
    } else if (key == GLFW_KEY_P){
        sub(A,B);
    } else if (key == GLFW_KEY_0){
        void(*a[4])(void) = {A, D, B, C};
        multOr(4, a);
    } else if (key == GLFW_KEY_1){
        and(A,C);
    } else if (key == GLFW_KEY_2){
        and(A,B);
    } else if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_D)
			camAddTheta(&cam, -0.1);
		else if (key == GLFW_KEY_A)
			camAddTheta(&cam, 0.1);
		else if (key == GLFW_KEY_S)
			camAddPhi(&cam, -0.1);
		else if (key == GLFW_KEY_W)
			camAddPhi(&cam, 0.1);
	}
	glPopMatrix();
    glfwSwapBuffers(window);
}

void destroyScene(void) {
    meshGLDestroy(&sphere1Mesh);
    meshGLDestroy(&sphere2Mesh);
    meshGLDestroy(&box1Mesh);
    meshGLDestroy(&box2Mesh);
    sceneDestroy(&box1Node);
    sceneDestroy(&box2Node);
    sceneDestroy(&sphere1Node);
    sceneDestroy(&sphere2Node);

}

/* Returns 0 on success, non-zero on failure. */
int initializeShaderProgram(void) {
    GLchar vertexCode[] = "\
        uniform mat4 viewing;\
        uniform mat4 modeling;\
        attribute vec3 position;\
        attribute vec2 texCoords;\
        attribute vec3 normal;\
        uniform vec2 spice;\
        varying vec4 rgba;\
        void main() {\
            gl_Position = viewing * modeling * vec4(position, 1.0);\
            rgba = vec4(texCoords, spice) + vec4(normal, 1.0);\
        }";
    GLchar fragmentCode[] = "\
        varying vec4 rgba;\
        void main() {\
            gl_FragColor = rgba;\
        }";
    program = makeProgram(vertexCode, fragmentCode);
    if (program != 0) {
        glUseProgram(program);
        attrLocs[0] = glGetAttribLocation(program, "position");
        attrLocs[1] = glGetAttribLocation(program, "texCoords");
        attrLocs[2] = glGetAttribLocation(program, "normal");
        viewingLoc = glGetUniformLocation(program, "viewing");
        modelingLoc = glGetUniformLocation(program, "modeling");
        unifLocs[0] = glGetUniformLocation(program, "spice");
    }
    return (program == 0);
}

/* Enable buffers before drawing. */
void init(void)
{
    glEnable(GL_DEPTH_TEST);
    glDepthRange(1.0, 0.0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0, 0.0, 0.0, 0.0);
}



int main(void) {
    glfwSetErrorCallback(handleError);
    if (glfwInit() == 0)
        return 1;
    window = glfwCreateWindow(512, 512, "Scene Graph", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return 2;
    }
    glfwSetWindowSizeCallback(window, handleResize);
    glfwMakeContextCurrent(window);
    fprintf(stderr, "main: OpenGL %s, GLSL %s.\n", 
        glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    /* Initialize meshes one by one. */
    init();
    if (initializeSphere1() != 0)
        return 3;
    if (initializeSphere2() != 0)
        return 3;
    if (initializeBox1() != 0)
        return 5;
    if (initializeBox2() != 0)
        return 5;
    if (initializeShaderProgram() != 0)
        return 4;
    GLdouble target[3] = {0.0, 0.0, 0.0};
    camSetControls(&cam, camPERSPECTIVE, M_PI / 5.0, 10.0, 512.0, 512.0, 10.0, 
        M_PI / 5.0, M_PI /5.0, target);
    /* Set the rendering function to the pointer. */
    A = renderSphere1;
    B = renderBox1;
    C = renderSphere2;
    D = renderBox2;
    glfwSetKeyCallback(window, handleKey);
    init();
    while (glfwWindowShouldClose(window) == 0) { 
        glfwPollEvents();
    }
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

