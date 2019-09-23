#include <iostream>
#include "marchingCubes.hpp"


MarchingCubes mn(10.f);

using namespace std;
int main(){
	cout<<"heey"<<endl;
	mn.genModel();
	cout<<"sweet"<<endl;
	mn.diag();
	return 0;
}
