#ifndef DEFENSE_H
#define DEFENSE_H
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Bn_Node{
public:
	Bn_Node() : id(-1), name(""), type(""), Ftype("") {}
	Bn_Node(int _id, string _name, string _type, string _Ftype, vector<Bn_Node*> _FI_arr, vector<Bn_Node*> _FO_arr) : id(_id), name(_name), type(_type), Ftype(_Ftype), FI_arr(_FI_arr), FO_arr(_FO_arr) {}
	~Bn_Node(){}
	friend class Bn_Ntk;

private:
	int id;
	string name;
	string type;
	string Ftype;
	vector<Bn_Node*> FI_arr;
	vector<Bn_Node*> FO_arr;
};

class Bn_Ntk{
public:
	Bn_Ntk(){ }
	~Bn_Ntk(){ }
	void parser(int argc, char* argv[]);


private:
	vector<Bn_Node*> Node_arr;
	vector<Bn_Node*> PI_arr;
	vector<Bn_Node*> PO_arr;
	vector<Bn_Node*> Key_arr;
};

#endif //DEFENSE_H
