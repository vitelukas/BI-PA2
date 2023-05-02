#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;
#endif /* __PROGTEST__ */

class CString;
class CPerson;
class CMail;
class CMailIterator;
class CMailServer;
bool emailCmp (const CPerson &a, const CPerson &b);

//------------------------------------------------------ STRING --------------------------------------------------------
class CString {
public:

    CString(const char* str)
            : m_len(strlen(str)), m_capacity(m_len + 1), m_str(new char[m_capacity])
    {
        memcpy(m_str, str, m_len + 1);
    }

    CString (const CString & src)
            : m_len(src.m_len), m_capacity(src.m_len + 1), m_str(new char [m_capacity])
    {
        memcpy(m_str, src.m_str, m_len + 1);
    }

    CString &operator = (const CString &src) {
        if (&src != this ) {
            delete [] m_str;
            m_len = src.m_len;
            m_capacity = src.m_len + 1;
            m_str = new char [m_capacity];
            memcpy(m_str, src.m_str, m_len + 1);
        }
        return *this;
    }

    ~CString()
    {
        delete [] m_str;
    }

    size_t length() const {
        return m_len;
    }

    char &operator[](size_t idx) {
        return m_str[idx];
    }

    char operator[](size_t idx) const {
        return m_str[idx];
    }

    void append(const char *str, size_t len) {
        if (m_len + len + 1 > m_capacity) {
            while (m_len + len + 1 > m_capacity)
                m_capacity += m_capacity / 2 + 10;
            char *tmp = new char[m_capacity];
            memcpy(tmp, m_str, m_len);
            memcpy(tmp + m_len, str, len);
            delete[] m_str;
            m_str = tmp;
        }
        else
            memcpy(m_str + m_len, str, len);

        m_len += len;
        m_str[m_len] = '\0';
    }

    const bool operator == (const CString& other) const {
        if (this->m_str && other.m_str)
            return strcmp(m_str, other.m_str) == 0;
        return false;
    }

    friend ostream& operator<<(ostream& os, const CString& str);

private:
    size_t m_len = 0;
    size_t m_capacity = 0;
    char *m_str = nullptr;

    friend class CMail;
    friend class CMailServer;
    friend class CMailIterator;
    friend class CPerson;
    friend bool emailCmp (const CPerson &a, const CPerson &b);
};

ostream& operator<<(ostream& os, const CString& str) {
    os << str.m_str;
    return os;
}

//----------------------------------------------------- VECTOR ---------------------------------------------------------
template <typename T>
class CVector {
public:
    CVector()
            : m_data(nullptr), m_size(0), m_capacity(0)
    {
    }

    CVector(const CVector &other)
            : m_data(nullptr), m_size(0), m_capacity(0)
    {
        reserve(other.m_capacity);
        for (size_t i = 0; i < other.m_size; i++) {
            push_back(other.m_data[i]);
        }
    }

    CVector &operator=(const CVector &other) {
        if (this != &other) { // zamezení samopřiřazení
            delete[] m_data;

            m_data = nullptr;
            m_size = 0;
            m_capacity = 0;

            reserve(other.m_capacity);
            for (size_t i = 0; i < other.m_size; i++) {
                push_back(other.m_data[i]);
            }
        }
        return *this;
    }

    ~CVector() {
        delete[] m_data;
    }

    void push_back(const T &value) {
        if (m_size == m_capacity) { // pokud je kapacita plná, musíme ji zvětšit
            if (m_capacity == 0) {
                resize(1);
            } else {
                resize(m_capacity * 2);
            }
        }
        m_data[m_size] = value;
        m_size++;
    }

    void pop_back() {
        if (m_size > 0) {
            m_size--;
        }
    }

    void insert(size_t index, const T &value) {
        if (m_size == m_capacity) { // pokud je kapacita plná, musíme ji zvětšit
            if (m_capacity == 0) {
                resize(1);
            } else {
                resize(m_capacity * 2);
            }
        }
        for (size_t i = m_size - 1; i >= index; i--) { // posunutí prvků o jednu pozici doprava
            m_data[i + 1] = m_data[i];
        }
        m_data[index] = value;
        m_size++;
    }

    void reserve(size_t newCapacity) {
        if (newCapacity > m_capacity) {
            resize(newCapacity);
        }
    }

    void resize(size_t newCapacity) {
        T *newData = new T[newCapacity];
        for (size_t i = 0; i < m_size; i++) {
            newData[i] = m_data[i]; // přenesení dat z původního pole do nového
        }
        delete[] m_data;
        m_data = newData;
        m_capacity = newCapacity;
    }

