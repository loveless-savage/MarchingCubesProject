#include <cubeTable.hpp>


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
	// loop for every possible 3D cube state, of which there are 256
	for(int stateIdxOfCube=0; stateIdxOfCube<256; stateIdxOfCube++){

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
		for(int faceIdxInCube=0; faceIdxInCube<0b110; faceIdxInCube++){
			// which dimensions do we use for the current face?
			int dimA, dimB;
			switch(faceIdxInCube>>1){
				case 0: dimA=2; dimB=4; break;
				case 1: dimA=4; dimB=1; break;
				case 2: dimA=1; dimB=2; break;
			}
			// extract corners of cube that coincide on current face
			if(faceIdxInCube&1){
				stateIdxOfFace = (getBit( stateIdxOfCube, 0 )) // TODO
							   + (getBit( stateIdxOfCube, dimA )<<1)
							   + (getBit( stateIdxOfCube, dimB )<<2)
							   + (getBit( stateIdxOfCube, dimA|dimB )<<3);
			}else{
				stateIdxOfFace = (getBit( stateIdxOfCube, 0 ))
							   + (getBit( stateIdxOfCube, dimA )<<1)
							   + (getBit( stateIdxOfCube, dimB )<<2)
							   + (getBit( stateIdxOfCube, dimA|dimB )<<3);
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
		
		/* now condense vertices into vertsByCube[]
		 * currently, edgesByFace[] holds a bunch of duplicate vertices
		 * these vertices are grouped in pairs according to which ones are connected by mesh edges
		 * now we want to order all the vertices so they align into rings
		 * we start at the beginning of edgesByFace[] and loop through all vertices
		 * with each vertex not having been selected already, we push it onto vertsByCube[]
		 */
		char* vertsByCube = new char[vNum3d/2+1];
		for(int i=0;i<vNum3d;i++){
			// don't count vertices if they've been counted already!
			if(getBit(edgesByFace[i],7)) continue;
			// if not yet counted, use as a seed to capture a ring
			// this loop continues until the ring returns to the original vertex
			while(edgesByFace[i]!=edgesByFace[j]) {
			}
		}

		delete[] vertsByCube;
	}
}
// find the index for a given vertex when attached to another face
char CubeTable::congruentEdge3D(char idx){
	char otherIdx = idx;
	/* cube faces are arranged so their indices do this:
	 * (pole)                   +----+
	 * @--------+               |2  3|
	 * |\       |\        (pole)|0  1|
	 * | \      | \   -->  +----@----+----+----+
	 * |  +--------+  -->  |1  0|0  2|3  1|2  3|
	 * +--|-----+  |  -->  |3  2|1  3|2  0|0  1|
	 *  \ |      \ |  -->  +----+----+----@----+
	 *   \|       \|                 |1  0|(pole)
	 *    +--------@                 |3  2|
	 *           (pole)              +----+
	 * this way all the "0" face corners point into the cube "poles", uppermost/lowermost
	 * and all "3" face corners point away from the poles into the "equator"
	 *
	 * all mesh vertices are clarified by the two grid corners on either side
	 * always listing "0" or "3" first, then "1" or "2" next
	 */
	if(getBit2(idx,0)==1) otherIdx++;
	/* all edges have a "0" on both connecting faces, or a "3" on both
	 * thus we never have to change the first vertex
	 * however, along the equator, edges with a "1" on one face have it on the other as well
	 * ...and vice versa with "2"
	 * on the other hand, edges stemming from the poles have a "1" and a "2" each
	 * so when the mesh vertex is next to a 0 corner, we must reverse its "1" or "2"
	 */
	if(getBit2(idx,2)==0) otherIdx ^= 0b00000011;
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

	for(i=0;i<(256>>2);i++) delete[] table3D[i];
	delete[] table3D;

#if DIMENSION == 4
	for(i=0;i<(1<<(16-3));i++) delete[] table4D[i];
	delete[] table4D;
#endif
}
