#include "dnf.h"
#include "invertedListDNF.h"
#include "tools.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main()
{
    ifstream in("assignment.txt");
    Pair<int, int>* pairs = new Pair<int, int>[3000];

    for (int i = 0; i < 3000; i++) {
        string str;
        in >> pairs[i].attribute >> pairs[i].value >> str >> pairs[i].weight;
        if (str == "IN") {
            pairs[i].annotation = IN;
        }
        else {
            pairs[i].annotation = NOT_IN;
        }
    }

    InvertedListDNF<int, int> list;
    for (int k = 0; k < 10; k++) {
        DNF<int, int> dnf;
        for (int i = 0; i < 3; i++) {
            Conjunction<int, int> conj;
            for (int j = 0; j < 10; j++) {
                int index = k * 30 + i * 10 + j;
                conj.insert(pairs[index].attribute, pairs[index].value, pairs[index].annotation, pairs[index].weight);
            }
            dnf.insert(conj);
        }
        list.addDNF(dnf);
    }

    Conjunction<int, int> assigns[10];
    for (int i = 0; i < 10; i++) {
        int ass[30];
        randomArray(ass, 30, 0, 2999);
        for (int j = 0; j < 30; j++) {
            assigns[i].insert(pairs[ass[j]].attribute, pairs[ass[j]].value, pairs[ass[j]].annotation, pairs[ass[j]].weight);
        }
    }

    for (int i = 0; i < 10; i++) {
        auto res = list.assign(assigns[i], 5);
    }

    return 0;
}