    void erase(size_t index) {
        for (size_t i = index + 1; i < m_size; i++) { // posunutí prvků o jednu pozici doleva
            m_data[i - 1] = m_data[i];
        }
        m_size--;
    }

    bool empty() const {
        return m_size == 0;
    }

    size_t size() const {
        return m_size;
    }

    size_t capacity() const {
        return m_capacity;
    }

    T &operator[](size_t index) {
        return m_data[index];
    }

    const T &operator[](size_t index) const {
        return m_data[index];
    }

    const T *begin() const {
        return &m_data[0];
    }

    const T *end() const {
        return &m_data[m_size];
    }
private:
    T *m_data; // ukazatel na m_data
    size_t m_size = 0; // velikost vektoru
    size_t m_capacity = 0; // kapacita vektoru

};

//------------------------------------------------------ CPerson -------------------------------------------------------
class CPerson {
public:
    CPerson()
            : m_email(""), m_inbox(), m_outbox()
    {
    }

    CPerson (const CString email)
            : m_email(email), m_inbox(), m_outbox()
    {
    }

    const bool operator == (const CPerson& other) const {
        return ( m_email.m_str == other.m_email.m_str );
    }

    const CString& getEmail() const {
        return m_email;
    }

private:
    CString m_email;
    CVector<CMail> m_inbox;
    CVector<CMail> m_outbox;

    friend bool emailCmp (const CPerson &a, const CPerson &b);
    friend class CMailServer;
};

//------------------------------------------------------- CMail --------------------------------------------------------
class CMail {
public:
    CMail()
            : m_from(""), m_to(""), m_body("")
    {
    }

    CMail(const char *from, const char *to, const char *body)
            : m_from(from), m_to(to), m_body(body)
    {
    }

    bool operator==(const CMail &x) const {
        if (m_from.m_str && m_to.m_str && m_body.m_str &&
            x.m_from.m_str && x.m_to.m_str && x.m_body.m_str) {
            return (strcmp(m_from.m_str, x.m_from.m_str) == 0 &&
                    strcmp(m_to.m_str, x.m_to.m_str) == 0 &&
                    strcmp(m_body.m_str, x.m_body.m_str) == 0);
        }
        return false;
    }

    friend ostream &operator<<(ostream &os, const CMail &email);

private:
    CString m_from;
    CString m_to;
    CString m_body;
    friend class CMailServer;
};

ostream &operator<<(ostream &os, const CMail &email) {
    os << "From: " << email.m_from << ", ";
    os << "To: " << email.m_to << ", ";
    os << "Body: " << email.m_body;
    return os;
}
//-------------------------------------------------- CMailIterator -----------------------------------------------------
class CMailIterator {
public:
    CMailIterator() = default;

    CMailIterator(const CVector<CMail>& mails)
            : m_mails(mails), m_index(0)
    {
    }

    ~CMailIterator() = default;

    explicit operator bool(void) const {
        return m_index < m_mails.size();
    }

    bool operator!(void) const {
        return m_index >= m_mails.size();
    }

    const CMail &operator*(void) const {
        return m_mails[m_index];
    }

    CMailIterator &operator++(void) {
        ++m_index;
        return *this;
    }

    void print (void) {
        for (size_t i = 0; i < m_mails.size(); i++) {
            cout << m_mails[i] << endl;
        }
    }

private:
    CVector<CMail> m_mails;
    size_t m_index = 0;
};

//---------------------------------------------------- CMailServer -----------------------------------------------------
class CMailServer {
public:
    CMailServer(void) = default;

    CMailServer(const CMailServer &src) {
        if (this != &src) {
            m_people = src.m_people;
        }
    }

    ~CMailServer(void) = default;

    CMailServer &operator=(const CMailServer &src) {
        if (this != &src) {
            m_people = src.m_people;
        }
        return *this;
    }

    void sendMail(const CMail &m) {
        // najdeme odesílatele v seznamu schránek
        CString sender = m.m_from;
        if ( !findPerson(sender) ) { // Pokud odesílatel ještě nemá zřízenou schránku, tak mu ji vytvoříme a zařadíme ji do seřazeného seznamu lidí
            m_people.push_back( CPerson(sender) );
        }
        m_people[idx].m_outbox.push_back(m);

        CString recipient = m.m_to;
        if ( !findPerson(recipient) ) { // Pokud odesílatel ještě nemá zřízenou schránku, tak mu ji vytvoříme a zařadíme ji do seřazeného seznamu lidí
            m_people.push_back( CPerson(recipient) );
        }
        m_people[idx].m_inbox.push_back(m);
    }

    // najdeme schránku odesílatele
    // pokud schránka neexistuje, vrátíme prázdný iterátor
    // return CMailIterator();
    CMailIterator outbox(const char *email) const {
        CMailIterator result;
        if ( findPerson(email) ) {
            result = CMailIterator(m_people[idx].m_outbox);
        }
        return result;
    }

