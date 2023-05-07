#ifndef __PROGTEST__
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <list>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <stack>
#include <queue>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <iterator>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

//! ----------------------------------------------------------- CContest CLASS START ---------------------------------------------------------------
template <typename M_>
class CContest
{
public:
    CContest() = default;
    ~CContest() = default;

    CContest &addMatch(const string &contestant1, const string &contestant2, const M_ &result) {
        string c1 = contestant1 < contestant2 ? contestant1 : contestant2;
        string c2 = c1 == contestant1 ? contestant2 : contestant1;

        SMatch possibleDuplicate(c1, c2, result);
        SMatch newMatch(contestant1, contestant2, result);

        auto search = m_duplicates.find(possibleDuplicate);
        if (search == m_duplicates.end()) {
            m_duplicates.insert(possibleDuplicate);
            m_matches.insert(newMatch);
        } else {
            throw logic_error("Duplicate match!");
        }

        return *this; //? Return a reference to the current object so that we can chain calls
    }

    bool isOrdered(function<int(M_)> comparator) const {
        list<string> foo;
        return kahn(comparator, foo);
    }

    list<string> results(function<int(M_)> comparator) const {
        list<string> res;
        if (kahn(comparator, res))
            return res;
        else
            throw logic_error("No results.");
    }

    //* ------------------------------------------------------- HELPER METHODS ---------------------------------------------------------------
    bool kahn(function<int(M_)> comparator, list<string> &topSortResults) const {
        //? Adjacency map(graph) of players and who they defeated + num of indegrees of each player
        map<string, pair<set<string>, int>> players;

        for (const auto &match : m_matches) {
            string player1 = match.m_player1;
            string player2 = match.m_player2;
            M_ result = match.m_result;

            int comparison = comparator(result);

            if (comparison > 0) {
                players[player1].first.insert(player2); // player1 defeated player2
                players[player2].second++;              // increase the indegree value of player2
            } else if (comparison < 0) {
                players[player2].first.insert(player1);
                players[player1].second++;
            } else {
                return false; //? The match ended as a draw -> result list is ambiguous
            }
        }

        //? queue of players "on the top" (with 0 indegrees == nobody defeated them)
        queue<string> startingNodes;
        for (const auto &p : players) {
            if (p.second.second == 0) {
                startingNodes.push(p.first);
            }
        }

        return bfs(comparator, players, startingNodes, topSortResults);
    }

    bool bfs(function<int(M_)> comparator, map<string, pair<set<string>, int>> players, queue<string> que, list<string> &topSortResults) const {
        size_t numOfVisitedNodes = 0;

        while (!que.empty()) {
            if (que.size() > 1) // If there are more than one nodes in the queue
                return false;   // the graph is autmatically invalid (ambiguous)

            string node = que.front();

            que.pop();
            numOfVisitedNodes++;
            topSortResults.push_back(node);

            for (const string &neighbour : players.at(node).first) {
                players[neighbour].second--; //? Decrease the indegree value of the neighbour
                if (players[neighbour].second == 0)
                    que.push(neighbour);
            }
        }

        return numOfVisitedNodes == players.size(); // Check if the graph is DAG
    }

private:
    struct SMatch {
    public:
        string m_player1;
        string m_player2;
        M_ m_result;

        SMatch(const string &p1, const string &p2, const M_ &r)
            : m_player1(p1), m_player2(p2), m_result(r) {
        }

        friend bool operator<(const SMatch &a, const SMatch &b) {
            return a.m_player1 < b.m_player1 || (a.m_player1 == b.m_player1 && a.m_player2 < b.m_player2);
        }
    };

    set<SMatch> m_matches;
    set<SMatch> m_duplicates;
};

//! ------------------------------------------------------------ CContest CLASS END -------------------------------------------------------------

#ifndef __PROGTEST__
struct CMatch {
public:
    CMatch(int a, int b)
        : m_A(a), m_B(b) {}

    int m_A;
    int m_B;
};

