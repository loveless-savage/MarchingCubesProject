#include <marchingCubes.hpp>

// convert grid coordinates to world space coordinates
#define G_TO_W(gcoord) (gcoord*frameSize/cellNum-frameSize/2)

using namespace std;

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
#if DIMENSION == 4
		case 3:
			mask = 0b0000000011111111;
			break;
#endif
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

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//

// generate mesh with normals from given vector field
void MarchingCubes::genModel(){
	cout<<"alright, let's get this model going";
#if DIMENSION == 4
	for(int l=0;l<cellNum+1;l++){
#endif
	for(int k=0;k<cellNum+1;k++){
		cout<<endl;
	for(int j=0;j<cellNum+1;j++){
		cout<<"|";
	for(int i=0;i<cellNum+1;i++){
		cout<<"p";
	// point level
		do_u(k,j,i);
#if LOOKUP_TABLE
	// grab mesh chunk from lookup table
		// first figure out what our index is based on current grid cell
		int idx = 0;
		for(int bit=0;bit<=POW2(DIMENSION);bit++){
			idx |= pts
#if DIMENSION == 4
				[l-getBit(bit,3)]
#endif
				[k-getBit(bit,2)] // jump around all corners of the cube
				[j-getBit(bit,1)]
				[i-getBit(bit,0)]
				.f > 0 ? 1<<bit : 0;
		}
		// at this point, idx == 0b10110001 or something
		// the leftmost bit of idx corresponds to the innermost corner of the grid cell

		readLookupTable(idx);

#else
	// line level: find possible vertices along 3 gridlines
		//if(l>0) interpLine(l,k,j,i,3); // interpolate down a hyperlayer
		if(k>0) interpVerts(k,j,i,2); // interpolate down a layer
		if(j>0) interpVerts(k,j,i,1); // interpolate back to previous row
		if(i>0) interpVerts(k,j,i,0); // interpolate back along the row
	// plane level: connect vertices with outer edges along 3 grid planes
		//if(l>0&&k>0) interpEdge(k,j,i,3,2);
		//if(l>0&&j>0) interpEdge(k,j,i,3,1);
		//if(l>0&&i>0) interpEdge(k,j,i,3,0);
		if(k>0&&j>0) interpEdges(k,j,i,2,1);
		if(k>0&&i>0) interpEdges(k,j,i,2,0);
		if(j>0&&i>0) interpEdges(k,j,i,1,0);
	// cube level: connect outer edges with triangles along 1 grid cube
		//if(l>0&&k>0&&j>0) interpTris(k,j,i,0);
		//if(l>0&&k>0&&i>0) interpTris(k,j,i,1);
		//if(l>0&&j>0&&i>0) interpTris(k,j,i,2);
		if(k>0&&j>0&&i>0) interpTris(k,j,i,3);
#endif
	}}}//}
	cout<<endl;
};

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

// vector field operator
void MarchingCubes::do_u(int k,int j,int i){
	// deduce actual world space coordinates from grid index
	float x,y,z;//,w;
	x = G_TO_W(i);
	y = G_TO_W(j);
	z = G_TO_W(k);
#if DIMENSION == 4
	w = G_TO_W(l);
#endif

	// vector equation
	pts[k][j][i].x = sin(x);
	pts[k][j][i].y = sin(y);
	pts[k][j][i].z = sin(z);
#if DIMENSION == 4
	pts[l][k][j][i].w = sin(w);
#endif
	// scalar equation
	pts[k][j][i].f = sin(x)+sin(y)+sin(z);
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
// read from lookup table
void MarchingCubes::readLookupTable(int idx){
	int tempIdx = idx;
#if REFL_LOOKUP
	// we want to find a failing corner, so if all corners in the grid cell pass we invert them
	if(idx == POW2(POW2(DIMENSION))-1) tempIdx=0;

	// find the first failing corner
	int bit = POW2(DIMENSION);
	while(getBit(tempIdx,bit-1)) bit--; // loop while the current corner is passing
	bit--; // now bit holds the position of the first failing corner

	// here we reflect the grid cell until it puts the failing corner leftmost
	// for each 0 in the binary expansion of bit, invert the corresponding dimension
#if DIMENSION == 4
	if(getBit(bit,3)==0) tempIdx=reflectIdx(tempIdx,3);
#endif
	if(getBit(bit,2)==0) tempIdx=reflectIdx(tempIdx,2);
	if(getBit(bit,1)==0) tempIdx=reflectIdx(tempIdx,1);
	if(getBit(bit,0)==0) tempIdx=reflectIdx(tempIdx,0);

	// analyze corners adjacent to the failing corner
	int idxPrefix = getBit(tempIdx,POW2(DIMENSION)-1-1)
				  + (getBit(tempIdx,POW2(DIMENSION)-1-2)<<1)
				  + (getBit(tempIdx,POW2(DIMENSION)-1-4)<<2)
#if DIMENSION == 4
				  + (getBit(tempIdx,POW2(DIMENSION)-1-8)<<3)
#endif
				  ;
	
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
				+ getBit(idxPrefix,2)
#if DIMENSION == 4
				+ getBit(idxPrefix,3)
#endif
				;

	// figure out what axes to swap
	int axisBit=DIMENSION,axisOther=0;
	int axisBit2=0,axisOther2=0; // if there are two swaps- might not happen
	switch(caseNum){
#if DIMENSION == 4
		case 3: // put the failing corner in the w dimension
			// find the single failing corner
			while(getBit(idxPrefix,axisBit-1)) axisBit--;
			axisBit--;
			// swap the appropriate corner with the w axis corner
			axisOther=3;
			break;
#endif
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
#endif //#if REFL_LOOKUP
	// first figure out how to store a mesh // TODO
	//lookupTable->getSize(tempIdx);
	//lookupTable->getArray(tempIdx);
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
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

