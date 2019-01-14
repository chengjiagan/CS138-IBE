/* the structure that represents conjunctions
 * an assignment can also be represented by this conjunctions
 */

#pragma once

#include <utility>

// the difination of pair/key
// Atr: the type of attribute
// Val: the type of value
template <class Atr, class Val>
struct Pair
{
    Atr attribute;
    Val value;
    Belong annotation;
    double weight;
};

enum Belong
{
    NOT_IN, // v in A
    IN // v not in A
};

template <class Atr, class Val>
struct Conjunction
{
    int k;
    Pair* pairs;
    Conjunction(int _k, Pair<Atr, Val>* _pairs);
    ~Conjunction() { delete[] pairs; }
};

template <class Atr, class Val>
Conjunction<Atr, Val>::Conjunction(int _k, Pair<Atr, Val>* _pairs)
{
    k = _k;
    pairs = new Pair[k];
    for (int i = 0; i < k; i++)
    {
        pairs[i] = _pairs[i];
    }
}