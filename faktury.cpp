#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <iterator>
#include <functional>

using namespace std;

class CDate {
    public:
        //---------------------------------------------------------------------------------------------
        CDate(int y, int m, int d)
                : m_Year(y),
                  m_Month(m),
                  m_Day(d)
        {
        }

        //---------------------------------------------------------------------------------------------
        int compare(const CDate &x) const {
            if (m_Year != x.m_Year)
                return m_Year - x.m_Year;
            if (m_Month != x.m_Month)
                return m_Month - x.m_Month;
            return m_Day - x.m_Day;
        }

        //---------------------------------------------------------------------------------------------
        int year(void) const {
            return m_Year;
        }

        //---------------------------------------------------------------------------------------------
        int month(void) const {
            return m_Month;
        }

        //---------------------------------------------------------------------------------------------
        int day(void) const {
            return m_Day;
        }

        //---------------------------------------------------------------------------------------------
        friend ostream &operator<<(ostream &os, const CDate &x) {
            char oldFill = os.fill();
            return os << setfill('0') << setw(4) << x.m_Year << "-"
                      << setw(2) << static_cast<int> ( x.m_Month ) << "-"
                      << setw(2) << static_cast<int> ( x.m_Day )
                      << setfill(oldFill);
        }
        //---------------------------------------------------------------------------------------------
    private:
        int16_t m_Year;
        int8_t m_Month;
        int8_t m_Day;
};

#endif /* __PROGTEST__ */
//-----------------------------------------------| ↑ KONECT PROGTEST PREKLADU ↑ |-------------------------------------------

size_t insertionOrder = 0;

void normalizeString(string &str) {
    size_t i = 0, j = 0;
    size_t len = str.length();
    for( i = 0; i < len; i++) {
        if ( str[0] == ' ' ) {
            for( i = 0; i < (len - 1); i++) {
                str[i] = str[i + 1];
            }
            str[i] = '\0';
            len--;
            i = -1;
            continue;
        }
        if(str[i] == ' ' && str[i+1] ==' ') {
            for(j = i; j < (len-1); j++)
            {
                str[j] = str[j+1];
            }
            str[j] = '\0';
            len--;
            i--;
        }
        str[i] = tolower(str[i]);
    }
    if (str[i-1] == ' ') {
        str[i - 1] = '\0';
        i--;
    }

    string tmp = str;
    str.clear();
    for (size_t l = 0; l < i; l++) {
        str.push_back(tmp[l]);
    }
}

//-------------------------------------------------------| CInvoice |---------------------------------------------------
class CInvoice {
    public:
        CInvoice(const CDate &date, const string &seller, const string &buyer, unsigned int amount, double vat)
            : m_isIssued(0), m_isAccepted(0), m_date(date), m_seller(seller), m_buyer(buyer), m_amount(amount), m_vat(vat), m_insertionOrder(0)
        {
            m_normalizedSeller = seller;
            normalizeString(m_normalizedSeller);
            m_normalizedBuyer = buyer;
            normalizeString(m_normalizedBuyer);
        }

        CDate date(void) const {
            return m_date;
        }

        string seller(void) const {
            return m_seller;
        }

        string buyer(void) const {
            return m_buyer;
        }

        unsigned int amount(void) const {
            return m_amount;
        }

        double vat(void) const {
            return m_vat;
        }

        size_t getInsertionOrder(void) const {
            return m_insertionOrder;
        }

        string getNormSeller(void) const {
            return m_normalizedSeller;
        }

        string getNormBuyer(void) const {
            return m_normalizedBuyer;
        }

        string getOfiSeller(void) const {
            return m_officialSeller;
        }

        string getOfiBuyer(void) const {
            return m_officialBuyer;
        }

