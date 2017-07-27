/*Author: Yanhan Lyu*/
/*** 2 x 2 Matrices ***/

/* Pretty-prints the given matrix, with one line of text per row of matrix. */
void mat22Print(double m[2][2]) {
	int i;
	for (i = 0; i < 2; i += 1)
		printf("%f    %f\n", m[i][0], m[i][1]);
}

/* Returns the determinant of the matrix m. If the determinant is 0.0, then the 
matrix is not invertible, and mInv is untouched. If the determinant is not 0.0, 
then the matrix is invertible, and its inverse is placed into mInv. */
double mat22Invert(double m[2][2], double mInv[2][2]) {
	double det = m[0][0]*m[1][1] - m[0][1]*m[1][0];
	if (det != 0){
		double scalar = 1/det;
		mInv[0][0] = scalar*m[1][1];
		mInv[0][1] = -scalar*m[0][1];
		mInv[1][0] = -scalar*m[1][0];
		mInv[1][1] = scalar*m[0][0];
	}
	return det;
}

/* Multiplies a 2x2 matrix m by a 2-column v, storing the result in mTimesV. 
The output should not */
void mat221Multiply(double m[2][2], double v[2], double mTimesV[2]) {
	mTimesV[0] = m[0][0]*v[0]+m[0][1]*v[1];
	mTimesV[1] = m[1][0]*v[0]+m[1][1]*v[1];
}

/* Fills the matrix m from its two columns. */
void mat22Columns(double col0[2], double col1[2], double m[2][2]) {
	m[0][0] = col0[0];
	m[1][0] = col0[1];
	m[0][1] = col1[0];
	m[1][1] = col1[1];
}

/* Multiplies the 3x3 matrix m by the 3x3 matrix n. */
void mat333Multiply(double m[3][3], double n[3][3], double mTimesN[3][3]){
	mTimesN[0][0] = m[0][0] * n[0][0] + m[0][1]*n[1][0] + m[0][2]*n[2][0];
	mTimesN[0][1] = m[0][0] * n[0][1] + m[0][1]*n[1][1] + m[0][2]*n[2][1];
	mTimesN[0][2] = m[0][0] * n[0][2] + m[0][1]*n[1][2] + m[0][2]*n[2][2];

	mTimesN[1][0] = m[1][0] * n[0][0] + m[1][1]*n[1][0] + m[1][2]*n[2][0];
	mTimesN[1][1] = m[1][0] * n[0][1] + m[1][1]*n[1][1] + m[1][2]*n[2][1];
	mTimesN[1][2] = m[1][0] * n[0][2] + m[1][1]*n[1][2] + m[1][2]*n[2][2];

	mTimesN[2][0] = m[2][0] * n[0][0] + m[2][1]*n[1][0] + m[2][2]*n[2][0];
	mTimesN[2][1] = m[2][0] * n[0][1] + m[2][1]*n[1][1] + m[2][2]*n[2][1];
	mTimesN[2][2] = m[2][0] * n[0][2] + m[2][1]*n[1][2] + m[2][2]*n[2][2];
}

/* Multiplies the 3x3 matrix m by the 3x1 matrix v. */
void mat331Multiply(double m[3][3], double v[3], double mTimesV[3]){
	mTimesV[0] = m[0][0] * v[0] + m[0][1]*v[1]+ m[0][2]*v[2];
	mTimesV[1] = m[1][0] * v[0] + m[1][1]*v[1] + m[1][2]*v[2];
	mTimesV[2] = m[2][0] * v[0] + m[2][1]*v[1] + m[2][2]*v[2];
}

/* Builds a 3x3 matrix representing 2D rotation and translation in homogeneous 
coordinates. More precisely, the transformation first rotates through the angle 
theta (in radians, counterclockwise), and then translates by the vector (x, y). 
*/
void mat33Isometry(double theta, double x, double y, double isom[3][3]){
	double r[3][3]; 
	r[0][0] = cos(theta);
	r[0][1] = -sin(theta);
	r[0][2] = 0;
	r[1][0] = sin(theta);
	r[1][1] = cos(theta);
	r[1][2] = 0;
	r[2][0] = 0;
	r[2][1] = 0;
	r[2][2] = 1;
	double t[3][3];
	t[0][0] = 1;
	t[0][1] = 0;
	t[0][2] = x;
	t[1][0] = 0;
	t[1][1] = 1;
	t[1][2] = y;
	t[2][0] = 0;
	t[2][1] = 0;
	t[2][2] = 1;
	mat333Multiply(t,r, isom);
}






