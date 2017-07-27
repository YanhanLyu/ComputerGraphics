/*Author Yanhan Lyu, this is the main for initializing secene, windows and 
the other things that we need to use.*/
#include <stdio.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include  "stdarg.h"
#include "000pixel.h"
#include "100vector.c"
#include "100matrix.c"
#include "040texture.c"
#include "110depth.c"
#include "120renderer.c"

#define renVARYDIMBOUND 16
#define renVERTNUMBOUND 3000

#define renATTRX 0
#define renATTRY 1
#define renATTRZ 2
#define renATTRS 3
#define renATTRT 4
#define renATTRN 5
#define renATTRO 6
#define renATTRP 7

#define renVARYX 0
#define renVARYY 1
#define renVARYZ 2
#define renVARYS 3
#define renVARYT 4


#define renUNIFR 0
#define renUNIFG 1
#define renUNIFB 2
#define renUNIFALPHA 3
#define renUNIFPHI 4
#define renUNIFTHETA 5
#define renUNIFTRANSX 6 
#define renUNIFTRANSY 7
#define renUNIFTRANSZ 8
#define renUNIFISOMETRY 9
#define renUNIFVIEW 25

#define renTEXR 0
#define renTEXG 1
#define renTEXB 2

/* Sets rgb, based on the other parameters, which are unaltered. attr is an 
interpolated attribute vector. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[], 
        double vary[], double rgb[]) {
    texSample(tex[0], vary[renVARYS], vary[renVARYT]);
    rgb[0] = tex[0]->sample[renTEXR];
    rgb[1] = tex[0]->sample[renTEXG];
    rgb[2] = tex[0]->sample[renTEXB];
    rgb[3] = vary[renVARYZ];
}

/* Writes the vary vector, based on the other parameters. */
void transformVertex(renRenderer *ren, double unif[], double attr[], 
        double vary[]) {
    double newAttr[4] = {attr[renATTRX], attr[renATTRY], attr[renATTRZ], 1};
    double xyz[4]; 
    double newXYZ[4];
    mat441Multiply((double(*)[4])(&unif[renUNIFISOMETRY]), newAttr, xyz);
    mat441Multiply((double(*)[4])(&unif[renUNIFVIEW]), xyz, newXYZ);
    vary[renVARYX] = newXYZ[renVARYX];
    vary[renVARYY] = newXYZ[renVARYY];
    vary[renVARYZ] = newXYZ[renVARYZ];
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];
}

/* If unifParent is NULL, then sets the uniform matrix to the 
rotation-translation M described by the other uniforms. If unifParent is not 
NULL, but instead contains a rotation-translation P, then sets the uniform 
matrix to the matrix product P * M. */
void updateUniform(renRenderer *ren, double unif[], double unifParent[]) {
    double axis[3] = {unif[renUNIFALPHA], unif[renUNIFPHI], unif[renUNIFTHETA]};
    double trans[3] = {unif[renUNIFTRANSX], unif[renUNIFTRANSY], unif[renUNIFTRANSZ]};
    vecUnit(3, axis, axis);
    double rotation[3][3];
    mat33AngleAxisRotation(unif[renUNIFALPHA], axis, rotation);
    if (unifParent == NULL) {
        /* The nine uniforms for storing the matrix start at index 
        renUNIFISOMETRY. So &unif[renUNIFISOMETRY] is an array containing those 
        nine numbers. We use '(double(*)[3])' to cast it to a 3x3 matrix. */
        mat44Isometry(rotation, trans, (double(*)[4])(&unif[renUNIFISOMETRY]));
    } else {
        double m[4][4];
        mat44Isometry(rotation, trans, m);
        mat444Multiply((double(*)[4])(&unifParent[renUNIFISOMETRY]), m, (double(*)[4])(&unif[renUNIFISOMETRY]));
    } 
    /*pack the matrix into the unif array*/
    packArray(ren->viewing, &unif[renUNIFVIEW]);
}