        const bool operator < (const CInvoice &b) const {
            if ( this->date().compare(b.date()) < 0 ) {        // inv1.date je mensi nez inv2.date
                return true;
            } else if ( this->date().compare(b.date()) > 0) {  // inv1.date je vetsi nez inv2.date
                return false;
            }
            if ( this->getNormSeller() < b.getNormSeller()) {
                return true;
            } else if ( this->getNormSeller() > b.getNormSeller()) {
                return false;
            }
            if ( this->getNormBuyer() < b.getNormBuyer() ) {
                return true;
            } else if ( this->getNormBuyer() > b.getNormBuyer() ) {
                return false;
            }
            if (this->amount() < b.amount()) {
                return true;
            } else if (this->amount() > b.amount()) {
                return false;
            }
            if (this->vat() < b.vat()) {
                return true;
            } else if (this->vat() > b.vat()) {
                return false;
            }
            return false;
        }

        bool operator==(const CInvoice &other) const {
            if (this->date().compare(other.date()) == 0 &&
                this->getNormSeller() == other.getNormSeller() &&
                this->getNormBuyer() == other.getNormBuyer() &&
                this->amount() == other.amount() &&
                this->vat() == other.vat()) {
                return true;
            }
            return false;
        }

        friend ostream& operator<<(ostream& os, const CInvoice& invoice) {
            os << invoice.date() << ", ";
            os << invoice.seller() << ", ";
            os << invoice.buyer() << ", ";
            os << invoice.amount() << ", ";
            os << invoice.vat() << ", ";
            os << "is issued = " << invoice.m_isIssued << ", ";
            os << "is accepted = " << invoice.m_isAccepted << ", ";
            os << "is paired = " << invoice.m_isPaired << ", ";
            os << "insertion order = " << invoice.m_insertionOrder;
            os << endl;
            return os;
        }

    mutable int m_isIssued = 0;
    mutable int m_isAccepted = 0;
    mutable int m_isPaired = 0;

private:
        CDate m_date;
        string m_seller;
        string m_buyer;
        unsigned int m_amount;
        double m_vat;   // DPH

        mutable size_t m_insertionOrder;
        string m_normalizedSeller;
        string m_normalizedBuyer;

        mutable string m_officialSeller;
        mutable string m_officialBuyer;

        int deletedIssued = 0;
        int deletedAccepted = 0;

        friend class CVATRegister;
};

//-------------------------------------------------------| CSortOpt |---------------------------------------------------
class CSortOpt {
    public:
        static const int BY_DATE = 0;
        static const int BY_BUYER = 1;
        static const int BY_SELLER = 2;
        static const int BY_AMOUNT = 3;
        static const int BY_VAT = 4;

        CSortOpt(void) {}

        CSortOpt &addKey(int key, bool ascending = true) {
            m_sortKeys.emplace_back(key, ascending);
            return *this;
        }

        bool operator() (const CInvoice &inv1, const CInvoice &inv2) const {
            for (const auto &sortKey : m_sortKeys) {
                switch (sortKey.first) {
                    case BY_DATE:
                        if ( inv1.date().compare(inv2.date()) != 0 ) {        // inv1.date je mensi nez inv2.date
                            return (sortKey.second ?
                                    inv1.date().compare(inv2.date()) < 0 :
                                    inv2.date().compare(inv1.date()) < 0);
                        }
                        break;  // inv1.date is the same as inv2.date
                    case BY_SELLER:
                        if ( strcasecmp(inv1.seller().c_str(), inv2.seller().c_str()) != 0) {
                            return (sortKey.second ?
                                    strcasecmp(inv1.seller().c_str(), inv2.seller().c_str()) < 0 :
                                    strcasecmp(inv2.seller().c_str(), inv1.seller().c_str()) < 0);
                        }
                        break;
                    case BY_BUYER:
                        if ( strcasecmp(inv1.buyer().c_str(), inv2.buyer().c_str()) != 0) {
                            return (sortKey.second ?
                                    strcasecmp(inv1.buyer().c_str(), inv2.buyer().c_str()) < 0 :
                                    strcasecmp(inv2.buyer().c_str(), inv1.buyer().c_str()) < 0);
                        }
                        break;
                    case BY_AMOUNT:
                        if (inv1.amount() != inv2.amount()) {
                            return (sortKey.second ?
                                    inv1.amount() < inv2.amount() :
                                    inv2.amount() < inv1.amount());
                        }
                        break;
                    case BY_VAT:
                        if (inv1.vat() != inv2.vat()) {
                            return (sortKey.second ?
                                    inv1.vat() < inv2.vat() :
                                    inv2.vat() < inv1.vat());
                        }
                        break;
                }
            }
            // If invoices are the same according to all sorting keys, compare their insertion order
//            cout << "insertion order compare:" << endl;
//            cout << inv1 << "   #   " << inv2;
//            cout << inv1.getInsertionOrder() << ", " << inv2.getInsertionOrder() << endl;
            return inv1.getInsertionOrder() < inv2.getInsertionOrder();
        }

