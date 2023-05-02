#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream> 
#include <iomanip> 
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>
#endif /* __PROGTEST__ */
using namespace std;

/*
 * Helper class to store individual people, who are then stored in a database in class CPersonalAgenda
 */
class CPerson {
    public:
        CPerson ( void ) = default;
        ~CPerson( void ) = default;
        string m_name = "", m_surname = "", m_email = "";
        unsigned int m_salary = 0;

    bool operator == (const CPerson & other) const {
        return ((m_name == other.m_name && m_surname == other.m_surname) || m_email == other.m_email);
    }
};

bool nameCmp (const CPerson & a, const CPerson & b) {
    return a.m_surname < b.m_surname || (a.m_surname == b.m_surname && a.m_name < b.m_name);
}

bool emailCmp (const CPerson & a, const CPerson & b) {
    return a.m_email < b.m_email;
}

static bool salaryCmp(const unique_ptr<CPerson>& a, const unique_ptr<CPerson>& b) {
    return a->m_salary < b->m_salary;
}

/*
 * Compare the people primarily by salary, secondly if two or more people have the same salary, compare them by surname,
 * thirdly if the people have the same surname, compare them by name
 * This function is used to look up the people in salarySortedDbs vector, and also locate them when adding/deleting people
 */
static bool complexCmp(const unique_ptr<CPerson>& a, const unique_ptr<CPerson>& b) {
    return a->m_salary < b->m_salary || (a->m_salary == b->m_salary && (a->m_surname < b->m_surname || (a->m_surname == b->m_surname && a->m_name < b->m_name)));
}

//-------------------------------------------------------------------------- CPersonalAgenda begin ---------------------------------------------------------------------------------------------
/*
 * Class for storing database(s) of people (CPerson objects) as a sorted vector
 */
class CPersonalAgenda
{
  public:
    CPersonalAgenda  ( void ) = default;
    ~CPersonalAgenda ( void ) = default;

    // We first find the index at which the target should be placed for each of the three databases and then we place him there to keep the databases sorted after each insert.
    bool add ( const string & name, const string & surname, const string & email, unsigned int salary ) {
        // Return false if the person with this name, surname and email already exists
        if ( findEmail(email, idx) || findName(name, surname, idx) )
            return false;

        const CPerson target = {name, surname, email, salary};
        auto personPtr = make_unique<CPerson>(target);

        // Add the person into nameSortedDbs
        m_nameSortedDbs.insert(m_nameSortedDbs.begin() + idx, target);

        // Add the person into emailSortedDbs
        findEmail(email, idx);
        m_emailSortedDbs.insert(m_emailSortedDbs.begin() + idx, target);

        // Add the person into salarySortedDbs
        findSalary(name, surname, salary, idx);
        m_salarySortedDbs.insert(m_salarySortedDbs.begin() + idx, move(personPtr));
        return true;
    }

    bool del ( const string & name, const string & surname ) {
        if ( !findName(name, surname, idx) )
            return false;

        // Delete the person from nameSortedDbs
        string email = m_nameSortedDbs[idx].m_email;
        unsigned int salary = m_nameSortedDbs[idx].m_salary;
        m_nameSortedDbs.erase(m_nameSortedDbs.begin() + idx);

        // Delete the person from salarySortedDbs if he wasn't already deleted
        if (findSalary(name, surname, salary, idx))
            m_salarySortedDbs.erase(m_salarySortedDbs.begin() + idx);

        // Delete the person from emailSortedDbs
        del(email);
        return true;
    }

    bool del ( const string & email ) {
        if ( !findEmail(email, idx) )
            return false;

        string name = m_emailSortedDbs[idx].m_name;
        string surname = m_emailSortedDbs[idx].m_surname;
        unsigned int salary = m_emailSortedDbs[idx].m_salary;
        m_emailSortedDbs.erase(m_emailSortedDbs.begin() + idx);

        if (findSalary(name, surname, salary, idx))
            m_salarySortedDbs.erase(m_salarySortedDbs.begin() + idx);

        del(name, surname);
        return true;
    }

    bool changeName ( const string & email, const string & newName, const string & newSurname ) {
        // Return false if there's already a person with the same name and surname we want to newly set for a given person,
        // or if there isn't a person with the given email we were looking for
        if ( findName( newName, newSurname, idx) || !findEmail(email, idx) )
            return false;

        unsigned int salary = m_emailSortedDbs[idx].m_salary;

        del(email);
        add(newName, newSurname, email, salary);
        return true;
    }

