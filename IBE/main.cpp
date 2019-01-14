#include <iostream>
#include "conjunction.h"
#include "invertedList.h"

int main()
{
    Conjunction<char, int> conj1;
    conj1.insert('A', 3, IN, 0);
    conj1.insert('B', 1, IN, 0);
    Conjunction<char, int> conj2;
    conj2.insert('A', 3, IN, 0);
    conj2.insert('C', 0, IN, 0);
    Conjunction<char, int> conj3;
    conj3.insert('A', 3, IN, 0);
    conj3.insert('C', 1, IN, 0);
    conj3.insert('B', 2, NOT_IN, 0);
    Conjunction<char, int> conj4;
    conj4.insert('B', 2, IN, 0);
    conj4.insert('C', 1, IN, 0);
    Conjunction<char, int> conj5;
    conj5.insert('A', 3, IN, 0);
    conj5.insert('A', 4, IN, 0);
    Conjunction<char, int> conj6;
    conj6.insert('B', 1, NOT_IN, 0);
    conj6.insert('B', 2, NOT_IN, 0);

    InvertedList<char, int> list;
    list.addConj(conj1);
    list.addConj(conj2);
    list.addConj(conj3);
    list.addConj(conj4);
    list.addConj(conj5);
    list.addConj(conj6);

    Conjunction<char, int> assign;
    assign.insert('A', 3, IN, 0);
    assign.insert('B', 2, IN, 0);
    assign.insert('C', 1, IN, 0);

    auto& result = list.assign(assign);
}