/*Author Yanhan Lyu, this is the main for initializing secene, windows and 
the other things that we need to use.
*/
#include <stdio.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include  "stdarg.h"
#include "000pixel.h"
#include "100vector.c"
#include "130matrix.c"
#include "040texture.c"
#include "110depth.c"
#include "130renderer.c"

#define renVARYDIMBOUND 16
#define renVERTNUMBOUND 5000

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
#define renVARYW 3
#define renVARYS 4
#define renVARYT 5


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
    double newAttr[4] = {attr[renVARYX], attr[renVARYY], attr[renVARYZ], 1};
    double xyz[4]; 
    double eyeXYZ[4];
    double view[4];
    mat441Multiply((double(*)[4])(&unif[renUNIFISOMETRY]), newAttr, xyz);
    //printf("wordCoordinates, x, %f, y, %f, z %f, ", xyz[0], xyz[1], xyz[2]);
    mat441Multiply(ren->viewing, xyz, eyeXYZ);
    //printf("eyeCoordinates, x, %f, y, %f, z %f, w %f\n", eyeXYZ[0], eyeXYZ[1], eyeXYZ[2],eyeXYZ[3]);
    //vecScale(4, 1/eyeXYZ[3],eyeXYZ, eyeXYZ);
    //mat441Multiply(ren->viewport, eyeXYZ, view);
    //printf("view, %f, %f, %f, %f\n", view[0],view[1],view[2],view[3]);
    vary[renVARYX] = eyeXYZ[renVARYX];
    vary[renVARYY] = eyeXYZ[renVARYY];
    vary[renVARYZ] = eyeXYZ[renVARYZ];
    vary[renVARYW] = eyeXYZ[renVARYW];
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];
    //printf("%f, %f", attr[renATTRS], attr[renATTRT]);

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
#include "140clipping.c"
#include "140mesh.c"
#include "090scene.c"

/*global variables declared to be used*/
double unifA[41] = {1,1,1, 0,0,0,0,2,2,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double unifB[41] = {1,1,1, 0,0,0,0,0,0 ,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
depthBuffer depth;
texTexture textureA, textureB;
texTexture *tex[2];
renRenderer myRenderer = {
    .transformVertex = transformVertex,
    .colorPixel = colorPixel,
    .updateUniform = updateUniform,
    .texNum = 1,
    .varyDim = 6,
    .unifDim = 41,
    .attrDim = 8,
    .depth = &depth,
};

renRenderer *ren = &myRenderer;


meshMesh mesher1, mesher2;
meshMesh *mesh1 = &mesher1;
meshMesh *mesh2 = &mesher2;

sceneNode nodeA, nodeB;

double distance = 20.0;


/*draw the picture*/
void draw(void){
    pixClearRGB(0.0, 0.0, 0.0);
    depthClearZs(myRenderer.depth, -10000);
    sceneRender(&nodeA, &myRenderer, NULL);
    double target[3] = {0,0,0};
    /*use the renLookFrom*/
    renLookAt(ren, target, distance,nodeA.unif[renUNIFPHI],nodeA.unif[renUNIFTHETA]);
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
        case GLFW_KEY_P :
            distance += 1;
            break;
        case GLFW_KEY_M:
            distance -= 1;
            break;
        case GLFW_KEY_O:
            renSetFrustum(ren, renORTHOGRAPHIC, M_PI/3.0, 10.0, 10.0);
            break;
        case GLFW_KEY_ENTER:
            renSetFrustum(ren, renPERSPECTIVE, M_PI/3.0, 10.0, 10.0);
            break;
    }
    draw();
}


/* This functions draws the window with the given picture. */
int main(void) {
    printf("Use W,A,S,D to control the object, and P,M to set the distance.\n");
    tex[0] = &textureA;
    tex[1] = &textureB;
    
    if (pixInitialize(512, 512, "Pixel Graphics") != 0) {
        return 1;
    } else if (meshInitializeBox(mesh1, 0, 3, 0, 3, 0, 3)){
        return 2;
    } else if (meshInitializeSphere(mesh2, 2, 10, 20)){
       return 3;
    }else if (texInitializeFile(&textureA, "A.jpg") != 0) {
        return 4;
    }else if (texInitializeFile(&textureB, "A.jpg") != 0) {
        return 5;
    } else if (sceneInitialize(&nodeA, ren, unifA, &tex[0], mesh1, &nodeB, NULL) != 0){
        return 6;
    }else if (sceneInitialize(&nodeB, ren, unifB, &tex[1], mesh2, NULL, NULL) != 0){
        return 7;
    } else if (depthInitialize(&depth, 512, 512)){
        return 8;
    }else {
        renSetFrustum(ren, renPERSPECTIVE, M_PI/3.0, 10.0, 10.0);
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