#include "analysis_inst.h"

using namespace std;

/*three-address-instruction*/
extern vector<vector<string> > inst;
int inst_id;
ofstream	generated_c_code_file;

//extern ofstream generated_c_code_file;
string all_c_inst;

/*define global variables*/
vector<symbol> global_variables;
int global_variables_num_last = 0;

/*define constant */
vector<long> constant; 

/*define stack frame for local variables and global storage area for global variables*/
stack<char>	stack_frame;
char		global_storage_area[MAXSPCAEFORGLOBALVARIABLES];

/*define function name*/
vector<vector<char> > function_name;

/*define virtual register heap*/
vector<string> virtual_register_heap;

/*symbol table*/
vector<symbol> symbol_table;

/*param table*/
vector<symbol> param_table;
int param_num;

/*call list*/
vector<string> call_list;

vector<string> inter_code;


void AnalysisData(string Data, vector<symbol>& function_parameter, vector<symbol>& local_variables, vector<symbol>& global_variables) {
	int pos = Data.find("#");
	if (pos != -1) {
		int pos_base = Data.find("base");
		int pos_offset = Data.find("offset");
		if (pos_base != -1) {
			symbol t;
			t.name = Data.substr(pos,pos + 1);
			if (t.name.c_str()[0] >= '0' && t.name.c_str()[0] <= '9') {
				global_variables.push_back(t);
			}
			if (t.name.c_str()[0] == '-') {
				global_variables.push_back(t);
			}
		}
		else if (pos_offset != -1) {
			int t = atoi(Data.substr(pos, pos + pos_offset).c_str());
			constant.push_back(t);
		}
		else {
			symbol t;
			t.name = Data.substr(0, pos);
			function_parameter.push_back(t);
		}
	}
}

symbol JudgeDataKind(string Data) {
	symbol t;
	int pos_hashtag = Data.find("#");
	int pos_leftpara = Data.find("(");
	int pos_leftbracket = Data.find("[");
	if (pos_hashtag != -1) { // '#'
		int pos_base = Data.find("base");
		int pos_offset = Data.find("offset");
		if (pos_base != -1) {
			t.name = Data.substr(0, pos_base - 1);
			t.value = atoi(Data.substr(pos_hashtag + 1).c_str());
			t.kind = VAR;
		}
		else if (pos_offset != -1) {
			t.name = Data.substr(0, pos_base - 1);
			t.value = atoi(Data.substr(pos_hashtag + 1).c_str());
			t.kind = VAR;
		}
		else {
			/* common var */
			t.kind = VAR;
			t.name = Data.substr(0, pos_hashtag);
			t.value = atoi(Data.substr(pos_hashtag+1).c_str());
			// if stack frame offset > 0, the var is local var, others, the var is param var
			int stack_frame_offset;
			//stack_frame_offset = atoi(Data.substr(pos_hashtag + 1).c_str());
			if (t.value < 0) {
				int flag = false;
				for (int i = 0; i < symbol_table.size(); i++) {
					if (symbol_table[i].name == t.name) {
						flag = true;
						break;
					}
				}
				if (!flag)
					symbol_table.push_back(t);
			}
			else {
				int flag = false;
				for (int i = 0; i < param_table.size(); i++) {
					if (param_table[i].name == t.name) {
						flag = true;
						break;
					}
				}
				if (!flag)
					param_table.push_back(t);
			}
		}
	}
	else if (pos_leftpara != -1) {
		t.kind = REG;
		t.name = Data.substr(1, Data.size() - 2);
		t.value = atoi(t.name.c_str());
	}
	else if (pos_leftbracket != -1) {
		t.kind = ADDR;
		t.name = Data.substr(1, Data.size() - 2);
		t.value = atoi(t.name.c_str());
	}
	else if (Data == "GP") {
		t.kind = POINTER;
		t.name = "GP";
		t.finish = false;
	}
	else if(Data == "FP"){
		t.kind = POINTER;
		t.name = "FP";
	}
	else {
		/*constant*/
		t.kind = CONST;
		t.value = atoi(Data.c_str());
		t.name = Data.c_str();
	}
	return t;
}

