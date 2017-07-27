/* triRender rasterizes a triangle whose vertices are given in 
a counter-clockwise order and interplote it with alpha, beta, gamma
 and the given miu. The getChi and helperPQ are helper functions to get
 chi and the pq we need.*/

#include <stdio.h>
#include <math.h>
#include "000pixel.h"

/* We get chi through alpha, beta, gamma and pq calculated in helperPQ. The
 equation is chi = alpha+q*(gamma - alpha)+p*(beta-alpha)*/
void getChi(double chi[2], double alpha[2],double beta[2],double gamma[2],double pq[2]){
	double betaMinusAlpha[2] = {0,0};
	vecSubtract(2, beta, alpha, betaMinusAlpha);
	double gammaMinusAlpha[2] = {0,0};
	vecSubtract(2, gamma, alpha, gammaMinusAlpha);
	double pTimesBetaMinusAlpha[2] = {0,0};
	vecScale(2, pq[0], betaMinusAlpha, pTimesBetaMinusAlpha);
    double qTimesGammaMinusAlpha[2] = {0,0};
	vecScale(2, pq[1], gammaMinusAlpha, qTimesGammaMinusAlpha);
	double sumOfPQVecotors[2] = {0,0};
	vecAdd(2, pTimesBetaMinusAlpha, qTimesGammaMinusAlpha, sumOfPQVecotors);
	vecAdd(2, sumOfPQVecotors, alpha, chi);
}

/* We use the position coordinates of point a, b, c and the point x itself
 to calculate p and q.*/
void  helperPQ(double a[2], double b[2], double c[2], double x[2], double pq[2]){
	double bMinusA[2] = {0,0};
	double cMinusA[2] = {0,0};
	double xMinusA[2] = {0,0};
	double newMatrix[2][2] = {{0,0}, {0,0}};
	vecSubtract(2, b, a, bMinusA);
	vecSubtract(2, c, a, cMinusA);
	vecSubtract(2, x, a, xMinusA);
	mat22Columns(bMinusA, cMinusA, newMatrix);
	double inverNewMatrix[2][2] = {{0,0},{0,0}};
	mat22Invert(newMatrix, inverNewMatrix);
	mat221Multiply(inverNewMatrix, xMinusA, pq);
}

