#include <iostream>
#include "marchingCubes.hpp"
#include "cubeTable.hpp"


CubeTable lTab;
//MarchingCubes mn(10.f,lTab);
int idx; // user inputted value

using namespace std;
int main(){
	cout<<"heey it's main()"<<endl;
	return 0;
	//mn.genModel();
	while(idx>=0){
		cin>>idx;
		cout<<"okey dokey\t"<<int(lTab.getNumVerts(idx))<<endl;
		for(int i=0;i<int(lTab.getNumVerts(idx));i++){
			cout<<"\t";
			hrEdgeIdx(lTab.getVertexArray(idx)[i]);
		}
		cout<<endl;
	}
	cout<<"k done"<<endl;
}
