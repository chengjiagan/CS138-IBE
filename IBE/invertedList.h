/* the inverted list for DNF
 */

#pragma once
#include "conjunction.h"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <functional>

template <class Atr, class Val>
class InvertedList {
private:
    struct PairNode {
        Atr attribute;
        Val value;
        bool any;
        PairNode(Atr _attr, Val _val, bool _any = false) : attribute(_attr), value(_val), any(_any) {}
        bool operator== (const PairNode& other) const { return attribute == other.attribute && value == other.value && any == other.any; }
    };
    struct PairHash
    {
        std::size_t operator()(const PairNode& pair) const noexcept
        {
            std::size_t h1 = std::hash<Atr>{}(pair.attribute);
            std::size_t h2 = std::hash<Val>{}(pair.value);
            return h1 ^ (h2 << 1);
        }
    };
    struct EntryNode {
        int id;
        Belong annotation;
        double weight;
        EntryNode* next;
        EntryNode (int _index, Belong _anno, double _weight, EntryNode* _next)
            : id(_index), annotation(_anno), weight(_weight), next(_next) {}
        bool operator< (const EntryNode& other) const
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
        EntryNode* head;
        EntryNode* current;
        Plist()
            : head(nullptr), current(nullptr) {}
        Plist(int id, Belong anno, double weight)
        {
            head = new EntryNode(id, anno, weight, nullptr);
        }
        bool operator< (const Plist& other) const
        {
            if (current == nullptr) {
                return false;
            }
            else if (other.current == nullptr) {
                return true;
            }
            else {
                return *current < other.currentEntry();
            }
        }
        const EntryNode& currentEntry() const { return *current; }
        void skip(int id)
        {
            while (current != nullptr && current->id < id) {
                current = current->next;
            }
        }
        void insert(int id, Belong anno, double weight)
        {
            EntryNode** node = &head;
            while (*node != nullptr && (*node)->id < id) {
                node = &((*node)->next);
            }
            *node = new EntryNode(id, anno, weight, *node);
        }
        void init() { current = head; }
    };
    std::vector<Conjunction<Atr, Val>> conjs;
    std::vector<std::unordered_map<PairNode, Plist, PairHash>> kindex;
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

    if (kindex.size() < conj.k + 1) {
        kindex.resize(conj.k + 1);
    }
    auto& plists = kindex[conj.k];
    for (const auto& i : conj.pairs) {
        const auto& iter = plists.find(PairNode(i.attribute, i.value));
        if (iter != plists.end()) {
            iter->second.insert(id, i.annotation, i.weight);
        }
        else {
            plists.insert({ PairNode(i.attribute, i.value), Plist(id, i.annotation, i.weight) });
        }
    }
    if (conj.k == 0) {
        const auto& iter = plists.find(PairNode(0, 0, true));
        if (iter != plists.end()) {
            iter->second.insert(id, IN, 0);
        }
        else {
            plists.insert({ PairNode(0, 0, true), Plist(id, IN, 0) });
        }
    }
}

template <class Atr, class Val>
const std::vector<int> InvertedList<Atr, Val>::assign(const Conjunction<Atr, Val>& assign) const
{
    std::vector<int> result;
    for (int k = std::min(assign.k - 1, (int)kindex.size() - 1); k >= 0; k--) {
        auto plists = getPlists(assign, k);
        for (auto& i : plists) {
            i.init();
        }
        std::sort(plists.begin(), plists.end());
        int K = k == 0 ? 1 : k;
        if (plists.size() < K) {
            continue;
        }
        while (plists[K - 1].current != nullptr) {
            int next_id = 0;
            if (plists[0].currentEntry().id == plists[K - 1].currentEntry().id) {
                if (plists[0].currentEntry().annotation == NOT_IN) {
                    int reject = plists[0].currentEntry().id;
                    for (int l = K; l < plists.size(); l++) {
                        if (plists[l].currentEntry().id == reject) {
                            plists[l].skip(reject + 1);
                        }
                        else {
                            break;
                        }
                    }
                }
                else {
                    result.push_back(plists[K - 1].currentEntry().id);
                }
                next_id = plists[K - 1].currentEntry().id + 1;
            }
            else {
                next_id = plists[K - 1].currentEntry().id;
            }
            for (int l = 0; l < K; l++) {
                plists[l].skip(next_id);
            }
            std::sort(plists.begin(), plists.end());
        }
    }
    return result;
}

template <class Atr, class Val>
const std::vector<typename InvertedList<Atr, Val>::Plist>
InvertedList<Atr, Val>::getPlists(const Conjunction<Atr, Val>& assign, int k) const
{
    std::vector<Plist> result;
    for (const auto& i : assign.pairs) {
        const auto& iter = kindex[k].find(PairNode(i.attribute, i.value));
        if (iter != kindex[k].end()) {
            result.push_back(iter->second);
        }
    }
    return result;
}

template <class Atr, class Val>
const Conjunction<Atr, Val> InvertedList<Atr, Val>::conjOf(const int i) const
{
    return conjs[i];
}