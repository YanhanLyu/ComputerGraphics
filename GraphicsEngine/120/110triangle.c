/*Author: Yanhan Lyu*/
/* triRender rasterizes a triangle with the given attributes. 
This new triRender will not draw the triangles on the back of
the window*/

#include <stdio.h>
#include <math.h>
#include "000pixel.h"
/* We get chi through alpha, beta, gamma and pq calculated in helperPQ. The
 equation is chi = alpha+q*(gamma - alpha)+p*(beta-alpha)*/
void getChi(double vectorA[renVARYDIMBOUND], double vectorB[renVARYDIMBOUND], 
    double vectorC[renVARYDIMBOUND], double attr[renVARYDIMBOUND], double pq[2], double x[2]){
    double a[2] = {vectorA[0], vectorA[1]};
    double b[2] = {vectorB[0], vectorB[1]};
    double c[2] = {vectorC[0], vectorC[1]};
    double bMinusA[2] = {0};
    double cMinusA[2] = {0};
    double xMinusA[2] = {0};
    double newMatrix[2][2] = {{0}, {0}};
    vecSubtract(2, b, a, bMinusA);
    vecSubtract(2, c, a, cMinusA);
    vecSubtract(2, x, a, xMinusA);
    mat22Columns(bMinusA, cMinusA, newMatrix);
    double inverNewMatrix[2][2] = {{0},{0}};
    mat22Invert(newMatrix, inverNewMatrix);
    mat221Multiply(inverNewMatrix, xMinusA, pq);
    double betaMinusAlpha[renVARYDIMBOUND] = {0};
    vecSubtract(renVARYDIMBOUND, vectorB, vectorA, betaMinusAlpha);
    double gammaMinusAlpha[renVARYDIMBOUND] = {0};
    vecSubtract(renVARYDIMBOUND, vectorC, vectorA, gammaMinusAlpha);
    double pTimesBetaMinusAlpha[renVARYDIMBOUND] = {0};
    vecScale(renVARYDIMBOUND, pq[0], betaMinusAlpha, pTimesBetaMinusAlpha);
    double qTimesGammaMinusAlpha[renVARYDIMBOUND] = {0};
    vecScale(renVARYDIMBOUND, pq[1], gammaMinusAlpha, qTimesGammaMinusAlpha);
    double sumOfPQVecotors[renVARYDIMBOUND] = {0};
    vecAdd(renVARYDIMBOUND, pTimesBetaMinusAlpha, qTimesGammaMinusAlpha, sumOfPQVecotors);
    vecAdd(renVARYDIMBOUND, sumOfPQVecotors, vectorA, attr);
}



