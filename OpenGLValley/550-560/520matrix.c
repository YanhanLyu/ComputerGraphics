/*Author: Yanhan Lyu*/

/*** 2 x 2 Matrices ***/

/* Pretty-prints the given matrix, with one line of text per row of matrix. */


/* Returns the determinant of the matrix m. If the determinant is 0.0, then the 
matrix is not invertible, and mInv is untouched. If the determinant is not 0.0, 
then the matrix is invertible, and its inverse is placed into mInv. */
GLdouble mat22Invert(GLdouble m[2][2], GLdouble mInv[2][2]) {
	GLdouble det = m[0][0]*m[1][1] - m[0][1]*m[1][0];
	if (det != 0){
		GLdouble scalar = 1/det;
		mInv[0][0] = scalar*m[1][1];
		mInv[0][1] = -scalar*m[0][1];
		mInv[1][0] = -scalar*m[1][0];
		mInv[1][1] = scalar*m[0][0];
	}
	return det;
}

/* Multiplies a 2x2 matrix m by a 2-column v, storing the result in mTimesV. 
The output should not */
void mat221Multiply(GLdouble m[2][2], GLdouble v[2], GLdouble mTimesV[2]) {
	mTimesV[0] = m[0][0]*v[0]+m[0][1]*v[1];
	mTimesV[1] = m[1][0]*v[0]+m[1][1]*v[1];
}

/* Fills the matrix m from its two columns. */
void mat22Columns(GLdouble col0[2], GLdouble col1[2], GLdouble m[2][2]) {
	m[0][0] = col0[0];
	m[1][0] = col0[1];
	m[0][1] = col1[0];
	m[1][1] = col1[1];
}

/* Multiplies the 3x3 matrix m by the 3x3 matrix n. */
void mat333Multiply(GLdouble m[3][3], GLdouble n[3][3], GLdouble mTimesN[3][3]){
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
void mat331Multiply(GLdouble m[3][3], GLdouble v[3], GLdouble mTimesV[3]){
	mTimesV[0] = m[0][0] * v[0] + m[0][1]*v[1]+ m[0][2]*v[2];
	mTimesV[1] = m[1][0] * v[0] + m[1][1]*v[1] + m[1][2]*v[2];
	mTimesV[2] = m[2][0] * v[0] + m[2][1]*v[1] + m[2][2]*v[2];
}

/* Builds a 3x3 matrix representing 2D rotation and translation in homogeneous 
coordinates. More precisely, the transformation first rotates through the angle 
theta (in radians, counterclockwise), and then translates by the vector (x, y). 
*/
void mat33Isometry(GLdouble theta, GLdouble x, GLdouble y, GLdouble isom[3][3]){
	GLdouble r[3][3] = {
        {cos(theta), -sin(theta), 0},
        {sin(theta), cos(theta), 0},
        {0, 0, 1},
    };
    GLdouble t[3][3] = {
        {1, 0, x},
        {0, 1, y},
        {0, 0, 1},
    };
	mat333Multiply(t,r, isom);
}
/*3*3 matrix addition*/
void mat33Add(GLdouble u[3][3], GLdouble v[3][3], GLdouble uPlusV[3][3]){
	for (int i = 0; i<3; i+=1){
		for (int j=0; j<3; j+=1){
			uPlusV[i][j] = u[i][j] + v[i][j];
		} 	
	}
}
/*3*3 matrix scale*/
void matrixScale(GLdouble s, GLdouble m[3][3], GLdouble sTimesM[3][3]) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			sTimesM[i][j] = m[i][j]*s;
		}
	}
}

/* Given a length-1 3D vector axis and an angle theta (in radians), builds the 
rotation matrix for the rotation about that axis through that angle. Based on 
Rodrigues' rotation formula R = I + (sin theta) U + (1 - cos theta) U^2. */
void mat33AngleAxisRotation(GLdouble theta, GLdouble axis[3], GLdouble rot[3][3]){
	GLdouble axisU[3][3] = {{0,-axis[2],axis[1]},
						{axis[2], 0, -axis[0]},
						{-axis[1], axis[0], 0}};
	GLdouble identity[3][3] = {{1,0,0},
							 {0,1,0}, 
							 {0,0,1}};
	GLdouble axisUSquare[3][3];
	mat333Multiply(axisU, axisU, axisUSquare);
	GLdouble sinthetaU[3][3];
	matrixScale(sin(theta), axisU, sinthetaU);
	GLdouble oneMinusCosthetaUSquare[3][3];
	matrixScale(1-cos(theta), axisUSquare, oneMinusCosthetaUSquare);
	GLdouble addition[3][3];
	mat33Add(sinthetaU, oneMinusCosthetaUSquare, addition);
	mat33Add(addition, identity, rot);
}