/* rasterize a triangle with its vertex a, b, and c which are always
set in a counter-clockwise order.*/
void triRender(double a[2], double b[2], double c[2], double rgb[3],
               texTexture *tex, double alpha[2], double beta[2], double gamma[2]){
	/* initialize the vector A, B and C.*/
	double vectorA[2];
	double vectorB[2];
	double vectorC[2];
	double newAlpha[2];
	double newBeta[2];
	double newGamma[2];
	/* The basic algorithm is that we want the vertex at the most left to be
	the vertex A, and if there are two vertices at the most left having 
	the same x-coordinates, we then compare their y-coordinates and set the lower one 
	to be vertex A. Then we use the assumption that the position of these 
	three vertices are given in counter-clockwise to set the other two vertices.*/
	
	/* case1: If {vectorA[0],vectorA[1]} is the incontrovertibly most left vertix*/
	if (a[0]< b[0] && a[0]<c[0]){
		vecCopy(2, a, vectorA);
		vecCopy(2, b, vectorB);
		vecCopy(2, c, vectorC);
        /* we also need to reassign the alpha, beta and gamma to newAlpha, newBeta and newGamma*/
		vecCopy(2, alpha, newAlpha);
        vecCopy(2, beta, newBeta);
        vecCopy(2, gamma, newGamma);
	/* case2: If {vectorB[0],vectorB[1]} is the incontrovertibly most left vertix*/
    } else if (b[0] < a[0] && b[0] < c[0]){
		vecCopy(2, b, vectorA);
		vecCopy(2, c, vectorB);
		vecCopy(2, a, vectorC);
        vecCopy(2, beta, newAlpha);
        vecCopy(2, gamma, newBeta);
        vecCopy(2, alpha, newGamma);
	/* case3: If {vectorC[0],vectorC[1]} is the incontrovertibly most left vertix*/
	} else if (c[0]<a[0] && c[0]<b[0]){
        vecCopy(2, c, vectorA);
        vecCopy(2, a, vectorB);
        vecCopy(2, b, vectorC);
        vecCopy(2, gamma, newAlpha);
        vecCopy(2, alpha, newBeta);
        vecCopy(2, beta, newGamma);
    /*handle the case that a[0] == c[0], there is only one possibility*/
	} else if (a[0] == c[0] && b[0] != c[0]){
        vecCopy(2, a, vectorA);
        vecCopy(2, b, vectorB);
        vecCopy(2, c, vectorC);
        vecCopy(2, alpha, newAlpha);
        vecCopy(2, beta, newBeta);
        vecCopy(2, gamma, newGamma);
    /*handle the case that a[0] == b[0], there is only one possibility*/
    } else if  (a[0] == b[0] && a[0] != c[0]){
        vecCopy(2, b, vectorA);
        vecCopy(2, c, vectorB);
        vecCopy(2, a, vectorC);
        vecCopy(2, beta, newAlpha);
        vecCopy(2, gamma, newBeta);
        vecCopy(2, alpha, newGamma);
    /*handle the case that b[0] == c[0], there is only one possibility*/
    } else if (b[0] == c[0] && a[0] != b[0]){
        vecCopy(2, c, vectorA);
        vecCopy(2, a, vectorB);
        vecCopy(2, b, vectorC);
        vecCopy(2, gamma, newAlpha);
        vecCopy(2, alpha, newBeta);
        vecCopy(2, beta, newGamma);
    /* in this case, it is a vertical line*/
    } else {
        vecCopy(2, a, vectorA);
        vecCopy(2, b, vectorB);
        vecCopy(2, c, vectorC);
        vecCopy(2, alpha, newAlpha);
        vecCopy(2, beta, newBeta);
        vecCopy(2, gamma, newGamma);
    }
    /* declare the variables I need to use*/
    double pq[2];
    double chi[2];
    int x0;
    int x1;
    double x1_low;
    double x1_high;
    /* if vectorB[0] is smaller or equal than vectorC[0] */
    if (vectorB[0] <= vectorC[0]) {
        /* this is a vertical line */
        if (vectorB[0] == vectorC[0] && vectorA[0] == vectorC[0]) {
            for (x1 = ceil(fmin(fmin(vectorA[1],vectorB[1]),fmin(vectorA[1],vectorC[1]))); x1 <= floor(fmax(fmax(vectorA[1],vectorB[1]),fmax(vectorA[1],vectorC[1]))); x1 ++) {
                /* just draw a vertical line*/
                pixSetRGB(ceil(vectorA[0]), x1, rgb[0], rgb[1], rgb[2]);
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
                    helperPQ(vectorA, vectorB, vectorC,x,pq);
                    getChi(chi,newAlpha, newBeta, newGamma, pq);
                    texSample(tex, chi[0], chi[1]);
                    pixSetRGB(x0, x1, tex->sample[0]*rgb[0], tex->sample[1]*rgb[1], tex->sample[2]*rgb[2]);
                }
            }
        /* normal cases*/
        } else {
            for (x0 = ceil(vectorB[0]); x0 <= floor(vectorC[0]); x0 ++) {
                x1_low = vectorC[1] + (vectorB[1]-vectorC[1])/(vectorB[0]-vectorC[0]) * (x0 - vectorC[0]);
                x1_high = vectorA[1] + (vectorC[1]-vectorA[1])/(vectorC[0]-vectorA[0]) * (x0 - vectorA[0]);
                for (x1 = ceil(x1_low); x1 <= floor(x1_high); x1 ++) {
                    double x[2] = {x0, x1};
                    helperPQ(vectorA, vectorB, vectorC,x,pq);
                    getChi(chi,newAlpha, newBeta, newGamma, pq);
                    texSample(tex, chi[0], chi[1]);
                    pixSetRGB(x0, x1, tex->sample[0]*rgb[0], tex->sample[1]*rgb[1], tex->sample[2]*rgb[2]);
                }
            }
            for (x0 = ceil(vectorA[0]); x0 <= floor(vectorB[0]); x0 ++) {
                x1_low = vectorA[1] + (vectorB[1]-vectorA[1])/(vectorB[0]-vectorA[0]) * (x0 - vectorA[0]);
                x1_high = vectorA[1] + (vectorC[1]-vectorA[1])/(vectorC[0]-vectorA[0]) * (x0 - vectorA[0]);
                for (x1 = ceil(x1_low); x1 <= floor(x1_high); x1 ++) {
                    double x[2] = {x0, x1};
                    helperPQ(vectorA, vectorB, vectorC,x,pq);
                    getChi(chi,newAlpha, newBeta, newGamma, pq);
                    texSample(tex, chi[0], chi[1]);
                    pixSetRGB(x0, x1, tex->sample[0]*rgb[0], tex->sample[1]*rgb[1], tex->sample[2]*rgb[2]);
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
                    
                    helperPQ(vectorA, vectorB, vectorC,x,pq);
                    getChi(chi,newAlpha, newBeta, newGamma, pq);
                    texSample(tex, chi[0], chi[1]);
                    pixSetRGB(x0, x1, tex->sample[0]*rgb[0], tex->sample[1]*rgb[1], tex->sample[2]*rgb[2]);
                }
            }
        /* normal cases*/
        }else {
            for (x0 = ceil(vectorA[0]); x0 <= floor(vectorC[0]); x0 ++) {
                x1_low = vectorA[1] + (vectorB[1]-vectorA[1])/(vectorB[0]-vectorA[0]) * (x0 - vectorA[0]);
                x1_high = vectorA[1] + (vectorC[1]-vectorA[1])/(vectorC[0]-vectorA[0]) * (x0 - vectorA[0]);
                for (x1 = ceil(x1_low); x1 <= floor(x1_high); x1 ++) {
                    double x[2] = {x0, x1};
                    helperPQ(vectorA, vectorB, vectorC,x,pq);
                    getChi(chi,newAlpha, newBeta, newGamma, pq);
                    texSample(tex, chi[0], chi[1]);
                    pixSetRGB(x0, x1, tex->sample[0]*rgb[0], tex->sample[1]*rgb[1], tex->sample[2]*rgb[2]);
                }
            }
            for (x0 = ceil(vectorC[0]); x0 <= floor(vectorB[0]); x0 ++) {
                x1_low = vectorA[1] + (vectorB[1]-vectorA[1])/(vectorB[0]-vectorA[0]) * (x0 - vectorA[0]);
                x1_high = vectorC[1] + (vectorB[1]-vectorC[1])/(vectorB[0]-vectorC[0]) * (x0 - vectorC[0]);
                for (x1 = ceil(x1_low); x1 <= floor(x1_high); x1 ++) {
                    double x[2] = {x0, x1};
                    helperPQ(vectorA, vectorB, vectorC,x,pq);
                    getChi(chi,newAlpha, newBeta, newGamma, pq);
                    texSample(tex, chi[0], chi[1]);
                    pixSetRGB(x0, x1, tex->sample[0]*rgb[0], tex->sample[1]*rgb[1], tex->sample[2]*rgb[2]);
                }
            }
        }
    }  
}








