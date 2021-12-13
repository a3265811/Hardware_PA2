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
	cerr << "Can't delete node ( " << str << " )" << endl;
}

void Bn_Ntk::printNodeArr(char *argv[]){
	fstream fout;
	fout.open(argv[2],ios::out);
	if(!fout)
		cerr << "Can't open file!!" << endl;
	for(int i = 0; i < PI_arr.size(); i++){
		fout << "INPUT(" << PI_arr[i]->name << ")" << endl;
	}
	for(int i = 0; i < PO_arr.size(); i++){
		fout << "OUTPUT(" << PO_arr[i]->name << ")" << endl;
	}
	for(int i = 0; i < Node_arr.size(); i++){
		if(!(Node_arr[i]->type == "PI" || (Node_arr[i]->type == "PO" && Node_arr[i]->Ftype == "buf" && Node_arr[i]->FI_arr.size() == 0))){
			fout << Node_arr[i]->name << " = " << Node_arr[i]->Ftype << "(";
			for(int j = 0; j < Node_arr[i]->FI_arr.size(); j++){
				if(j == Node_arr[i]->FI_arr.size()-1)
					fout << Node_arr[i]->FI_arr[j]->name << ")" << endl;
				else
					fout << Node_arr[i]->FI_arr[j]->name << ", ";
			}
		}
		string str;
		//cin >> str;
	}
	fout.close();
	fout.open("check.bench",ios::out);
	fout << "------------------------------------------------------------------" << endl;
	for(int i = 0; i < Node_arr.size(); i++){
		fout << Node_arr[i]->id << " " << Node_arr[i]->name << " " << Node_arr[i]->type << " " << Node_arr[i]->Ftype << endl;
	}
	fout.close();
}

void Bn_Ntk::changeName(Bn_Node* node){
	node->name = node->name + "$enc";
	return;
}

void Bn_Ntk::addCost(string str, int num){
	// calculat cost;
	if(str == "and" || str == "or" || str == "nor" || str == "nand")
		cost = cost + num -1;
	else if(str == "not" || str == "buf")
		cost += 1;
	else if(str == "xor" || str == "xnor")
		cost += 3;
	else if(str == "Changebuf")
		cost -= 1;
	else
		cerr << "error type cost: " << str << endl;
}

Bn_Node* Bn_Ntk::addPI(){
	string str = "keyinput" + to_string(new_PI++);
	Bn_Node *temp_node;
	temp_node = new Bn_Node(counter++, str, "PI", "buf");
	PI_arr.push_back(temp_node);
	Node_arr.push_back(temp_node);
	addCost("buf", 1);
	return temp_node;
}

// assume source_A is key input
void Bn_Ntk::insertGate(Bn_Node* source_A, Bn_Node* source_B, string Ftype){
	int pos;
	vector<Bn_Node*> gate_FO = source_B->FO_arr;
	string new_name = "CHG" + to_string(new_count++)  +"$enc";
	Bn_Node *new_gate = new Bn_Node(counter++, new_name, "Internal", Ftype);
	// deal with fanin connection
	new_gate->FI_arr.push_back(source_A);
	new_gate->FI_arr.push_back(source_B);
	source_A->FO_arr.push_back(new_gate);
	source_B->FO_arr.push_back(new_gate);


	// deal with fanout connection
	for(int i = 0; i < gate_FO.size(); i++){
		// remove gate_FO fonout of source_B
		pos = findNode(source_B->FO_arr, gate_FO[i]->name);
		if(pos != -1)
			source_B->FO_arr.erase(source_B->FO_arr.begin() + pos);
		else
			cerr << "failed to change fanout form source_B" << endl;
		// and new_get to gate_FO fanin
		gate_FO[i]->FI_arr.push_back(new_gate);
		// remove source_B fanin of gate_FO
		pos = findNode(gate_FO[i]->FI_arr, source_B->name);
		if(pos != -1)
			gate_FO[i]->FI_arr.erase(gate_FO[i]->FI_arr.begin() + pos);
		else
			cerr << "failed to change fanin form gate_FO" << endl;
		new_gate->FO_arr.push_back(gate_FO[i]);
	}
	// add new_gate to Node_arr
	Node_arr.push_back(new_gate);
	addCost(Ftype, new_gate->FI_arr.size());

}

bool Bn_Ntk::denseCompare(Bn_Node* node1, Bn_Node* node2){
	int num1 = node1->FI_arr.size() + node1->FO_arr.size();
	int num2 = node2->FI_arr.size() + node2->FO_arr.size();
	return num1 > num2;
}

void Bn_Ntk::denseSort(vector<Bn_Node*> &dense_vec){
	for(int i = 0; i < Node_arr.size(); i++){
		if(Node_arr[i]->type != "PI" && Node_arr[i]->type != "PO")
			dense_vec.push_back(Node_arr[i]);
	}
	sort(dense_vec.begin(),dense_vec.end(),denseCompare);
}

