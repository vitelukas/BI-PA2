#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <climits>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

// uncomment if your code implements initializer lists
#define EXTENDED_SYNTAX

class CRange;
class CRangeList;
bool intervalsCmp (const CRange &a, const CRange &b);
//---------------------------------------------------- CRange ----------------------------------------------------------
class CRange {
  public:
    CRange() :
        m_lo(0), m_hi(0)
    {
    }
    CRange(long long lo, long long hi) :
        m_lo(lo), m_hi(hi)
    {
        if(lo > hi)
            throw logic_error("lo cannot be bigger than hi");
    }

    friend CRangeList operator + (const CRange &a, const CRange &b );

    friend CRangeList operator - (const CRange &a, const CRange &b);

    const bool operator == (const CRange& other) const {
        return ( m_lo == other.m_lo && m_hi == other.m_hi );
    }
    bool operator != (const CRange &other) const {
        return !(*this == other);
    }

    friend ostream& operator << (ostream &os, const CRange &r) {
        ios::fmtflags originalFlags = os.flags();
        os.setf(ios::dec, ios::basefield);
        if (r.m_lo == r.m_hi)
            os << r.m_lo;
        else
            os << "<" << r.m_lo << ".." << r.m_hi << ">";
        os.flags(originalFlags);
        return os;
    }

//------------------------------------------------------------------------------- Private
    private:
    long long m_lo;
    long long m_hi;
    friend class CRangeList;
    friend bool intervalsCmp (const CRange &a, const CRange &b);
};

//----------------------------------------------------------------------------- CRangeList -----------------------------------------------------------------------------------------
class CRangeList
{
  public:

    class iterator {
    public:
        iterator() :
            m_vector(nullptr), m_index(0)
        {
        }

        iterator(CRangeList* parent, size_t index) :
            m_vector(parent), m_index(index)
        {
        }

        // Pre increment operator for an iterator
        iterator& operator++() {
            if ((m_vector && m_index) < (m_vector->m_intervals.size() - 1))
                ++m_index;
            return *this;
        }

        iterator operator++(int) {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator!=(const iterator& other) const {
            return (m_vector != other.m_vector) || (m_index != other.m_index);
        }

        bool operator==(const iterator& other) const {
            return !(*this != other);
        }

        CRange& operator*() {
            return m_vector->m_intervals[m_index];
        }

        CRange* operator->() {
            return &(m_vector->m_intervals[m_index]);
        }

    private:
        CRangeList* m_vector;
        size_t m_index;
    };

    iterator begin() {
        return iterator(this, 0);
    }

    iterator end() {
        return iterator(this, m_intervals.size());
    }

    CRangeList(void) = default;

    CRangeList(initializer_list<CRange> ranges) {
        this->m_intervals.clear();
        for (auto &r : ranges) {
            *this += r;
        }
        simplify();
    }

    CRangeList &operator = (const CRange &other) {
        this->m_intervals.clear();
        return *this += other;
    }

    CRangeList operator + (const CRange &other) const {
        CRangeList result;
        result = *this;
        return result += other;
    }
    CRangeList& operator += (const CRange &other) {
        if ( !locateInterval(other) )
            m_intervals.insert(m_intervals.begin() + idx, other);
        simplify();
        return *this;
    }
    CRangeList& operator += (const CRangeList &other) {
        for (const auto &interval : other.m_intervals) {
            *this += interval;
        }
        simplify();
        return *this;
    }

