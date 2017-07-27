/*We rewrite 02CSGdemo.c in GLFW but failed. It seems that even the depth test does not work correctly.
*compile command:  clang 03CSGdemo.c 000pixel.o -lglfw -framework OpenGL
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
#include <GLUT/glut.h>

/* Set up globals for the mesh. */
GLdouble alpha = 0.0;
GLuint program;
GLint attrLocs[3];
GLint viewingLoc, modelingLoc;
GLint unifLocs[1];
camCamera cam;
/* Allocate three meshes and three scene graph nodes. */
meshMesh mesh;
meshGLMesh box1Mesh, box2Mesh, sphereMesh, capsuleMesh;
sceneNode box1Node, box2Node, sphereNode, capsuleNode;
/* Initialize window. */
GLFWwindow *window;

int whichTree = 0;
int whereSoFar;
int whereToStop = -1;
typedef enum {CONTINUE, STOP} progressEnum;
int stenSize = 8;

int numPrims = 4;
int curPrim = 0;

int winWidth,winHeight;
GLfloat 	*depthSave = NULL;
GLubyte 	*stencilSave = NULL;
GLubyte 	*colorSave = NULL;
enum {COLOR, DEPTH, STENCILVALUES, STENCILPLANES} bufferInterest = COLOR;

GLfloat *depthResults = NULL;

void renderSphere(void);
void renderCapsule(void);
void renderBox1(void);
void renderBox2(void);

void resizeBuffers(void)
{
    if(colorSave != NULL)
        free(colorSave);
    colorSave = malloc(winWidth * winHeight * 4 * sizeof(GLubyte));
    if(depthSave != NULL)
        free(depthSave);
    depthSave = malloc(winWidth * winHeight * sizeof(GLfloat));
    stencilSave = (GLubyte *)depthSave;
}

struct primitive {
    void		(*draw)(void);
};

struct primitive prims[20] = {
    renderBox1, renderSphere, renderBox2, renderCapsule
};

void drawPrim(int i)
{
    struct primitive *p = &prims[i];
    glPushMatrix();
    p->draw();
    glPopMatrix();
}


struct product {
    int targetPrim;
    int frontFace;
    int whichSurface;
    int numTrimPrims;
    int *trimmingPrims;
    int *isComplemented;
};


int prodAtrimmingPrims1[] = {1};
int prodAisComplemented1[] = {1};
int prodBtrimmingPrims1[] = {0};
int prodBisComplemented1[] = {0};


int prodAtrimmingPrims2[] = {1};
int prodAisComplemented2[] = {0};
int prodBtrimmingPrims2[] = {0};
int prodBisComplemented2[] = {1};


int prodAtrimmingPrims3[] = {1};
int prodAisComplemented3[] = {0};
int prodBtrimmingPrims3[] = {0};
int prodBisComplemented3[] = {0};


int prodAtrimmingPrims4[] = {1, 2, 3};
int prodAisComplemented4[] = {0, 1, 1};
int prodBtrimmingPrims4[] = {1, 0, 2};
int prodBisComplemented4[] = {0, 0, 1};
int prodCtrimmingPrims4[] = {1, 0, 3};
int prodCisComplemented4[] = {0, 0, 1};
int prodDtrimmingPrims4[] = {0, 2, 3};
int prodDisComplemented4[] = {0, 1, 1};


struct product products[][4] = {
    {	/* A - B */
	{
	    0, 1, 0,
	    1,
	    prodAtrimmingPrims1,
	    prodAisComplemented1,
	},
	{
	    1, 0, 0,
	    1,
	    prodBtrimmingPrims1,
	    prodBisComplemented1,
	},
    },

    {    /* B - A */
	{
	    0, 0, 0,
	    1,
	    prodAtrimmingPrims2,
	    prodAisComplemented2,
	},
	{
	    1, 1, 0,
	    1,
	    prodBtrimmingPrims2,
	    prodBisComplemented2,
	},
    },

    {    /* A and B */
	{
	    0, 1, 0,
	    1,
	    prodAtrimmingPrims3,
	    prodAisComplemented3,
	},
	{
	    1, 1, 0,
	    1,
	    prodBtrimmingPrims3,
	    prodBisComplemented3,
	},
    },

