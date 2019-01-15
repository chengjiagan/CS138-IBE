#include <iostream>
#include "dnf.h"
#include "invertedListDNF.h"
#include <fstream>
#include <cstring>
#include <windows.h>
#include <ctime>
#include <stdlib.h>

using namespace std;

struct assignment
{
	int name;
	int value;
	Belong b;
	double weight;
	friend ofstream &operator<<(ofstream &os, const assignment &t)
	{
		os << t.name << ' ' << t.value << ' '<<t.b<<' '<<t.weight<<endl;
		return os;
	}
	friend ifstream &operator>>(ifstream &in, assignment &t)
	{
		char s[20];
		in >> t.name >> t.value ;
		in.getline(s,20,' ');
		in>> t.weight;
		in.get();
		if (!strcmp(s, "IN")) t.b = Belong(1);
		else t.b = NOT_IN;
		return in;
	}
};

int main()
{
	ifstream in("assignment.txt");
	assignment t;
	Conjunction<int, int> conj[50];

	double time = 0;
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime;
	LARGE_INTEGER nEndTime;
	QueryPerformanceFrequency(&nFreq);

	QueryPerformanceCounter(&nBeginTime);

	for (int i = 0; i < 5000; i++) {
		in >> t;
		conj[i/20].insert(t.name, t.value, t.b, t.weight);
	}

	DNF<int, int> dnf;
	for(int i=0;i<50;++i)
	dnf.insert(conj[i]);

	InvertedListDNF<int, int> list;
	list.addDNF(dnf);
	Conjunction<int, int> assign;
	assign.insert(t.name,t.value,t.b,t.weight);

	QueryPerformanceCounter(&nEndTime);
	time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;//计算程序执行时间单位为s
	cout << "spend time:\n" << (time * 1000000);
	cin.get();
	
	
   /* Conjunction<char, int> conj6;
    conj6.insert('B', 1, NOT_IN, 0);
    conj6.insert('B', 2, NOT_IN, 0);

    DNF<char, int> dnf3;
    dnf3.insert(conj5);
    dnf3.insert(conj6);


    InvertedListDNF<char, int> list;
    list.addDNF(dnf1);
    list.addDNF(dnf2);
    list.addDNF(dnf3);

    Conjunction<char, int> assign;
    assign.insert('A', 3, IN, 0.8);
    assign.insert('B', 2, IN, 1.0);
    assign.insert('C', 1, IN, 0.9);

    auto& result = list.assign(assign, 2);*/
}