#include "marchingCubes.hpp"

// convert grid coordinates to world space coordinates
#define G_TO_W(gcoord) (gcoord*frameSize/cellNum-frameSize/2)

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

// generate mesh with normals from given vector field // TODO
void MarchingCubes::genModel(){
	cout<<"alright, let's get this model going";

	// calculate all values that lie on grid points
	for(int k=0;k<cellNum+1;k++) for(int j=0;j<cellNum+1;j++) for(int i=0;i<cellNum+1;i++) do_u(k,j,i);
//syncThreads();

	// gather pointers to the lookup table rather than copying at this point
	for(int k=0;k<cellNum;k++){
	for(int j=0;j<cellNum;j++){
	for(int i=0;i<cellNum;i++){
		// first figure out what our index is based on current grid cell
		int idx = 0;
		for(int bit=0;bit<=POW2(DIMENSION);bit++){
			idx |= pts
				[k-1+getBit(bit,2)] // iterate through all corners of the cube
				[j-1+getBit(bit,1)] // access each one's calculated scalar value
				[i-1+getBit(bit,0)] // then do a pass/fail test
				.f > 0 ? 1<<bit : 0; // if pass, flip the corresponding bit to 1
		}
		// at this point, idx == 0b10110001 or something
		// the leftmost bit of idx corresponds to the innermost corner of the grid cell

		// in the arrays, k j and i coordinates are condensed into one coordinate
		readLookupTable(idx, k*cellNum*cellNum + j*cellNum + i);

	}}} // gather pointers to the lookup table
//syncThreads();
	
// this next section is serial! it cannot be parallelized
	// condense all values into one array
	for(int k=0;k<cellNum;k++){
	for(int j=0;j<cellNum;j++){
	for(int i=0;i<cellNum;i++){
		// keep track of how many total verts were found
		if( coord(k,j,i) > 0 ){ // except the first voxel!
			vertSums[coord(k,j,i)] = vertSums[coord(k,j,i)-1] + vertSizes[coord(k,j,i)];
		}
		// loop through all mesh indices in this voxel
		for(int idxNum=0; idxNum<indexSizes[coord(k,j,i)]; idxNum++){
			// for each mesh index, check if the associated vertex has been used in an adjacent voxel
			// then if it has, add an index pointing to that vertex
			// if not, add the new vertex
			pushIdxWithoutDups(k,j,i,idxNum);
		}
	}}} // condense all values into one array each
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
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
				cout<<(pts[k][j][i].f>0?" ":" ")<<" ";
			}
		}
	}
	cout<<endl;
}
#endif

// read from linear arrays with 3D coordinates
int MarchingCubes::coord(int k, int j, int i){ return k*cellNum*cellNum+j*cellNum+i ; }
// read from vertsTmp[] using indices out of indicesTmp[]
char MarchingCubes::vertsTmp_byIdx(int k, int j, int i, int idxNum){
	return vertsTmp[coord(k,j,i)][ indicesTmp[coord(k,j,i)][idxNum] ];
}

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
	 * we also don't subtract one from the coordinate parallel to the focus grid edge
	 */
	// check that focus edge is not parallel to axis k, and that k!=0,
	// and that we are on the bottom edge of the voxel
	if( AXISOF(focusVert)!=2 && k!=0 && getBit(PT1OF(focusVert),2)==0 ){
		k -= 1; // go to the voxel below
		focusVert |= 1<<4; // focus on the top of the new voxel instead of the bottom
	} 
	// check that focus edge is not parallel to axis j, and that j!=0,
	// and that we are on the back edge of the voxel
	if( AXISOF(focusVert)!=1 && j!=0 && getBit(PT1OF(focusVert),1)==0 ){
		j -= 1; // go to the voxel behind
		focusVert |= 1<<3; // focus on the front of the new voxel instead of the back
	} 
	// check that focus edge is not parallel to axis i, and that i!=0,
	// and that we are on the back edge of the voxel
	if( AXISOF(focusVert)!=0 && i!=0 && getBit(PT1OF(focusVert),0)==0 ){
		i -= 1; // go to the voxel behind
		focusVert |= 1<<2; // focus on the front of the new voxel instead of the back
	} 

	// with our new focus voxel, search through its indices for the duplicate vertex
	for(int subIdxNum=0; subIdxNum< 0/*TODO*/; subIdxNum++){
	}

	// if no duplicates were successfully found, we must push the focus vertex onto the vertex stack
	pushGlobalCoordinatesVertex(k,j,i,focusVert);
	
	/*
	pushIdxWithoutDups(k,j,i,idxNum);
	// check for whether this vertex has been used before // TODO
	char current_vtx = vertsTmp_byIdx(k,j,i,idxNum);
	// it wasn't used for certain if it is on the outer edge of the voxel
	if( PT2OF(current_vtx)==7 // voxels are indexed with the 0 pole facing in
	// ...or if it's on the bottom edge of the entire frame
		|| (PT1OF(current_vtx)==0 && k*j*i==0)
	){}
	// add the index to the cumulative vector
	indices.push_back(indicesTmp[coord(k,j,i)][idxNum]);
	*/
}

