/*Author: Yanhan Lyu*/
/* This first piece of code declares a new data type called renRenderer. */
typedef struct renRenderer renRenderer;

/* This second piece of code declares the struct that underlies the type. */
struct renRenderer {
	int varyDim;
    int unifDim;
    int texNum;
    int attrDim;
    /* declare function pointer*/
    void (*colorPixel)(struct renRenderer *renRenderer, double unif[], texTexture *tex[], double vary[], double rgb[]);
    void (*transformVertex)(struct renRenderer *renRenderer, double unif[], double attr[], double vary[]);
    void (*updateUniform)(struct renRenderer *ren, double unif[], double unifParent[]);
};
