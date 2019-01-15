/* the inverted list for DNF
 */

#pragma once
#include "dnf.h"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <utility>
#include <queue>

template <class Atr, class Val>
class InvertedListDNF {
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
        double ub;
        double ws;
        Plist()
            : head(nullptr), current(nullptr) {}
        Plist(int id, Belong anno, double weight, double _ub)
        {
            ub = _ub;
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
    std::vector<DNF<Atr, Val>> dnfs;
    std::vector<std::pair<Conjunction<Atr, Val>, int>> conjs;
    std::vector<std::unordered_map<PairNode, Plist, PairHash>> kindex;
    const std::pair<std::vector<Plist>, double> getPlists(const Conjunction<Atr, Val>& assign, int k) const;
    void addConj(const Conjunction<Atr, Val>& conj, int d);

public:
    void addDNF(const DNF<Atr, Val>& dnf);
    const std::vector<std::pair<int, double>> assign(const Conjunction<Atr, Val>& assign, int n) const;
    const DNF<Atr, Val> DNFOf(const int d) const { return dnfs[d]; }
};

template <class Atr, class Val>
void InvertedListDNF<Atr, Val>::addDNF(const DNF<Atr, Val>& dnf)
{
    int d = dnfs.size();
    dnfs.push_back(dnf);
    for (const auto& i : dnf.conjs) {
        addConj(i, d);
    }
}

template <class Atr, class Val>
void InvertedListDNF<Atr, Val>::addConj(const Conjunction<Atr, Val>& conj, int d)
{
    int id = conjs.size();
    conjs.push_back({ conj, d });

    if (kindex.size() < conj.k + 1) {
        kindex.resize(conj.k + 1);
    }
    auto& plists = kindex[conj.k];
    for (const auto& i : conj.pairs) {
        const auto& iter = plists.find(PairNode(i.attribute, i.value));
        if (iter != plists.end()) {
            iter->second.insert(id, i.annotation, i.weight);
            if (i.weight > iter->second.ub) {
                iter->second.ub = i.weight;
            }
        }
        else {
            plists.insert({ PairNode(i.attribute, i.value), Plist(id, i.annotation, i.weight, i.weight) });
        }
    }
    if (conj.k == 0) {
        const auto& iter = plists.find(PairNode(0, 0, true));
        if (iter != plists.end()) {
            iter->second.insert(id, IN, 0);
        }
        else {
            plists.insert({ PairNode(0, 0, true), Plist(id, IN, 0, 0) });
        }
    }
}

template <class Atr, class Val>
const std::vector<std::pair<int, double>> InvertedListDNF<Atr, Val>::assign(const Conjunction<Atr, Val>& assign, int n) const
{
    std::vector<std::pair<int, double>> top_n(n, { -1, -1 });
    for (int k = std::min(assign.k - 1, (int)kindex.size() - 1); k >= 0; k--) {
        auto res = getPlists(assign, k);
        if (res.second <= top_n.back().second) {
            continue;
        }
        std::vector<Plist>& plists = res.first;
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
                    double sum = 0.0;
                    for (int i = 0; i < K; i++) {
                        sum += plists[i].ws * plists[i].currentEntry().weight;
                    }
                    if (sum > top_n.back().second) {
                        bool find_same = false;
                        int d = conjs[plists[K - 1].currentEntry().id].second;
                        for (auto i = top_n.begin(); i != top_n.end(); i++) {
                            if (i->first == d && i->second < sum) {
                                top_n.erase(i);
                                top_n.push_back({ d, sum });
                                std::sort(top_n.begin(), top_n.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
                                    return b.second < a.second;
                                });
                                find_same = true;
                                break;
                            }
                        }
                        if (!find_same) {
                            top_n.push_back({ d, sum });
                            std::sort(top_n.begin(), top_n.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
                                return b.second < a.second;
                            });
                            top_n.pop_back();
                        }
                    }
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
    return top_n;
}

template <class Atr, class Val>
const std::pair<typename std::vector<typename InvertedListDNF<Atr, Val>::Plist>, double>
InvertedListDNF<Atr, Val>::getPlists(const Conjunction<Atr, Val>& assign, int k) const
{
    std::vector<Plist> result;
    std::priority_queue<double> score;
    for (const auto& i : assign.pairs) {
        const auto& iter = kindex[k].find(PairNode(i.attribute, i.value));
        if (iter != kindex[k].end()) {
            result.push_back(iter->second);
            result.back().ws = i.weight;
            score.push(i.weight * iter->second.ub);
        }
    }
    double sum = 0;
    for (int i = 0; i < k; i++) {
        sum += score.top();
        score.pop();
    }
    return { result, sum };
}
