#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;


int main(int argc, char *argv[]){
	string str;
	fstream fin;
	fstream fout;
	fin.open(argv[1]);
	fout.open(argv[2], std::fstream::out | std::fstream::app);
	int start = -1;
	int end = -1;
	bool flag = false;
	str = argv[1];
	start = str.find("/");
	start = str.find("/", start + 1);
	start = str.find("/", start + 1);
	end = str.find(".", start);
	fout << str.substr(start + 1, end - start - 1) << ",";
	if(str.find("unfinished") != -1){
		flag = true;
	}
	while(getline(fin,str)){
		if(str.find("inputs") != -1){
			start = str.find("=");
			end = str.find(" ");
			fout << str.substr(start + 1, end - start - 1) << ",";
			start = str.find("=", end);
			end = str.find(" ", start);
			fout << str.substr(start + 1, end - start - 1) << ",";
			start = str.find("=", end);
			end = str.find(" ", start);
			fout << str.substr(start + 1, end - start - 1) << ",";
			start = str.find("=", end);
			end = str.find(" ", start);
			fout << str.substr(start + 1, end - start - 1) << ",";
			if(flag){
				fout << "-,> 2h" << endl;
			}
		}
//		else if(str.find("key") != -1){
//			start = str.find("=");
//			fout << str.substr(start + 1, str.length - start - 1).length()  << ",";
//		}
		else if(str.find("cpu_time") != -1){
			start = str.find("=");
			end = str.find(";");
			fout << str.substr(start + 1, end - start - 1) << ",";
			start = str.find("=", end);
			end = str.find(";", start);
			start = str.find("=", end);
			end = str.find(";", start);
			start = str.find("=", end);
			end = str.find(";", start);
			fout << fixed << setprecision(2) << stod(str.substr(start + 1, end - start - 1)) << endl;
		}
	}
	return 0;
}