string HandleData(string Data) {
	string result;
	symbol data_1 = JudgeDataKind(Data);
	if (data_1.kind == REG) {
		result = virtual_register_heap[data_1.value];
	}
	else if ((data_1.kind == CONST) || (data_1.kind == VAR)) {
		result = data_1.name;
	}
	else if ((data_1.kind == ADDR)) {
		result = data_1.name;
	}
	else if ((data_1.kind == POINTER)) {
		result = data_1.name;
	}
	return result;
}

void HandleArithmeticInst(int& id) {
	string operator_t;
	string first = HandleData(inst[id][2]);
	string t;
	if (inst[id][1] == "neg") {
		operator_t = "!";
		t = operator_t + first;
	}
	else if (inst[id][1] == "load") {
		t = first;
	}
	else if (inst[id][1] == "param") {
		t = first;
		call_list.push_back(t);
	}
	else if (inst[id][1] == "call") {
		string t = "Func_" + first;
		t += "(";
		for (int i = 0; i < call_list.size(); i++) {
			t += call_list[i] + ",";
		}

		if (!call_list.empty())
			t = t.substr(0, t.size() - 1);
		t += ")";
		while (!call_list.empty()) call_list.pop_back();

		inter_code.push_back(t + ";");
	}
	else {
		string second = HandleData(inst[id][3]);
		if (inst[id][1] == "add") {
			operator_t = "+";
		}
		else if (inst[id][1] == "sub") {
			operator_t = "-";
		}
		else if (inst[id][1] == "mul") {
			operator_t = "*";
		}
		else if (inst[id][1] == "div") {
			operator_t = "/";
		}
		else if (inst[id][1] == "mod") {
			operator_t = "%";
		}
		else if (inst[id][1] == "cmpeq") {
			operator_t = "==";
		}
		else if (inst[id][1] == "cmple") {
			operator_t = "<=";
		}
		else if (inst[id][1] == "cmplt") {
			operator_t = "<";
		}
		else if (inst[id][1] == "move") {
			operator_t = "=";
		}
		else if ((inst[id][1] == "store") || (inst[id][1] == "load")) {
			operator_t = "=";
		}

		if (inst[id][1] == "move") {
			t = second + operator_t;
			t += first;
			inter_code.push_back(t + ";");
		}
		else if ((inst[id][1] == "store")) {
			t = second + operator_t;
			t += first;
			inter_code.push_back(t + ";");
		}
		else if ((inst[id][1] == "add") && ((second == "GP") || second == "FP")) {
			symbol t_var = JudgeDataKind(inst[id][2]);
			t_var.finish = false;
			if (second == "GP") {
				bool flag = false;
				for (int i = 0; i < global_variables.size(); i++) {
					if (global_variables[i].name == t_var.name) {
						flag = true;
					}
				}
				if (!flag)
					global_variables.push_back(t_var);
			}
			if (second == "FP") {
				bool flag = false;
				for (int i = 0; i < symbol_table.size(); i++) {
					if (symbol_table[i].name == t_var.name) {
						flag = true;
					}
				}
				if (!flag)
					symbol_table.push_back(t_var);
			}
			t = first;
		}
		/*global array*/
		else if ((inst[id][1] == "add") && (inst[id - 1][1] == "add") && (inst[id - 1][3] == "GP") && (inst[id - 2][1] == "mul")) {
			bool flag = false;
			string t_string = virtual_register_heap[id - 1];
			for (int i = 0; i < global_variables.size(); i++) {
				if (global_variables[i].name == t_string) {
						global_variables[i].kind = ARRAY;
				}
			}
			t = first + "[";
			t_string = HandleData(inst[id - 2][2]);
			t += t_string + "]";
		}
		/*local array*/
		else if ((inst[id][1] == "add") && (inst[id - 1][1] == "add") && (inst[id - 1][3] == "FP") && (inst[id - 2][1] == "mul")) {
			
			bool flag = false;
			string t_string = virtual_register_heap[id - 1];
			for (int i = 0; i < symbol_table.size(); i++) {
				if (symbol_table[i].name == t_string) {
					symbol_table[i].kind = ARRAY;
				}
			}
			t = first + "[";
			t_string = HandleData(inst[id - 2][2]);
			t += t_string + "]";
		}
		/* 2Î¬Êý×é */
		else if ((inst[id][1] == "add") && (inst[id - 1][1] == "mul") && (inst[id - 2][1] == "add") && (inst[id - 3][1] == "add") && (inst[id - 3][3] == "FP") && (inst[id - 4][1] == "mul")) {
			bool flag = false;
			string t_string = virtual_register_heap[id - 1];
			for (int i = 0; i < symbol_table.size(); i++) {
				if (symbol_table[i].name == t_string) {
					symbol_table[i].kind = ARRAY;
				}
			}
			t = HandleData(inst[id-3][2]) + "[";
			int t_int= JudgeDataKind(inst[inst_id - 4][2]).value * JudgeDataKind(inst[inst_id - 4][3]).value / 8 + JudgeDataKind(inst[inst_id - 1][2]).value;
			char t_char[10];
			string t_s;
			sprintf(t_char, "%d",t_int);
			t_s = t_char;
			t +=  t_s + "]";
		}
		else {
			t = "(" + first;
			t += operator_t + second;
			t += ")";
		}
	}
	virtual_register_heap[id] = t;
}

