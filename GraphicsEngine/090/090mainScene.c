/*This is the main function to draw the scene
 Author: Yanhan Lyu*/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "000pixel.h"
#include "090matrix.c"
#include "070vector.c"
#include "040texture.c"
#include "090renderer.c"

#define renVARYDIMBOUND 16
#define renVERTNUMBOUND 16

#define renATTRX 0
#define renATTRY 1
#define renATTRS 2
#define renATTRT 3
#define renATTRR 4
#define renATTRG 5
#define renATTRB 6

#define renVARYX 0
#define renVARYY 1
#define renVARYS 2
#define renVARYT 3
#define renVARYR 4
#define renVARYG 5
#define renVARYB 6

#define renUNIFR 0
#define renUNIFG 1
#define renUNIFB 2
#define renUNIFTRANSX 3
#define renUNIFTRANSY 4
#define renUNIFTHETA 5
#define renUNIFISOMETRY 6

#define renTEXR 0
#define renTEXG 1
#define renTEXB 2



/* Sets rgb, based on the other parameters, which are unaltered. attr is an 
interpolated attribute vector. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[], 
        double vary[], double rgb[]) {
    texSample(tex[0], vary[renVARYS], vary[renVARYT]);
    rgb[0] = tex[0]->sample[renTEXR] * unif[renUNIFR];
    rgb[1] = tex[0]->sample[renTEXG] * unif[renUNIFG];
    rgb[2] = tex[0]->sample[renTEXB] * unif[renUNIFB];
}

/* Writes the vary vector, based on the other parameters. */
void transformVertex(renRenderer *ren, double unif[], double attr[], 
        double vary[]) {
    double attrScreen[3] = {attr[renATTRX], attr[renATTRY], 1};
    double newAttrScreen[3] = {0};
    mat331Multiply((double(*)[3])(&unif[renUNIFISOMETRY]), attrScreen, newAttrScreen);
    vary[renVARYX] = newAttrScreen[0];
    vary[renVARYY] =newAttrScreen[1];
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];
}

/* If unifParent is NULL, then sets the uniform matrix to the 
rotation-translation M described by the other uniforms. If unifParent is not 
NULL, but instead contains a rotation-translation P, then sets the uniform 
matrix to the matrix product P * M. */
void updateUniform(renRenderer *ren, double unif[], double unifParent[]) {
    if (unifParent == NULL)
        /* The nine uniforms for storing the matrix start at index 
        renUNIFISOMETRY. So &unif[renUNIFISOMETRY] is an array containing those 
        nine numbers. We use '(double(*)[3])' to cast it to a 3x3 matrix. */
        mat33Isometry(unif[renUNIFTHETA], unif[renUNIFTRANSX], 
            unif[renUNIFTRANSY], (double(*)[3])(&unif[renUNIFISOMETRY]));
    else {
        double m[3][3];
        mat33Isometry(unif[renUNIFTHETA], unif[renUNIFTRANSX], 
            unif[renUNIFTRANSY], m);
        mat333Multiply((double(*)[3])(&unifParent[renUNIFISOMETRY]), m, 
            (double(*)[3])(&unif[renUNIFISOMETRY]));
    }
}

#include "090triangle.c"
#include "090mesh.c"
#include "090scene.c"

texTexture textureA;
texTexture textureB;
texTexture textureC;
texTexture textureD;
renRenderer renderer;
meshMesh mesherA;
meshMesh mesherB;
meshMesh mesherC;
meshMesh mesherD;
sceneNode nodeA;
sceneNode nodeB;
sceneNode nodeC;
sceneNode nodeD;
texTexture *texA[1];
texTexture *texB[1];
texTexture *texC[1];
texTexture *texD[1];

/*set one uniform in the unif*/
void sceneSetOneUniform(sceneNode *node, int i, double unif){
        node -> unif[i] = unif;
}

/*draw the picture*/
void draw(void){
    pixClearRGB(0.0, 0.0, 0.0);
    sceneRender(&nodeA, &renderer, NULL);
}

/* This callback is called once per animation frame. As parameters it receives 
the time for the current frame and the time for the previous frame. Both times 
are measured in seconds since some distant past time. */ 
void handleTimeStep(double oldTime, double newTime) {
    if (floor(newTime) - floor(oldTime) >= 1.0)
        printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
        sceneSetOneUniform(&nodeA, renUNIFTHETA, newTime*5);
        draw();
}



/* draw the four pictures A,B,C,D with rotation angle theta and trasition x and y*/
int main(void) {
    renderer.unifDim = 15,
    renderer.texNum = 1,
    renderer.varyDim = 4;
    renderer.attrDim = 4;
    renderer.colorPixel = colorPixel;
    renderer.transformVertex = transformVertex;
    renderer.updateUniform = updateUniform;
    renRenderer *ren = &renderer;
    meshMesh *meshA = &mesherA;
    meshMesh *meshB = &mesherB;
    meshMesh *meshC = &mesherC;
    meshMesh *meshD = &mesherD;
    texA[0] = &textureA;
    texB[0] = &textureB;
    texC[0] = &textureC;
    texD[0] = &textureD;
    double unifA[16] = {1.0, 1.0, 1.0, 0.0, 0.0, 0.2, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    double unifB[16] = {1.0, 1.0, 1.0, 0.0, 0.0, 0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    double unifC[16] = {1.0, 1.0, 1.0, 0.0, 0.0, 0.2, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    double unifD[16] = {1.0, 1.0, 1.0, 0.0, 0.0, 0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
	if (pixInitialize(512, 512, "Pixel Graphics") != 0){
		return 1;
    } else if (meshInitializeEllipse(meshA, 300.0, 300.0, 150.0, 150.0, 50.0) != 0){
        return 2;
    } else if (meshInitializeEllipse(meshB, 250.0, 200.0, 50.0, 50.0, 50.0) != 0){
        return 3;
    } else if (meshInitializeEllipse(meshC, 150.0, 150.0, 60.0, 60.0, 50.0) != 0){
        return 4;
    } else if (meshInitializeEllipse(meshD, 350.0, 350.0, 70.0, 70.0, 50.0) != 0){
        return 5;
    } else if (texInitializeFile(&textureA, "A.jpg") != 0){
        return 6;
    } else if (texInitializeFile(&textureB, "B.jpeg") != 0){
        return 7;
    } else if (texInitializeFile(&textureC, "C.jpeg") != 0){
        return 8;
    } else if (texInitializeFile(&textureD, "D.jpg") != 0){
        return 9;
    } else if (sceneInitialize(&nodeA, ren, unifA, texA, meshA, &nodeB,NULL) != 0){
        return 10;
    } else if (sceneInitialize(&nodeB, ren, unifB, texB, meshB, &nodeC, NULL) != 0){
        return 11;
    } else if (sceneInitialize(&nodeC, ren, unifC, texC, meshC, NULL, &nodeD) != 0){
        return 12;
    } else if (sceneInitialize(&nodeD, ren, unifD, texD, meshD, NULL, NULL) != 0){
        return 13;
    } else{
        draw();
        pixSetTimeStepHandler(handleTimeStep);
        pixRun();
        texDestroy(texA[0]);
        texDestroy(texB[0]);
        texDestroy(texC[0]);
        texDestroy(texD[0]);
        meshDestroy(meshA);
        meshDestroy(meshB);
        meshDestroy(meshC);
        meshDestroy(meshD);
        sceneDestroy(&nodeA);
        sceneDestroy(&nodeB);
        sceneDestroy(&nodeC);
        sceneDestroy(&nodeD);
        return 0;
    }
}