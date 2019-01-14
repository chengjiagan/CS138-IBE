/* the structure that represents conjunctions
 * an assignment can also be represented by this conjunctions
 */

#pragma once

#include <vector>

 // the difination of pair/key
 // Atr: the type of attribute
 // Val: the type of value
enum Belong
{
    NOT_IN, // v in A
    IN // v not in A
};

template <class Atr, class Val>
struct Pair
{
    Atr attribute;
    Val value;
    Belong annotation;
    double weight;
    Pair(Atr _attr, Val _val, Belong _anno, double _weight)
        : attribute(_attr), value(_val), annotation(_anno), weight(_weight) {}
};

template <class Atr, class Val>
struct Conjunction
{
    std::vector<Pair<Atr, Val>> pairs;
    int k;
    Conjunction() : k(0) {}
    void insert(Atr attr, Val val, Belong anno, double weight)
    {
        if (anno == IN) {
            bool new_item = true;
            for (const auto& i : pairs) {
                if (i.attribute == attr) {
                    new_item = false;
                }
            }
            if (new_item) {
                k++;
            }
        }
        pairs.push_back(Pair<Atr, Val>(attr, val, anno, weight));
    }
};