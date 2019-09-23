#include <marchingCubes.hpp>

// grab a single bit value and return 1 or 0
#define GETBIT(n,bit) ((n>>bit)&1)
// convert grid coordinates to world space coordinates
#define G_TO_W(gcoord) (gcoord*frameSize/cellNum-frameSize/2)

// generate mesh with normals from given vector field
void MarchingCubes::genModel(){
	// add each following line of squares to make the bottom plane
	//for(int l=0;l<cellNum+1;l++){
	for(int k=0;k<cellNum+1;k++){
	for(int j=0;j<cellNum+1;j++){
	for(int i=0;i<cellNum+1;i++){
	// point level
		do_u(k,j,i);
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
	}}}
};

// vector field operator
void MarchingCubes::do_u(int k,int j,int i){
	// deduce actual world space coordinates from grid index
	float x,y,z;//,w;
	x = G_TO_W(i);
	y = G_TO_W(j);
	z = G_TO_W(k);
	//w = G_TO_W(l);

	// vector equation
	pts[k][j][i].x = sin(x);
	pts[k][j][i].y = sin(y);
	pts[k][j][i].z = sin(z);
	//pts[l][k][j][i].w = sin(w);
	// scalar equation
	pts[k][j][i].f = sin(x)+sin(y)+sin(z);
};

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
	if((mainpt->f>0)^(otherpt->f>0)){
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
}
// interpolate triangles
void MarchingCubes::interpTris(int k, int j, int i, int axis_norm){
}
// interpolate simplices
void MarchingCubes::interpSimp(int k, int j, int i){
}