    { /* A and B - D - C */
	{
	    0, 1, 0,
	    3,
	    prodAtrimmingPrims4,
	    prodAisComplemented4,
	},
	{
	    3, 0, 0,
	    3,
	    prodBtrimmingPrims4,
	    prodBisComplemented4,
	},
	{
	    2, 0, 0,
	    3,
	    prodCtrimmingPrims4,
	    prodCisComplemented4,
	},
	{
	    1, 1, 0,
	    3,
	    prodDtrimmingPrims4,
	    prodDisComplemented4,
	},
    },
};


int numProducts[4] = {2, 2, 2, 4};

void pushOrthoView(float left, float right, float bottom, float top,
    float znear, float zfar)
{
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(left, right, bottom, top, znear, zfar);
}


void popView(void)
{
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

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

void drawNoCSG(void)
{
    int i;

    glDisable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    for(i = 0; i < numPrims; i++)
    {
	    drawPrim(i);
    }

    glPopMatrix();
}

int whereSoFar;

int sCountMask = 0x01;	/* 1-convexity maximum */
int sCountShift = 0;
int sPMask = 1;
int sPShift = 0;


void drawFarRect(void)
{
    pushOrthoView(0, 1, 0, 1, 0, 1);

    /* Can I just draw & let be clipped? */
    glBegin(GL_QUADS);
    glVertex3f(0, 0, -1);
    glVertex3f(1, 0, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(0, 1, -1);
    glEnd();

    popView();
}


progressEnum renderPrimDepths(int targetPrim, int frontFace, int whichSurface)
{
    glClear(GL_DEPTH_BUFFER_BIT);


    glDepthFunc(GL_ALWAYS);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glStencilMask(sCountMask);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glStencilFunc(GL_ALWAYS, 0, 0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    drawFarRect();

    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    if(frontFace)
        glCullFace(GL_BACK);
    else
        glCullFace(GL_FRONT);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glStencilFunc(GL_EQUAL, whichSurface, sCountMask);
    glStencilOp(GL_INCR, GL_INCR, GL_INCR);

    drawPrim(targetPrim);

    return(CONTINUE);
}

progressEnum trimWithPrimitive(int trimPrim, int isComplemented)
{
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glDepthMask(GL_FALSE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glStencilMask(sPMask);
    glDisable(GL_CULL_FACE);

    glDisable(GL_DEPTH_TEST);

    glStencilFunc(GL_ALWAYS, isComplemented ? (1 << sPShift) : 0, 0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    drawFarRect();

    glClearStencil(isComplemented ? (1 << sPShift) : 0);
    glClear(GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);
    drawPrim(trimPrim);

    /* stencil == 0 where pixels were not inside */
    /* so now set Z to far where stencil == 0, everywhere pixels trimmed */

    glStencilFunc(GL_EQUAL, 0, sPMask);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glDepthMask(1);
    glDepthFunc(GL_ALWAYS);
    glDisable(GL_LIGHTING);
    drawFarRect();
    glEnable(GL_LIGHTING);

    return(CONTINUE);
}


progressEnum markProductPixels(int product, int accumBit)
{
    int i;

    struct product *p;

    p = &products[whichTree][product];

    if(renderPrimDepths(p->targetPrim, p->frontFace, p->whichSurface) == STOP)
        return(STOP);

    for(i = 0; i < p->numTrimPrims; i++)
    {
        if(trimWithPrimitive(p->trimmingPrims[i], p->isComplemented[i]) == STOP)
	    return(STOP);
    }

    /* set accumulator stencil bit for this primitive everywhere depth != far */
    glStencilFunc(GL_ALWAYS, 1 << accumBit, 0);
    glStencilOp(GL_KEEP, GL_ZERO, GL_REPLACE);
    glStencilMask(1 << accumBit);
    glDepthMask(0);
    glDepthFunc(GL_GREATER);
    glDisable(GL_LIGHTING);
    drawFarRect();
    glEnable(GL_LIGHTING);

    return(CONTINUE);
}

progressEnum drawProduct(int product, int accumBit)
{
    struct product *p;
    p = &products[whichTree][product];

    glEnable(GL_CULL_FACE);
    if(p->frontFace)
        glCullFace(GL_BACK);
    else
        glCullFace(GL_FRONT);

    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL, 1 << accumBit, 1 << accumBit);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    drawPrim(p->targetPrim);

    return(CONTINUE);
}


void drawCSG(void)
{
    int i;
    int accumBit;
    int firstProduct;
    int lastProduct;

    whereSoFar = 0;
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); 
    glClear(GL_COLOR_BUFFER_BIT);

    /* Only have to do this if you're going to look at the stencil buffer */
    glClearStencil(0);
    glStencilMask((1 << stenSize) - 1);
    glClear(GL_STENCIL_BUFFER_BIT);

    glPushMatrix();

    firstProduct = 0;

    while(firstProduct != numProducts[whichTree]) {   
        /*
         * set lastProduct so that accum bits for first to last fit in
         * stencil buffer minus bits needed for surface counting bits
         */
        //printf("Test: first is %i, num is %i\n", firstProduct, numProducts[whichTree]);
        lastProduct = firstProduct + (stenSize - 1) - 1;
        //printf("Before: first is %i, last is %i\n", firstProduct, lastProduct);
        if(lastProduct >= numProducts[whichTree]) {
            lastProduct = numProducts[whichTree] - 1;
        }

        accumBit = 1;		/* first available after counting bits */

        for(i = firstProduct; i <= lastProduct; i++) {
            if(markProductPixels(i, accumBit++) == STOP) {
                goto doneWithFrame;
            }
            printf("Mark: product, accumbit: %i, %i\n", i, accumBit);
            //printf("Inside: first is %i, last is %i\n", firstProduct, lastProduct);
        }

        glDepthMask(GL_TRUE);
        glClear(GL_DEPTH_BUFFER_BIT);

        accumBit = 1;		/* first available after counting bits */
    
        for(i = firstProduct; i <= lastProduct; i++) {
            if(drawProduct(i, accumBit++) == STOP) {
                goto doneWithFrame;
            }
            printf("Draw: product, accumbit: %i, %i\n", i, accumBit);
        }
        firstProduct = lastProduct + 1;
        //printf("After: first is %i, last is %i\n", firstProduct, lastProduct);
    }

    glDisable(GL_STENCIL_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    for(i = 0; i < numProducts[whichTree]; i++)
    {
        struct product *p;
        p = &products[whichTree][i];
        printf("Num is: %i, %i, %i, %i; i is: %i\n", p->targetPrim, p->frontFace, 
            p->whichSurface, p->numTrimPrims, i);
        drawPrim(p->targetPrim);
    }
    glDisable(GL_BLEND);

    doneWithFrame:
    glPopMatrix();
}


/* This function initializes a sphere mesh centered at the origin. */
int initializeSphere(void){
    if (meshInitializeSphere(&mesh, 1.3, 16, 32) != 0)
        return 1;
    meshGLInitialize(&sphereMesh, &mesh);
    meshDestroy(&mesh);
    if (sceneInitialize(&sphereNode, 2, &sphereMesh, NULL, NULL) != 0)
        return 2;
    GLdouble trans[3] = {1.0, 1.0, 0.0};
    sceneSetTranslation(&sphereNode, trans);
    GLdouble unif[2] = {1.0, 1.0};
    sceneSetUniform(&sphereNode, unif);
    return 0;
}

int initializeCapsule(void){
    if (meshInitializeCapsule(&mesh, 1.0, 4.0, 16, 32) != 0)
        return 1;
    meshGLInitialize(&capsuleMesh, &mesh);
    meshDestroy(&mesh);
    if (sceneInitialize(&capsuleNode, 2, &capsuleMesh, NULL, NULL) != 0)
        return 2;
    GLdouble trans[3] = {0.2, 0.9, 1.0};
    sceneSetTranslation(&capsuleNode, trans);
    GLdouble unif[2] = {1.0, 1.0};
    sceneSetUniform(&capsuleNode, unif);
    return 0;
}

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

/* This function initializes a box mesh centered at the origin. */
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

/* This function renders the sphereNode. */
void renderSphere(void) {
    glPushMatrix();
    glUseProgram(program);
    camRender(&cam, viewingLoc);   
    /* This rendering code is different from that in 520mainCamera.c. */
    GLdouble identity[4][4];
    mat44Identity(identity);
    GLuint unifDims[1] = {2};
    GLuint attrDims[3] = {3, 2, 3};
    sceneRender(&sphereNode, identity, modelingLoc, 1, unifDims, unifLocs, 3, 
        attrDims, attrLocs);
    glPopMatrix();
}

void renderCapsule(void) {
    glPushMatrix();
    glUseProgram(program);
    camRender(&cam, viewingLoc);   
    /* This rendering code is different from that in 520mainCamera.c. */
    GLdouble identity[4][4];
    mat44Identity(identity);
    GLuint unifDims[1] = {2};
    GLuint attrDims[3] = {3, 2, 3};
    sceneRender(&capsuleNode, identity, modelingLoc, 1, unifDims, unifLocs, 3, 
        attrDims, attrLocs);
    glPopMatrix();
}

/* This function renders the boxNode. */
void renderBox1(void) {
    glPushMatrix();
    glUseProgram(program);
    camRender(&cam, viewingLoc);
    /* This rendering code is different from that in 520mainCamera.c. */
    GLdouble identity[4][4];
    mat44Identity(identity);
    GLuint unifDims[1] = {2};
    GLuint attrDims[3] = {3, 2, 3};
    sceneRender(&box1Node, identity, modelingLoc, 1, unifDims, unifLocs, 3, 
        attrDims, attrLocs);
    glPopMatrix();
}

/* This function renders the boxNode. */
void renderBox2(void) {
    glPushMatrix();
    glUseProgram(program);
    camRender(&cam, viewingLoc);
    /* This rendering code is different from that in 520mainCamera.c. */
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

void handleKey(GLFWwindow *window, int key, int scancode, int action,
        int mods) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glPushMatrix();

    if (key == GLFW_KEY_Z){
        drawNoCSG();
	}
	
	switch(key)
    {
        case '1':
        case '2':
        case '3':
        case '4':
	    whichTree = key - '1';
	    break;
	}
	
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_D)
			camAddTheta(&cam, -0.1);
		else if (key == GLFW_KEY_A)
			camAddTheta(&cam, 0.1);
		else if (key == GLFW_KEY_S)
			camAddPhi(&cam, -0.1);
		else if (key == GLFW_KEY_W)
			camAddPhi(&cam, 0.1);
	}
	
	drawCSG();
	glPopMatrix();
    glfwSwapBuffers(window);
}

void destroyScene(void) {
    meshGLDestroy(&sphereMesh);
    meshGLDestroy(&capsuleMesh);
    meshGLDestroy(&box1Mesh);
    meshGLDestroy(&box2Mesh);
    sceneDestroy(&box1Node);
    sceneDestroy(&box2Node);
    sceneDestroy(&capsuleNode);
    sceneDestroy(&sphereNode);

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

int main(void) {
	winWidth = 512;
	winHeight = 512;
	
    glfwSetErrorCallback(handleError);
    if (glfwInit() == 0)
        return 1;
    window = glfwCreateWindow(winWidth,winHeight, "Scene Graph", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return 2;
    }
    glfwSetWindowSizeCallback(window, handleResize);
    glfwMakeContextCurrent(window);
    fprintf(stderr, "main: OpenGL %s, GLSL %s.\n", 
        glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
    /* Initialize a whole scene, rather than just one mesh. */
    init();
    if (initializeSphere() != 0)
        return 3;
    if (initializeCapsule() != 0)
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

    glfwSetKeyCallback(window, handleKey);
    while (glfwWindowShouldClose(window) == 0) {
        glfwPollEvents();
    }

    glDeleteProgram(program);
    /* Don't forget to destroy the whole scene. */
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}