// push a new vertex onto the verts vector, first converting it to global space coordinates! // TODO
void MarchingCubes::pushGlobalCoordinatesVertex(int k, int j, int i, int idx){
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
// vector field operator
void MarchingCubes::do_u(int k,int j,int i){
	// deduce actual world space coordinates from grid index
	float x,y,z;//,w;
	x = G_TO_W(i);
	y = G_TO_W(j);
	z = G_TO_W(k);

	// vector equation
	pts[k][j][i].x = sin(x);
	pts[k][j][i].y = sin(y);
	pts[k][j][i].z = sin(z);
	// scalar equation
	pts[k][j][i].f = sin(x)+sin(y)+sin(z);
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
// read from lookup table
void MarchingCubes::readLookupTable(int idx, int coord){
#if REFLECTION_COMPRESSION
	int tempIdx = idx;
	// we want to find a failing corner, so if all corners in the grid cell pass we invert them
	if(idx == POW2(POW2(DIMENSION))-1) tempIdx=0;

	// find the first failing corner
	int bit = POW2(DIMENSION);
	while(getBit(tempIdx,bit-1)) bit--; // loop while the current corner is passing
	bit--; // now bit holds the position of the first failing corner

	// here we reflect the grid cell until it puts the failing corner leftmost
	// for each 0 in the binary expansion of bit, invert the corresponding dimension
	if(getBit(bit,2)==0) tempIdx=reflectIdx(tempIdx,2);
	if(getBit(bit,1)==0) tempIdx=reflectIdx(tempIdx,1);
	if(getBit(bit,0)==0) tempIdx=reflectIdx(tempIdx,0);

	// analyze corners adjacent to the failing corner
	int idxPrefix = getBit(tempIdx,POW2(DIMENSION)-1-1)
				  + (getBit(tempIdx,POW2(DIMENSION)-1-2)<<1)
				  + (getBit(tempIdx,POW2(DIMENSION)-1-4)<<2);
	
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
	int caseNum = getBit(idxPrefix,0)
				+ getBit(idxPrefix,1)
				+ getBit(idxPrefix,2);

	// figure out what axes to swap
	int axisBit=DIMENSION,axisOther=0;
	int axisBit2=0,axisOther2=0; // if there are two swaps- might not happen
	switch(caseNum){
		case 2: // put two passing corners in the x & y dimensions
			// we are doing two flips! use axisBit2
			axisBit2=DIMENSION;
			// find the first passing corner
			while(getBit(idxPrefix,axisBit2-1)==0) axisBit2--;
			axisBit2--;
			// is it already partially ordered? if so, skip rightmost bit
			if(getBit(idxPrefix,0)){
				axisOther2=1;
				axisOther=1;
			}
			// swap first pair of axes!
			swapAxisIdx(tempIdx,axisBit2,axisOther2);

			// find the second failing corner- might be swapped to the same position!
			while(getBit(idxPrefix,axisBit-1)==0) axisBit--;
			axisBit--;
			// second swap is done outside the switch statement
			break;

		case 1: // put the passing corner in the x dimension
			// find the single passing corner
			while(getBit(idxPrefix,axisBit-1)==0) axisBit--;
			axisBit--;
			// swap the appropriate corner with the x axis corner
			axisOther=0;
			break;

		default: // all 0's or all 1's
			axisBit=0; // axes are the same, so nothing will happen
			break;
	}
	swapAxisIdx(tempIdx,axisBit,axisOther);
	// now that we have rotated our cube, we can actually read from the lookup table!
#endif // #if REFLECTION_COMPRESSION
	// make the focus element of vertsTmp[] point to the correct entry of the lookup table
	vertsTmp[coord] = lookupTable->getVertexArray(idx);
	// same with indicesTmp
	indicesTmp[coord] = lookupTable->getIndexArray(idx);

	// we also want to remember the sizes of each array, since they vary
	vertSizes[coord] = lookupTable->getNumVerts(idx);
	indexSizes[coord] = lookupTable->getNumIndices(idx);
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//real-time interpolation
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