    bool changeEmail ( const string & name, const string & surname, const string & newEmail ) {
        if ( findEmail(newEmail, idx) || !findName(name, surname, idx) )
            return false;

        unsigned int salary = m_nameSortedDbs[idx].m_salary;

        del(name, surname);
        add(name, surname, newEmail, salary);
        return true;
    }

    bool setSalary ( const string & name, const string & surname, unsigned int salary ) {
        if ( !findName(name, surname, idx) )
            return false;

        unsigned int oldSalary = m_nameSortedDbs[idx].m_salary;

        // Change the salary for the person in nameSortedDbs
        m_nameSortedDbs[idx].m_salary = salary;
        auto personPtr = make_unique<CPerson>(m_nameSortedDbs[idx]);

        // Change the salary for the person in emailSortedDbs
        findEmail(m_nameSortedDbs[idx].m_email, idx);
        m_emailSortedDbs[idx].m_salary = salary;

        // Change the salary for the person in salarySortedDbs
        findSalary(name, surname, oldSalary, idx);
        m_salarySortedDbs.erase(m_salarySortedDbs.begin() + idx);                   // -> erase the person with oldSalary
        findSalary(name, surname, salary, idx);
        m_salarySortedDbs.insert(m_salarySortedDbs.begin() + idx, move(personPtr)); // -> insert the same person again, to keep the database sorted (but set his salary to the new salary)

        return true;
    }

    bool setSalary ( const string & email, unsigned int salary ) {
        if ( !findEmail(email, idx) )
            return false;

        unsigned int oldSalary = m_emailSortedDbs[idx].m_salary;

        m_emailSortedDbs[idx].m_salary = salary;
        auto personPtr = make_unique<CPerson>(m_emailSortedDbs[idx]);
        string name = m_emailSortedDbs[idx].m_name;
        string surname = m_emailSortedDbs[idx].m_surname;

        findName(name, surname, idx);
        m_nameSortedDbs[idx].m_salary = salary;

        findSalary(name, surname, oldSalary, idx);
        m_salarySortedDbs.erase(m_salarySortedDbs.begin() + idx);
        findSalary(name, surname, salary, idx);
        m_salarySortedDbs.insert(m_salarySortedDbs.begin() + idx, move(personPtr));
        return true;
    }

    unsigned int getSalary ( const string & name, const string & surname ) const {
        if ( findName(name, surname, idx) )
            return m_nameSortedDbs[idx].m_salary;
        return 0;
    }

    unsigned int getSalary ( const string & email ) const {
        if ( findEmail(email, idx) )
            return m_emailSortedDbs[idx].m_salary;
        return 0;
    }

    bool getRank ( const string & name, const string & surname, int & rankMin, int & rankMax ) const {
        unsigned int salary;
        if ( !findName(name, surname, idx) )
            return false;

        salary = m_nameSortedDbs[idx].m_salary;
        getRankGeneral(name, surname, "", salary, rankMin, rankMax);
        return true;
    }

    bool getRank ( const string & email, int & rankMin, int & rankMax ) const {
        unsigned int salary;
        if ( !findEmail(email, idx) )
            return false;

        salary = m_emailSortedDbs[idx].m_salary;
        getRankGeneral("", "", email, salary, rankMin, rankMax);
        return true;
    }

    void getRankGeneral ( const string & name, const string & surname, const string & email, const unsigned int salary, int & rankMin, int & rankMax ) const {
        const CPerson target = {"", "", "", salary};
        const auto first = lower_bound(m_salarySortedDbs.begin(), m_salarySortedDbs.end(), make_unique<CPerson>(target), salaryCmp);
        const auto last = upper_bound(first, m_salarySortedDbs.end(), make_unique<CPerson>(target), salaryCmp);

        // rankMin == the number of people with smaller salary than the targeted person
        // rankMax == rankMin + the number of people with the same salary as the targeted person (except the targeted person)
        rankMin = distance(m_salarySortedDbs.begin(), first);
        rankMax = distance(m_salarySortedDbs.begin(), last > m_salarySortedDbs.begin() ? last - 1 : last);
    }

    bool getFirst ( string & outName, string & outSurname ) const {
        if (m_nameSortedDbs.empty())
            return false;
        outName = m_nameSortedDbs[0].m_name;
        outSurname = m_nameSortedDbs[0].m_surname;
        return true;
    }

