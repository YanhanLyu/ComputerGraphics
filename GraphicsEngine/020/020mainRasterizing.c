/* This is the main function for the 020triangle.c
It demonstates the rasterization of a triangle given 
its vertices in counter-clockwise order.
*/
#include "020triangle.c"

int main(void){
	if (pixInitialize(512, 512, "Pixel Graphics") != 0){
		return 1;
	}
	else {
      triRender(20,30,350,200,150,400,1,1,0);
		pixRun();
		return 0;
	}
}
    
