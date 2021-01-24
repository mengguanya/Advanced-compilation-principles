#include "read_inst.h"

using namespace std;

vector<vector<string> > inst;

extern ofstream	generated_c_code_file;

vector<string> DecodeInst(string inst) {

	inst = inst.substr(10);

	vector<string> decode_inst;
	/*decode the number of the instruction*/
	int colon_pos = inst.find(':');
	string inst_num = inst.substr(0, colon_pos);
	decode_inst.push_back(inst_num);

	inst = inst.substr(colon_pos + 2);
	/*decode the sub instruction*/
	while (true) {
		int space_pos = inst.find(' ');
		if (space_pos == -1)
			space_pos = inst.size() - 1;
		string sub_inst = inst.substr(0, space_pos);
		decode_inst.push_back(sub_inst);
		if (space_pos == inst.size() - 1)
			break;
		inst = inst.substr(space_pos + 1);
	}
	/*
	for (int i = 0; i < decode_inst.size(); i++)
		cout << decode_inst[i];
	cout << endl;*/
	return decode_inst;
}

void ReadInst(string three_address_code_file_name) {
	ifstream three_address_code_file;
	string generated_c_code_file_name;
	/*cout << "please input the name of three address code file: ";
	cin >> three_address_code_file_name;
	cout << endl;
	cout << "please input the name of generated C code file: ";
	cin >> three_address_code_file_name;
	cout << endl;*/

	//three_address_code_file_name = ".//examples//test1.txt";
	//generated_c_code_file_name = ".//examples//test1.c.txt";
	/*
	cout<<"please input the name of the 3-adddress-code file: "<<endl; 
	cin>>three_address_code_file_name;
	cout<<endl;
	*/
	three_address_code_file_name = ".//examples//" +  three_address_code_file_name +"//" + three_address_code_file_name + ".txt";
	generated_c_code_file_name = three_address_code_file_name.substr(0,three_address_code_file_name.size()-4);
	generated_c_code_file_name += "_new.c";
	three_address_code_file.open(three_address_code_file_name.c_str());
	generated_c_code_file.open(generated_c_code_file_name.c_str());
	/*if (three_address_code_file == NULL) {
		cout << "ERROR: three address code file doesn't exist!" << endl;
		exit(0);
	}
	else if(generated_c_code_file == NULL){
		cout << "ERROR: can't create the 'generated C code file'" << endl;
		exit(0);
	}*/
	string three_address_code;
	
	vector<string> block_inst;
	inst.push_back(block_inst);

	while (getline(three_address_code_file, three_address_code))
	{
		//cout << three_address_code << endl;

		vector<string> decode_inst = DecodeInst(three_address_code);
		inst.push_back(decode_inst);
	}
	three_address_code_file.close();
	//generated_c_code_file.close();
	return;
}