    private:
        vector<pair<int, bool>> m_sortKeys;
        friend class CVATRegister;
};

//------------------------------------------------------| CVATRegister |------------------------------------------------
class CVATRegister {
    public:
        CVATRegister(void) {}

        // Register company into m_companies if it doesn't exist yet
        // using normalized name for comparison
        bool registerCompany(const string &name) {
            string target = name;
            normalizeString(target);
//            cout << "working on company:" << name << "   ===>    " << "target company =>" << target << "|" << endl;
            //auto search = m_companies.find(target);
            if ( m_companies.find(target) != m_companies.end() ) {    // The given company is already registered
                //cout << "company:   |" << target << "|     found in the list here:     |" << (search != m_companies.end() ? search->first : "") << "|" << endl;
                return false;
            }
//            cout << "company:   |" << target << "|     not found here: " << (search != m_companies.end() ? search->first : "") << endl;
            m_companies[target];
            m_companies[target].first = name;
            return true;
        }

        // prida invoice do seznamu invoices firmy, ktera invoice vydala (prodala)
        // using normalized name for comparison
        bool addIssued(const CInvoice &x) {
//            cout << "seller and buyer =" << x.getNormSeller() << "#" << x.getNormBuyer() << "#" << endl;
            // Return false if the Invoice cannot be verified
            if ( !verifyInvoice( x, x.getNormSeller() ) ) {
//                cout << "cannot verify issued invoice\n";
                return false;
            }
            auto itIssued = m_companies[x.getNormSeller()].second.find(x);
            if ((itIssued != m_companies[x.getNormSeller()].second.end()) &&
                (findInvoice(m_companies[x.getNormSeller()].second, x ) && itIssued->m_isIssued == 1 )) {
                return false;
            }

            // Add the invoice to the seller company register
            x.m_officialSeller = m_companies[x.getNormSeller()].first;
            x.m_officialBuyer = m_companies[x.getNormBuyer()].first;
//            cout << "invoice after all edits =" << x << endl;

            m_companies[x.getNormSeller()].second.insert(x);
            m_companies[x.getNormBuyer()].second.insert(x);

            auto itBuyer = m_companies[x.getNormBuyer()].second.find(x);
            auto itSeller = m_companies[x.getNormSeller()].second.find(x);
            itBuyer->m_isIssued = 1;
            itSeller->m_isIssued = 1;
            itBuyer->m_insertionOrder = insertionOrder;
            itSeller->m_insertionOrder = insertionOrder;
            if (itSeller->m_isAccepted == 1)
                itSeller->m_isPaired = 1;
            if (itBuyer->m_isAccepted == 1)
                itBuyer->m_isPaired = 1;

            if (itBuyer->m_isAccepted == 0 && itSeller->m_isAccepted == 0)
                insertionOrder++;

//            cout << "OFFICIAL COMPANY NAME =" << m_companies[x.getNormBuyer()].first << endl;
            return true;
        }