void Bn_Ntk::denseInsert(int num){
	int pos;
	int dice;
	Bn_Node* temp_node;
	vector<Bn_Node*> dense_arr;
	denseSort(dense_arr);
	srand(time(NULL));
	int r_pos;

	for(int i = 0; i < num; i++){
		dice = rand();
		if(dice < 0.2)
			dice = 2;
		else if(0.2 < dice && dice < 0.4)
			dice = 3;
		else if(0.4 < dice && dice < 0.6)
			dice = 6;
		else
			dice = 7;
//		insertGate(addPI(), dense_arr[i], gate_choice[6]);
		r_pos = rand() % dense_arr.size();
		insertGate(addPI(), dense_arr[i], gate_choice[dice]);
		if(quota < 3)
			return;
		else
			quota -= 3; // xor mode
	}
}

void Bn_Ntk::doublePIkey(int num){
	vector<Bn_Node*> dense_arr;
	denseSort(dense_arr);
	
	for(int i = 0; i < num; i++){
		Bn_Node* temp_node1 = addPI();
		Bn_Node* temp_node2 = addPI();
		while(dense_arr[i]->Ftype == "buf" || dense_arr[i]->Ftype == "not" || dense_arr[i]->Ftype == "xor" || dense_arr[i]->Ftype == "xnor")
			i++;
		dense_arr[i]->FI_arr.push_back(temp_node1);
		dense_arr[i]->FI_arr.push_back(temp_node2);
		changeName(dense_arr[i]);
	
		if(quota < 4 || num == 0)
			return;
		else
			quota -= 4;
	}
}

void Bn_Ntk::notbuf2xor(int num){
	vector<Bn_Node*> dense_arr;
	denseSort(dense_arr);
	for(int i = 0; i < num; i++){
		while(dense_arr[i]->Ftype != "buf" && dense_arr[i]->Ftype != "not")
			i++;
		dense_arr[i]->Ftype = "xor";
		dense_arr[i]->FI_arr.push_back(addPI());
		changeName(dense_arr[i]);
		if(quota < 2 || num == 0)
			return;
		else
			quota -= 2;
	}
}

void Bn_Ntk::treeEncryption(int num){
	vector<Bn_Node*> dense_arr;
	denseSort(dense_arr);
	srand(time(NULL));
	for(int i = 0; i < num; i++){
		if(i >= dense_arr.size())
			return;
		int pos = 0;
		int dice = rand() % 10;
		/*while(pos < dense_arr[i]->FI_arr.size() && dense_arr[i]->FI_arr[pos]->type == "PI")
			pos++;
		if(pos >= dense_arr[i]->FI_arr.size()){
			num++;
			continue;
		}*/
		
		insertGate(addPI(), dense_arr[i]->FI_arr[0], "xor");
		insertGate(addPI(), dense_arr[i]->FI_arr[0]->FO_arr[0], "and");
		insertGate(addPI(), dense_arr[i], "xor");
		if(quota < 6 || num == 0)
			return;
		else
			quota -= 6;
	}
}

void Bn_Ntk::test(char *argv[]){
//	cout << "initial cost: " << cost << endl;
	//denseInsert(internal_count * 0.5);
	//notbuf2xor(internal_count * 0.5);
	treeEncryption(internal_count * 0.5);
	printNodeArr(argv);
	string str = argv[2];
	str = str + " ";
	str = str + argv[1];
	str = "./satattack/bin/sld " + str;
	const char *c = str.c_str();
	system(c);
}



bool Bn_Ntk::parser(int argc, char* argv[]){
	string str;
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
			temp_node = new Bn_Node(counter++, name, "PI", "buf");
			Node_arr.push_back(temp_node);
			PI_arr.push_back(temp_node);
			addCost("buf", 1);
		}
		else if(str.find("OUTPUT") != -1){ // output
			Bn_Node* temp_node;
			string name;
			int start = str.find("(");
			name = str.substr(start + 1, str.length() - start - 2 );
			temp_node = new Bn_Node(counter++, name, "PO", "buf");
			Node_arr.push_back(temp_node);
			PO_arr.push_back(temp_node);
			addCost("buf", 1);
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
			for(int i = 0; i < Ftype.length(); i++){
				if(65 <= Ftype[i] && Ftype[i] <= 90)
					Ftype[i] += 32;
			}

			pos = findNode(Node_arr, name);
			if(pos != -1){
				temp_node = Node_arr[pos];
				Node_arr.erase(Node_arr.begin()+pos);
				temp_node->Ftype = Ftype;
				addCost("Changebuf", 1);
			}
			else{
				temp_node = new Bn_Node(counter++, name, "internal", Ftype);
				internal_count++;
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
			addCost(Ftype, temp_node->FI_arr.size());
		}
	}
	fin.close();
	quota = cost * 0.1;
	test(argv);
	return 0;
}