void mat33Identity(GLdouble m[3][3]){
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			if (i == j){
				m[i][j] = 1;
			} else{
				m[i][j] = 0;
			}
		}
	}
}
void mat44Identity(GLdouble m[4][4]){
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			if (i == j){
				m[i][j] = 1;
			} else{
				m[i][j] = 0;
			}
		}
	}
}





/*calculate the transpose of a 3*3 matrix u*/
void mat33Transpose(GLdouble u[3][3], GLdouble transposeU[3][3]){
	for (int i = 0; i<3; i+=1){
		for (int j=0; j<3; j+=1){
			transposeU[j][i] = u[i][j];
		} 	
	}
}

/* Given two length-1 3D vectors u, v that are perpendicular to each other. 
Given two length-1 3D vectors a, b that are perpendicular to each other. Builds 
the rotation matrix that rotates u to a and v to b. */
void mat33BasisRotation(GLdouble u[3], GLdouble v[3], GLdouble a[3], GLdouble b[3], 
        GLdouble rot[3][3]){
	GLdouble uVCross[3];
	vec3Cross(u, v, uVCross);
	GLdouble r[3][3] = {
		{u[0], v[0], uVCross[0]},
		{u[1], v[1], uVCross[1]},
		{u[2], v[2], uVCross[2]},
	};

	GLdouble aBCross[3];
	vec3Cross(a, b, aBCross);
	GLdouble s[3][3] = {
		{a[0], b[0], aBCross[0]},
		{a[1], b[1], aBCross[1]},
		{a[2], b[2], aBCross[2]},
	};
	GLdouble rT[3][3];
	mat33Transpose(r, rT);
	mat333Multiply(s, rT, rot);
}

/* Multiplies m by n, placing the answer in mTimesN. */
void mat444Multiply(GLdouble m[4][4], GLdouble n[4][4], GLdouble mTimesN[4][4]){
	for (int i = 0; i < 4; i+=1) {
		for (int j = 0; j < 4; j+=1) {
			mTimesN[i][j] = m[i][0]*n[0][j] + m[i][1]*n[1][j] + m[i][2]*n[2][j] + m[i][3]*n[3][j];
		}
	}
}

/* Multiplies m by v, placing the answer in mTimesV. */
void mat441Multiply(GLdouble m[4][4], GLdouble v[4], GLdouble mTimesV[4]){
	for (int i = 0; i < 4; i+=1) {
		mTimesV[i] = m[i][0]*v[0] + m[i][1]*v[1] + m[i][2]*v[2] + m[i][3]*v[3];
	}

}

/* Given a rotation and a translation, forms the 4x4 homogeneous matrix 
representing the rotation followed in time by the translation. */
void mat44Isometry(GLdouble rot[3][3], GLdouble trans[3], GLdouble isom[4][4]){
	GLdouble rotNew[4][4] = {
		{rot[0][0], rot[0][1], rot[0][2], 0},
		{rot[1][0], rot[1][1], rot[1][2], 0},
		{rot[2][0], rot[2][1], rot[2][2], 0},
		{0,          0,         0,        1}
	};
	GLdouble transNew[4][4] = {
		{1, 0, 0, trans[0]},
		{0, 1, 0, trans[1]},
		{0, 0, 1, trans[2]},
		{0, 0, 0, 1}
	};
	mat444Multiply(transNew, rotNew, isom);
}

/*calculate the transpose of a 4*4 matrix m*/
void mat44Transpose(GLdouble m[4][4], GLdouble t[4][4]){
	for (int i = 0; i<4; i+=1){
		for (int j=0; j<4; j+=1){
			t[j][i] = m[i][j];
		} 	
	}

}

/*pack the matrix into the array*/
void packArray(GLdouble v[4][4], GLdouble unif[]){
	unif[0] = v[0][0];
	unif[1] = v[0][1];
	unif[2] = v[0][2];
	unif[3] = v[0][3];

	unif[4] = v[1][0];
	unif[5] = v[1][1];
	unif[6] = v[1][2];
	unif[7] = v[1][3];

	unif[8] = v[2][0];
	unif[9] = v[2][1];
	unif[10] = v[2][2];
	unif[11] = v[2][3];

	unif[12] = v[3][0];
	unif[13] = v[3][1];
	unif[14] = v[3][2];
	unif[15] = v[3][3];
}

