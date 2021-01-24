#ifndef ANALYSIS
#define ANALYSIS

#include "common.h"
#include "def.h"

using namespace std;

struct symbol {
	string name;
	long value;
	int kind;
	int size;
	bool finish;//whether the global var has been print
	bool Compare(const symbol& a, const symbol& b) {
		return a.value > b.value;
	}
};

void AnalysisInst();
void HandleStatement(int &id);
void GlobalDefine();
bool Compare(const symbol& a, const symbol& b);

#endif
