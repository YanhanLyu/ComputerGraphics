/*Author Yanhan Lyu, this is the main for initializing secene, windows and 
the other things that we need to use.*/
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
#define renVARYWORLDX 6
#define renVARYWORLDY 7
#define renVARYWORLDZ 8
#define renVARYUNITNORMALX 9
#define renVARYUNITNORMALY 10
#define renVARYUNITNORMALZ 11



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
#define renUNIFLIGHTWORLDX 41
#define renUNIFLIGHTWORLDY 42
#define renUNIFLIGHTWORLDZ 43
#define renUNIFLIGHTR 44
#define renUNIFLIGHTG 45
#define renUNIFLIGHTB 46
#define renUNIFCAMERAWORLDX 47
#define renUNIFCAMERAWORLDY 48
#define renUNIFCAMERAWORLDZ 49
#define renUNIFSHINESS 50

#define renTEXR 0
#define renTEXG 1
#define renTEXB 2

/* Sets rgb, based on the other parameters, which are unaltered. attr is an 
interpolated attribute vector. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[], 
        double vary[], double rgb[]) {
    texSample(tex[0], vary[renVARYS], vary[renVARYT]);
    double ambient = 0.1;
    /*diffuse effect*/
    /*declare the variables*/
    double lightCoordinates[3];
    double vertexNormal[3];
    double worldPixelCoordinates[3];
    double dirLight[3];
    double lightRGB[3];
    double d;
    double dot;
    vecCopy(3, &unif[renUNIFLIGHTWORLDX], lightCoordinates);
    vecCopy(3, &unif[renUNIFLIGHTR], lightRGB);
    vecCopy(3, &vary[renVARYWORLDX], worldPixelCoordinates);
    vecCopy(3, &vary[renVARYUNITNORMALX], vertexNormal);
    vecSubtract(3, lightCoordinates, worldPixelCoordinates, dirLight);
    /*unit dirLight and vertexNormal*/
    vecUnit(3, dirLight, dirLight);
    vecUnit(3, vertexNormal, vertexNormal);
    dot = vecDot(3, vertexNormal, dirLight);
    d = fmax(0, dot);
    rgb[0] = tex[0]->sample[renTEXR] * (d+ambient) * lightRGB[0];
    rgb[1] = tex[0]->sample[renTEXG] * (d+ambient) * lightRGB[1];
    rgb[2] = tex[0]->sample[renTEXB] * (d+ambient) * lightRGB[2];
    /*specular*/
    /*declare the variables*/
    double cameraDirection[3];
    double reflctionDir[3];
    double vecMinus[3];
    double ClearCoatRGB[3] = {1.0, 1.0, 1.0};
    double cameraCoordinates[3] = {unif[renUNIFCAMERAWORLDX], unif[renUNIFCAMERAWORLDY], unif[renUNIFCAMERAWORLDZ]};
    double specularRGB[3]; 
    double shineness = unif[renUNIFSHINESS];
    /*do spcular calculation*/
    vecSubtract(3, cameraCoordinates, worldPixelCoordinates, cameraDirection);
    vecUnit(3, cameraDirection, cameraDirection);
    vecScale(3, 2*dot, vertexNormal, vecMinus);
    vecSubtract(3, vecMinus, dirLight, reflctionDir);
    vecUnit(3, reflctionDir,reflctionDir);
    double specIntensity = pow(fmax(0, vecDot(3, cameraDirection, reflctionDir)), shineness);
    /*solve the situation that light is from back*/
    if (dot <= 0){
        specIntensity = 0;
    }
    /*RGB = SPECULARINT * lightRGB * clearCoat*/
    for (int i = 0; i < 3; i += 1) {
        specularRGB[i] = specIntensity*lightRGB[i]*ClearCoatRGB[i];
        rgb[i] += specularRGB[i];
    }
    /*fog effect */
    double fog[3] = {0.5, 0.5, 0.5};
    double z =  vary[renVARYZ];

    for (int i = 0; i < 3; i += 1) {
        rgb[i] = ((z+1)/2) * rgb[i] + (1-((z+1)/2)) * fog[i];
    }
    /*set z value*/
    rgb[3] = vary[renVARYZ];
}


/* Writes the vary vector, based on the other parameters. */
void transformVertex(renRenderer *ren, double unif[], double attr[], 
        double vary[]) {
    double newAttr[4] = {attr[renVARYX], attr[renVARYY], attr[renVARYZ], 1};
    double xyz[4]; 
    double eyeXYZ[4];
    double view[4];
    double normals[4] = {attr[renATTRN], attr[renATTRO], attr[renATTRP], 0};
    double newNormals[4];
    mat441Multiply((double(*)[4])(&unif[renUNIFISOMETRY]), newAttr, xyz);
    /*store the world coordinates*/
    vary[renVARYWORLDX] = xyz[0];
    vary[renVARYWORLDY] = xyz[1];
    vary[renVARYWORLDZ] = xyz[2];
    /*calculat the rotation of normals*/
    mat441Multiply((double(*)[4])(&unif[renUNIFISOMETRY]), normals, newNormals);
    /*do unit vector calculation*/
    vecUnit(3, newNormals, newNormals);
    vary[renVARYUNITNORMALX] = newNormals[0];
    vary[renVARYUNITNORMALY] = newNormals[1];
    vary[renVARYUNITNORMALZ] = newNormals[2];
    mat441Multiply(ren->viewing, xyz, eyeXYZ);
    vary[renVARYX] = eyeXYZ[renVARYX];
    vary[renVARYY] = eyeXYZ[renVARYY];
    vary[renVARYZ] = eyeXYZ[renVARYZ];
    vary[renVARYW] = eyeXYZ[renVARYW];
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];
}

/*This is to set the light position and lightRGB*/
void setLight(double unif[], double x, double y, double z, double r, double g, double b) {
    unif[renUNIFLIGHTWORLDX] = x;
    unif[renUNIFLIGHTWORLDY] = y;
    unif[renUNIFLIGHTWORLDZ] = z;
    unif[renUNIFLIGHTR] = r;
    unif[renUNIFLIGHTG] = g;
    unif[renUNIFLIGHTB] = b;
}

/*This is to set the camera world coordinate and shiness*/
void setCamera(double unif[], double x, double y, double z, double shiness) {
    unif[renUNIFCAMERAWORLDX] = x;
    unif[renUNIFCAMERAWORLDX] = y;
    unif[renUNIFCAMERAWORLDX] = z;
    unif[renUNIFSHINESS] = shiness;
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
    /*write light and camera*/
    setLight(unif, 0,2,5,1,1,1);
    setCamera(unif, ren->cameraTranslation[0],ren->cameraTranslation[1],ren->cameraTranslation[2],128);
    
}


#include "110triangle.c"
#include "140clipping.c"
#include "140mesh.c"
#include "090scene.c"

/*global variables declared to be used*/
double unifA[51] = {1,1,1, 0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,1,1,0,1,1,1,10};
double unifB[51] = {1,1,1, 0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,1,1,0,0,0,0,10};

depthBuffer depth;
texTexture textureA, textureB;
texTexture *tex[2];
renRenderer myRenderer = {
    .transformVertex = transformVertex,
    .colorPixel = colorPixel,
    .updateUniform = updateUniform,
    .texNum = 1,
    .varyDim = 12,
    .unifDim = 51,
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
    /*use the renLookAt*/
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
    printf("Use W,A,S,D to control the object, and P,M to set the distance, Enter changes to perspective and O changes to orthographic. \n");
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