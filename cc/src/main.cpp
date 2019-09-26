#include <iostream>
#include "marchingCubes.hpp"
#include "cubeTable.hpp"


CubeTable lTab;
//MarchingCubes mn(10.f,lTab);
int idx; // user inputted value

using namespace std;
int main(){
	cout<<"heey"<<endl;
	//mn.genModel();
	while(idx>=0){
		cin>>idx;
		cout<<"okey dokey"<<endl;
		/*
		for(int i=0;i<lTab.getSize(idx);i++){
			cout<<"\t"<<lTab.getArray(idx)[i];
		}
		*/
		cout<<endl;
	}
	cout<<"k done"<<endl;
	return 0;
}