    CMailIterator inbox(const char *email) const {
        CMailIterator result;
        if ( findPerson(email) ) {
            result = CMailIterator(m_people[idx].m_inbox);
        }
        return result;
    }

    /* // TODO - implement lower bound into the code
    template <typename ForwardIterator, typename T, typename Compare>
    ForwardIterator custom_lower_bound(ForwardIterator lo, ForwardIterator hi, const T& value, Compare comp) const {
        while (lo <= hi) {
            ForwardIterator mid = lo + (hi - lo) / 2;
            if (comp(*mid, value)) {
                lo = mid + 1;
            } else {
                hi = mid - 1;
            }
        }
        return lo;
    }
    */

    bool findPerson(const CString &targetEmail) const {
        idx = 0;
        if ( m_people.size() == 0 )
            return false;

        for (size_t i = 0; i < m_people.size(); i++) {
            if (m_people[i].m_email == targetEmail) {
                idx = i;
                return true;
            }
        }
        idx = m_people.size();
        return false;
    }

    friend ostream &operator<<(ostream &os, const CMailServer &server) {
        for (size_t i = 0; i < server.m_people.size(); ++i) {
            os << server.m_people[i].getEmail() << endl;
        }
        return os;
    }

private:
    CVector<CPerson> m_people;
    mutable size_t idx = 0;
    friend bool emailCmp(const CPerson &a, const CPerson &b);
};

//------------------------------------------------ HELPER FUNCTION -----------------------------------------------------
bool emailCmp (const CPerson &a, const CPerson &b) {
    return a.m_email.m_str < b.m_email.m_str;
}

//------------------------------------------------- PROGTEST BLOCK -----------------------------------------------------
#ifndef __PROGTEST__

bool matchOutput(const CMail &m, const char *str) {
    ostringstream oss;
    oss << m;
    return oss.str() == str;
}

