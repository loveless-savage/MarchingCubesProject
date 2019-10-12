#include "marchingCubes.hpp"

// access different parts of the mesh vertex ID's
#define AXISOF(idx) (idx&3)
#define PT1OF(idx) (idx>>2)
#define PT2OF(idx) (PT1OF(idx)|(1<<AXISOF(idx)))

using namespace std;

#if REFLECTION_COMPRESSION
// reflections to align the first zero in each index to the leftmost position
int reflectIdx(int idx, int axis){
	int shift = POW2(axis);
	int mask;
	switch(axis){
		case 0:
			mask = 0b01010101;//01010101;
			break;
		case 1:
			mask = 0b00110011;//00110011;
			break;
		case 2:
			mask = 0b00001111;//00001111;
			break;
		default:
			return idx;
	}
	return ((idx&mask)<<shift) + ((idx>>shift)&mask);
}

// reflections to align the corners adjacent to the leftmost zero by quantity
int swapAxisIdx(int idx, int axis1, int axis2){
	// degenerate case- trying to swap axis with itself! don't change idx
	if(axis1==axis2) return idx;

	// assemble a new index with the bits rearranged by swapped axes
	int tempIdx=0, reflBit;
	for(int bit=0;bit<POW2(DIMENSION);bit++){
		// swap axes in the bit counter
		reflBit=bit;
		reflBit &= POW2(DIMENSION)-1 -(1<<axis1) -(1<<axis2); // clear both focus bits with a bitmask
		reflBit |= getBit(bit,axis1)<<axis2; // exchange focus bits
		reflBit |= getBit(bit,axis2)<<axis1;

		// grab bit from idx and place it in tempIdx at position reflBit
		tempIdx |= getBit(idx,bit)<<reflBit;
	}
	return tempIdx;
}
#endif //#if REFLECTION_COMPRESSION

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//

// generate mesh with normals from given vector field
void MarchingCubes::genModel(){
	cout<<"GENERATING MODEL WITH MARCHING CUBES ALGORITHM"<<endl;

	// calculate all values that lie on grid points
	implicitFormula(cellNum, frameSize, pts, norms);
//syncThreads();
	cout<<"\t--> field equation evaluated for all grid points"<<endl;

	// gather pointers to the lookup table rather than copying at this point
	for(int k=0;k<cellNum;k++){
	for(int j=0;j<cellNum;j++){
	for(int i=0;i<cellNum;i++){
		// first find our index that describes our voxel's pass/fail permutation
		int idx = 0;
		for(int bit=0;bit<POW2(DIMENSION);bit++){
			idx |= pts
				[k+getBit(bit,2)] // iterate through all corners of the cube
				[j+getBit(bit,1)] // access each one's calculated scalar value
				[i+getBit(bit,0)] // then do a pass/fail test
				.w > 0 ? 1<<bit : 0; // if pass, flip the corresponding bit to 1
		}
		// at this point, idx == 0b10110001 or something
		// the leftmost bit of idx corresponds to the innermost corner of the grid cell

		// in the arrays, k j and i coordinates are condensed into one coordinate
		readLookupTable(idx, coord(k,j,i)); // TODO

	}}} // gather pointers to the lookup table
//syncThreads();
	cout<<"\t--> accessed pointers to lookup table"<<endl;
	
// this next section is serial! it cannot be parallelized
	// condense all values into one array
	for(int k=0;k<cellNum;k++){
	for(int j=0;j<cellNum;j++){
	for(int i=0;i<cellNum;i++){
		// keep track of how many total verts were found
		//if( coord(k,j,i) > 0 ){ // except the first voxel!
			//vertSums[coord(k,j,i)] = vertSums[coord(k,j,i)-1] + vertSizes[coord(k,j,i)-1];
		//}
		// loop through all mesh indices in this voxel
		for(int idxNum=0; idxNum<12; idxNum++){
			// write zero to all unused voxel meshes
			if( idxNum>=indexSizes[coord(k,j,i)] ){
				offsettedIndices[coord(k,j,i)][idxNum] = 0;
			}
			// for each mesh index, check if the associated vertex has been used in an adjacent voxel
			// then if it has, add an index pointing to that vertex
			// if not, add the new vertex
			pushIdxWithoutDups(k,j,i,idxNum);
		}
	}}} // condense all values into one array
	cout<<"\t--> meshes condensed into one set of buffers"<<endl;

	// at this point, all that remains is to set up OpenGL buffers to capture the data!
	setupGL();
	cout<<"\t--> OpenGL buffers set up"<<endl;
	cout<<"MARCHING CUBES MODEL GENERATED"<<endl;
	cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<endl;
};

