/* the inverted list for DNF
 */

#pragma once
#include "conjunction.h"
#include <vector>
#include <unordered_map>
#include <algorithm>

template <class Atr, class Val>
class InvertedList {
private:
    std::vector<Conjunction<Atr, Val>> conjs;
    std::vector<std::unordered_map<PairNode, Plist>> kindex;
    struct PairNode {
        Atr attribute;
        Val value;
        PairNode(Atr _attr, Val _val) : attribute(_attr), value(_val) {}
    };
    struct EntryNode {
        int id;
        Belong annotation;
        double weight;
        EntryNode(int _index, Belong _anno, double _weight) 
            : id(_index), annotation(_anno), weight(_weight)
        {}
        bool operator< (const EntryNode& other)
        {
            if (id == other.id) {
                return annotation < other.annotation;
            }
            else {
                return id < other.id;
            }
        }
    };
    struct Plist {
        std::vector<EntryNode> plist;
        int current_entry;
        Plist(const std::vector<EntryNode>& _plist, int _current = 0)
            : plist(_plist), current_entry(_current) {}
        bool operator< (const Plist& other)
        {
            if (current_entry == plist.size()) {
                return true;
            }
            else if (other.current_entry == other.plist.size()) {
                return false;
            }
            else {
                return plist[current_entry] < other.currentEntry();
            }
        }
        EntryNode currentEntry() { return plist[current_entry]; }
        void skip(int id)
        {
            while (plist[current_entry] < id) {
                current_entry++;
            }
        }
    };
    const std::vector<Plist> getPlists(const Conjunction<Atr, Val>& assign, int k) const;

public:
    void addConj(const Conjunction<Atr, Val>& conj);
    const std::vector<int> assign(const Conjunction<Atr, Val>& assign) const;
    const Conjunction<Atr, Val> conjOf(const int i) const;
};

template <class Atr, class Val>
void InvertedList<Atr, Val>::addConj(const Conjunction<Atr, Val>& conj)
{
    int id = conjs.size();
    conjs.push_back(conj);

    if (kindex.size() - 1 < conj.k) {
        kindex.resize(k + 1);
    }
    auto& plists = kindex[conj.k];
    for (int i = 0; i < conj.k; i++) {
        auto& plist = plists[PairNode(conj.pairs[i].attribute, conj.pairs[i].value)];
        plist.plist.push_back(EntryNode(id, conj.pairs[i].annotation, conj.pairs[i].weight));
        std::sort(plist.plist.begin(), plist.plist.end());
    }
}

template <class Atr, class Val>
const std::vector<int> InvertedList<Atr, Val>::assign(const Conjunction<Atr, Val>& assign) const
{
    std::vector<int> result;
    for (int k = std::min(assign.k, kindex.size() - 1); k >= 0; k--) {
        auto plists = getPlists(assign, k);
        int K = k == 0 ? 1 : k;
        if (plists.size() < k) {
            continue;
        }
        while (plists[K - 1].current_entry < plists[K - 1].plist.size()) {
            int next_id = 0;
            std::sort(plists.begin(), plists.end());
            if (plists[0].currentEntry().id == plists[K - 1].currentEntry().id) {
                if (plists[0].currentEntry().annotation == NOT_IN) {
                    int reject = plist[0].currentEntry().id;
                    for (int l = K; l >= plists.size() - 1; l--) {
                        if (plists[l].currentEntry().id == reject) {
                            plists[l].skip(reject + 1);
                        }
                        else {
                            break;
                        }
                    }
                }
                else {
                    result.push_back(plists[K - 1].currentEntry.id);
                }
                next_id = plists[K - 1].currentEntry.id + 1;
            }
            else {
                next_id = plists[K - 1].currentEntry.id;
            }
            for (int l = 0; l < K; l++) {
                plists[l].skip(next_id);
            }
        }
    }
}

template <class Atr, class Val>
const std::vector<typename InvertedList<Atr, Val>::Plist> 
InvertedList<Atr, Val>::getPlists(const Conjunction<Atr, Val>& assign, int k) const
{
    std::vector<Plist> result;
    for (int i = 0; i < assign.k; i++) {
        auto& iter = kindex[k].find(PairNode(assign.pairs[i].attribute, assign.pairs[i].value));
        if (iter != kindex[k].end()) {
            result.push_back(*iter);
        }
    }
    return result;
}

template <class Atr, class Val>
const Conjunction<Atr, Val> InvertedList<Atr, Val>::conjOf(const int i) const
{
    return conjs[i];
}