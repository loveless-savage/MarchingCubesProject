#include "cubeTable.hpp"

using namespace std;

// print the value of an edge in readable format, given face-based ID
void hrEdgeIdx(char idx){
	cout<<getBit(idx,7)<<":"
		<<getBit2(idx,5)<<(getBit(idx,4)?"+":"-")<<":"
		<<getBit2(idx,2)<<"-"<<getBit2(idx,0)<<" "
		<<flush;
}

// find a value in an array, starting at a given position
int findByIdx(char array[], int arraySize, char idx, int startPos){
	for(int j=startPos; j<arraySize; j++){
		// return once the desired value is found (ignoring the first, used/unused value)
		if((array[j]&0b01111111)==(idx&0b01111111)) return j;
	}
	cout<<endl<<"hoo boy, you have a broken loop! couldn't find index ";
	hrEdgeIdx(idx);
	cout<<endl<<"array= ";
	for(int i=0;i<arraySize;i++) hrEdgeIdx(array[i]);
	cout<<endl;
	exit(EXIT_FAILURE);
}

// return the number of elements in each micro-mesh
char CubeTable::getNumVerts(int idx){
	return table3D[2*idx][0];
}
char CubeTable::getNumIndices(int idx){
	return table3D[2*idx+1][0];
}
// return the location of the array of elements
char* CubeTable::getVertexArray(int idx){
	return &table3D[idx*2][1];
}
char* CubeTable::getIndexArray(int idx){
	return &table3D[idx*2+1][1];
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
void CubeTable::genTable2D(){
	/* when clarifying the placement of a mesh edge on a grid face, we give it an index.
	 * this index employs two chars in sequence in the array of edges.
	 * each char in the aforementioned pair clarifies an edge of the grid face like this:
	 * table2D[permutationIdx][i] = 0b1101;
	 * where the first two bits ^^
	 * ...and the last two bits   ^^ each name the number of a face corner
	 * face corners- and by deduction, edges- are numbered on a face like this:
	 * {0}-0001-{1}  .--> x
	 *  |        |  y|
	 * 0010      |   V
	 *  |      1101
	 *  |        |
	 * {2}-1110-{3}
	 * values for mVertexIdxInFace are store in *table2D[permutationIdx].
	*/
	// all corners the same
	// 00
	// 00
	table2D[0b0000] = new char[1]{0};
	// 11
	// 11
	table2D[0b1111] = new char[1]{0};

	// alternating pass/fail: trim off failing corners
	// 10
	// 01
	table2D[0b1001] = new char[1+2];
	table2D[0b1001][0] = 2;
	table2D[0b1001][1] = 0b0001;
	table2D[0b1001][2] = 0b1101;
	table2D[0b1001][3] = 0b0010;
	table2D[0b1001][4] = 0b1110;
	// 01
	// 10
	table2D[0b0110] = new char[1+2];
	table2D[0b0110][0] = 2;
	table2D[0b0110][1] = 0b0001;
	table2D[0b0110][2] = 0b0010;
	table2D[0b0110][3] = 0b1110;
	table2D[0b0110][4] = 0b1101;

	for(int i=0;i<4;i++){
		// 1 corner passing (4 cases)
		// 10 | 01 | 00 | 00
		// 00 | 00 | 10 | 01
		table2D[1<<i] = new char[1+1];
		table2D[1<<i][0] = 1;
		if(i==0 || i==3){ // corners 0 & 3 are always listed first in the bit field
			table2D[1<<i][1] = (i<<2) + (i^1);
			table2D[1<<i][2] = (i<<2) + (i^2);
		}else{
			table2D[1<<i][1] = i + ((i^1)<<2);
			table2D[1<<i][2] = i + ((i^2)<<2);
		}
		// 1 corner failing (4 cases)
		// 01 | 10 | 11 | 11
		// 11 | 11 | 01 | 10
		table2D[15-(1<<i)] = new char[1+1];
		table2D[15-(1<<i)][0] = 1;
		if(i==0 || i==3){ // corners 0 & 3 are always listed first in the bit field
			table2D[15-(1<<i)][1] = (i<<2) + (i^1);
			table2D[15-(1<<i)][2] = (i<<2) + (i^2);
		}else{
			table2D[15-(1<<i)][1] = i + ((i^1)<<2);
			table2D[15-(1<<i)][2] = i + ((i^2)<<2);
		}
	}

	// 2 adjacent corners passing
	// 11
	// 00
	table2D[0b1100] = new char[1+1];
	table2D[0b1100][0] = 1;
	table2D[0b1100][1] = 0b0010;
	table2D[0b1100][2] = 0b1101;
	// 00
	// 11
	table2D[0b0011] = new char[1+1];
	table2D[0b0011][0] = 1;
	table2D[0b0011][1] = 0b0010;
	table2D[0b0011][2] = 0b1101;
	// 10
	// 10
	table2D[0b1010] = new char[1+1];
	table2D[0b1010][0] = 1;
	table2D[0b1010][1] = 0b0001;
	table2D[0b1010][2] = 0b1110;
	// 01
	// 01
	table2D[0b0101] = new char[1+1];
	table2D[0b0101][0] = 1;
	table2D[0b0101][1] = 0b0001;
	table2D[0b0101][2] = 0b1110;
}

void CubeTable::genTable3D(){
	cout<<"ok let's make this table"<<endl;

	genTable2D();

	// loop for every possible 3D cube state, of which there are 256
	for(int stateIdxOfCube=0; stateIdxOfCube<256; stateIdxOfCube++){
		cout<<endl<<"state "<<stateIdxOfCube<<" ( "<<bitset<8>(stateIdxOfCube)<<" )";

		// set up array of vertices, indexed by face number
		char edgesByFace[4*6];

		// loop through all cube faces and extract 1D mesh edges that lie on the faces
		// each face has an index that describes the states of its corners
		int stateIdxOfFace;
		// how many mesh edges are on the current face? 0, 1, or 2
		int mEdgeCountInFace;
		// we need a counter to keep track of how many vertices we've collected so far
		int vNum3d=0;
		/* each 2D face of the cube has an index associated:
		 * faceIdxInCube = 0b011
		 *  where these two  ^^  bits store the normal axis: 00=x,01=y,10=z
		 *  ...and this bit    ^ stores if the face is next to root corner: 0=yes,1=no
		 * when identifying a mesh vertex in a grid cube:
		 * mVertexIdxInCube = faceIdxInCube<<4 + mVertexIdxInFace;
		 * (example)      = 0b0_011_1101
		 */
		//cout<<"\n\t\t faces: ";
		for(int faceIdxInCube=0; faceIdxInCube<0b110; faceIdxInCube++){
			//cout<<faceIdxInCube;
			// which dimensions do we use for the current face?
			int dimA, dimB;
			switch(faceIdxInCube>>1){
				case 0: dimA=2; dimB=4; break;
				case 1: dimA=4; dimB=1; break;
				case 2: dimA=1; dimB=2; break;
			}
			// extract corners of cube that coincide on current face
			if(faceIdxInCube&1){ // is the face opposite the origin corner?
				stateIdxOfFace = (getBit( stateIdxOfCube, 7-0 )) // TODO
							   + (getBit( stateIdxOfCube, 7-dimA )<<1)
							   + (getBit( stateIdxOfCube, 7-dimB )<<2)
							   + (getBit( stateIdxOfCube, 7-(dimA|dimB) )<<3);
			}else{ // if the face is adjacent to the origin corner
				stateIdxOfFace = (getBit( stateIdxOfCube, 0 ))
							   + (getBit( stateIdxOfCube, dimA )<<1)
							   + (getBit( stateIdxOfCube, dimB )<<2)
							   + (getBit( stateIdxOfCube, (dimA|dimB) )<<3);
			}

			// how many mesh vertices this face? 0, 1, or 2
			mEdgeCountInFace = table2D[stateIdxOfFace][0];
			// do nothing if the lookup array is empty
			if(mEdgeCountInFace==0) continue;

			// copy all vertices from 2D lookup table
			edgesByFace[vNum3d++] = table2D[stateIdxOfFace][1] | (faceIdxInCube<<4);
			edgesByFace[vNum3d++] = table2D[stateIdxOfFace][2] | (faceIdxInCube<<4);
			// sometimes there are two mesh edges to copy
			if(mEdgeCountInFace==2) {
				edgesByFace[vNum3d++] = table2D[stateIdxOfFace][3] | (faceIdxInCube<<4);
				edgesByFace[vNum3d++] = table2D[stateIdxOfFace][4] | (faceIdxInCube<<4);
			}
		}
		//cout<<"\tdone"<<endl<<flush; // faces:  done

		// print a table to show all the relevant values
		cout<<endl<<"  # | edgesByFace | congruent3D | faceto3Dcoord";
		for(int j=0;j<vNum3d;j++){
			cout<<endl<<" "<<((j<10)?" ":"");
			cout<<j<<" |   ";
			hrEdgeIdx(edgesByFace[j]);
			cout<<" |  ";
			hrEdgeIdx(congruentEdge3D(edgesByFace[j]));
			cout<<"  |  ";
			cout<<(faceCoordTo3DCoord(edgesByFace[j])>>2)<<"-";
			cout<<(faceCoordTo3DCoord(edgesByFace[j])&3);
		}
		cout<<endl<<endl;
		
		/* now condense vertices into vertsByCube[]
		 * currently, edgesByFace[] holds a bunch of duplicate vertices
		 * these vertices are grouped in pairs according to which ones are connected by mesh edges
		 * now we want to order all the vertices so they align into rings
		 * we start at the beginning of edgesByFace[] and loop through all vertices
		 * with each vertex not having been selected already, we push it onto vertsByCube[]
		 * vertsByCube is actually a pointer to the current location in the table
		 */
		// allocate one more char than we need for actual vertices...
		table3D[2*stateIdxOfCube] = new char[vNum3d/2+1];
		// ...because the first entry in each array tells how many vertices were allocated
		table3D[2*stateIdxOfCube][0] = vNum3d/2;
		// make a pointer to the second element, because that's where the vertices begin
		char* vertsByCube = &table3D[2*stateIdxOfCube][1];

		// every other position in the lookup table is an index table, OpenGL VAO style
		table3D[2*stateIdxOfCube+1] = new char[12+1]; // maximum of 4 mesh triangles per cube
		char* indicesByCube=&table3D[2*stateIdxOfCube+1][1];

		cout<<"rings found: "<<endl;
		// keep track of rings
		int ringStart=0, ringSize=0;
		// how many indices do we have?
		int numIndices=0;
		for(int i=0;i<vNum3d;i+=2){
			// don't count vertices if they've been counted already!
			if(getBit(edgesByFace[i],7)) continue;

			cout<<"new ring"<<endl;
			// if not yet counted, use as a seed to capture a ring
			int ringHead = i; // start at the current vertex
			ringSize=0; // reset reported size for the next ring

			do{ // ring loop!
				cout<<"\tringHead="<<ringHead<<" (vtx=";
				hrEdgeIdx(edgesByFace[ringHead]);
				cout<<")   ***change face"<<endl;

				// find coincident vertex
				ringHead = findByIdx( /*change face*/
					edgesByFace,vNum3d,
					congruentEdge3D(edgesByFace[ringHead]),
				i);
				// mark our new vertex as used
				edgesByFace[ringHead] ^= 1<<7;


				cout<<"\tringHead="<<ringHead<<" (vtx=";
				hrEdgeIdx(edgesByFace[ringHead]);
				cout<<")   ***same face";

				// add it to the ring, but convert it to universal cube coords
				vertsByCube[ringStart+ringSize] = faceCoordTo3DCoord( edgesByFace[ringHead] );
				ringSize++; // the ring is bigger now!

				// prep the mesh vertex connected to the current one on the same face
				/*same face*/
				ringHead ^=1; // always next to it in edgesByFace[]
				// mark our new vertex as used
				edgesByFace[ringHead] ^= 1<<7;

				cout<<"   (ringHead="<<ringHead<<",i="<<i<<")"<<endl;
			// this loop continues until the ring returns to the original vertex
			}while(ringHead != i); // ring loop!

			cout<<"{("<<ringSize<<") ";
			for(int j=ringStart; j<ringStart+ringSize; j++){
				cout<<(vertsByCube[j]>>2)<<"-"<<((vertsByCube[j]>>2)|(1<<(vertsByCube[j]&3)))<<" ";
			}
			cout<<"}"<<endl;

			// now with our completed ring, interpolate triangles to turn it into a mesh
			interpTris(ringStart, ringSize, indicesByCube, numIndices);
			// remember that we already took care of this ring
			ringStart += ringSize;
		}
		// now that rings are finished being interpolated, remember how many indices were used
		table3D[2*stateIdxOfCube+1][0] = numIndices;
		cout<<endl<<"rings done"<<endl;

		cout<<"vertsByCube: ";
		for(int j=0;j<vNum3d/2;j++) cout<<(vertsByCube[j]>>2)<<"-"<<((vertsByCube[j]>>2)|(1<<(vertsByCube[j]&3)))<<" ";
		cout<<endl<<"indicesByCube: ";
		for(int j=0;j<numIndices;j++) cout<<int(indicesByCube[j])<<" ";
	}
	cout<<endl<<"all cases taken care of!"<<endl;
}

// accessing parts of the face indices can be confusing! let's make some macros for readability
// these macros are explained inside CubeTable::CongruentEdge3D()
#define FACEIDOF(idx) getBit2(idx,5)
#define POLEOF(idx) getBit(idx,4)
#define CORNERAOF(idx) getBit2(idx,2)
#define CORNERBOF(idx) getBit2(idx,0)


// find the index for a given vertex when attached to another face
char CubeTable::congruentEdge3D(char idx){
	char otherIdx = idx & 0b10011111; // clear face ID bits, since it's definitely changing
	/* cube faces are arranged so their indices do this:
	 * (pole)                   +----+
	 * @--------+               |1  3|
	 * |\       |\      (pole 0)|0  2|
	 * | \      | \   -->  +----@----+----+----+
	 * |  +--------+  -->  |2  0|0  1|3  1|2  3|
	 * +--|-----+  |  -->  |3  1|2  3|2  0|0  1|
	 *  \ |      \ |  -->  +----+----+----@----+
	 *   \|       \|                 |1  0|(pole 1)
	 *    +--------@                 |3  2|
	 *           (pole)              +----+
	 * this way all the "0" face corners point into the cube "poles", uppermost/lowermost
	 * and all "3" face corners point away from the poles into the "equator"
	 *
	 * the passed value for idx is interpreted as follows:
	 * idx = 0b00011101 =  0b0-00-1---11-01
	 *  -unused ─────────────┘ ││ │   ││ ││
	 * grid face ID:           ││ │   ││ ││
	 *  -normal axis, 0 1 or 2 ┴┘ │   ││ ││
	 *  -adjacent pole, 0 or 1 ───┘   ││ ││
	 * edge ID(grid corners on either side of the mesh vertex):
	 *  -corner A, either 0 or 3 ─────┴┘ ││
	 *  -corner B, either 1 or 2 ────────┴┘
	 */

	/* the normal of an adjacent grid face is either 0 1 or 2, and different from the current face
	 * to figure out if it is +1 or -1 (modulo 3) from the current face, we look at the edge ID
	 * the diagram shows corner indices of one grid cube face, with face normals in the face centers:
	 *       +-----+
	 *       |01 11|          when corner A and corner B are both even or both odd,
	 *       | (0) |          add 1 to the face normal: 0->1,1->2,2->0
	 *       |00 10|
	 * +-----+-----+-----+    when corner A and corner B are different in even/odd-ness,
	 * |10 00|00 01|11 01|    subtract 1 from the face normal: 0->2,2->1,1->0
	 * | (2) | (1) | (2) |
	 * |11 01|10 11|10 00|    to see of corners A and B differ in even/odd-ness,
	 * +-----+-----+-----+    cross their last bits with an XOR (^) test
	 *       |11 01|          --> ( getBit(idx,2) ^ getBit(idx,0) )
	 *       | (0) |
	 *       |10 00|
	 *       +-----+
	 */
	if ( getBit(idx,2) ^ getBit(idx,0) ) { // differ --> subtract 1
		if(FACEIDOF(idx)==0){
			otherIdx |= 2<<5; // if face normal==0, wrap up to 2
		}else{
			otherIdx |= (FACEIDOF(idx)-1)<<5;
		}
	}else{ // same even/odd-ness --> add 1
		// if face normal==2, leave the new normal cleared
		if(FACEIDOF(idx)<2){ // ...the normal spot was already cleared though! do nothing if ==2
			otherIdx |= (FACEIDOF(idx)+1)<<5;
		}
	}

	/* if corner A=0, the adjacent face is on the same pole of the cube
	 * but if A=3, the face is on the opposite pole
	 * thus if the edge index has a 3 in it, we toggle the face ID hemisphere
	 */
	if(CORNERAOF(idx)==3) otherIdx ^= 0b00010000;

	/* all edges have A=0 for both faces adjacent to it, or A=3 for both faces
	 * since it's always the same, we NEVER HAVE TO CHANGE corner A
	 */

	/* on every edge of the cube, one of the face ID's gives B=1, and the other gives B=2
	 * thus we will always invert B from 1 to 2 and vice versa
	 */
	otherIdx ^= 0b00000011; // conveniently, 01 ^ 11 == 10 and vice versa

	return otherIdx;
}
// convert mVertexIdx from face-based to universal cube coords // TODO
char CubeTable::faceCoordTo3DCoord(char idx){
	/*
	 * the index for an edge of a face in the cube,
	 * which is the passed value for idx, is interpreted as follows:
	 * idx = 0b00011101 =  0b0-00-1---11-01
	 *  -unused ─────────────┘ ││ │   ││ ││
	 * grid face ID:           ││ │   ││ ││
	 *  -normal axis, 0 1 or 2 ┴┘ │   ││ ││
	 *  -adjacent pole, 0 or 1 ───┘   ││ ││
	 * edge ID(grid corners on either side of the mesh vertex):
	 *  -corner A, either 0 or 3 ─────┴┘ ││
	 *  -corner B, either 1 or 2 ────────┴┘
	 *
	 * the universal index for an edge in the cube is interpreted as follows:
	 * idx = 0b00011010 = 0b000-110-10
	 *  -unused ────────────┴┴┘ │││ ││
	 *  -root cube vertex, 0-6 ─┴┴┘ ││
	 *  -axis, 0 1 or 2 ────────────┴┘
	 * example:
	 *    0-------2
	 *    |\       \
	 *    | \       \
	 *    |  4-------6
	 *    1  ║focus  |
	 *     \ ║<--    |
	 *      \║edge   |
	 *       5-------7
	 *  the focus edge lies between cube corners 4 and 5
	 *  the root (smaller) vertex is saved = 4
	 *  the focus edge is parallel to axis 0 because vertices (5)-(4) = 2^(0)
	 *  thus idx= (4)<<2 + (0)<<0 = 0b000100-00 = 16
	 */

	// first calculate parallel axis
	char axis = ( FACEIDOF(idx) // face normal, found in face ID
		// if face corner A=3, swap face corner B between 1 & 2
		+ (CORNERAOF(idx)^CORNERBOF(idx)) // A^B
	) % 3; // 3 axes in a cyclic system

	// now figure out the root vertex
	char root;
	// root vtx depends on the hemisphere of the cube and corner A of the focus face
	// these are found at bits 0b00011100 of idx
	switch( (idx&0b00011100)>>2 ){
		// pole 0, corner A=0
		case 0b000:
			// easy- the edge touches pole 0!
			root = 0;
			break;
		// pole 0, corner A=3
		case 0b011:
			// root vtx is whichever cube corner corresponds to face corner B
			// this narrows it down to 1, 2 or 4
			// whether corner B is 1 or 2 tells us how much the axis differs from the corner
			root = 1<<( // 2 to the power of
				(FACEIDOF(idx)+CORNERBOF(idx))%3 // add face ID and corner B to get 0 1 or 2
				// (we add modulus operator because the axis numbers loop back around)
			);
			break;
		// pole 1, corner A=3
		case 0b111:
			// in this case, root vtx is again 1, 2 or 4
			// it matches the cube corner that corresponds to face corner A=3
			// because of this, the state of corner B doesn't change the value of root
			root = 1<<(FACEIDOF(idx));
			break;
		// pole 1, corner A=0
		case 0b100:
			// in this situation, face corner A corresponds to cube corner 7
			// root vtx corresponds to face corner B
			// either 6, 5 or 3... in other words (7-1), (7-2), or (7-4)
			// which of {1 2 4} is used? refer to case 0b011
			root = 1<<( // 2 to the power of
				(FACEIDOF(idx)+CORNERBOF(idx))%3 // add face ID and corner B to get 0 1 or 2
				// (we add modulus operator because the axis numbers loop back around)
			);
			root = (7-root); // invert
			break;
		// this will only happen if the face-based idx was calculated wrong
		default:
			cout<<endl<<"AAAAAAHHHH! corner A isn't 0 or 3, it's "<<CORNERAOF(idx)<<endl;
	}

	return (root<<2) + axis;
}
#if 0
char CubeTable::faceCoordTo3DCoord(char idx){
	/*
	 * the index for an edge of a face in the cube,
	 * which is the passed value for idx, is interpreted as follows:
	 * idx = 0b00011101 =  0b0-00-1---11-01
	 *  -unused ─────────────┘ ││ │   ││ ││
	 * grid face ID:           ││ │   ││ ││
	 *  -normal axis, 0 1 or 2 ┴┘ │   ││ ││
	 *  -adjacent pole, 0 or 1 ───┘   ││ ││
	 * edge ID(grid corners on either side of the mesh vertex):
	 *  -corner A, either 0 or 3 ─────┴┘ ││
	 *  -corner B, either 1 or 2 ────────┴┘
	 *
	 * the universal index for an edge in the cube is interpreted as follows:
	 * idx = 0b1011 =          0b10-11
	 *  -parallel axis, 0 1 or 2 ┴┘ ││
	 *  -stem face vtx, 0 1 2 or 3 ─┴┘
	 * the stem face is the face adjacent to pole 0 whose normal is parallel with the focus edge.
	 * since each face's vertices are numbered 0-3 relative to itself,
	 * the face vertex coinciding with the focus edge gives its relative face ID to the edge.
	 * example:
	 *
	 * (pole 0)
	 *    @-------+
	 *    |\ stem  \    @--------+
	 *    | \ face  \   |0      1|
	 *    |  X-------+  |  stem  | stem face vtx = 2
	 *    +  ║focus  |  |  face  |       Λ
	 *     \ ║<--    |  |2      3|       |
	 *      \║edge   |  X--------+       |
	 *       +-------@  └─coincident vtx=2
	 *
	 *  in this example, the focus edge is parallel to axis 1
	 *  thus idx= (1)<<2 + (2) = 0b01-10 = 6
	 */
	// calculate parallel axis
	char otherIdx = ( FACEIDOF(idx) + // face normal
		// if corner A=3, swap corner B between 1 & 2
		(CORNERAOF(idx)^CORNERBOF(idx)) // A^B
	) % 3; // 3 axes
	// push parallel axis value up two bytes to make room for stem face vtx
	otherIdx<<=2;

	// stem face vtx depends on the hemisphere of the cube and corner A of the focus face
	// these are found at bits 0b00011100 of idx
	switch( (idx&0b00011100)>>2 ){
		// pole 0, corner A=0
		case 0b000:
			otherIdx |= 0;
			break;
		// pole 1, corner A=0
		case 0b100:
			otherIdx |= 3;
			break;
		// pole 1, corner A=3
		case 0b111:
			// corner B will match the stem face vtx in this case
			otherIdx |= CORNERBOF(idx);
			break;
		// pole 0, corner A=3
		case 0b011:
			// invert corner B between 1 & 2
			otherIdx |= CORNERBOF(idx)^0b11;
			break;
		// this will only happen if the face-based idx was calculated wrong
		default:
			cout<<endl<<"AAAAAAHHHH! corner A isn't 0 or 3, it's "<<CORNERAOF(idx)<<endl;
	}

	return otherIdx;
}
#endif

// given a ring of mesh verts on a cube, interpolate them into a triangle mesh // TODO
void CubeTable::interpTris(int ringStart, int ringSize, char indicesByCube[], int& numIndices){
	// connect all edges of the ring to the first vertex via triangles
	// loop through all vertices in the ring, except the first (connecting point)...
	// ...and second (absorbed into triangle after)
	for(int i=2; i<ringSize; i++){
		// first vertex of the ring is connected to all the others via triangles
		indicesByCube[numIndices++] = ringStart;

		// connect the vertex before the focus vertex
		indicesByCube[numIndices++] = ringStart+(i-1);
		// connect the focus vertex
		indicesByCube[numIndices++] = ringStart+i;
		// now we have a triangle!
	}

}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
// lots of space to free from the heap when we're done!
void CubeTable::deleteTable(){
	cout<<"freeing memory"<<endl;
	int i=0;
// clear each array, smallest to largest

	cout<<"\t->table2D:  ";
	for(i=0;i<16;i++) {delete[] table2D[i]; cout<<"*"<<flush;}
	cout<<"done"<<endl;

	cout<<"\t->table3D:  ";
	for(i=0;i<256;i++) {delete[] table3D[i]; cout<<"*"<<flush;}
	cout<<"done"<<endl;
}
