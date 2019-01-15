/* the structure that represents DNF
 * an assignment can also be represented by this conjunctions
 */

#pragma once

#include <vector>

enum Belong
{
    NOT_IN, // v in A
    IN // v not in A
};

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
    Pair(Atr _attr, Val _val, Belong _anno, double _weight)
        : attribute(_attr), value(_val), annotation(_anno), weight(_weight) {}
};

// conjunction regarded as combination of pairs
template <class Atr, class Val>
struct Conjunction
{
    std::vector<Pair<Atr, Val>> pairs;
    int k; // the number of IN pairs
    Conjunction() : k(0) {}
    void insert(Atr attr, Val val, Belong anno, double weight)
    {
        if (anno == IN) {
            // prevent repeat count
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

// DNF regarded as combination of conjunctions
template <class Atr, class Val>
struct DNF {
    std::vector<Conjunction<Atr, Val>> conjs;
    void insert(const Conjunction<Atr, Val>& conj)
    {
        conjs.push_back(conj);
    }
};