#if 0
// diagnostic printout of the mesh
void MarchingCubes::diag(){
	cout<<"aw yiss"<<endl;
	for(int k=0;k<cellNum+1;k++){
		cout<<"\nmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm";
		cin.ignore(); // pause at each level
		for(int j=0;j<cellNum+1;j++){
			// vertices
			cout<<endl;
			for(int i=0;i<cellNum+1;i++){
				cout<<(j>0&&verts[k][j][i][1]!=-1?"|":" ")<<" ";
				cout<<(k>0&&verts[k][j][i][2]!=-1?"\\":" ")<<" ";
			}
			cout<<endl;
			// points
			for(int i=0;i<cellNum+1;i++){
				cout<<(i>0&&verts[k][j][i][0]!=-1?"=":" ")<<" ";
				cout<<(pts[k][j][i].w>0?" ":" ")<<" ";
			}
		}
	}
	cout<<endl;
}
#endif

// return a scaling matrix to normalize frame size
glm::mat4 MarchingCubes::frameScaleMatrix(){
	return glm::scale(vec3( 1.f/frameSize ));
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
// read from linear arrays with 3D coordinates
int MarchingCubes::coord(int k, int j, int i){
	return k*cellNum*cellNum+j*cellNum+i;
}
// read from vertsTmp[] using indices out of indicesTmp[]
char MarchingCubes::vertsTmp_byIdx(int k, int j, int i, int idxNum){
	return vertsTmp[coord(k,j,i)][ indicesTmp[coord(k,j,i)][idxNum] ];
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
// read from lookup table
void MarchingCubes::readLookupTable(int idx, int coord) {
#if REFLECTION_COMPRESSION
	int tempIdx = idx;
	// we want to find a failing corner, so if all corners in the grid cell pass we invert them
	if (idx == POW2(POW2(DIMENSION)) - 1) tempIdx = 0;

	// find the first failing corner
	int bit = POW2(DIMENSION);
	while (getBit(tempIdx, bit - 1)) bit--; // loop while the current corner is passing
	bit--; // now bit holds the position of the first failing corner

		   // here we reflect the grid cell until it puts the failing corner leftmost
		   // for each 0 in the binary expansion of bit, invert the corresponding dimension
	if (getBit(bit, 2) == 0) tempIdx = reflectIdx(tempIdx, 2);
	if (getBit(bit, 1) == 0) tempIdx = reflectIdx(tempIdx, 1);
	if (getBit(bit, 0) == 0) tempIdx = reflectIdx(tempIdx, 0);

	// analyze corners adjacent to the failing corner
	int idxPrefix = getBit(tempIdx, POW2(DIMENSION) - 1 - 1)
		+ (getBit(tempIdx, POW2(DIMENSION) - 1 - 2) << 1)
		+ (getBit(tempIdx, POW2(DIMENSION) - 1 - 4) << 2);

	/* we have 4 cases for our adjacent corners:
	* -> all the same (0's or 1's)
	* -> just one corner passes (0100, 100, 0001...)
	* -> two passing corners (1100, 101, 011...)
	* -> three passing corners (0111, 1011...)
	*     (not relevant in 3 dimensions)
	* we can compress these cases to 2 bits...
	* ... rather than the 4 or 5 necessary to store the states of leftmost & adjacent corners
	* this significantly reduces the size of the lookup table!
	*/
	// calculate the case number
	int caseNum = getBit(idxPrefix, 0)
		+ getBit(idxPrefix, 1)
		+ getBit(idxPrefix, 2);

	// figure out what axes to swap
	int axisBit = DIMENSION, axisOther = 0;
	int axisBit2 = 0, axisOther2 = 0; // if there are two swaps- might not happen
	switch (caseNum) {
	case 2: // put two passing corners in the x & y dimensions
			// we are doing two flips! use axisBit2
		axisBit2 = DIMENSION;
		// find the first passing corner
		while (getBit(idxPrefix, axisBit2 - 1) == 0) axisBit2--;
		axisBit2--;
		// is it already partially ordered? if so, skip rightmost bit
		if (getBit(idxPrefix, 0)) {
			axisOther2 = 1;
			axisOther = 1;
		}
		// swap first pair of axes!
		swapAxisIdx(tempIdx, axisBit2, axisOther2);

		// find the second failing corner- might be swapped to the same position!
		while (getBit(idxPrefix, axisBit - 1) == 0) axisBit--;
		axisBit--;
		// second swap is done outside the switch statement
		break;

	case 1: // put the passing corner in the x dimension
			// find the single passing corner
		while (getBit(idxPrefix, axisBit - 1) == 0) axisBit--;
		axisBit--;
		// swap the appropriate corner with the x axis corner
		axisOther = 0;
		break;

	default: // all 0's or all 1's
		axisBit = 0; // axes are the same, so nothing will happen
		break;
	}
	swapAxisIdx(tempIdx, axisBit, axisOther);
	// now that we have rotated our cube, we can actually read from the lookup table!
#endif // #if REFLECTION_COMPRESSION
	//lookupTable->diag(idx);

	// make the focus element of vertsTmp[] point to the correct entry of the lookup table
	vertsTmp[coord] = lookupTable->getVertexArray(idx);
	// same with indicesTmp
	indicesTmp[coord] = lookupTable->getIndexArray(idx);

	// we also want to remember the sizes of each array, since they vary
	vertSizes[coord] = lookupTable->getNumVerts(idx);
	indexSizes[coord] = lookupTable->getNumIndices(idx);
}

// ISSUE SOMEWHERE IN MarchingCubes::pushIdxWithoutDups(k,j,i,idxNum)?
// push a new index onto the indices vector, unless it is a duplicate
void MarchingCubes::pushIdxWithoutDups(int k, int j, int i, int idxNum){
	// first capture the ID of the focus vertex
	char focusVert = vertsTmp_byIdx(k,j,i,idxNum);

	/* eliminate duplicates
	 * each mesh vertex lies on a grid edge, so 4 voxels reference each mesh vertex
	 * (except edges on the outside border!)
	 * we always want to check for the vertex's occurence in the earliest processed voxel of the four
	 * so we go through each coordinate {k j i} and evaluate whether to subtract 1
	 * we won't if the focus voxel is on the border!
	 * we also don't subtract one from the coordinate that's parallel to the focus grid edge
	 * if we can switch to an adjacent voxel, then because it shares a cube edge,
	 * there geometrically MUST be a duplicate of our focus vertex in that voxel
	 */
	// place to store the possibly new coordinates
	int nk=k, nj=j, ni=i;
	// did we switch to another voxel to check for duplicates?
	bool inNewVoxel=false;
	// check that focus edge is not parallel to axis k, and that k!=0,
	// and that we are on the bottom edge of the voxel
	if( AXISOF(focusVert)!=2 && k!=0 && getBit(PT1OF(focusVert),2)==0 ){
		nk -= 1; // go to the voxel below
		focusVert |= 1<<4; // focus on the top of the new voxel instead of the bottom
		inNewVoxel = true;
	} 
	// check that focus edge is not parallel to axis j, and that j!=0,
	// and that we are on the back edge of the voxel
	if( AXISOF(focusVert)!=1 && j!=0 && getBit(PT1OF(focusVert),1)==0 ){
		nj -= 1; // go to the voxel behind
		focusVert |= 1<<3; // focus on the front of the new voxel instead of the back
		inNewVoxel = true;
	} 
	// check that focus edge is not parallel to axis i, and that i!=0,
	// and that we are on the back edge of the voxel
	if( AXISOF(focusVert)!=0 && i!=0 && getBit(PT1OF(focusVert),0)==0 ){
		ni -= 1; // go to the voxel behind
		focusVert |= 1<<2; // focus on the front of the new voxel instead of the back
		inNewVoxel = true;
	} 

	// with our new focus voxel, search through its indices for the duplicate vertex
	// by the way: the focus voxel could've not changed, so we might be in the same voxel!
	for(int subIdxNum=0; subIdxNum<indexSizes[coord(nk,nj,ni)]; subIdxNum++){
		// if we find a match, use that as the index to push on
		if( vertsTmp_byIdx(nk,nj,ni,subIdxNum) == focusVert ){
			// if we are still in the original voxel, did we reach the focus index?
			// in that case, stop the loop and go straight to the non-duplicate section
			if( !inNewVoxel && subIdxNum==idxNum ) break;

			// what's the global index number for this previously included copy?
			GLuint globalDupIdx = offsettedIndices[coord(nk,nj,ni)][subIdxNum];
			indices.push_back(globalDupIdx);
			// record this in the static-size array for future reference
			offsettedIndices[coord(k,j,i)][idxNum] = globalDupIdx;

			// and we don't have to push the vertex onto the master vertex buffer; it's already in there!
		}
	}
	

	// if we reach this point, there were no duplicates of this vertex in the entire model up to now
	indices.push_back( GLuint(verts.size()) );
	// record this number for future reference
	offsettedIndices[coord(k,j,i)][idxNum] = GLuint(verts.size());

	// push the focus vertex onto the vertex stack
	pushGlobalCoordinatesVertex(k,j,i,focusVert);
}

// push a new vertex onto the verts vector, first converting it to global space coordinates!
void MarchingCubes::pushGlobalCoordinatesVertex(int k, int j, int i, int idx){
	// width of a voxel
	float stepLength = 2*frameSize/cellNum;

	// convert the points of the coincident grid edge into vectors
	vec3 pt1 = vec3(0.f,0.f,0.f);
	if( getBit(PT1OF(idx),0)==1 ) pt1.x = 1.f; // offset each dimension as necessary
	if( getBit(PT1OF(idx),1)==1 ) pt1.y = 1.f;
	if( getBit(PT1OF(idx),2)==1 ) pt1.z = 1.f;

	vec3 pt2 = vec3(0.f,0.f,0.f);
	if( getBit(PT2OF(idx),0)==1 ) pt2.x = 1.f;
	if( getBit(PT2OF(idx),1)==1 ) pt2.y = 1.f;
	if( getBit(PT2OF(idx),2)==1 ) pt2.z = 1.f;

	// figure out how much of a voxel's length to offset the mesh vertex
	float scalar1 = pts[k+int(pt1.z)][j+int(pt1.y)][i+int(pt1.x)].w;
	float scalar2 = pts[k+int(pt2.z)][j+int(pt2.y)][i+int(pt2.x)].w;
	float offsetFraction = ( 0.f-scalar1 )/( scalar2-scalar1 );
	offsetFraction = 0.5; // TODO

	// interpolate position in space based on scalar values
	vec3 ptInterpolated = pt2-pt1;
	ptInterpolated *= stepLength * offsetFraction;

	// interpolate normals
	vec3 norm1 = norms[k+int(pt1.z)][j+int(pt1.y)][i+int(pt1.x)];
	vec3 norm2 = norms[k+int(pt2.z)][j+int(pt2.y)][i+int(pt2.x)];
	vec3 normInterpolated = norm1;
	normInterpolated += (norm2-norm1)*offsetFraction;

	// find the global coordinates of the voxel
	vec3 globalCoords = vec3( -frameSize );
	globalCoords.x += stepLength*i;
	globalCoords.y += stepLength*j;
	globalCoords.z += stepLength*k;

	// package all info together into a struct Vertex
	Vertex newVert;
	newVert.position_ = globalCoords + ptInterpolated;
	newVert.normal_ = normInterpolated;
	newVert.texcoords_ = vec2(0.f);
	verts.push_back(newVert);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
// set up OpenGL buffers
void MarchingCubes::setupGL(){
// set up VBO: all the vertices of the model in an array
	// make a buffer and store its ID in vbo_
	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	// copy the vertex coordinates into the buffer we just bound
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), &verts[0], GL_STATIC_DRAW);

// Vertex array object setup
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

// Elememt buffer object setup- stores indices to connect vertices
	glGenBuffers(1, &ebo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	// copy indices into our freshly baked element buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

// set up buffers available to the vertex shader
// glVertexAttribPointer( layout_location_#, buffer_element_size, type, normalized, stride, array_buffer_offset );
	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	// normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal_));
	// texture coordination
	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoords_));
}


