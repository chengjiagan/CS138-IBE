#include <iostream>
#include "dnf.h"
#include "invertedListDNF.h"

int main()
{
    Conjunction<char, int> conj1;
    conj1.insert('A', 3, IN, 0.1);
    conj1.insert('B', 1, IN, 4.0);
    Conjunction<char, int> conj2;
    conj2.insert('A', 3, IN, 0.1);
    conj2.insert('C', 0, IN, 0.3);
    Conjunction<char, int> conj3;
    conj3.insert('A', 3, IN, 0.2);
    conj3.insert('C', 1, IN, 0.5);
    conj3.insert('B', 2, NOT_IN, 0);
    Conjunction<char, int> conj4;
    conj4.insert('B', 2, IN, 1.5);
    conj4.insert('C', 1, IN, 0.9);
    Conjunction<char, int> conj5;
    conj5.insert('A', 3, IN, 0.1);
    conj5.insert('A', 4, IN, 0.5);
    Conjunction<char, int> conj6;
    conj6.insert('B', 1, NOT_IN, 0);
    conj6.insert('B', 2, NOT_IN, 0);

    DNF<char, int> dnf1;
    dnf1.insert(conj1);
    dnf1.insert(conj2);
    DNF<char, int> dnf2;
    dnf2.insert(conj3);
    dnf2.insert(conj4);
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

    auto& result = list.assign(assign, 2);
}