/* Given a rotation and translation, forms the 4x4 homogeneous matrix 
representing the inverse translation followed in time by the inverse rotation. 
That is, the isom produced by this function is the inverse to the isom 
produced by mat44Isometry on the same inputs. */
void mat44InverseIsometry(GLdouble rot[3][3], GLdouble trans[3], 
        GLdouble isom[4][4]){
	GLdouble rotTranspose[4][4];
	GLdouble transInverse[4][4] = {
		{1, 0, 0, -trans[0]},
		{0, 1, 0, -trans[1]},
		{0, 0, 1, -trans[2]},
		{0, 0, 0, 1}
	};
	GLdouble rotNew[4][4] = {
		{rot[0][0], rot[0][1], rot[0][2], 0},
		{rot[1][0], rot[1][1], rot[1][2], 0},
		{rot[2][0], rot[2][1], rot[2][2], 0},
		{0, 0, 0, 1}
	};
	mat44Transpose(rotNew, rotTranspose);
	mat444Multiply(rotTranspose, transInverse, isom);
}

/* Builds a 4x4 matrix representing orthographic projection with a boxy viewing 
volume [left, right] x [bottom, top] x [far, near]. That is, on the near plane 
the box is the rectangle R = [left, right] x [bottom, top], and on the far 
plane the box is the same rectangle R. Keep in mind that 0 > near > far. Maps 
the viewing volume to [-1, 1] x [-1, 1] x [-1, 1]. */
void mat44Orthographic(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, 
        GLdouble far, GLdouble near, GLdouble proj[4][4]) {
	proj[0][0] = 2/(right-left);
	proj[0][1] = 0;
	proj[0][2] = 0;
	proj[0][3] = (-right-left)/(right-left);
	proj[1][0] = 0;
	proj[1][1] = 2/(top-bottom);
	proj[1][2] = 0;
	proj[1][3] = (-top-bottom)/(top-bottom);
    proj[2][0] = 0;
    proj[2][1] = 0;
    proj[2][2] = 2/(near-far);
    proj[2][3] = (-near-far)/(near-far);
	proj[3][0] = 0;
	proj[3][1] = 0;
	proj[3][2] = 0;
	proj[3][3] = 1;
}

/* Builds a 4x4 matrix that maps a projected viewing volume 
[-1, 1] x [-1, 1] x [-1, 1] to screen [0, w - 1] x [0, h - 1] x [-1, 1]. */
void mat44Viewport(GLdouble width, GLdouble height, GLdouble view[4][4]) {
    view[0][0] = (width-1)/2;
	view[0][1] = 0;
	view[0][2] = 0;
	view[0][3] = (width-1)/2;
	view[1][0] = 0;
	view[1][1] = (height-1)/2;
	view[1][2] = 0;
	view[1][3] = (height-1)/2;
    view[2][0] = 0;
    view[2][1] = 0;
    view[2][2] = 1;
    view[2][3] = 0;
	view[3][0] = 0;
	view[3][1] = 0;
	view[3][2] = 0;
	view[3][3] = 1;
}

/* Builds a 4x4 matrix representing perspective projection. The viewing frustum 
is contained between the near and far planes, with 0 > near > far. On the near 
plane, the frustum is the rectangle R = [left, right] x [bottom, top]. On the 
far plane, the frustum is the rectangle (far / near) * R. Maps the viewing 
volume to [-1, 1] x [-1, 1] x [-1, 1]. */
void mat44Perspective(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, 
        GLdouble far, GLdouble near, GLdouble proj[4][4]) {
    proj[0][0] = (-2*near)/(right-left);
	proj[0][1] = 0;
	proj[0][2] = (right+left)/(right-left);
	proj[0][3] = 0;
	proj[1][0] = 0;
	proj[1][1] = (-2*near)/(top-bottom);
	proj[1][2] = (top+bottom)/(top-bottom);
	proj[1][3] = 0;
    proj[2][0] = 0;
    proj[2][1] = 0;
    proj[2][2] = (-near-far)/(near-far);
    proj[2][3] = (2*near*far)/(near-far);
	proj[3][0] = 0;
	proj[3][1] = 0;
	proj[3][2] = -1;
	proj[3][3] = 0;
}

/* We convert the stored one-row-after-another C matrice into
one-column-after-another OpenGL matrice */
void mat44OpenGL(GLdouble m[4][4], GLfloat openGL[4][4]) {
	for (int i = 0; i < 4; i += 1)
		for (int j = 0; j < 4; j += 1)
			openGL[i][j] = m[j][i];
}