class HigherScoreThreshold {
public:
    HigherScoreThreshold(int diffAtLeast)
        : m_DiffAtLeast(diffAtLeast) {}

    int operator()(const CMatch &x) const {
        return (x.m_A > x.m_B + m_DiffAtLeast) - (x.m_B > x.m_A + m_DiffAtLeast);
    }

private:
    int m_DiffAtLeast;
};

int HigherScore(const CMatch &x) {
    return (x.m_A > x.m_B) - (x.m_B > x.m_A);
}

int main(void) {
    CContest<CMatch> x;

    x.addMatch("C++", "Pascal", CMatch(10, 3))
        .addMatch("C++", "Java", CMatch(8, 1))
        .addMatch("Pascal", "Basic", CMatch(40, 0))
        .addMatch("Java", "PHP", CMatch(6, 2))
        .addMatch("Java", "Pascal", CMatch(7, 3))
        .addMatch("PHP", "Basic", CMatch(10, 0));

    assert(!x.isOrdered(HigherScore));
    try {
        list<string> res = x.results(HigherScore);
        assert("Exception missing!" == nullptr);
    } catch (const logic_error &e) {
    } catch (...) {
        assert("Invalid exception thrown!" == nullptr);
    }

    x.addMatch("PHP", "Pascal", CMatch(3, 6));

    assert(x.isOrdered(HigherScore));
    try {
        list<string> res = x.results(HigherScore);
        assert((res == list<string>{"C++", "Java", "Pascal", "PHP", "Basic"}));
    } catch (...) {
        assert("Unexpected exception!" == nullptr);
    }

    assert(!x.isOrdered(HigherScoreThreshold(3)));
    try {
        list<string> res = x.results(HigherScoreThreshold(3));
        assert("Exception missing!" == nullptr);
    } catch (const logic_error &e) {
    } catch (...) {
        assert("Invalid exception thrown!" == nullptr);
    }

    assert(x.isOrdered([](const CMatch &x) {
        return (x.m_A < x.m_B) - (x.m_B < x.m_A);
    }));
    try {
        list<string> res = x.results([](const CMatch &x) {
            return (x.m_A < x.m_B) - (x.m_B < x.m_A);
        });
        assert((res == list<string>{"Basic", "PHP", "Pascal", "Java", "C++"}));
    } catch (...) {
        assert("Unexpected exception!" == nullptr);
    }

    CContest<bool> y;

    y.addMatch("Python", "PHP", true)
        .addMatch("PHP", "Perl", true)
        .addMatch("Perl", "Bash", true)
        .addMatch("Bash", "JavaScript", true)
        .addMatch("JavaScript", "VBScript", true);

    assert(y.isOrdered([](bool v) {
        return v ? 10 : -10;
    }));
    try {
        list<string> res = y.results([](bool v) {
            return v ? 10 : -10;
        });
        assert((res == list<string>{"Python", "PHP", "Perl", "Bash", "JavaScript", "VBScript"}));
    } catch (...) {
        assert("Unexpected exception!" == nullptr);
    }

    y.addMatch("PHP", "JavaScript", false);
    assert(!y.isOrdered([](bool v) {
        return v ? 10 : -10;
    }));
    try {
        list<string> res = y.results([](bool v) {
            return v ? 10 : -10;
        });
        assert("Exception missing!" == nullptr);
    } catch (const logic_error &e) {
    } catch (...) {
        assert("Invalid exception thrown!" == nullptr);
    }

    try {
        y.addMatch("PHP", "JavaScript", false);
        assert("Exception missing!" == nullptr);
    } catch (const logic_error &e) {
    } catch (...) {
        assert("Invalid exception thrown!" == nullptr);
    }

    try {
        y.addMatch("JavaScript", "PHP", true);
        assert("Exception missing!" == nullptr);
    } catch (const logic_error &e) {
    } catch (...) {
        assert("Invalid exception thrown!" == nullptr);
    }
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