        // prida invoice do seznamu invoices firmy, ktera invoice prijala (koupila)
        // using normalized name for comparison
        bool addAccepted(const CInvoice &x) {
//            cout << "seller and buyer =" << x.getNormSeller() << "#" << x.getNormBuyer() << "#" << endl;
            // Return false if the Invoice cannot be verified
            if (!verifyInvoice( x, x.getNormBuyer() ) ) {
//                cout << "cannot verify accepted invoice\n";
                return false;
            }
            auto itAccepted = m_companies[x.getNormBuyer()].second.find(x);
            if ((itAccepted != m_companies[x.getNormBuyer()].second.end()) &&
                (findInvoice(m_companies[x.getNormBuyer()].second, x ) && itAccepted->m_isAccepted == 1 )) {
                return false;
            }


            // Add the invoice to the seller company register
            x.m_officialBuyer = m_companies[x.getNormBuyer()].first;
            x.m_officialSeller = m_companies[x.getNormSeller()].first;

            m_companies[x.getNormSeller()].second.insert(x);
            m_companies[x.getNormBuyer()].second.insert(x);

            auto itBuyer = m_companies[x.getNormBuyer()].second.find(x);
            auto itSeller = m_companies[x.getNormSeller()].second.find(x);
            itBuyer->m_isAccepted = 1;
            itSeller->m_isAccepted = 1;
            itBuyer->m_insertionOrder = insertionOrder;
            itSeller->m_insertionOrder = insertionOrder;
            if (itSeller->m_isIssued == 1)
                itSeller->m_isPaired = 1;
            if (itBuyer->m_isIssued == 1)
                itBuyer->m_isPaired = 1;

            if ( itSeller->m_isIssued == 0 && itBuyer->m_isIssued == 0)
                insertionOrder++;

//            cout << "OFFICIAL COMPANY NAME =" << m_companies[x.getNormBuyer()].first << "|" << endl;
            return true;
        }

        // prida invoice do seznamu invoices firmy, ktera invoice vydala (prodala)
        // using normalized name for comparison
        bool delIssued(const CInvoice &x) {
            if ( m_companies[x.getNormSeller()].second.find(x) == m_companies[x.getNormSeller()].second.end() )
                return false;

            auto itBuyer = m_companies[x.getNormBuyer()].second.find(x);
            if (itBuyer != m_companies[x.getNormBuyer()].second.end()) {
                itBuyer->m_isIssued = 0;
                itBuyer->m_isPaired = 0;
            }
            if (itBuyer->m_isIssued == 0 && itBuyer->m_isAccepted == 0) {
                m_companies[x.getNormBuyer()].second.erase(x);
            }

            auto itSeller = m_companies[x.getNormSeller()].second.find(x);
            if (itSeller != m_companies[x.getNormSeller()].second.end()) {
                itSeller->m_isIssued = 0;
                itSeller->m_isPaired = 0;
            }
            if (itSeller->m_isIssued == 0 && itSeller->m_isAccepted == 0) {
                m_companies[x.getNormSeller()].second.erase(x);
            }

            return true;
        }

        // prida invoice do seznamu invoices firmy, ktera invoice prijala (koupila)
        // using normalized name for comparison
        bool delAccepted(const CInvoice &x) {
            if ( m_companies[x.getNormBuyer()].second.find(x) == m_companies[x.getNormBuyer()].second.end() )
                return false;

            auto itBuyer = m_companies[x.getNormBuyer()].second.find(x);
            if (itBuyer != m_companies[x.getNormBuyer()].second.end()) {
                itBuyer->m_isAccepted = 0;
                itBuyer->m_isPaired = 0;
            }
            if (itBuyer->m_isIssued == 0 && itBuyer->m_isAccepted == 0) {
                m_companies[x.getNormBuyer()].second.erase(x);
            }

            auto itSeller = m_companies[x.getNormSeller()].second.find(x);
            if (itSeller != m_companies[x.getNormSeller()].second.end()) {
                itSeller->m_isAccepted = 0;
                itSeller->m_isPaired = 0;
            }
            if (itSeller->m_isIssued == 0 && itSeller->m_isAccepted == 0) {
                m_companies[x.getNormSeller()].second.erase(x);
            }

            return true;
        }