#include "110triangle.c"
#include "100mesh.c"
#include "090scene.c"

/*global variables declared to be used*/
double unifA[41] = {1,1,1, 0,0,0,-350,-350,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double unifB[41] = {1,1,1, 0,0,0,0,0,0 ,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
depthBuffer depth;
texTexture textureA, textureB;
texTexture *tex[2];
renRenderer myRenderer = {
    .transformVertex = transformVertex,
    .colorPixel = colorPixel,
    .updateUniform = updateUniform,
    .texNum = 1,
    .varyDim = 5,
    .unifDim = 41,
    .attrDim = 8,
    .depth = &depth
};
renRenderer *ren = &myRenderer;

meshMesh mesher1, mesher2;
meshMesh *mesh1 = &mesher1;
meshMesh *mesh2 = &mesher2;

sceneNode nodeA, nodeB;


/*draw the picture*/
void draw(void){
    pixClearRGB(0.0, 0.0, 0.0);
    depthClearZs(myRenderer.depth, -100);
    sceneRender(&nodeA, &myRenderer, NULL);
    double position[3] = {1,1,1};
    /*use the renLookFrom*/
    renLookFrom(ren, position, nodeA.unif[renUNIFPHI],nodeA.unif[renUNIFTHETA]);
    renUpdateViewing(ren);
}  

/*handle the time step*/
void handleTimeStep(double oldTime, double newTime) {
    sceneSetOneUniform(&nodeA, renUNIFALPHA, 0.02);
    draw();   
}

/*handle the key. L can increase renUNIFTHETA, R can decrease renUNIFTHETA, 
U can decrease renUNIFPHI, and D can increase renUNIFPHI*/
void handleKeyUp(int key, int shiftIsDown, int controlIsDown,
        int altOptionIsDown, int superCommandIsDown) {
    switch(key) {
        case GLFW_KEY_D :
            sceneSetOneUniform(&nodeA, renUNIFTHETA, -0.02);
            break;
        case GLFW_KEY_A :
            sceneSetOneUniform(&nodeA, renUNIFTHETA, 0.02);
            break;
        case GLFW_KEY_S :
            sceneSetOneUniform(&nodeA, renUNIFPHI, 0.02);
            break;
        case GLFW_KEY_W :
            sceneSetOneUniform(&nodeA, renUNIFPHI, -0.02);
            break;
    }
    draw();
    
}


/* This functions draws the window with the given picture. */
int main(void) {
    printf("Use W,A,S,D to control the object.\n");
    tex[0] = &textureA;
    tex[1] = &textureB;
    if (pixInitialize(512, 512, "Pixel Graphics") != 0) {
        return 1;
    } else if (meshInitializeBox(mesh1, 0, 200, 0, 200, 0, 200)){
        return 2;
    } else if (meshInitializeSphere(mesh2, 75, 10, 20)){
        return 3;
    }else if (texInitializeFile(&textureA, "A.jpg") != 0) {
        return 4;
    }else if (texInitializeFile(&textureB, "C.jpeg") != 0) {
        return 5;
    } else if (sceneInitialize(&nodeA, ren, unifA, &tex[0], mesh1, &nodeB, NULL) != 0){
        return 6;
    }else if (sceneInitialize(&nodeB, ren, unifB, &tex[1], mesh2, NULL, NULL) != 0){
        return 7;
    } else if (depthInitialize(&depth, 512, 512)){
        return 8;
    }else {
        draw();
        pixSetTimeStepHandler(handleTimeStep);
        pixSetKeyUpHandler(handleKeyUp);
        pixRun();
        texDestroy(tex[0]);
        texDestroy(tex[1]);
        meshDestroy(mesh1);
        meshDestroy(mesh2);
        sceneDestroy(&nodeA);
        sceneDestroy(&nodeB);
        depthDestroy(&depth);
        return 0;
    }   
}