    CRangeList operator - (const CRange &other) const {
        CRangeList result;
        result = *this;
        return result -= other;
    }
    CRangeList& operator -= (const CRange &other) {
        if (m_intervals.empty())
            return *this;

        CRangeList tmp;
        tmp = other;
        return *this -= tmp;
    }
    CRangeList& operator -= (const CRangeList &other) {
        if (m_intervals.empty() || other.m_intervals.empty())
            return *this;

        // For each interval in the vector of intervals m_intervals
        for (size_t it = 0; it < m_intervals.size(); it++) {
            // Check each interval from the other vector of intervals
            for ( auto &otherRange : other.m_intervals) {
                if (m_intervals.empty() || other.m_intervals.empty())
                    return *this;

                    // m_intervals[it] interval and otherRange are completely different (no overlap at all), skip it
                if (m_intervals[it].m_lo > otherRange.m_hi || m_intervals[it].m_hi < otherRange.m_lo) {
                    continue;

                    // m_intervals[it] is completely contained in otherRange, subtract it whole
                } else if (m_intervals[it].m_lo >= otherRange.m_lo && m_intervals[it].m_hi <= otherRange.m_hi) {
                    m_intervals.erase(m_intervals.begin() + it);
                    --it;
                    break;

                    // otherRange is completely contained in m_intervals[it], split m_intervals[it] into two parts
                } else if (m_intervals[it].m_lo < otherRange.m_lo && m_intervals[it].m_hi > otherRange.m_hi) {
                    long long int oldHi = m_intervals[it].m_hi;
                    m_intervals[it].m_hi = otherRange.m_lo-1;
                    m_intervals.insert( m_intervals.begin() + (it+1), CRange (otherRange.m_hi+1, oldHi));
                    it++;

                    // m_intervals[it] overlaps with left part of otherRange, keep the left part of m_intervals[it]
                } else if (m_intervals[it].m_lo < otherRange.m_lo && m_intervals[it].m_hi <= otherRange.m_hi) {
                    m_intervals[it].m_hi = otherRange.m_lo-1;

                    // m_intervals[it] overlaps with right part of otherRange, keep the right part of m_intervals[it]
                } else if (m_intervals[it].m_lo >= otherRange.m_lo && m_intervals[it].m_hi > otherRange.m_hi) {
                    m_intervals[it].m_lo = otherRange.m_hi+1;
                }
            }
        }
        return *this;
    }

    bool operator == (const CRangeList &other) const {
        if ((m_intervals.empty() && !other.m_intervals.empty()) ||
            (!m_intervals.empty() && other.m_intervals.empty()))    // If one of the vectors is empty, return the original vector
            return false;

        for (size_t i = 0; i < m_intervals.size(); i++) {
            if (m_intervals[i].m_lo != other.m_intervals[i].m_lo ||
                m_intervals[i].m_hi != other.m_intervals[i].m_hi)
                    return false;
        }
        return true;
    }
    bool operator != (const CRangeList &other) const {
        return !(*this == other);
    }

    const bool includes (const long long &target) const {
        if (m_intervals.empty())
            return false;
        CRange tmp(target, target);
        return includes(tmp);
    }
    /*
     * 3 CASES:
     *      1. interval is supposed to be at the index 0
     *          - target.m_lo is smaller the m_lo of the interval at the idx 0 -> target cannot be included whole in the list of intervals
     *          - target.m_lo is the same as the m_lo of the interval at the idx 0 -> check and return if m_intervals[idx].m_hi >= target.m_hi
     *
     *      2. interval is somewhere in the middle of the vector
     *          - can either be included in the interval before it
     *          - can be exactly the same as the interval on the given index
     *          - can have the same start as the interval on the given index -> ?belongs? to the interval on the given index
     *
     *      3. interval is supposed to be at end of the vector
     *          - the whole target either is included in the previous interval or isn't included in the vector at all
     */
    const bool includes (const CRange &target) const {
        if (m_intervals.empty())
            return false;

        locateInterval(target);
        // target is at the start of the vector
        if (idx == 0) {
            if ( target.m_lo < m_intervals[idx].m_lo) {
                return false;
            } else {        // target.m_lo == m_intervals[idx].m_lo
                return m_intervals[idx].m_hi >= target.m_hi;
            }
        // target is somewhere in the interval
        } else if (idx > 0) {
            // target is somewhere in the middle of the vector
            if (idx != m_intervals.size()) {
                if (target.m_lo < m_intervals[idx].m_lo && m_intervals[idx - 1].m_hi >= target.m_hi) {    // target interval ?is included? in the previous m_intervals interval
                    return true;
                } else if (target.m_lo == m_intervals[idx].m_lo && m_intervals[idx].m_hi >= target.m_hi) {
                    return true;
                }
            // target is at the end of the vector
            } else {
                return m_intervals[idx-1].m_hi >= target.m_hi;
            }
        }
        return false;
    }

