#include <cubeTable.hpp>

using namespace std;

// find a value in an array, starting at a given position
int findByIdx(char array[], int arraySize, char idx, int startPos){
	for(int j=startPos; j<arraySize; j++){
		if(array[j]|(1<<7)==idx|(1<<7)) return j;
	}
	cout<<endl<<"hoo boy, you have a broken loop! couldn't find index "<<bitset<8>(idx)<<endl;
	cout<<"array= ";
	for(int i=0;i<arraySize;i++) cout<<bitset<8>(array[i])<<"  ";
	cout<<endl;
	exit(EXIT_FAILURE);
}

// return the number of elements in each micro-mesh
char CubeTable::getSize(int idx){
#if DIMENSION == 4
	return table4D[idx][0];
#else
	return table3D[idx][0];
#endif
}
// return the location of the array of elements
char* CubeTable::getArray(int idx){
#if DIMENSION == 4
	return &table4D[idx][1];
#else
	return &table3D[idx][1];
#endif
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
	cout<<"2D done"<<endl<<flush;

	// loop for every possible 3D cube state, of which there are 256
	for(int stateIdxOfCube=0; stateIdxOfCube<256; stateIdxOfCube++){
		cout<<endl<<"state "<<stateIdxOfCube<<flush;

		// set up array of vertices, indexed by face number
		char edgesByFace[4*6];
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
		cout<<"\n\t\t"<<flush;
		for(int faceIdxInCube=0; faceIdxInCube<0b110; faceIdxInCube++){
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
		cout << "\tall faces collected " << endl << "edges:";
		for (int j = 0; j < vNum3d; j++) cout <<" "<< bitset<8>(edgesByFace[j]);
		cout << flush;
		
		/* now condense vertices into vertsByCube[]
		 * currently, edgesByFace[] holds a bunch of duplicate vertices
		 * these vertices are grouped in pairs according to which ones are connected by mesh edges
		 * now we want to order all the vertices so they align into rings
		 * we start at the beginning of edgesByFace[] and loop through all vertices
		 * with each vertex not having been selected already, we push it onto vertsByCube[]
		 * vertsByCube is actually a pointer to the current location in the table
		 */
		table3D[stateIdxOfCube] = new char[vNum3d/2+1];
		char* vertsByCube = table3D[stateIdxOfCube];
		cout<<"\n\tverts compressed: "<<flush;
		// keep track of rings
		int ringStart=1, ringSize=0;
		for(int i=0;i<vNum3d;i+=2){
			// don't count vertices if they've been counted already!
			if(getBit(edgesByFace[i],7)) continue;
			// if not yet counted, use as a seed to capture a ring
			int ringHead = i; // start at the current vertex
			ringSize=0;
			/*ERROR HERE*/
			do{ // ring loop!
				// find coincident vertex
				ringHead = findByIdx(
					edgesByFace,(vNum3d),
					congruentEdge3D(edgesByFace[ringHead]),
				i);
				// mark our new vertex as used
				edgesByFace[ringHead] |= 1<<7;
				// add it to the ring, but convert it to universal cube coords
				vertsByCube[ringStart+ringSize] = edgesByFace[ringHead]; // TODO
				ringSize++; // the ring is bigger now!

				// prep the mesh vertex connected to the current one on the same face
				ringHead ^=1; // always next to it in edgesByFace[]
				edgesByFace[ringHead] |= 1 << 7; // used vertex
				
				cout << " ringHead="<<ringHead<<" i="<<i<< " edge=" << bitset<8>(edgesByFace[ringHead]) <<" "<< bitset<8>(edgesByFace[0]) << flush;

			// this loop continues until the ring returns to the original vertex
			}while(ringHead != i); // ring loop!

			// now with our completed ring, interpolate triangles to turn it into a mesh
			interpTris(vertsByCube, ringStart, ringSize);
			// remember that we already took care of this ring
			ringStart += ringSize;

		}
		cout<<"\trings found" << endl << "edges:";
		for (int j = 0; j < vNum3d; j++) cout << " " << bitset<8>(edgesByFace[j]);
		cout << flush;
	}
	cout<<"all cases taken care of!"<<endl;
}
// find the index for a given vertex when attached to another face
char CubeTable::congruentEdge3D(char idx){
	char otherIdx = idx & 0b10011111; // clear face ID bits, since it's definitely changing
	/* cube faces are arranged so their indices do this:
	 * (pole)                   +----+
	 * @--------+               |2  3|
	 * |\       |\        (pole)|0  1|
	 * | \      | \   -->  +----@----+----+----+
	 * |  +--------+  -->  |1  0|0  2|3  2|1  3|
	 * +--|-----+  |  -->  |3  2|1  3|1  0|0  2|
	 *  \ |      \ |  -->  +----+----+----@----+
	 *   \|       \|                 |2  0|(pole)
	 *    +--------@                 |3  1|
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
	 *       |10 11|          when corner A and corner B are both even or both odd,
	 *       | (2) |          add 1 to the face normal: 0->1,1->2,2->0
	 *       |00 01|
	 * +-----+-----+-----+    when corner A and corner B are different in even/odd-ness,
	 * |01 00|00 10|11 10|    subtract 1 from the face normal: 0->2,2->1,1->0
	 * | (0) | (1) | (0) |
	 * |11 10|01 11|01 00|    to see of corners A and B differ in even/odd-ness,
	 * +-----+-----+-----+    cross their last bits with an XOR (^) test
	 *       |11 10|          --> ( getBit(idx,2) ^ getBit(idx,0) )
	 *       | (2) |
	 *       |01 00|
	 *       +-----+
	 */
	if ( getBit(idx,2) ^ getBit(idx,0) ) { // differ --> subtract 1
		if(getBit2(idx,5)==0){
			otherIdx |= 2<<5; // if face normal==0, wrap up to 2
		}else{
			otherIdx |= (getBit2(idx,5)-1)<<5;
		}
	}else{ // same even/odd-ness --> add 1
		// if face normal==2, leave the new normal cleared
		if(getBit2(idx,5)<2){ // ...the normal spot was already cleared though! do nothing if ==2
			otherIdx |= (getBit2(idx,5)+1)<<5;
		}
	}

	/* if corner A=0, the adjacent face is on the same pole of the cube
	 * but if A=3, the face is on the opposite pole
	 * thus if the edge index has a 3 in it, we toggle the face ID hemisphere
	 */
	if(getBit2(idx,2)==3) otherIdx ^= 0b00010000;

	/* all edges have A=0 for both faces adjacent to it, or A=3 for both faces
	 * since it's always the same, we NEVER HAVE TO CHANGE corner A
	 */

	/* also, along the equator, corner B is the same for both adjacent faces.
	 * thus if A=3 (on equator), then we do not change corner B
	 * on the other hand, edges stemming from the poles have different values for B
	 * so when A=0, we must reverse corner B
	 */
	if(getBit2(idx,2)==0) otherIdx ^= 0b00000011; // conveniently, 0b01 ^ 0b11 == 0b10 and vice versa

	return otherIdx;
}
// given a ring of mesh verts on a cube, interpolate them into a triangle mesh
void CubeTable::interpTris(char vertsByCube[], int ringStart, int ringSize){
	// to compensate for rotation/reflection, we must keep track of the originally 
}

void CubeTable::genTable4D(){
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
// lots of space to free from the heap when we're done!
void CubeTable::deleteTable(){
	int i=0;
// clear each array, smallest to largest

	for(i=0;i<16;i++) delete[] table2D[i];
	delete[] table2D;

	for(i=0;i<(256);i++) delete[] table3D[i];
	delete[] table3D;

#if DIMENSION == 4
	for(i=0;i<(1<<(16-3));i++) delete[] table4D[i];
	delete[] table4D;
#endif
}