// render the generated model from the GL wrapper
void MarchingCubes::Draw(GLuint program){
	// setup materials
	GLuint diffuse_count = 1;
	GLuint specular_count = 1;
	// attach all textures associated with the mesh
	/*
	for (GLuint i = 0; i < textures_.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		stringstream ss;
		string num;
		string type = textures_[i].type_;
		if (type == "texture_diffuse")
			ss << diffuse_count++;
		else if (type == "texture_specular")
			ss << specular_count++;
		num = ss.str();
		string mattype = "material." + type + num;
		GLuint loc = glGetUniformLocation(program, mattype.c_str());
		glUniform1i(loc, i);
		glBindTexture(GL_TEXTURE_2D, textures_[i].id_);
	}
	*/
	glActiveTexture(GL_TEXTURE0);

	// draw
	glBindVertexArray(vao_);
	//glDrawArrays(GL_TRIANGLES, 0, indices_.size());
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//real-time interpolation: deprecated
#if LOOKUP_TABLE == 0
// interpolate vertices
void MarchingCubes::interpVerts(int k, int j, int i, int axis){
	// pointers to grid points we want
	vec4 *mainpt=&(pts[k][j][i]), *otherpt;
	switch(axis){
		case 0: otherpt=&(pts[k][j][i-1]); break;
		case 1: otherpt=&(pts[k][j-1][i]); break;
		case 2: otherpt=&(pts[k-1][j][i]); break;
	}

	// if current gridline doesn't host a vertex
	if((mainpt->f<0)^(otherpt->f>0)){
		verts[k][j][i][axis]=-1;
		return;
	}

	// if vertices are different, interpolate
	float itpVal = mainpt->f - 0;
	itpVal /= mainpt->f - otherpt->f;

	verts[k][j][i][axis]= itpVal * frameSize/cellNum;
}

// interpolate lines
void MarchingCubes::interpEdges(int k, int j, int i, int axis1, int axis2){
	// capture which mesh vertices are active around the current grid square
	bool vvals[4];
	vvals[0]=(verts[k][j][i][0]!=-1);
	switch(axis1+4*axis2){
		case 1+(0<<2): case 0+(1<<2):
			vvals[1]=(verts[k][j][i-1][0]!=-1);
			vvals[2]=(verts[k][j-1][i][0]!=-1);
			vvals[3]=(verts[k][j-1][i-1][0]!=-1);
			break;
		case 2+(0<<2): case 0+(2<<2):
			vvals[1]=(verts[k][j][i-1][0]!=-1);
			vvals[2]=(verts[k-1][j][i][0]!=-1);
			vvals[3]=(verts[k-1][j][i-1][0]!=-1);
			break;
		case 2+(1<<2): case 1+(2<<2):
			vvals[1]=(verts[k][j-1][i][0]!=-1);
			vvals[2]=(verts[k-1][j][i][0]!=-1);
			vvals[3]=(verts[k-1][j-1][i][0]!=-1);
			break;
		default: // don't give the same axis twice
			cout<<endl<<"oops- you tried to give the same dimension to a plane twice!"
				<<endl<<"DEGENERACY ERROR"<<endl;
			exit(EXIT_FAILURE);
	}

	// collect all vertices on grid square borders
}

// interpolate triangles
void MarchingCubes::interpTris(int k, int j, int i, int axis_norm){
}
// interpolate simplices
void MarchingCubes::interpSimp(int k, int j, int i){
}
#endif