        list <CInvoice> unmatched(const string &company, const CSortOpt &sortBy) const {
            list <CInvoice> result;
            string target = company;
            normalizeString(target);
            if (m_companies.count(target) <= 0)
                return result;
            const set <CInvoice> &targetCompanyInvoices = m_companies.at(target).second;
            for (const auto &invoice : targetCompanyInvoices) {
//                cout << invoice;
                if (invoice.m_isPaired == 0) {
                    result.emplace_back( invoice.date(), invoice.getOfiSeller(), invoice.getOfiBuyer(), invoice.amount(), invoice.vat() );
                }
                auto it = result.rbegin();
                it->m_insertionOrder = invoice.m_insertionOrder;
            }
//            cout << "FINAL LIST OF UNMATCHED INVOICES BEFORE SORT:\n";
//            printList(result);
            result.sort( sortBy );
            return result;
        }


        bool verifyInvoice (const CInvoice &x, string targetCompany) {
            string normSeller = x.getNormSeller();
            string normBuyer = x.getNormBuyer();

            // Return false if:
            //  - seller == buyer
            //  - at least one of the companies isn't registered
            //  - identical invoice has already been added
            if (normSeller == normBuyer ||
                m_companies.find(normSeller) == m_companies.end() ||
                m_companies.find(normBuyer) == m_companies.end() ) {
                return false;
            }
            return true;
        }

        bool findInvoice (const set<CInvoice> invoices, const CInvoice &target) const {
            const auto it = lower_bound(invoices.begin(), invoices.end(), target, invoiceCmp);
            if (it == invoices.end()) {
    //                cout << "reached end" << endl;
                return false;
            }
    //            cout << "lower_bound found this~~> " << *it;
            return *it == target;
        }

        static bool invoiceCmp (const CInvoice &a, const CInvoice &b ) {
            return a < b;
        }

        // Print the names of registered companies (the names are normalized)
        friend ostream& operator<<(ostream& os, const CVATRegister &reg) {
            for (const auto& company : reg.m_companies) {
                os << "|" << company.first << "|" << endl;
            }
            return os;
        }

        void printInvoices (const string &company) {
            set<CInvoice> &invoices = m_companies[company].second;

            for (const CInvoice& invoice : invoices) {
//                cout << "Invoice for company " << company << " ==>";
                cout << invoice;
            }
        }

        void printList (const list<CInvoice> invoices) const {
            for (auto &inv : invoices) {
                cout << inv.date() << ", ";
                cout << inv.seller() << ", ";
                cout << inv.buyer() << ", ";
                cout << inv.amount() << ", ";
                cout << inv.vat();
                cout << endl;
            }
        }

        friend void printList (const list<CInvoice> invoices);

    private:
        map<string, pair<string, set<CInvoice>>> m_companies; // stores normalized names of companies
};

//----------------------------------------------------| PROGTEST PREKLAD |----------------------------------------------
#ifndef __PROGTEST__

void printList (const list<CInvoice> invoices) {
    for (auto &inv : invoices) {
        cout << inv.date() << ", ";
        cout << inv.seller() << ", ";
        cout << inv.buyer() << ", ";
        cout << inv.amount() << ", ";
        cout << inv.vat();
        cout << endl;
    }
}

bool equalLists(const list <CInvoice> &a, const list <CInvoice> &b) {
    /*
    cout << "\nMY LIST OF UNMATCHED INVOICES:\n";
    printList(a);
    cout << "----------------------------------------------\n";
    cout << "CORRECT LIST OF UNMATCHED INVOICES:\n";
    printList(b);
    cout << endl;
     */

    if (a.size() != b.size() ||
        !equal(a.begin(), a.end(), b.begin()))
        return false;

    return true;
}