    bool getNext ( const string & name, const string & surname, string & outName, string & outSurname ) const {
        // Check if the database is empty, or if we can't find the searched person, or if the searched person is at the end of the database
        if (m_nameSortedDbs.empty()
            || !findName(name, surname, idx)
            || idx == m_nameSortedDbs.size()-1 ) {
            return false;
        }
        outName = m_nameSortedDbs[idx+1].m_name;
        outSurname = m_nameSortedDbs[idx+1].m_surname;
        return true;
    }

//---------------------------------------------------------------------- Private section of CPersonalAgenda ---------------------------------------------------------------------------------------
  private:
    vector<CPerson> m_nameSortedDbs;
    vector<CPerson> m_emailSortedDbs;
    vector<unique_ptr<CPerson>> m_salarySortedDbs;
    mutable size_t idx = 0;

    /*
     * All three 'find' methods bellow are pretty much the same. All of them use std::lower_bound to find the person in the given vector, or eventually the place,
     * where the person is supposed to be, which is used for example in method 'add' to keep the vectors sorted all the time.
     * The return value differs based on the result of the search.
     * The function returns the found index in all cases, so that we can keep our database sorted, when we are inserting new people,
     * or when we want to find a person in the database, or when we want to erase people from the database
     */
    bool findName(const string & name, const string & surname, size_t & idx) const {
        idx = 0;
        const CPerson target = {name, surname, "", 0};

        auto it = lower_bound(m_nameSortedDbs.begin(), m_nameSortedDbs.end(), target, nameCmp);

        if (!m_nameSortedDbs.empty()) {         // If the database is empty, we can return false right away, since there are no people in it, and therefore we wouldn't be able to find our target anyway
            idx = it - m_nameSortedDbs.begin();
            if (it == m_nameSortedDbs.end()) {  // target wasn't found or is supposed to be at the end of the vector
                return false;
            } else {
                return *it == target;           // Return true if the person, who is on the found index in the database, is actually the target we were looking for
                                                // -> else return false (the found index is just the place where the target is supposed to be placed)
            }
        }
        return false;   // The person wasn't found in the given database
    }
    bool findEmail(const string & email, size_t & idx) const {
        idx = 0;
        const CPerson target = {"", "", email, 0};

        auto it = lower_bound(m_emailSortedDbs.begin(), m_emailSortedDbs.end(), target, emailCmp );

        if (!m_emailSortedDbs.empty()) {
            idx = it - m_emailSortedDbs.begin();
            if (it == m_emailSortedDbs.end()) {
                return false;
            } else {
                return *it == target;
            }
        }
        return false;
    }
    bool findSalary(const string& name, const string& surname, const unsigned int& salary, size_t& idx) {
        idx = 0;
        const CPerson target = { name, surname, "", salary };

        auto it = lower_bound(m_salarySortedDbs.begin(), m_salarySortedDbs.end(), make_unique<CPerson>(target), complexCmp);

        if (!m_salarySortedDbs.empty()) {
            idx = it - m_salarySortedDbs.begin();
            if (it == m_salarySortedDbs.end()) {
                return false;
            }
            else {
                return **it == target;
            }
        }
        return false;
    }
};
//-------------------------------------------------------------------------- CPersonalAgenda end ---------------------------------------------------------------------------------------------