void HandleBranchInst(int &id){
	string first = HandleData(inst[id][2]);
	symbol data_2 = JudgeDataKind(inst[id][3]);
	if (inst[id][1] == "blbs") {
		first = "!(" + first;
		first = first + ")";
	}
	int ret_id = data_2.value;
	string t_operator;
	if ((inst[data_2.value - 1][1] == "br") && (JudgeDataKind(inst[data_2.value - 1][2]).value <= id)) {
		t_operator = "while(";
	}
	else {
		t_operator = "if(";
	}
	
	string t = t_operator + first;
	t += "){";
	inter_code.push_back(t);
	id++;
	while (id < ret_id - 1) {
		HandleStatement(id);
	}
	if ((inst[id][1] == "br")&&JudgeDataKind(inst[id][2]).value > id) {
		inter_code.push_back("}");
		HandleStatement(id);
	}
	else {
		HandleStatement(id);
		inter_code.push_back("}");
	}
}

void WriteFunction(int function_id) {
	GlobalDefine();
	string c_inst;
	string function_name;
	if (inst[function_id - 1][1] == "entrypc") {
		function_name = "void main";
	}
	else {
		char function_id_char[10];
		sprintf(function_id_char, "%d", function_id);
		string inst_id_string = function_id_char;
		function_name = "void Func_" + inst_id_string;
	}
	c_inst += function_name;
	c_inst += " (";
	sort(param_table.begin(), param_table.end(), Compare);
	for (int i = 0; i < param_table.size(); i++) {
		c_inst += "long " + param_table[i].name;
		c_inst += ",";
	}
	for (int i = 0; i < param_num - param_table.size(); i++) {
		char i_char[10];
		sprintf(i_char,"%d",i); 
		string i_string = i_char;
		c_inst += "long param_" + i_string;
		c_inst += ",";
	}
	/*the param list is not empty, then delete the last ,*/
	if(param_num > 0)
		c_inst = c_inst.substr(0, c_inst.size() - 1);
	while (!param_table.empty()) param_table.pop_back();
	c_inst += " ){\n";

	sort(symbol_table.begin(), symbol_table.end(), Compare);
	for (int i = 0; i < symbol_table.size(); i++) {
		if (i == 0) {
			symbol_table[i].size = (0 - symbol_table[i].value) / 8;
		}
		else {
			symbol_table[i].size = (symbol_table[i - 1].value - symbol_table[i].value) / 8;
		}
		if (symbol_table[i].kind == VAR) {
			c_inst += "long " + symbol_table[i].name;
			c_inst += ";\n";
		}
		else if (symbol_table[i].kind == ARRAY) {
			string t_string;
			char t_char[10];
			sprintf(t_char,"%d",symbol_table[i].size);
			t_string = t_char;
			c_inst += "long " + symbol_table[i].name;
			c_inst += "[" + t_string;
			c_inst += "];\n";
		}
	}
	while (!symbol_table.empty()) symbol_table.pop_back();
	for(int i=0;i<inter_code.size();i++)
		c_inst += inter_code[i] + "\n";
	while (!inter_code.empty()) inter_code.pop_back();
	c_inst += "}\n";
	generated_c_code_file << c_inst;
}

