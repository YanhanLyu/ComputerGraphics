/* triRender rasterizes a triangle whose vertices are given in 
a counter-clockwise order and whose color is set by the given 
r,g and b.*/

#include <stdio.h>
#include <math.h>
#include "000pixel.h"


/* rasterize a triangle with its vertex a, b, and c which are always
set in a counter-clockwise order.*/
void triRender(double a0, double a1, double b0, double b1, double c0, 
	double c1, double r, double g, double b) {
	/* initialize the vector A, B and C.*/
	double vectorA[2];
	double vectorB[2];
	double vectorC[2];
	/* The basic algorithm is that we want the vertex at the most left to be
	the vertex A, and if there are two vertices at the most left having 
	the same x-coordinates, we then compare their y-coordinates and set the lower one 
	to be vertex A. Then we use the assumption that the position of these 
	three vertices are given in counter-clockwise to set the other two vertices.*/
	
	/* case1: If {a0,a1} is the incontrovertibly most left vertix*/
	if (a0< b0 && a0 <c0){
		vectorA[0] = a0;
		vectorA[1] = a1;
		vectorB[0] = b0;
		vectorB[1] = b1;
		vectorC[0] = c0;
		vectorC[1] = c1;
	/* case2: If {b0,b1} is the incontrovertibly most left vertix*/
    } else if (b0 < a0 && b0 < c0){
		vectorA[0] = b0;
		vectorA[1] = b1;
		vectorB[0] = c0;
		vectorB[1] = c1;
		vectorC[0] = a0;
		vectorC[1] = a1;
	/* case3: If {c0,c1} is the incontrovertibly most left vertix*/
	} else if (c0<a0 && c0<b0){
		vectorA[0] = c0;
		vectorA[1] = c1;
		vectorB[0] = a0;
		vectorB[1] = a1;
		vectorC[0] = b0;
		vectorC[1] = b1;
	} else {
		/* If a and b have the same x-coordinates, then c
		must be on their right because of counter-clockwise*/
		if(a0 == b0 && a0 != c0){
			vectorA[0] = b0;
			vectorA[1] = b1;
			vectorB[0] = c0;
			vectorB[1] = c1;
			vectorC[0] = a0;
			vectorC[1] = a1;	
		/* If b and c have the same x-coordinates, then the only case
		we have not handled is that a is on the right of b and c.*/
		} else if (b0 == c0 && a0 != b0){
			vectorA[0] = c0;
			vectorA[1] = c1;
			vectorB[0] = a0;
			vectorB[1] = a1;
			vectorC[0] = b0;
			vectorC[1] = b1;
		/* If a and c have the same x-coordinates, then the only case
		we have not handled is that b is on the right of a and c.*/
		} else if (a0 == c0 && a0 != b0){
			vectorA[0] = a0;
			vectorA[1] = a1;
			vectorB[0] = b0;
			vectorB[1] = b1;
			vectorC[0] = c0;
			vectorC[1] = c1;
		}else {
			printf("Sorry, usually I don't draw lines, since my work is to rasterize a triangle./n");
		}
	}
	/* Then we take out the newly-arranged vertices a,b and c*/ 
	a0 = vectorA[0];
	a1 = vectorA[1];
	b0 = vectorB[0];
	b1 = vectorB[1];
	c0 = vectorC[0];
	c1 = vectorC[1];
	/* first case, c0 <= b0. We do not consider the a0 since
	it must be smaller or equal than c0 and b0. */
	if (c0 <= b0){
        /*case1: a0 is the very left c0 can reach.The other cases
         are similar as this one, so a0 == c0 case is an example.*/
		if (a0 == c0){
			int x0;
			/* The left bound is (int)ceil(a0) and the right bound is (int)floor(b0)*/
			for (x0=(int)ceil(a0); x0<=(int)floor(b0); x0++){
				int x1_low;
				int x1_high;
				/*The lower bound is ac and the higher bound is bc*/
				x1_low = a1+(b1-a1)/(b0-a0)*(x0-a0);
				x1_high = c1+(b1-c1)/(b0-c0)*(x0-c0);
				int x1;
				for (x1=(int)ceil(x1_low); x1<=(int)floor(x1_high); x1++){
					pixSetRGB(x0,x1,r,g,b);
				}
			}
        /*case2: b0 is the very right c0 can reach with our outer bound c0 <= b0.*/
		} else if (c0 == b0){
			int x0;
			for (x0=(int)ceil(a0); x0<=(int)floor(b0); x0++){
				int x1_low;
				int x1_high;
				x1_low = a1+(b1-a1)/(b0-a0)*(x0-a0);
				x1_high = a1+(c1-a1)/(c0-a0)*(x0-a0);
				int x1;
				for (x1=(int)ceil(x1_low); x1<=(int)floor(x1_high); x1++){
					pixSetRGB(x0,x1,r,g,b);
				}
			}
		/*case3: a0<c0<b0, which can be considered as a combination of case1 and case2.*/
		} else {
			int x0;
			for (x0=(int)ceil(a0); x0<=(int)floor(c0); x0++){
				int x1_low;
				int x1_high;
				x1_low = a1+(b1-a1)/(b0-a0)*(x0-a0);
				x1_high = a1+(c1-a1)/(c0-a0)*(x0-a0);
				int x1;
				for (x1=(int)ceil(x1_low); x1<=(int)floor(x1_high); x1++){
					pixSetRGB(x0,x1,r,g,b);
				}
			}
			for (x0=(int)floor(c0)+1; x0<=(int)floor(b0); x0++){
				int x1_low;
				int x1_high;
				x1_low = a1+(b1-a1)/(b0-a0)*(x0-a0);
				x1_high = c1+(b1-c1)/(b0-c0)*(x0-c0);
				int x1;
				for (x1=(int)ceil(x1_low); x1<=(int)floor(x1_high); x1++){	
					pixSetRGB(x0,x1,r,g,b);
				}
			}
		}
	/* c0 > b0, the algorithm is similar as the cases above*/
	} else {
		int x0;
		for (x0=(int)ceil(a0); x0<=(int)floor(b0); x0++){
			int x1_low;
			int x1_high;
			x1_low = a1+(b1-a1)/(b0-a0)*(x0-a0);
			x1_high = a1+(c1-a1)/(c0-a0)*(x0-a0);
			int x1;
			for (x1=(int)ceil(x1_low); x1<=(int)floor(x1_high); x1++){
				pixSetRGB(x0,x1,r,g,b);
			}
		}
		for (x0=(int)floor(b0)+1; x0<=(int)floor(c0); x0++){
			int x1_low;
			int x1_high;
			x1_low = x1_high = c1+(b1-c1)/(b0-c0)*(x0-c0);
			x1_high = a1+(c1-a1)/(c0-a0)*(x0-a0);
			int x1;
			for (x1=(int)ceil(x1_low); x1<=(int)floor(x1_high); x1++){	
				pixSetRGB(x0,x1,r,g,b);
			}

		}
		
	}	
}