#ifndef __PROGTEST__
int main ( void )
{
  string outName, outSurname;
  int lo, hi;
  CPersonalAgenda b1;
  assert ( b1 . add ( "John", "Smith", "john", 30000 ) );
  assert ( b1 . add ( "John", "Miller", "johnm", 35000 ) );
  assert ( b1 . add ( "Peter", "Smith", "peter", 23000 ) );
  assert ( b1 . getFirst ( outName, outSurname )
           && outName == "John"
           && outSurname == "Miller" );
  assert ( b1 . getNext ( "John", "Miller", outName, outSurname )
           && outName == "John"
           && outSurname == "Smith" );
  assert ( b1 . getNext ( "John", "Smith", outName, outSurname )
           && outName == "Peter"
           && outSurname == "Smith" );
  assert ( ! b1 . getNext ( "Peter", "Smith", outName, outSurname ) );
  assert ( b1 . setSalary ( "john", 32000 ) );
  assert ( b1 . getSalary ( "john" ) ==  32000 );
  assert ( b1 . getSalary ( "John", "Smith" ) ==  32000 );
  assert ( b1 . getRank ( "John", "Smith", lo, hi )
           && lo == 1
           && hi == 1 );
  assert ( b1 . getRank ( "john", lo, hi )
           && lo == 1
           && hi == 1 );
  assert ( b1 . getRank ( "peter", lo, hi )
           && lo == 0
           && hi == 0 );
  assert ( b1 . getRank ( "johnm", lo, hi )
           && lo == 2
           && hi == 2 );
  assert ( b1 . setSalary ( "John", "Smith", 35000 ) );
  assert ( b1 . getSalary ( "John", "Smith" ) ==  35000 );
  assert ( b1 . getSalary ( "john" ) ==  35000 );
  assert ( b1 . getRank ( "John", "Smith", lo, hi )
           && lo == 1
           && hi == 2 );
  assert ( b1 . getRank ( "john", lo, hi )
           && lo == 1
           && hi == 2 );
  assert ( b1 . getRank ( "peter", lo, hi )
           && lo == 0
           && hi == 0 );
  assert ( b1 . getRank ( "johnm", lo, hi )
           && lo == 1
           && hi == 2 );
  assert ( b1 . changeName ( "peter", "James", "Bond" ) );
  assert ( b1 . getSalary ( "peter" ) ==  23000 );
  assert ( b1 . getSalary ( "James", "Bond" ) ==  23000 );
  assert ( b1 . getSalary ( "Peter", "Smith" ) ==  0 );
  assert ( b1 . getFirst ( outName, outSurname )
           && outName == "James"
           && outSurname == "Bond" );
  assert ( b1 . getNext ( "James", "Bond", outName, outSurname )
           && outName == "John"
           && outSurname == "Miller" );
  assert ( b1 . getNext ( "John", "Miller", outName, outSurname )
           && outName == "John"
           && outSurname == "Smith" );
  assert ( ! b1 . getNext ( "John", "Smith", outName, outSurname ) );
  assert ( b1 . changeEmail ( "James", "Bond", "james" ) );
  assert ( b1 . getSalary ( "James", "Bond" ) ==  23000 );
  assert ( b1 . getSalary ( "james" ) ==  23000 );
  assert ( b1 . getSalary ( "peter" ) ==  0 );
  assert ( b1 . del ( "james" ) );
  assert ( b1 . getRank ( "john", lo, hi )
           && lo == 0
           && hi == 1 );
  assert ( b1 . del ( "John", "Miller" ) );
  assert ( b1 . getRank ( "john", lo, hi )
           && lo == 0
           && hi == 0 );
  assert ( b1 . getFirst ( outName, outSurname )
           && outName == "John"
           && outSurname == "Smith" );
  assert ( ! b1 . getNext ( "John", "Smith", outName, outSurname ) );
  assert ( b1 . del ( "john" ) );
  assert ( ! b1 . getFirst ( outName, outSurname ) );
  assert ( b1 . add ( "John", "Smith", "john", 31000 ) );
  assert ( b1 . add ( "john", "Smith", "joHn", 31000 ) );
  assert ( b1 . add ( "John", "smith", "jOhn", 31000 ) );

  CPersonalAgenda b2;
  assert ( ! b2 . getFirst ( outName, outSurname ) );
  assert ( b2 . add ( "James", "Bond", "james", 70000 ) );
  assert ( b2 . add ( "James", "Smith", "james2", 30000 ) );
  assert ( b2 . add ( "Peter", "Smith", "peter", 40000 ) );
  assert ( ! b2 . add ( "James", "Bond", "james3", 60000 ) );
  assert ( ! b2 . add ( "Peter", "Bond", "peter", 50000 ) );
  assert ( ! b2 . changeName ( "joe", "Joe", "Black" ) );
  assert ( ! b2 . changeEmail ( "Joe", "Black", "joe" ) );
  assert ( ! b2 . setSalary ( "Joe", "Black", 90000 ) );
  assert ( ! b2 . setSalary ( "joe", 90000 ) );
  assert ( b2 . getSalary ( "Joe", "Black" ) ==  0 );
  assert ( b2 . getSalary ( "joe" ) ==  0 );
  assert ( ! b2 . getRank ( "Joe", "Black", lo, hi ) );
  assert ( ! b2 . getRank ( "joe", lo, hi ) );
  assert ( ! b2 . changeName ( "joe", "Joe", "Black" ) );
  assert ( ! b2 . changeEmail ( "Joe", "Black", "joe" ) );
  assert ( ! b2 . del ( "Joe", "Black" ) );
  assert ( ! b2 . del ( "joe" ) );
  assert ( ! b2 . changeName ( "james2", "James", "Bond" ) );
  assert ( ! b2 . changeEmail ( "Peter", "Smith", "james" ) );
  assert ( ! b2 . changeName ( "peter", "Peter", "Smith" ) );
  assert ( ! b2 . changeEmail ( "Peter", "Smith", "peter" ) );
  assert ( b2 . del ( "Peter", "Smith" ) );
  assert ( ! b2 . changeEmail ( "Peter", "Smith", "peter2" ) );
  assert ( ! b2 . setSalary ( "Peter", "Smith", 35000 ) );
  assert ( b2 . getSalary ( "Peter", "Smith" ) ==  0 );
  assert ( ! b2 . getRank ( "Peter", "Smith", lo, hi ) );
  assert ( ! b2 . changeName ( "peter", "Peter", "Falcon" ) );
  assert ( ! b2 . setSalary ( "peter", 37000 ) );
  assert ( b2 . getSalary ( "peter" ) ==  0 );
  assert ( ! b2 . getRank ( "peter", lo, hi ) );
  assert ( ! b2 . del ( "Peter", "Smith" ) );
  assert ( ! b2 . del ( "peter" ) );
  assert ( b2 . add ( "Peter", "Smith", "peter", 40000 ) );
  assert ( b2 . getSalary ( "peter" ) ==  40000 );

//----------------------------------------------------------------------------- Custom asserts ---------------------------------------------------------------------------------------------
  CPersonalAgenda b3;
  assert ( ! b3 . del ( "peter" ) );
  assert ( ! b3 . del ( "John", "Doe" ) );
  assert ( b3 . add ( "Lucius", "Raven", "rave", 70000 ) );
  assert ( ! b3 . del ( "Rave" ) );
  assert ( b3 . changeName ( "rave", "Lucius", "Falcon" ) );
  assert ( ! b2 . changeEmail ( "Lucius", "Raven", "ravenous" ) );
  assert ( b3 . getSalary ( "ravenous" ) ==  0 );
  assert ( b3 . getSalary ( "Lucius", "Raven" ) ==  0 );
  assert ( b3 . getSalary ( "Lucius", "Falcon" ) ==  70000 );
  assert ( ! b3 . getRank ( "john", lo, hi ) );
  assert ( b3 . add ( "Peter", "Parker", "spiderman", 40000 ) );
  assert ( ! b3 . getRank ( "someone", lo, hi ) );
  assert ( b3 . getRank ( "spiderman", lo, hi )
           && lo == 0
           && hi == 0);
  assert ( b3 . add ( "Quang", "Trong", "quatro", 90000 ) );
  assert ( b3 . add ( "Steve", "idk", "steveo", 65000 ) );
  assert ( ! b3 . getSalary ( "Quatro" ) );
  assert ( ! b3 . add ( "Peter", "Parker", "spider", 23000 ) );
  assert ( ! b3 . add ( "Petr", "Parker", "spiderman", 8000 ) );
  assert ( b3 . getSalary ( "spiderman" ) == 40000 );
  assert ( b3 . getSalary ( "Peter", "Parker" ) == 40000 );
  assert ( ! b3 . del ( "Steve", "Trong" ) );

  CPersonalAgenda b4;
  assert (  b4 . add ( "→S○meone$420◙⌠", "Nobody", "none", 10000 ) );
  assert ( b4 . getSalary ( "→S○meone$420◙⌠", "Nobody" ) == 10000 );
  assert ( b4 . add ( "Peter", "John", "PJ", 12000 ) );
  assert ( b4 . add ( "Peter", "asd24%#5235#523Ä0↕ú83↕73≥H◄╛8189♫8♦8♥•61531•╘9╚4r30#165.", "glyph", 12000 ) );
  assert (  b4 . add ( "Jane", "Roe", "jare", 16000 ) );
  assert (  b4 . add ( "JaNe", "Roe", "jaren", 20000 ) );
  assert ( b4 . getSalary ( "Jane", "Roe" ) == 16000 );
  assert ( b4 . getSalary ( "jare" ) == 16000 );
  assert ( b4 . getSalary ( "JaNe", "Roe" ) == 20000 );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