int main(void) {
    CVATRegister test1;
    test1.registerCompany("Abc");
    test1.registerCompany("Test");
    test1.addIssued(CInvoice(CDate(2000, 1, 1), "Abc", "Test", 100, 20));
    test1.addAccepted(CInvoice(CDate(2000, 1, 1), "   Abc", "Test   ", 100, 20));
    assert(equalLists(test1.unmatched("Abc", CSortOpt().addKey(CSortOpt::BY_DATE, true)), {}));

    CVATRegister test2;
    test2.registerCompany(" Abc");
    test2.registerCompany("Test");
    test2.addIssued(CInvoice(CDate(2000, 1, 1), "Abc", "Test", 100, 20));
    test2.addAccepted(CInvoice(CDate(2000, 1, 1), "Abc", "TesT", 100, 20));
    assert(equalLists(test2.unmatched("Abc", CSortOpt().addKey(CSortOpt::BY_DATE, true)), {}));

    CVATRegister r;
    assert ( r . registerCompany ( "first Company" ) );
    assert ( r . registerCompany ( "Second     Company" ) );
    assert ( r . registerCompany ( "ThirdCompany, Ltd." ) );
    assert ( r . registerCompany ( "Third Company, Ltd." ) );
    assert ( !r . registerCompany ( "Third Company, Ltd." ) );
    assert ( !r . registerCompany ( " Third  Company,  Ltd.  " ) );
    assert ( r . addIssued ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", "Second Company ", 100, 20 ) ) );
    assert ( r . addIssued ( CInvoice ( CDate ( 2000, 1, 2 ), "FirSt Company", "Second Company ", 200, 30 ) ) );
    assert ( r . addIssued ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", "Second Company ", 100, 30 ) ) );
    assert ( r . addIssued ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", "Second Company ", 300, 30 ) ) );
    assert ( r . addIssued ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", " Third  Company,  Ltd.   ", 200, 30 ) ) );
    assert ( r . addIssued ( CInvoice ( CDate ( 2000, 1, 1 ), "Second Company ", "First Company", 300, 30 ) ) );
    assert ( r . addIssued ( CInvoice ( CDate ( 2000, 1, 1 ), "Third  Company,  Ltd.", "  Second    COMPANY ", 400, 34 ) ) );
    assert ( !r . addIssued ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", "Second Company ", 300, 30 ) ) );
    assert ( !r . addIssued ( CInvoice ( CDate ( 2000, 1, 4 ), "First Company", "First   Company", 200, 30 ) ) );
    assert ( !r . addIssued ( CInvoice ( CDate ( 2000, 1, 4 ), "Another Company", "First   Company", 200, 30 ) ) );
    assert ( equalLists ( r . unmatched ( "First Company", CSortOpt () . addKey ( CSortOpt::BY_SELLER, true ) . addKey ( CSortOpt::BY_BUYER, false ) . addKey ( CSortOpt::BY_DATE, false ) ),
                          list<CInvoice>
                          {
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Third Company, Ltd.", 200, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 2 ), "first Company", "Second     Company", 200, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 100, 20.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 100, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 300, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "Second     Company", "first Company", 300, 30.000000 )
                          } ) );

    assert ( equalLists ( r . unmatched ( "First Company", CSortOpt () . addKey ( CSortOpt::BY_DATE, true ) . addKey ( CSortOpt::BY_SELLER, true ) . addKey ( CSortOpt::BY_BUYER, true ) ),
                          list<CInvoice>
                          {
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 100, 20.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 100, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 300, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Third Company, Ltd.", 200, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "Second     Company", "first Company", 300, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 2 ), "first Company", "Second     Company", 200, 30.000000 )
                          } ) );
    assert ( equalLists ( r . unmatched ( "First Company", CSortOpt () . addKey ( CSortOpt::BY_VAT, true ) . addKey ( CSortOpt::BY_AMOUNT, true ) . addKey ( CSortOpt::BY_DATE, true ) . addKey ( CSortOpt::BY_SELLER, true ) . addKey ( CSortOpt::BY_BUYER, true ) ),
                          list<CInvoice>
                          {
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 100, 20.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 100, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Third Company, Ltd.", 200, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 2 ), "first Company", "Second     Company", 200, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 300, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "Second     Company", "first Company", 300, 30.000000 )
                          } ) );
    assert ( equalLists ( r . unmatched ( "First Company", CSortOpt () ),
                          list<CInvoice>
                          {
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 100, 20.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 2 ), "first Company", "Second     Company", 200, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 100, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 300, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Third Company, Ltd.", 200, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "Second     Company", "first Company", 300, 30.000000 )
                          } ) );
    assert ( equalLists ( r . unmatched ( "second company", CSortOpt () . addKey ( CSortOpt::BY_DATE, false ) ),
                          list<CInvoice>
                          {
                                  CInvoice ( CDate ( 2000, 1, 2 ), "first Company", "Second     Company", 200, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 100, 20.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 100, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 300, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "Second     Company", "first Company", 300, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "Third Company, Ltd.", "Second     Company", 400, 34.000000 )
                          } ) );
    assert ( equalLists ( r . unmatched ( "last company", CSortOpt () . addKey ( CSortOpt::BY_VAT, true ) ),
                          list<CInvoice>
                          {
                          } ) );
    assert ( r . addAccepted ( CInvoice ( CDate ( 2000, 1, 2 ), "First Company", "Second Company ", 200, 30 ) ) );
    assert ( r . addAccepted ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", " Third  Company,  Ltd.   ", 200, 30 ) ) );
    assert ( r . addAccepted ( CInvoice ( CDate ( 2000, 1, 1 ), "Second company ", "First Company", 300, 32 ) ) );
    assert ( equalLists ( r . unmatched ( "First Company", CSortOpt () . addKey ( CSortOpt::BY_SELLER, true ) . addKey ( CSortOpt::BY_BUYER, true ) . addKey ( CSortOpt::BY_DATE, true ) ),
                          list<CInvoice>
                          {
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 100, 20.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 100, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 300, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "Second     Company", "first Company", 300, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "Second     Company", "first Company", 300, 32.000000 )
                          } ) );
    assert ( !r . delIssued ( CInvoice ( CDate ( 2001, 1, 1 ), "First Company", "Second Company ", 200, 30 ) ) );
    assert ( !r . delIssued ( CInvoice ( CDate ( 2000, 1, 1 ), "A First Company", "Second Company ", 200, 30 ) ) );
    assert ( !r . delIssued ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", "Second Hand", 200, 30 ) ) );
    assert ( !r . delIssued ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", "Second Company", 1200, 30 ) ) );
    assert ( !r . delIssued ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", "Second Company", 200, 130 ) ) );
    assert ( r . delIssued ( CInvoice ( CDate ( 2000, 1, 2 ), "First Company", "Second Company", 200, 30 ) ) );
    assert ( equalLists ( r . unmatched ( "First Company", CSortOpt () . addKey ( CSortOpt::BY_SELLER, true ) . addKey ( CSortOpt::BY_BUYER, true ) . addKey ( CSortOpt::BY_DATE, true ) ),
                          list<CInvoice>
                          {
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 100, 20.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 100, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 300, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 2 ), "first Company", "Second     Company", 200, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "Second     Company", "first Company", 300, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "Second     Company", "first Company", 300, 32.000000 )
                          } ) );
    assert ( r . delAccepted ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", " Third  Company,  Ltd.   ", 200, 30 ) ) );
    assert ( equalLists ( r . unmatched ( "First Company", CSortOpt () . addKey ( CSortOpt::BY_SELLER, true ) . addKey ( CSortOpt::BY_BUYER, true ) . addKey ( CSortOpt::BY_DATE, true ) ),
                          list<CInvoice>
                          {
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 100, 20.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 100, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 300, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 2 ), "first Company", "Second     Company", 200, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Third Company, Ltd.", 200, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "Second     Company", "first Company", 300, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "Second     Company", "first Company", 300, 32.000000 )
                          } ) );
    assert ( r . delIssued ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", " Third  Company,  Ltd.   ", 200, 30 ) ) );
    assert ( equalLists ( r . unmatched ( "First Company", CSortOpt () . addKey ( CSortOpt::BY_SELLER, true ) . addKey ( CSortOpt::BY_BUYER, true ) . addKey ( CSortOpt::BY_DATE, true ) ),
                          list<CInvoice>
                          {
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 100, 20.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 100, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "first Company", "Second     Company", 300, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 2 ), "first Company", "Second     Company", 200, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "Second     Company", "first Company", 300, 30.000000 ),
                                  CInvoice ( CDate ( 2000, 1, 1 ), "Second     Company", "first Company", 300, 32.000000 )
                          } ) );
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
