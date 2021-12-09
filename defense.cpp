#include "defense.h"

int Bn_Ntk::findNode(vector<Bn_Node*> node_vec, string str){
	for(int i = 0; i < node_vec.size(); i++){
		if(node_vec[i]->name == str)
			return i;
	}
	return -1;
}

void Bn_Ntk::deleteNode(vector<Bn_Node*> &node_vec, string str){
	int temp_pos = findNode(node_vec, str);
		if(temp_pos != -1){
			node_vec.erase(node_vec.begin() + temp_pos);
			return;
		}
	cout << "Can't delete node ( " << str << " )" << endl;
}

void Bn_Ntk::printNodeArr(){
	fstream fout;
	fout.open("test_parser.txt",ios::out);
	for(int i = 0; i < PI_arr.size(); i++){
		fout << "INPUT(" << PI_arr[i]->name << ")" << endl;
	}
	for(int i = 0; i < PO_arr.size(); i++){
		fout << "OUTPUT(" << PO_arr[i]->name << ")" << endl;
	}
	for(int i = 0; i < Node_arr.size(); i++){
		if(Node_arr[i]->Ftype != "BUF"){
			fout << Node_arr[i]->name << " = " << Node_arr[i]->Ftype << "(";
			for(int j = 0; j < Node_arr[i]->FI_arr.size(); j++){
				if(j == Node_arr[i]->FI_arr.size()-1)
					fout << Node_arr[i]->FI_arr[j]->name << ")" << endl;
				else
					fout << Node_arr[i]->FI_arr[j]->name << ", ";
			}
		}
	}
}

void Bn_Ntk::changeName(Bn_Node* node, string str){
	node->name = str;
	return;
}

void addPIPO(string str, bool mode){
	if(!mode){ // PI
		Bn_Node *temp_node = new (counter++, str, "PI", "BUF");
		PI_arr.push_back(temp_node);
		Node_arr.push_back(temp_node);
	}
	else{ //PO
		Bn_Node *temp_node = new (counter++, str, "PO", "BUF");
		PO_arr.push_back(temp_node);
		Node_arr.push_back(temp_node);


	}
}

void Bn_Ntk::test(){
	printNodeArr();
}



bool Bn_Ntk::parser(int argc, char* argv[]){
	string str;
	int counter = 0;
	fstream fin;
	fin.open(argv[1],ios::in);
	if(!fin){
		cerr << "Can't open file: " << argv[1] << endl;
		return 1;
	}

	while(getline(fin,str)){
		if(str.find("#") != -1)
			continue;
		else if(str.find("INPUT") != -1){ // input
			Bn_Node* temp_node;
			string name;
			int start = str.find("(");
			name = str.substr(start + 1, str.length() - start - 2 );
			temp_node = new Bn_Node(counter++, name, "PI", "BUF");
			Node_arr.push_back(temp_node);
			PI_arr.push_back(temp_node);
		}
		else if(str.find("OUTPUT") != -1){ // output
			Bn_Node* temp_node;
			string name;
			int start = str.find("(");
			name = str.substr(start + 1, str.length() - start - 2 );
			temp_node = new Bn_Node(counter++, name, "PO", "BUF");
			Node_arr.push_back(temp_node);
			PO_arr.push_back(temp_node);
		}
		else{	// internal node
			if(str == "")
				continue;
			
			string name, Ftype;
			int start, end, pos;
			bool is_end = false;
			Bn_Node *temp_node;

			name = str.substr(0,str.find(" "));
			start = str.find("= ");
			end = str.find("(");
			Ftype = str.substr(start + 2, end - start - 2);
			pos = findNode(Node_arr, name);
			if(pos != -1){
				temp_node = Node_arr[pos];
				Node_arr.erase(Node_arr.begin()+pos);
				temp_node->Ftype = Ftype;
			}
			else{
				temp_node = new Bn_Node(counter++, name, "internal", Ftype);
			}

			start = end;
			end = str.find(",");
			if(!is_end && end == -1){
				end = str.find(")");
				is_end = true;
			}
			while(end != -1){
				int temp_fi = findNode(Node_arr, str.substr(start + 1, end - start - 1));
				if(temp_fi != -1){
					temp_node->FI_arr.push_back(Node_arr[temp_fi]);
					Node_arr[temp_fi]->FO_arr.push_back(temp_node);
				}
				start = str.find(" ",end);
				end = str.find(",",start);
				if(!is_end && end == -1){
					end = str.find(")");
					is_end = true;
				}
			}	
			Node_arr.push_back(temp_node);
		}
	}
	fin.close();
	test();
	return 0;
}