int main(void) {
    char from[100], to[100], body[1024];

    assert(CMail("john", "peter", "progtest deadline") == CMail("john", "peter", "progtest deadline"));
    assert(!(CMail("john", "peter", "progtest deadline") == CMail("john", "progtest deadline", "peter")));
    assert(!(CMail("john", "peter", "progtest deadline") == CMail("peter", "john", "progtest deadline")));
    assert(!(CMail("john", "peter", "progtest deadline") == CMail("peter", "progtest deadline", "john")));
    assert(!(CMail("john", "peter", "progtest deadline") == CMail("progtest deadline", "john", "peter")));
    assert(!(CMail("john", "peter", "progtest deadline") == CMail("progtest deadline", "peter", "john")));
    CMailServer s0;
    s0.sendMail(CMail("john", "peter", "some important mail"));
    strncpy(from, "john", sizeof(from));
    strncpy(to, "thomas", sizeof(to));
    strncpy(body, "another important mail", sizeof(body));
    s0.sendMail(CMail(from, to, body));
    strncpy(from, "john", sizeof(from));
    strncpy(to, "alice", sizeof(to));
    strncpy(body, "deadline notice", sizeof(body));
    s0.sendMail(CMail(from, to, body));
    s0.sendMail(CMail("alice", "john", "deadline confirmation"));
    s0.sendMail(CMail("peter", "alice", "PR bullshit"));
    CMailIterator i0 = s0.inbox("alice");
    assert(i0 && *i0 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i0, "From: john, To: alice, Body: deadline notice"));
    assert(++i0 && *i0 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i0, "From: peter, To: alice, Body: PR bullshit"));
    assert(!++i0);

    CMailIterator i1 = s0.inbox("john");
    assert(i1 && *i1 == CMail("alice", "john", "deadline confirmation"));
    assert(matchOutput(*i1, "From: alice, To: john, Body: deadline confirmation"));
    assert(!++i1);

    CMailIterator i2 = s0.outbox("john");
    assert(i2 && *i2 == CMail("john", "peter", "some important mail"));
    assert(matchOutput(*i2, "From: john, To: peter, Body: some important mail"));
    assert(++i2 && *i2 == CMail("john", "thomas", "another important mail"));
    assert(matchOutput(*i2, "From: john, To: thomas, Body: another important mail"));
    assert(++i2 && *i2 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i2, "From: john, To: alice, Body: deadline notice"));
    assert(!++i2);

    CMailIterator i3 = s0.outbox("thomas");
    assert(!i3);

    CMailIterator i4 = s0.outbox("steve");
    assert(!i4);

    CMailIterator i5 = s0.outbox("thomas");
    s0.sendMail(CMail("thomas", "boss", "daily report"));
    assert(!i5);

    CMailIterator i6 = s0.outbox("thomas");
    assert(i6 && *i6 == CMail("thomas", "boss", "daily report"));
    assert(matchOutput(*i6, "From: thomas, To: boss, Body: daily report"));
    assert(!++i6);

    CMailIterator i7 = s0.inbox("alice");
    s0.sendMail(CMail("thomas", "alice", "meeting details"));
    assert(i7 && *i7 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i7, "From: john, To: alice, Body: deadline notice"));
    assert(++i7 && *i7 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i7, "From: peter, To: alice, Body: PR bullshit"));
    assert(!++i7);

    CMailIterator i8 = s0.inbox("alice");
    assert(i8 && *i8 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i8, "From: john, To: alice, Body: deadline notice"));
    assert(++i8 && *i8 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i8, "From: peter, To: alice, Body: PR bullshit"));
    assert(++i8 && *i8 == CMail("thomas", "alice", "meeting details"));
    assert(matchOutput(*i8, "From: thomas, To: alice, Body: meeting details"));
    assert(!++i8);

    CMailServer s1(s0);
    s0.sendMail(CMail("joe", "alice", "delivery details"));
    s1.sendMail(CMail("sam", "alice", "order confirmation"));
    CMailIterator i9 = s0.inbox("alice");
    assert(i9 && *i9 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i9, "From: john, To: alice, Body: deadline notice"));
    assert(++i9 && *i9 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i9, "From: peter, To: alice, Body: PR bullshit"));
    assert(++i9 && *i9 == CMail("thomas", "alice", "meeting details"));
    assert(matchOutput(*i9, "From: thomas, To: alice, Body: meeting details"));
    assert(++i9 && *i9 == CMail("joe", "alice", "delivery details"));
    assert(matchOutput(*i9, "From: joe, To: alice, Body: delivery details"));
    assert(!++i9);

    CMailIterator i10 = s1.inbox("alice");
    assert(i10 && *i10 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i10, "From: john, To: alice, Body: deadline notice"));
    assert(++i10 && *i10 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i10, "From: peter, To: alice, Body: PR bullshit"));
    assert(++i10 && *i10 == CMail("thomas", "alice", "meeting details"));
    assert(matchOutput(*i10, "From: thomas, To: alice, Body: meeting details"));
    assert(++i10 && *i10 == CMail("sam", "alice", "order confirmation"));
    assert(matchOutput(*i10, "From: sam, To: alice, Body: order confirmation"));
    assert(!++i10);

    CMailServer s2;
    s2.sendMail(CMail("alice", "alice", "mailbox test"));
    CMailIterator i11 = s2.inbox("alice");
    assert(i11 && *i11 == CMail("alice", "alice", "mailbox test"));
    assert(matchOutput(*i11, "From: alice, To: alice, Body: mailbox test"));
    assert(!++i11);

    s2 = s0;
    s0.sendMail(CMail("steve", "alice", "newsletter"));
    s2.sendMail(CMail("paul", "alice", "invalid invoice"));
    CMailIterator i12 = s0.inbox("alice");
    assert(i12 && *i12 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i12, "From: john, To: alice, Body: deadline notice"));
    assert(++i12 && *i12 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i12, "From: peter, To: alice, Body: PR bullshit"));
    assert(++i12 && *i12 == CMail("thomas", "alice", "meeting details"));
    assert(matchOutput(*i12, "From: thomas, To: alice, Body: meeting details"));
    assert(++i12 && *i12 == CMail("joe", "alice", "delivery details"));
    assert(matchOutput(*i12, "From: joe, To: alice, Body: delivery details"));
    assert(++i12 && *i12 == CMail("steve", "alice", "newsletter"));
    assert(matchOutput(*i12, "From: steve, To: alice, Body: newsletter"));
    assert(!++i12);

    CMailIterator i13 = s2.inbox("alice");
    assert(i13 && *i13 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i13, "From: john, To: alice, Body: deadline notice"));
    assert(++i13 && *i13 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i13, "From: peter, To: alice, Body: PR bullshit"));
    assert(++i13 && *i13 == CMail("thomas", "alice", "meeting details"));
    assert(matchOutput(*i13, "From: thomas, To: alice, Body: meeting details"));
    assert(++i13 && *i13 == CMail("joe", "alice", "delivery details"));
    assert(matchOutput(*i13, "From: joe, To: alice, Body: delivery details"));
    assert(++i13 && *i13 == CMail("paul", "alice", "invalid invoice"));
    assert(matchOutput(*i13, "From: paul, To: alice, Body: invalid invoice"));
    assert(!++i13);

    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
