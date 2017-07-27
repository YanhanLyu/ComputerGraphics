/*Author: Yanhan Lyu*/
/*** In general dimensions ***/

/* Copies the dim-dimensional vector v to the dim-dimensional vector copy. */
void vecCopy(int dim, double v[], double copy[]) {
    int i;
    for (i=0; i<dim; i++){
        copy[i] = v[i];
    }
    
}

/* Adds the dim-dimensional vectors v and w. */
void vecAdd(int dim, double v[], double w[], double vPlusW[]) {
    int i;
    for (i=0; i<dim; i++){
        vPlusW[i] = v[i]+w[i];
    }
    
}

/* Subtracts the dim-dimensional vectors v and w. */
void vecSubtract(int dim, double v[], double w[], double vMinusW[]) {
    int i;
    for (i=0; i<dim; i++){
        vMinusW[i] = v[i] - w[i];
    }
    
}

/* Scales the dim-dimensional vector w by the number c. */
void vecScale(int dim, double c, double w[], double cTimesW[]) {
    int i;
    for (i=0; i<dim; i++){
        cTimesW[i] = c*w[i];
    }
    
}

/* Assumes that there are dim + 2 arguments, the last dim of which are doubles.
 Sets the dim-dimensional vector v to those doubles. */
void vecSet(int dim, double v[], ...){
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
        v[i] = va_arg(argumentPointer, double);
    va_end(argumentPointer);
}