/* rasterize a triangle with its vertex a, b, and c which are always
set in a counter-clockwise order.*/
void triRender(renRenderer *ren, double unif[], texTexture *tex[], double a[], double b[], double c[]){
	/* initialize the vector A, B and C.*/
	double vectorA[renVARYDIMBOUND];
	double vectorB[renVARYDIMBOUND];
	double vectorC[renVARYDIMBOUND];
	/* The basic algorithm is that we want the vertex at the most left to be
	the vertex A, and if there are two vertices at the most left having 
	the same x-coordinates, we then compare their y-coordinates and set the lower one 
	to be vertex A. Then we use the assumption that the position of these 
	three vertices are given in counter-clockwise to set the other two vertices.*/
	
	/* case1: If {vectorA[0],vectorA[1]} is the incontrovertibly most left vertix*/
	if (a[0]< b[0] && a[0]<c[0]){
		vecCopy(renVARYDIMBOUND, a, vectorA);
		vecCopy(renVARYDIMBOUND, b, vectorB);
		vecCopy(renVARYDIMBOUND, c, vectorC);
        /* we also need to reassign the alpha, beta and gamma to newAlpha, newBeta and newGamma*/
		
	/* case2: If {vectorB[0],vectorB[1]} is the incontrovertibly most left vertix*/
    } else if (b[0] < a[0] && b[0] < c[0]){
		vecCopy(renVARYDIMBOUND, b, vectorA);
		vecCopy(renVARYDIMBOUND, c, vectorB);
		vecCopy(renVARYDIMBOUND, a, vectorC);
        
	/* case3: If {vectorC[0],vectorC[1]} is the incontrovertibly most left vertix*/
	} else if (c[0]<a[0] && c[0]<b[0]){
        vecCopy(renVARYDIMBOUND, c, vectorA);
        vecCopy(renVARYDIMBOUND, a, vectorB);
        vecCopy(renVARYDIMBOUND, b, vectorC);
      
    /*handle the case that a[0] == c[0], there is only one possibility*/
	} else if (a[0] == c[0] && b[0] != c[0]){
        vecCopy(renVARYDIMBOUND, a, vectorA);
        vecCopy(renVARYDIMBOUND, b, vectorB);
        vecCopy(renVARYDIMBOUND, c, vectorC);
     
    /*handle the case that a[0] == b[0], there is only one possibility*/
    } else if  (a[0] == b[0] && a[0] != c[0]){
        vecCopy(renVARYDIMBOUND, b, vectorA);
        vecCopy(renVARYDIMBOUND, c, vectorB);
        vecCopy(renVARYDIMBOUND, a, vectorC);

    /*handle the case that b[0] == c[0], there is only one possibility*/
    } else if (b[0] == c[0] && a[0] != b[0]){
        vecCopy(renVARYDIMBOUND, c, vectorA);
        vecCopy(renVARYDIMBOUND, a, vectorB);
        vecCopy(renVARYDIMBOUND, b, vectorC);
    /* in this case, it is a vertical line*/
    } else {
        vecCopy(renVARYDIMBOUND, a, vectorA);
        vecCopy(renVARYDIMBOUND, b, vectorB);
        vecCopy(renVARYDIMBOUND, c, vectorC);
    }

    /* declare the variables I need to use*/
    double chi[2];
    int x0;
    int x1;
    double x1_low;
    double x1_high;
    double rgbz[4];
    double attr[ren->varyDim];
    double pq[2];
    /*if the det is <= 0, it's on the back, then we do not draw*/
    double m[2][2] = {
        {b[0]-a[0], c[0]-a[0]},
        {b[1]-a[1], c[1]-a[1]}
    };
    double InV[2][2]; 
    if (mat22Invert(m, InV) <= 0){
        return;
    }

    /* if vectorB[0] is smaller or equal than vectorC[0] */
    if (vectorB[0] <= vectorC[0]) {
        /* this is a vertical line */
        if (vectorB[0] == vectorC[0] && vectorA[0] == vectorC[0]) {
            for (x1 = ceil(fmin(fmin(vectorA[1],vectorB[1]),fmin(vectorA[1],vectorC[1]))); x1 <= floor(fmax(fmax(vectorA[1],vectorB[1]),fmax(vectorA[1],vectorC[1]))); x1 ++) {
                /* just draw a vertical line*/
                pixSetRGB(ceil(vectorA[0]), x1, rgbz[0], rgbz[1], rgbz[2]);
            }
        /* b and c have the same x-coordinates */
        } else if (vectorB[0] == vectorC[0] && vectorA[0] != vectorC[0]) {
           
            /*left and right bound*/
            for (x0 = ceil(vectorA[0]); x0 <= floor(vectorB[0]); x0 ++) {
                x1_low = vectorA[1] + (vectorB[1]-vectorA[1])/(vectorB[0]-vectorA[0]) * (x0 - vectorA[0]);
                x1_high = vectorA[1] + (vectorC[1]-vectorA[1])/(vectorC[0]-vectorA[0]) * (x0 - vectorA[0]);
                /*higher and lower bound*/
                for (x1 = ceil(x1_low); x1 <= floor(x1_high); x1 ++) {
                    double x[2] = {x0, x1};
                    getChi(vectorA, vectorB, vectorC, attr, pq,x);
                    ren->colorPixel(ren, unif, tex, attr,rgbz);
                    if (rgbz[3] > depthGetZ(ren->depth, x[0], x[1])) {
                        //printf("work");
                        //printf("rgbz[3] %f   ,", rgbz[3]);
                        pixSetRGB(x[0], x[1], rgbz[0], rgbz[1], rgbz[2]);
                        depthSetZ(ren->depth, x[0], x[1], rgbz[3]);
                    }   
                
                }
            }
        /* normal cases*/
        } else {
            for (x0 = ceil(vectorB[0]); x0 <= floor(vectorC[0]); x0 ++) {
                x1_low = vectorC[1] + (vectorB[1]-vectorC[1])/(vectorB[0]-vectorC[0]) * (x0 - vectorC[0]);
                x1_high = vectorA[1] + (vectorC[1]-vectorA[1])/(vectorC[0]-vectorA[0]) * (x0 - vectorA[0]);
                for (x1 = ceil(x1_low); x1 <= floor(x1_high); x1 ++) {
                    double x[2] = {x0, x1};
                    getChi(vectorA, vectorB, vectorC, attr, pq,x);
                    ren->colorPixel(ren, unif, tex, attr,rgbz);
                    if (rgbz[3] > depthGetZ(ren->depth, x[0], x[1])) {
                        //printf("work");
                        //printf("rgbz[3] %f   ,", rgbz[3]);
                        pixSetRGB(x[0], x[1], rgbz[0], rgbz[1], rgbz[2]);
                        depthSetZ(ren->depth, x[0], x[1], rgbz[3]);
                    } 
                }
            }
            for (x0 = ceil(vectorA[0]); x0 <= floor(vectorB[0]); x0 ++) {
                x1_low = vectorA[1] + (vectorB[1]-vectorA[1])/(vectorB[0]-vectorA[0]) * (x0 - vectorA[0]);
                x1_high = vectorA[1] + (vectorC[1]-vectorA[1])/(vectorC[0]-vectorA[0]) * (x0 - vectorA[0]);
                for (x1 = ceil(x1_low); x1 <= floor(x1_high); x1 ++) {
                    double x[2] = {x0, x1};
                    getChi(vectorA, vectorB, vectorC, attr, pq,x);
                    ren->colorPixel(ren, unif, tex, attr,rgbz);
                    if (rgbz[3] > depthGetZ(ren->depth, x[0], x[1])) {
                        //printf("work");
                        //printf("work");
                        //printf("rgbz[3] %f   ,", rgbz[3]);
                        pixSetRGB(x[0], x[1], rgbz[0], rgbz[1], rgbz[2]);
                        depthSetZ(ren->depth, x[0], x[1], rgbz[3]);
                        
                    } 
                }
            }
        }
    } else {
        /* if A and C have the same x-coordinates*/
        if (vectorA[0] == vectorC[0]){
            for (x0 = ceil(vectorC[0]); x0 <= floor(vectorB[0]); x0 ++) {
                x1_low = vectorA[1] + (vectorB[1]-vectorA[1])/(vectorB[0]-vectorA[0]) * (x0 - vectorA[0]);
                x1_high = vectorC[1] + (vectorB[1]-vectorC[1])/(vectorB[0]-vectorC[0]) * (x0 - vectorC[0]);
                for (x1 = ceil(x1_low); x1 <= floor(x1_high); x1 ++) {
                    double x[2] = {x0, x1};
                    getChi(vectorA, vectorB, vectorC, attr, pq,x);
                    ren->colorPixel(ren, unif, tex, attr,rgbz);
                    if (rgbz[3] > depthGetZ(ren->depth, x[0], x[1])) {
                        //printf("work");
                        //printf("work");
                        //printf("rgbz[3] %f   ,", rgbz[3]);
                        pixSetRGB(x[0], x[1], rgbz[0], rgbz[1], rgbz[2]);
                        depthSetZ(ren->depth, x[0], x[1], rgbz[3]);
                        
                    } 
                }
            }
        /* normal cases*/
        }else {
            for (x0 = ceil(vectorA[0]); x0 <= floor(vectorC[0]); x0 ++) {
                x1_low = vectorA[1] + (vectorB[1]-vectorA[1])/(vectorB[0]-vectorA[0]) * (x0 - vectorA[0]);
                x1_high = vectorA[1] + (vectorC[1]-vectorA[1])/(vectorC[0]-vectorA[0]) * (x0 - vectorA[0]);
                for (x1 = ceil(x1_low); x1 <= floor(x1_high); x1 ++) {
                    double x[2] = {x0, x1};
                    getChi(vectorA, vectorB, vectorC, attr, pq,x);
                    ren->colorPixel(ren, unif, tex, attr,rgbz);
                    if (rgbz[3] > depthGetZ(ren->depth, x[0], x[1])) {
                        //printf("work");
                        //printf("work");
                        //printf("rgbz[3] %f   ,", rgbz[3]);
                        pixSetRGB(x[0], x[1], rgbz[0], rgbz[1], rgbz[2]);
                        depthSetZ(ren->depth, x[0], x[1], rgbz[3]);
                        
                    } 
                }
            }
            for (x0 = ceil(vectorC[0]); x0 <= floor(vectorB[0]); x0 ++) {
                x1_low = vectorA[1] + (vectorB[1]-vectorA[1])/(vectorB[0]-vectorA[0]) * (x0 - vectorA[0]);
                x1_high = vectorC[1] + (vectorB[1]-vectorC[1])/(vectorB[0]-vectorC[0]) * (x0 - vectorC[0]);
                for (x1 = ceil(x1_low); x1 <= floor(x1_high); x1 ++) {
                    double x[2] = {x0, x1};
                    getChi(vectorA, vectorB, vectorC, attr, pq,x);
                    ren->colorPixel(ren, unif, tex, attr,rgbz);
                    if (rgbz[3] > depthGetZ(ren->depth, x[0], x[1])) {
                        //printf("work");
                        //printf("rgbz[3] %f   ,", rgbz[3]);
                        pixSetRGB(x[0], x[1], rgbz[0], rgbz[1], rgbz[2]);
                        depthSetZ(ren->depth, x[0], x[1], rgbz[3]);
                       
                    } 
                }
            }
        }
    }  
}








