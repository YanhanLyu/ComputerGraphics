/* This is the main function for the 020triangle.c
 It demonstates the rasterization of a triangle given
 its vertices in counter-clockwise order.
 */

#include "030vector.c"
#include "030matrix.c"
#include "040texture.c"
#include "040triangle.c"
#include <GLFW/glfw3.h>
#include <stdio.h>

texTexture myTexture = {};
int filtering = 0;

/* This function handles the situation that a key is pressed*/
void handleKeyUp(int key, int shiftIsDown, int controlIsDown,
                 int altOptionIsDown, int superCommandIsDown){
    pixClearRGB(0.0, 0.0, 0.0);
    double a[2] = {10,400};
    double b[2] = {250,20};
    double c[2] = {490,400};
    double rgb[3] = {1,1,1};
    double alpha[2] = {0,1};
    double beta[2] = {0.5,0};
    double gamma[2] = {1,1};
    /* handle the case if the user print the enter key.*/
    if (key == GLFW_KEY_ENTER){
        pixSetKeyUpHandler(handleKeyUp);
        if (filtering == 0){
            myTexture.filtering = 1;
            filtering = 1;
        } else {
            myTexture.filtering = 0;
            filtering = 0;
        }
        texInitializeFile(&myTexture, "Ringo.jpg");
        triRender(a,b,c,rgb,&myTexture,alpha,beta,gamma);
    }
}

/* Intialize a window with a triangle, in which there is a picture*/
int main(void){
    if (pixInitialize(512, 512, "Pixel Graphics") != 0){
        return 1;
    }
    else {
        double vectorA[2] = {10,400}, vectorB[2] = {250,20}, vectorC[2] = {490,400};
        double texCoordA[2] = {0,1}, texCoordB[2] = {0.5,0}, texCoordC[2] = {1,1};
        double rgb[3] = {1,1,1};
        double alpha[2] = {0,1};
        double beta[2] = {0.5,0};
        double gamma[2] = {1,1};
        texInitializeFile(&myTexture, "Ringo.jpg");
        triRender(vectorA,vectorB,vectorC, rgb, &myTexture, texCoordA,texCoordB,texCoordC);
        pixSetKeyUpHandler(handleKeyUp);
        pixRun();
        texDestroy(&myTexture);
        return 0;
    }
}

