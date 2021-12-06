#include "defense.h"

using namespace std;

int main(int argc, char* argv[]){
	Bn_Ntk circuit;
	if(circuit.parser(argc, argv)){
		exit(1);
	}
	return 0;
}
