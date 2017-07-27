/*Author: Yanhan Lyu*/


/* Copies the dim-dimensional vector v to the dim-dimensional vector copy. */
void vecCopy(int dim, GLdouble v[], GLdouble copy[]) {
    int i;
    for (i=0; i<dim; i++){
        copy[i] = v[i];
    }
    
}

/* Adds the dim-dimensional vectors v and w. */
void vecAdd(int dim, GLdouble v[], GLdouble w[], GLdouble vPlusW[]) {
    int i;
    for (i=0; i<dim; i++){
        vPlusW[i] = v[i]+w[i];
    }
    
}


/* Subtracts the dim-dimensional vectors v and w. */
void vecSubtract(int dim, GLdouble v[], GLdouble w[], GLdouble vMinusW[]) {
    int i;
    for (i=0; i<dim; i++){
        vMinusW[i] = v[i] - w[i];
    }
    
}

/* Scales the dim-dimensional vector w by the number c. */
void vecScale(int dim, GLdouble c, GLdouble w[], GLdouble cTimesW[]) {
    int i;
    for (i=0; i<dim; i++){
        cTimesW[i] = c*w[i];
    }
    
}

/* Assumes that there are dim + 2 arguments, the last dim of which are GLdoubles.
 Sets the dim-dimensional vector v to those GLdoubles. */
void vecSet(int dim, GLdouble v[], ...){
    int numArguments = dim;
    /* This variable will point to each anonymous argument in turn. */
    va_list argumentPointer;
    /* Tell the argument pointer to start at the argument after count. */
    va_start(argumentPointer, v);
    /* You give va_arg an argument pointer and a type name. It interprets the
     current argument to be of that type and returns that argument. As a side
     effect, it increments the argument pointer so that it points to the next
     argument. */
    int i;
    for (i = 0; i < numArguments; i ++)
        v[i] = va_arg(argumentPointer, GLdouble);
    va_end(argumentPointer);
}

/* Returns the dot product of the dim-dimensional vectors v and w. */
GLdouble vecDot(int dim, GLdouble v[], GLdouble w[]) {
    GLdouble result = 0.0;
    for (int i = 0; i < dim; i++)
        result += v[i]*w[i];
    return result;  
}

/* Returns the length of the vector v. */
GLdouble vecLength(int dim, GLdouble v[]) {
    GLdouble sum = vecDot(dim, v, v);
    return sqrt(sum);
}

/* Returns the length of the vector v. If the length is non-zero, then also 
places a scaled version of v into unit, so that unit has length 1. */
GLdouble vecUnit(int dim, GLdouble v[], GLdouble unit[]) {
    GLdouble length = vecLength(dim, v);
    if (length != 0) {
        for (int i = 0; i < dim; i++) {
            unit[i] = v[i]/length;
        }
    }
    return length;
}

/* Computes the cross product of v and w, and places it into vCrossW. */
void vec3Cross(GLdouble v[3], GLdouble w[3], GLdouble vCrossW[3]) {
    vCrossW[0] =  v[1] * w[2] - w[1] * v[2];
    vCrossW[1] = w[0] * v[2] - v[0] * w[2];
    vCrossW[2] = v[0] * w[1] - w[0] * v[1];
}

/* Computes the vector v from its spherical coordinates. rho >= 0.0 is the 
radius. 0 <= phi <= pi is the co-latitude. -pi <= theta <= pi is the longitude 
or azimuth. */
void vec3Spherical(GLdouble rho, GLdouble phi, GLdouble theta, GLdouble v[3]) {
    v[0] = rho*sin(phi)*cos(theta);
    v[1] = rho*sin(phi)*sin(theta);
    v[2] = rho*cos(phi);
}


/*** OpenGL ***/
void vecOpenGL(int dim, GLdouble v[], GLfloat openGL[]) {
    for (int i = 0; i < dim; i += 1)
        openGL[i] = v[i];
}