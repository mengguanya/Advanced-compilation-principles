#define _CRT_SECURE_NO_WARNINGS
#include"main.h"

using namespace std;

/*three-address-instruction*/
extern vector<vector<string> > inst;
extern int inst_id;
/*define stack frame for local variables and global storage area for global variables*/
extern stack<char>	stack_frame;
extern char		global_storage_area[MAXSPCAEFORGLOBALVARIABLES];

/*define virtual register heap*/
extern vector<DATA> virtual_register_heap;

int main(int argc, char** argv) {
	string file_name = argv[1];
	inst_id = 1;
	ReadInst(file_name);
	AnalysisInst();
}