void HandleIO(int& id) {
	string t;
	if (inst[id][1] == "wrl") {
		t = "WriteLine();";
	}
	else if (inst[id][1] == "write") {
		//string var = JudgeDataKind(inst[id][2]).name;
		string var = HandleData(inst[id][2]);
		t = "WriteLong(" + var;
		t += ");";
	}
	inter_code.push_back(t);
}

void HandleElse(int& id) {
	symbol data_1 = JudgeDataKind(inst[id][2]);
	int ret_id = data_1.value;
	string t;
	t = "else{";
	inter_code.push_back(t);
	id++;
	while (id < ret_id) {
		HandleStatement(id);
	}
	inter_code.push_back("}");
}

void HandleStatement(int& id) {
	if (inst[id][1] == "nop") {
		//do nothing
		id++;
	}
	else if ((inst[id][1] == "neg") || (inst[id][1] == "add") || (inst[id][1] == "sub") || (inst[id][1] == "mul") || (inst[id][1] == "div") || (inst[id][1] == "mod") || (inst[id][1] == "cmple") || (inst[id][1] == "cmplt") || (inst[id][1] == "cmpeq")||(inst[id][1] == "move")|| (inst[id][1] == "param")|| (inst[id][1] == "call") || (inst[id][1]== "store") || (inst[id][1] == "load")) {
		HandleArithmeticInst(id);
		id++;
	}
	else if ((inst[id][1] == "blbs") || (inst[id][1] == "blbc")) {
		HandleBranchInst(id);
	}
	else if ((inst[id][1] == "br")) {
		if ((JudgeDataKind(inst[id][2]).value) > id) {
			HandleElse(id);
		}
		else {
			id++;
		}
	}
	else if ((inst[id][1] == "enter")) {
		//do nothing
		id++;
	}
	else if ((inst[id][1] == "write") || (inst[id][1] == "wrl")||(inst[id][1] == "wr")) {
		HandleIO(id);
		id++;
	}
}

void HandleFunction(int &id) {
	int function_id = id;
	/*define function parameter*/
	vector<symbol> function_parameter;

	/*define local variables*/
	vector<symbol> local_variables;

	/*define tab number */
	int tab_num = 0;

	while (inst[id][1] != "ret")
	{
		HandleStatement(id);
	}
	param_num = atoi(inst[id][2].c_str()) / 8;
	WriteFunction(function_id);
}

bool Compare(const symbol& a, const symbol& b) {
	return a.value > b.value;
}

/************************* global define ****************************/
void GlobalDefine() {
	sort(global_variables.begin(), global_variables.end(), Compare);
	for (int i = 0; i < global_variables.size(); i++) {
		if (i == 0) {
			global_variables[i].size = (32768 - global_variables[i].value) / 8;
		}
		else {
			global_variables[i].size = (global_variables[i-1].value - global_variables[i].value) / 8;
		}
		if (global_variables[i].finish == false) {
			if (global_variables[i].kind == VAR) {
				generated_c_code_file << "long " << global_variables[i].name << ";\n";
			}
			else if (global_variables[i].kind == ARRAY) {
				generated_c_code_file << "long " << global_variables[i].name << "[" << global_variables[i].size << "];\n";
			}
			global_variables[i].finish = true;
		}
	}
	//while (global_variables.size() > 0) global_variables.pop_back();
}

/************************* parameter define ****************************/
void ParameterDefine() {

}

/*************************** function define ************************/
void FunctionDefine() {
	/*find the function entry*/
	
	/*define the parameters*/

	/*define the local variables*/

	/*define the other function body, including simple calculation, while/if, and so on*/

	/*if find ret instruction, then return*/
}

void AnalysisInst() {
	generated_c_code_file << "#include <stdio.h>\n#define WriteLine() printf(\"\\n\");\n#define WriteLong(x) printf(\" %lld\", (long)x);\n#define ReadLong(a) if (fscanf(stdin, \"%lld\", &a) != 1) a = 0;\n#define long long long\n";
	//distribute a virtual register for each inst 
	virtual_register_heap.resize(inst.size() + 1);
	//FunctionDefine();
	while (inst_id < inst.size())
	{
		//the entry of a function
		if (inst[inst_id][1] == "enter") {
			HandleFunction(inst_id);
		}
		else {
			inst_id++;
		}
	}	
}