    friend ostream& operator << (ostream &os, const CRangeList &l) {
        ios::fmtflags originalFlags = os.flags();
        os.setf(ios::dec, ios::basefield);
        int first = 1;
        os << "{";
        for (size_t i = 0; i < l.m_intervals.size(); i++ ) {
            os << (first ? "" : ",") << l.m_intervals[i];
            first = 0;
        }
        os << "}";
        os.flags(originalFlags);
        return os;
    }

//------------------------------------------------------------------------------ Helper functions
    bool locateInterval (const CRange &target) const {
        idx = 0;
        auto it = lower_bound(m_intervals.begin(), m_intervals.end(), target, intervalsCmp );
        idx = it - m_intervals.begin();
        if (m_intervals.empty() || it == m_intervals.end()) {
            return false;              // The intervalsList is empty or the target should be at the end of the intervals
        } else {
            return *it == target;       // The interval is already situated in the intervalsList
        }
        return false;                   // The target wasn't found in the intervalsList
    }

    void simplify () {
        if (m_intervals.empty())
            return;

        for (size_t i = 0; i+1 < m_intervals.size(); i++) {
            if ((m_intervals[i].m_hi >= m_intervals[i + 1].m_lo) ||
            (m_intervals[i].m_hi >= m_intervals[i + 1].m_lo-1) ) {
                    m_intervals[i+1].m_lo = m_intervals[i].m_lo;
                    m_intervals[i+1].m_hi = max(m_intervals[i].m_hi, m_intervals[i + 1].m_hi);
                    m_intervals.erase(m_intervals.begin() + i--);
            }
        }
    }
//------------------------------------------------------------------------------- Private
    private:
    vector<CRange> m_intervals;
    mutable size_t idx = 0;
};

//------------------------------------------------- CRangeList end -----------------------------------------------------
CRangeList operator + (const CRange &a, const CRange &b) {
    CRangeList result;
    result = a;
    return result += b;
}

CRangeList operator - (const CRange &a, const CRange &b) {
    CRangeList result;
    result = a;
    return result -= b;
}

bool intervalsCmp (const CRange &a, const CRange &b) {
    return a.m_lo < b.m_lo;
}

//---------------------------------------------------- Progtest --------------------------------------------------------
#ifndef __PROGTEST__

string toString ( const CRangeList &x ) {
  ostringstream oss;
  ios::fmtflags originalFlags = oss.flags();
  oss.setf(ios::dec, ios::basefield);
  oss << x;
  oss.flags(originalFlags);
  return oss . str ();
}


int main ( void ) {
  CRangeList a, b;

  assert ( sizeof ( CRange ) <= 2 * sizeof ( long long ) );
  a = CRange ( 5, 10 );
  a += CRange ( 25, 100 );
  assert ( toString ( a ) == "{<5..10>,<25..100>}" );
  a += CRange ( -5, 0 );
  a += CRange ( 8, 50 );
  assert ( toString ( a ) == "{<-5..0>,<5..100>}" );
  a += CRange ( 101, 105 ) + CRange ( 120, 150 ) + CRange ( 160, 180 ) + CRange ( 190, 210 );
  assert ( toString ( a ) == "{<-5..0>,<5..105>,<120..150>,<160..180>,<190..210>}" );
  a += CRange ( 106, 119 ) + CRange ( 152, 158 );
  assert ( toString ( a ) == "{<-5..0>,<5..150>,<152..158>,<160..180>,<190..210>}" );
  a += CRange ( -3, 170 );
  a += CRange ( -30, 1000 );
  assert ( toString ( a ) == "{<-30..1000>}" );
  b = CRange ( -500, -300 ) + CRange ( 2000, 3000 ) + CRange ( 700, 1001 );
  a += b;
  assert ( toString ( a ) == "{<-500..-300>,<-30..1001>,<2000..3000>}" );
  a -= CRange ( -400, -400 );
  assert ( toString ( a ) == "{<-500..-401>,<-399..-300>,<-30..1001>,<2000..3000>}" );
  a -= CRange ( 10, 20 ) + CRange ( 900, 2500 ) + CRange ( 30, 40 ) + CRange ( 10000, 20000 );
  assert ( toString ( a ) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}" );
  try
  {
    a += CRange ( 15, 18 ) + CRange ( 10, 0 ) + CRange ( 35, 38 );
    assert ( "Exception not thrown" == nullptr );
  }
  catch ( const logic_error & e )
  {
  }
  catch ( ... )
  {
    assert ( "Invalid exception thrown" == nullptr );
  }
  assert ( toString ( a ) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}" );
  b = a;
  assert ( a == b );
  assert ( !( a != b ) );
  b += CRange ( 2600, 2700 );
  assert ( toString ( b ) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}" );
  assert ( a == b );
  assert ( !( a != b ) );
  b += CRange ( 15, 15 );
  assert ( toString ( b ) == "{<-500..-401>,<-399..-300>,<-30..9>,15,<21..29>,<41..899>,<2501..3000>}" );
  assert ( !( a == b ) );
  assert ( a != b );
  assert ( b . includes ( 15 ) );
  assert ( b . includes ( 2900 ) );
  assert ( b . includes ( CRange ( 15, 15 ) ) );
  assert ( b . includes ( CRange ( -350, -350 ) ) );
  assert ( b . includes ( CRange ( 100, 200 ) ) );
  assert ( !b . includes ( CRange ( 800, 900 ) ) );
  assert ( !b . includes ( CRange ( -1000, -450 ) ) );
  assert ( !b . includes ( CRange ( 0, 500 ) ) );
  a += CRange ( -10000, 10000 ) + CRange ( 10000000, 1000000000 );
  assert ( toString ( a ) == "{<-10000..10000>,<10000000..1000000000>}" );
  b += a;
  assert ( toString ( b ) == "{<-10000..10000>,<10000000..1000000000>}" );
  b -= a;
  assert ( toString ( b ) == "{}" );
  b += CRange ( 0, 100 ) + CRange ( 200, 300 ) - CRange ( 150, 250 ) + CRange ( 160, 180 ) - CRange ( 170, 170 );
  assert ( toString ( b ) == "{<0..100>,<160..169>,<171..180>,<251..300>}" );
  b -= CRange ( 10, 90 ) - CRange ( 20, 30 ) - CRange ( 40, 50 ) - CRange ( 60, 90 ) + CRange ( 70, 80 );
  assert ( toString ( b ) == "{<0..9>,<20..30>,<40..50>,<60..69>,<81..100>,<160..169>,<171..180>,<251..300>}" );

//---------------------------------------------- Custom asserts --------------------------------------------------------

  CRangeList c, d;
  c += CRange ( 200, 300 ) - CRange ( 150, 250 );
  assert ( toString ( c ) == "{<251..300>}" );
  c -= c;

  c += CRange ( 0, 100 ) - CRange ( -1150, -250 );
  assert ( toString ( c ) == "{<0..100>}" );
  c -= c;

  c += CRange ( 0, 100 ) + CRange ( 200, 300 ) + CRange ( 320, 410 ) - CRange ( 150, 450 );
  assert ( toString ( c ) == "{<0..100>}" );
  c -= c;

  c += CRange ( 0, 20 ) + CRange ( 40, 60 ) + CRange ( 100, 180 ) +CRange ( 200, 300 ) - CRange ( 50, 450 );
  assert ( toString ( c ) == "{<0..20>,<40..49>}" );
  c -= c;

  c += CRange ( 14, 80 ) + CRange ( 100, 600 );
  assert (  c . includes ( CRange ( 160, 580 ) ) );
  assert ( !c . includes ( CRange ( 160, 900 ) ) );
  c -= c;

  c += CRange ( 69, 100 ) + CRange ( 120, 420 );
  assert (  c . includes ( CRange ( 69, 95 ) ) );
  assert ( !c . includes ( CRange ( 50, 80 ) ) );
  assert ( !c . includes ( CRange ( 69, 130 ) ) );
  c -= c;

  c += CRange ( 20, 96 ) + CRange ( 111, 177 ) + CRange ( 238, 300 ) + CRange ( 306, 400 );
  assert (  c . includes ( CRange ( 111, 170 ) ) );
  assert (  c . includes ( CRange ( 111, 177 ) ) );
  assert ( !c . includes ( CRange ( 111, 260 ) ) );
  assert ( !c . includes ( CRange ( 150, 200 ) ) );
  assert (  c . includes ( CRange ( 150, 166 ) ) );
  assert (  c . includes ( CRange ( 150, 177 ) ) );
  assert ( !c . includes ( CRange ( 350, 455 ) ) );
  assert (  c . includes ( CRange ( 370, 392 ) ) );
  c -= c;

  c += CRange ( 20, 60 ) + CRange ( 20, 96 ) + CRange ( 111, 177 ) + CRange ( 238, 300 ) + CRange ( 306, 400 ) + CRange ( 0, 20 ) + CRange ( 40, 60 ) + CRange ( 100, 180 ) + CRange ( 200, 300 ) - CRange ( 50, 450 ) + CRange ( 0, 100 ) + CRange ( 200, 300 ) - CRange ( 150, 250 ) + CRange ( 160, 180 ) - CRange ( 170, 170 ) - CRange (LLONG_MIN, LLONG_MAX);
  assert ( toString ( c ) == "{}" );
  c -= c;

  c += CRange (LLONG_MIN, LLONG_MAX) - CRange ( 40, 60 ) - CRange ( 111, 177 );
  assert ( toString ( c ) == "{<-9223372036854775808..39>,<61..110>,<178..9223372036854775807>}" );
  c -= c;

  c -= CRange (80, 100) - CRange ( 10, 40 );
  assert ( toString ( c ) == "{}" );
  c -= c;

  assert (  !c . includes ( CRange ( 370, 392 ) ) );

  assert ( c == d );
  c += CRange ( 111, 177 ) + CRange ( 238, 300 ) + CRange ( 306, 400 );
  assert ( !(c == d) );
  c -= c;
  d += CRange ( 111, 177 ) + CRange ( 238, 300 ) + CRange ( 306, 400 );
  assert ( !(c == d) );
  d -= d;

  c += CRange (LLONG_MIN+1, LLONG_MAX) + CRange (LLONG_MAX, LLONG_MAX);
  assert ( toString ( c ) == "{<-9223372036854775807..9223372036854775807>}" );
  c-= c;

  c += CRange ( 20, 60 ) + CRange ( 20, 96 ) + CRange ( 111, 177 ) + CRange ( 238, 300 ) + CRange ( 306, 400 ) + CRange ( 0, 20 ) + CRange ( 40, 60 ) + CRange ( 100, 180 ) + CRange ( 200, 300 ) - CRange ( 50, 450 ) + CRange ( 0, 100 ) + CRange ( 200, 300 ) - CRange ( 150, 250 ) + CRange ( 160, 180 ) - CRange ( 170, 170 ) + CRange (LLONG_MIN, LLONG_MAX);
  assert ( toString ( c ) == "{<-9223372036854775808..9223372036854775807>}" );
  c -= c;

  c += CRange ( 20, 60 ) - CRange ( 20, 96 ) - CRange ( 111, 177 ) - CRange ( 238, 300 ) - CRange ( 306, 400 ) - CRange ( 0, 20 ) - CRange ( 40, 60 ) - CRange ( 100, 180 ) - CRange ( 200, 300 ) - CRange ( 50, 450 ) - CRange ( 0, 100 ) + CRange ( 200, 300 ) - CRange ( 150, 250 ) + CRange ( 160, 180 ) - CRange ( 170, 170 ) + CRange (LLONG_MIN, LLONG_MAX);
  assert ( toString ( c ) == "{<-9223372036854775808..9223372036854775807>}" );
  c -= c;

  c += CRange ( 20, 60 ) - CRange ( 20, 96 ) - CRange ( 111, 177 ) - CRange ( 238, 300 ) - CRange ( 306, 400 ) - CRange ( 0, 20 ) - CRange ( 40, 60 ) - CRange ( 100, 180 ) - CRange ( 200, 300 ) - CRange ( 50, 450 ) - CRange ( 0, 100 ) + CRange ( 200, 300 ) - CRange ( 150, 250 ) + CRange ( 160, 180 ) - CRange ( 170, 170 ) - CRange (LLONG_MIN, LLONG_MAX);
  assert ( toString ( c ) == "{}" );
  c -= c;

  c += CRange (LLONG_MIN, LLONG_MAX) + CRange (LLONG_MIN, LLONG_MAX) + CRange ( 20, 96 ) + CRange ( 111, 177 ) + CRange ( 238, 300 ) + CRange ( 306, 400 ) + CRange ( 0, 20 ) + CRange ( 40, 60 );
  assert ( toString ( c ) == "{<-9223372036854775808..9223372036854775807>}" );
  c -= c;

  c += CRange (LLONG_MIN, LLONG_MAX) + CRange (LLONG_MIN, LLONG_MAX) + CRange (LLONG_MIN, LLONG_MAX) - CRange (LLONG_MIN, LLONG_MAX);
  assert ( toString ( c ) == "{}" );
  c -= c;

  c -= CRange (LLONG_MIN, LLONG_MAX) - CRange ( 111, 177 ) - CRange ( 238, 300 ) + CRange ( 238, 300 ) + CRange ( 306, 400 ) + CRange ( 0, 20 ) + CRange ( 40, 60 ) + CRange (LLONG_MIN, LLONG_MAX);
  assert ( toString ( c ) == "{}" );
  c -= c;

  c += CRange (LLONG_MIN, LLONG_MAX) - CRange ( 111, 177 ) - CRange ( 238, 300 ) + CRange ( 238, 300 ) + CRange ( 306, 400 ) + CRange ( 0, 20 ) + CRange ( 40, 60 ) + CRange (LLONG_MIN, LLONG_MAX);
  assert ( toString ( c ) == "{<-9223372036854775808..9223372036854775807>}" );
  c -= c;

  c += CRange ( 111, 177 ) + CRange ( 118, 300 ) + CRange ( -306, -200 ) + CRange ( 0, 20 ) + CRange ( 22, 60 ) + CRange (21,21);
  assert ( toString ( c ) == "{<-306..-200>,<0..60>,<111..300>}" );
  c -= c;

  c += CRange ( 111, 177 ) + CRange ( 118, 300 ) + CRange ( -306, -200 ) + CRange ( 0, 20 ) + CRange ( 22, 60 ) + CRange (21,21) - CRange (0,LLONG_MAX);
  assert ( toString ( c ) == "{<-306..-200>}" );
  c -= c;

  c += CRange ( 111, 177 ) + CRange ( 118, 300 ) + CRange ( -306, -200 ) + CRange ( 0, 20 ) + CRange ( 22, 60 ) + CRange (21,21) - CRange (301, 350);
  assert ( toString ( c ) == "{<-306..-200>,<0..60>,<111..300>}" );
  c -= c;

  c += CRange ( 111, 177 ) + CRange ( 118, 300 ) + CRange ( -306, -200 ) + CRange ( 0, 20 ) + CRange ( 22, 60 ) + CRange (21,21) - CRange (301, 350);
  d += CRange ( 111, 177 ) + CRange ( 118, 300 ) + CRange ( -306, -200 ) + CRange ( 0, 20 ) + CRange ( 22, 60 ) + CRange (21,21) - CRange (0,LLONG_MAX);
  c -= d;
  assert ( toString ( c ) == "{<0..60>,<111..300>}" );
  c -= c;
  d -= d;

  CRangeList e({ CRange(10, 20), CRange(30, 40), CRange(50, 60) });
  e -= d;
  e -= e;

  c -= c;
  c += CRange ( 111, 177 ) + CRange ( 238, 300 ) + CRange ( 306, 400 );
  c -= CRange (-100,500) - CRange (50,150);
  assert ( toString ( c ) == "{<111..150>}" );

  c = CRange ( 111, 177 ) + CRange ( 238, 300 ) + CRange ( 306, 400 );
  d = CRange (-100,500) + CRange (50,150);
  c -= d;
  assert ( toString ( c ) == "{}" );

  c = CRange ( 111, 177 ) + CRange ( 238, 300 ) + CRange ( 306, 400 );
  d = CRange ( 238, 300 ) + CRange ( 111, 177 );
  c -= d;
  assert ( toString ( c ) == "{<306..400>}" );

  c = CRange ( -5, 10 ) + CRange ( 15, 30 ) + CRange ( 40, 41 );
  d = CRange ( -5, 10 ) + CRange ( 15, 30 ) + CRange ( 40, 41 ) + CRange ( 90, 110 );
  c -= d;
  assert ( toString ( c ) == "{}" );

  c = CRange ( -5, 10 ) + CRange ( 15, 30 ) + CRange ( 40, 41 );
  d = CRange ( -10, 11 ) + CRange ( 14, 29 ) + CRange ( 40, 40 ) + CRange ( 90, 110 );
  c -= d;
  assert ( toString ( c ) == "{30,41}" );

  c = CRange ( 0, 10 ) + CRange ( 15, 30 ) + CRange ( 40, 41 ) + CRange ( 88, LLONG_MAX );
  d = CRange ( 0, 1 ) + CRange ( 12, 14 ) + CRange ( 41, 41 );
  c -= d;
  assert ( toString ( c ) == "{<2..10>,<15..30>,40,<88..9223372036854775807>}" );
  assert (  c . includes ( CRange ( 150, 166 ) ) );

  c = CRange ( 10, 100 ) + CRange ( 200, 2300 ) + CRange ( 10000, 10100 );
  assert (  !c . includes ( CRange ( 9, 12 ) ) );
  assert (  !c . includes ( CRange ( 144, 250 ) ) );
  assert (  c . includes ( CRange ( 100, 100 ) ) );
  assert (  c . includes ( CRange ( 200, 200 ) ) );
  assert (  c . includes ( CRange ( 10050, 10080 ) ) );

    CRangeList l { { LLONG_MIN, LLONG_MIN }, { LLONG_MIN, LLONG_MIN } };
    assert ( toString ( l ) == "{-9223372036854775808}" );

    c = CRange ( 10, 100 ) + CRange ( 200, 2300 ) + CRange ( 10000, 10100 );
    d = CRange ( 10, 100 ) + CRange ( 201, 2300 ) + CRange ( 10000, 10100 );
    assert ( !( c == d ) );
    assert ( a != b  );

    c = CRange ( LLONG_MIN, LLONG_MIN );
    d = CRange ( LLONG_MAX, LLONG_MAX );
    c += d;
    assert ( toString ( c ) == "{-9223372036854775808,9223372036854775807}" );

    c = CRange ( 10, 100 );
    d = CRange ( -50, 10 ) + CRange (26, 90) + CRange (91,99) + CRange (10,10) + CRange (10, 26);
    c -= d;
    assert ( toString ( c ) == "{100}" );

    c = CRange ( 10, 100 );
    d = CRange ( -50, 10 ) + CRange (26, 90) + CRange (91,99) + CRange (10,10) + CRange (10, 26);
    c -= d;
    assert ( toString ( c ) == "{100}" );

#ifdef EXTENDED_SYNTAX
  CRangeList x { { 5, 20 }, { 150, 200 }, { -9, 12 }, { 48, 93 } };
  assert ( toString ( x ) == "{<-9..20>,<48..93>,<150..200>}" );
  ostringstream oss;
  oss << setfill ( '=' ) << hex << left;
  for ( const auto & v : x + CRange ( -100, -100 ) )
    oss << v << endl;
  oss << setw ( 10 ) << 1024;
  assert ( oss . str () == "-100\n<-9..20>\n<48..93>\n<150..200>\n400=======" );
#endif /* EXTENDED_SYNTAX */
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
