#ifndef DEFENSE_H
#define DEFENSE_H
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

class Bn_Node{
public:
	Bn_Node() : id(-1), name(""), type(""), Ftype("") {}
	Bn_Node(int _id, string _name, string _type, string _Ftype) : id(_id), name(_name), type(_type), Ftype(_Ftype) {}
	Bn_Node(int _id, string _name, string _type, string _Ftype, vector<Bn_Node*> _FI_arr, vector<Bn_Node*> _FO_arr) : id(_id), name(_name), type(_type), Ftype(_Ftype), FI_arr(_FI_arr), FO_arr(_FO_arr) {}
	~Bn_Node(){}
	friend class Bn_Ntk;

private:
	int id;
	string name;
	string type;				// PI, PO, internal
	string Ftype;				// type of gate
	vector<Bn_Node*> FI_arr;
	vector<Bn_Node*> FO_arr;
};

class Bn_Ntk{
public:
	Bn_Ntk(){
		gate_choice.push_back("and");
		gate_choice.push_back("or");
		gate_choice.push_back("nor");
		gate_choice.push_back("nand");
		gate_choice.push_back("not");
		gate_choice.push_back("buf");
		gate_choice.push_back("xor");
		gate_choice.push_back("xnor");
	}
	~Bn_Ntk(){ }

	// modify function
	int findNode(vector<Bn_Node*> node_vec, string str);
	void deleteNode(vector<Bn_Node*> &node_vec, string str);
	void printNodeArr(char *argv[]);
	void changeName(Bn_Node* node, string str);
	void addCost(string str, int num);
	static bool denseCompare(Bn_Node* node1, Bn_Node* node2);

	// method
	bool parser(int argc, char* argv[]);
	Bn_Node* addPI();
	void insertGate(Bn_Node* source_A, Bn_Node* source_B, string Ftype, vector<Bn_Node*> gate_FO);
	void denseInsert(int num);
	void doublePIkey(int num);
	
	void test(char *argv[]);


private:
	vector<Bn_Node*> Node_arr;
	vector<Bn_Node*> PI_arr;
	vector<Bn_Node*> PO_arr;
	vector<Bn_Node*> Key_arr;
	vector<string> gate_choice;
	int counter = 0;
	int internal_count = 0;
	int new_count = 0; // new gate inside netlist
	int cost = 0;
	int quota = 0;
};


#endif //DEFENSE_H
