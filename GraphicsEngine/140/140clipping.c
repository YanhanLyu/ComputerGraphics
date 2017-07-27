/*Author: Yanhan Lyu*/

/*calculate the t based on the start vector and end vector*/
void calculateT(double start[renVARYDIMBOUND], double end[renVARYDIMBOUND], double result[renVARYDIMBOUND]){
    double t = (start[3] - start[2])/(start[3] - start[2] - end[3] + end[2]);
    double endMinusStart[renVARYDIMBOUND];
    vecSubtract(renVARYDIMBOUND, end, start, endMinusStart);
    vecScale(renVARYDIMBOUND, t, endMinusStart, endMinusStart);
    vecAdd(renVARYDIMBOUND, start, endMinusStart, result);
}

/*helper function to perform the homogeneous division and multiply by the viewport matrix*/
void helper(renRenderer *ren, double vector[renVARYDIMBOUND], double out[renVARYDIMBOUND]){
    double view[4] = {vector[renVARYX], vector[renVARYY], vector[renVARYZ], vector[renVARYW]};
    double screen[4];
    vecScale(4, 1/view[renVARYW], view, view);
    mat441Multiply(ren->viewport, view, screen);
    vecCopy(renVARYDIMBOUND, vector, out);
    out[renVARYX] = screen[renVARYX];
    out[renVARYY] = screen[renVARYY];
    out[renVARYZ] = screen[renVARYZ];
    out[renVARYW] = screen[renVARYW];
    out[renVARYS] = vector[renVARYS];
    out[renVARYT] = vector[renVARYT];
}

/*Perform clipping based on the situation that vertex a, b, c is clipped or not*/
void clipRender(renRenderer *ren, double unif[], texTexture *tex[], double a[], double b[], double c[]){
    
	double vectorA[renVARYDIMBOUND];
	double vectorB[renVARYDIMBOUND];
	double vectorC[renVARYDIMBOUND];

    vecCopy(renVARYDIMBOUND, a, vectorA);
    vecCopy(renVARYDIMBOUND, b, vectorB);
    vecCopy(renVARYDIMBOUND, c, vectorC);

    double one[renVARYDIMBOUND];
    double two[renVARYDIMBOUND];
    double newOne[renVARYDIMBOUND];
    double newTwo[renVARYDIMBOUND];
    double newA[renVARYDIMBOUND];
    double newB[renVARYDIMBOUND];
    double newC[renVARYDIMBOUND];
    // only a is clipped
    if ((vectorA[3] <= 0 || vectorA[2]>vectorA[3]) && 
        !(vectorB[3] <= 0 || vectorB[2]>vectorB[3]) && !(vectorC[3] <= 0 || vectorC[2]>vectorC[3])){
        calculateT(vectorA, vectorB, one);
        calculateT(vectorA, vectorB, two);

        helper(ren, one, newOne);
        helper(ren, two, newTwo);
        helper(ren, vectorB, newB);
        helper(ren, vectorC, newC);
        //printf("0");
        triRender(ren, unif, tex, newOne, newB, newTwo);
        triRender(ren, unif, tex, newTwo, newB, newC);
    // only b is clipped
    } else if ((vectorB[3] <= 0 || vectorB[2]>vectorB[3]) && 
        !(vectorA[3] <= 0 || vectorA[2]>vectorA[3]) && !(vectorC[3] <= 0 || vectorC[2]>vectorC[3])){
        calculateT(vectorA,vectorB,one);
        calculateT(vectorB,vectorC,two);

        helper(ren, one, newOne);
        helper(ren, two, newTwo);
        helper(ren, vectorA, newA);
        helper(ren, vectorC, newC);
        //printf("1");
        triRender(ren, unif, tex, newA, newOne, newC);
        triRender(ren, unif, tex, newOne, newTwo, newC);
    // only c is clipped
    } else if ((vectorC[3] <= 0 || vectorC[2]>vectorC[3]) && 
        !(vectorA[3] <= 0 || vectorA[2]>vectorA[3]) && !(vectorB[3] <= 0 || vectorB[2]>vectorB[3])){
        calculateT(vectorB, vectorC, one);
        calculateT(vectorA, vectorC, two);

        helper(ren, one, newOne);
        helper(ren, two, newTwo);
        helper(ren, vectorA, newA);
        helper(ren, vectorB, newB);

        //printf("2");
        triRender(ren, unif, tex, newB, newOne, newTwo);
        triRender(ren, unif, tex, newB, newTwo, newA);
    // a and b are clipped, but c is not
    }else if ((vectorA[3] <= 0 || vectorA[2]>vectorA[3]) && (vectorB[3] <= 0 || vectorB[2]>vectorB[3]) 
        && !(vectorC[3] <= 0 || vectorC[2]>vectorC[3])){
        calculateT(vectorA, vectorC, one);
        calculateT(vectorB, vectorC, two);

        helper(ren, one, newOne);
        helper(ren, two, newTwo);
        helper(ren, vectorC, newC);

        //printf("3");
        triRender(ren, unif, tex, newOne, newTwo, newC);
    // a and c are clipped, but b is not
    }else if ((vectorA[3] <= 0 || vectorA[2]>vectorA[3]) && (vectorC[3] <= 0 || vectorC[2]>vectorC[3]) 
        && !(vectorB[3] <= 0 || vectorB[2]>vectorB[3])){
        calculateT(vectorA,vectorB,one);
        calculateT(vectorB,vectorC,two);

        helper(ren, one, newOne);
        helper(ren, two, newTwo);
        helper(ren, vectorB, newB);

        //printf("4");
        triRender(ren, unif, tex, newOne, newB, newTwo);
    // b and c are clipped, but a is not
    } else if ((vectorB[3] <= 0 || vectorB[2]>vectorB[3]) && (vectorC[3] <= 0 || vectorC[2]>vectorC[3]) 
        && !(vectorA[3] <= 0 || vectorA[2]>vectorA[3])){
        calculateT(vectorA,vectorB,one);
        calculateT(vectorA,vectorC,two);

        helper(ren, one, newOne);
        helper(ren, two, newTwo);
        helper(ren, vectorA, newA);

        //printf("5");
        triRender(ren, unif, tex, newTwo, newA, newOne);
    // a, b, and c are all not clipped
    } else if (!(vectorB[3] <= 0 || vectorB[2]>vectorB[3]) && !(vectorC[3] <= 0 || vectorC[2]>vectorC[3]) 
        && !(vectorA[3] <= 0 || vectorA[2]>vectorA[3])){
        helper(ren, vectorA, newA);
        helper(ren, vectorB, newB);
        helper(ren, vectorC, newC);
        //printf("6");
        //printf("a, %f, %f, %f, %f\n", newB[0], newB[1], newB[2], newB[3]);
        triRender(ren, unif, tex, newA, newB, newC);
    }
} 


