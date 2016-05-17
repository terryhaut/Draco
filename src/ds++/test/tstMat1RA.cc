//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   ds++/test/tstMat1RA.cc
 * \author Shawn Pautz
 * \date   Wed Dec 23 17:00:00 1998
 * \brief  This program tests the Mat1 container as a model of a Random Access
 *         Container.
 * \note   Copyright (C) 1998-2011 Los Alamos National Security, LLC
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "../Mat.hh"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

bool passed = true;
double value = 4.23;

// The following class exists only to test the Mat1 container with a
// non-trivial type.  It ought not to have a default constructor, but
// current compiler limitations require it.

class DoubleContainer
{
  public:
    double data;

// Eliminate the following constructor when the compiler allows it.

    // DoubleContainer() : data(0.) {}

    DoubleContainer(double _data) : data(_data) {}

    DoubleContainer(const DoubleContainer& dc) : data(dc.data) {}

    DoubleContainer& operator=(const DoubleContainer& dc)
    {
        data = dc.data;
        return *this;
    }
};

//---------------------------------------------------------------------------//
// Test the Mat1 container according to the Random Access Container
// requirements.
//---------------------------------------------------------------------------//

typedef rtt_dsxx::Mat1<double> X;
typedef rtt_dsxx::Mat1<DoubleContainer> XDC;

void f1(const X& x, const X& xcopy)
{
    if (xcopy != x)
        passed = false;
    if (xcopy.size() != x.size())
        passed = false;
}

void f2(const X::iterator& ) { /* empty */}

void f3(const X::iterator& x, const X::iterator& xcopy)
{
    if (xcopy != x)
        passed = false;
}

void f4(const X::const_iterator& ) { /* empty */ }

void f5(const X::const_iterator& x, const X::const_iterator& xcopy)
{
    if (xcopy != x)
        passed = false;
}

void f6(const X::reverse_iterator& ) { /* empty */ }

void f7(const X::reverse_iterator& x, const X::reverse_iterator& xcopy)
{
    if (xcopy != x)
        passed = false;
}

void f8(const X::const_reverse_iterator& ) { /*empty*/}

void f9(const X::const_reverse_iterator& x,
        const X::const_reverse_iterator& xcopy)
{
    if (xcopy != x)
        passed = false;
}


// Test the required RA typedefs and functions

void t1()
{
    cout << "t1: beginning.\n";

    {
    // Test for required typedefs

        typedef X::value_type value_type;
        typedef X::reference reference;
        typedef X::const_reference const_reference;
        typedef X::pointer pointer;
        typedef X::const_pointer const_pointer;
        typedef X::iterator iterator;
        typedef X::const_iterator const_iterator;
        typedef X::difference_type difference_type;
        typedef X::size_type size_type;
        typedef X::reverse_iterator reverse_iterator;
        typedef X::const_reverse_iterator const_reverse_iterator;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value), w(6, value + 1.);

    // Test the copy constructor.

        f1(x, X(x));
        X y(x);
        if (y != x)
            passed = false;
        if (y.size() != x.size())
            passed = false;
        X z = x;
        if (z != x)
            passed = false;

    // Test assignment.

        w = x;
        if (w != x)
            passed = false;
        if (w.size() != x.size())
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value), y(6, value + 1.), z(8, value + 2.);

    // Test equivalence relations.

        y = x;
        if (!(x == y))
            passed = false;
        if ((x != y) != !(x == y))
            passed = false;

    // Invariants

        y = x;
        z = y;
        X* yp = &y;
        if ((yp == &y) && !(*yp == y))
            passed = false;
        if (y != y)
            passed = false;
        if ((x == y) && !(y == x))
            passed = false;
        if (((x == y) && (y == z)) && !(x == z))
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value), y(6, value + 1.), z(10, value - 2.);

    // Test ordering relations.

        y = x;
        if (x < y)
            passed = false;
        if ((x < y) != lexicographical_compare(x.begin(),x.end(),
                                               y.begin(),y.end()))
            passed = false;
        if ( (x > y) != (y < x) )
            passed = false;
        if ( (x <= y) != !(y < x) )
            passed = false;
        if ( (x >= y) != !(x < y) )
            passed = false;

        if (x < z)
            passed = false;
        if ( (x > z) != (z < x))
            passed = false;
        if ((x <= z) != !(z < x))
            passed = false;
        if ((x >= z) != !(x < z))
            passed = false;

    // Invariants

        if (x < x)
            passed = false;
        y = x;
        x[1] -= 1.;
        if ((x < y) != !(y < x))
            passed = false;
        z = y;
        z[1] += 1.;
        if (((x < y) && (y < z)) && !(x < z))
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X *x = new X(10, value);

    // Test destructor.

        delete x;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value), y(10, value), v(8, value + 1.), w(8, value + 1.);
        const X cx(10, value);

    // Test for required container member functions.

        X::iterator iter1 = x.begin();
        X::iterator iter2 = x.end();
        if ((iter1 == iter2) != (x.size() == 0))
            passed = false;

        X::const_iterator citer1 = cx.begin();
        X::const_iterator citer2 = cx.end();
        if ((citer1 == citer2) != (cx.size() == 0))
            passed = false;

        X::size_type size;
        X::size_type max_size;
        size = x.size();
        max_size = x.max_size();
        if (max_size < size)
            passed = false;

        if (x.empty() != (x.size() == 0))
            passed = false;

        x = y;
        v = w;
        x.swap(v);
        X tmp = y;
        y = w;
        w = tmp;

        if (x != y || v != w)
            passed = false;

        for (X::iterator iter = x.begin(); iter != x.end(); iter++) {}
        for (X::const_iterator iter = cx.begin(); iter != cx.end(); iter++) {}

        if (!(x.size() == static_cast<size_t>(distance(x.begin(),x.end()))))
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value), y(10, value);
        const X cx(10, value);

    // Test for required container member functions.

        X::reverse_iterator iter1 = x.rbegin();
        if (x.rbegin() != X::reverse_iterator(x.end()))
            passed = false;
        X::reverse_iterator iter2 = x.rend();
        if (x.rend() != X::reverse_iterator(x.begin()))
            passed = false;
        if ((iter1 == iter2) != (x.size() == 0))
            passed = false;

        X::const_reverse_iterator citer1 = cx.rbegin();
        if (cx.rbegin() != X::const_reverse_iterator(cx.end()))
            passed = false;
        X::const_reverse_iterator citer2 = cx.rend();
        if (cx.rend() != X::const_reverse_iterator(cx.begin()))
            passed = false;
        if ((citer1 == citer2) != (cx.size() == 0))
            passed = false;

        for (X::reverse_iterator iter = x.rbegin();
             iter != x.rend(); iter++) {}
        for (X::const_reverse_iterator iter = cx.rbegin();
             iter != cx.rend(); iter++) {}
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value), y(10, value);
        const X cx(10, value);

        x[3] = y[5];
        x[3] = cx[3];
    }

    cout << "t1: end\n";
}



// Test the X::iterator functionality

void t2()
{
    cout << "t2: beginning.\n";

    {
        typedef iterator_traits<X::iterator>::value_type value_type;
        typedef iterator_traits<X::iterator>::difference_type difference_type;
        typedef iterator_traits<X::iterator>::reference reference;
        typedef iterator_traits<X::iterator>::pointer pointer;
        typedef iterator_traits<X::iterator>::iterator_category
                                              iterator_category;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value);

    // Test the default constructor.

        f2(X::iterator());
        X::iterator iter1;

    // Test the copy constructor.

        iter1 = x.begin();
        f3(iter1, X::iterator(iter1));
        X::iterator iter2(iter1);
        if (iter2 != iter1)
            passed = false;
        X::iterator iter3 = iter1;
        if (iter3 != iter1)
            passed = false;

    // Test assignment.

        X::iterator iter4;
        iter4 = iter1;
        if (iter4 != iter1)
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value);

        X::iterator iter1, iter2, iter3;
        iter1 = x.begin();

    // Test equivalence relations.

        iter2 = iter1;
        if (!(iter1 == iter2))
            passed = false;
        if ((iter1 != iter2) != !(iter1 == iter2))
            passed = false;

    // Invariants

        iter2 = iter1;
        iter3 = iter2;
        X::iterator* iter2p = &iter2;
        if ((iter2p == &iter2) && !(*iter2p == iter2))
            passed = false;
        if (iter2 != iter2)
            passed = false;
        if ((iter1 == iter2) && !(iter2 == iter1))
            passed = false;
        if (((iter1 == iter2) && (iter2 == iter3)) && !(iter1 == iter3))
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value);

        X::iterator iter1, iter2, iter3;
        iter1 = x.begin();

    // Test ordering relations.

        iter2 = iter1;
        if (iter1 < iter2)
            passed = false;
        if ((iter1 > iter2) != (iter2 < iter1))
            passed = false;
        if ((iter1 <= iter2) != !(iter2 < iter1))
            passed = false;
        if ((iter1 >= iter2) != !(iter1 < iter2))
            passed = false;

        iter3 = iter1;
        ++iter3;
        if (iter3 < iter1)
            passed = false;
        if ((iter3 > iter1) != (iter1 < iter3))
            passed = false;
        if ((iter3 <= iter1) != !(iter1 < iter3))
            passed = false;
        if ((iter3 >= iter1) != !(iter3 < iter1))
            passed = false;

    // Invariants

        if (iter1 < iter1)
            passed = false;
        iter2 = iter1;
        iter2++;
        if ((iter1 < iter2) != !(iter2 < iter1))
            passed = false;
        iter3 = iter2;
        iter3++;
        if (((iter1 < iter2) && (iter2 < iter3)) && !(iter1 < iter3))
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value);

        X::iterator iter1, iter2, iter3;
        iter1 = x.begin();
        iter2 = iter1;
        iter3 = iter2;

    // Invariants

        if ((!(iter1 < iter2) && !(iter2 < iter1) &&
             !(iter2 < iter3) && !(iter3 < iter2))
          && !(!(iter1 < iter3) && !(iter3 < iter1)))
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value);

        X::iterator iter = x.begin();

    // Test dereferenceability.

        if (*iter != *(x.begin()))
            passed = false;
        *iter = value - 1.;
        if (*iter != value - 1.)
            passed = false;
    }

    {
        DoubleContainer dc(value);
        DoubleContainer dcarray[5] = {dc, dc, dc, dc, dc};

    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        XDC x(dcarray, 5);

        XDC::iterator iter = x.begin();

    // Test member access

        iter->data = value + 1.;
        if ((*iter).data != value + 1.)
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value);

        X::iterator iter1 = x.begin();
        X::iterator iter2 = x.begin();

    // Invariant

        if ((iter1 == iter2) != (&(*iter1) == &(*iter2)))
            passed = false;
        iter1++;
        if ((iter1 == iter2) != (&(*iter1) == &(*iter2)))
            passed = false;
    }

    {
        typedef iterator_traits<X::iterator>::value_type value_type;

        double darray[5] = {0, 1, 2, 3, 4};

    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(darray, 5);

        X::iterator iter1 = x.begin();
        X::iterator iter2 = x.begin();

    // Test increments

        ++iter1;

        iter1 = iter2;
        iter1++;
        ++iter2;
        if (iter1 != iter2)
            passed = false;

        iter2 = x.begin();
        iter1 = iter2;
        value_type t = *iter2;
        ++iter2;
        if (*iter1++ != t)
            passed = false;
        if (iter1 != iter2)
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value);

        X::iterator iter1 = x.begin();
        X::iterator iter2 = x.begin();

        if (!(&iter1 == &++iter1))
            passed = false;
        iter1 = iter2;
        if (!(++iter1 == ++iter2))
            passed = false;
    }

    {
        typedef iterator_traits<X::iterator>::value_type value_type;

        double darray[5] = {0, 1, 2, 3, 4};

    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(darray, 5);

        X::iterator iter1 = x.end();
        X::iterator iter2 = x.end();

    // Test decrements

        --iter1;
        if (!(&iter1 == &--iter1))
            passed = false;
        iter1 = iter2;
        if (!(--iter1 == --iter2))
            passed = false;
        iter1 = iter2;
        ++iter1;
        if (!(--iter1 == iter2))
            passed = false;

        iter1 = x.end();
        iter2 = iter1;
        X::iterator iter3 = iter2;
        --iter2;
        if (iter1-- != iter3)
            passed = false;
        if (iter1 != iter2)
            passed = false;

    // Invariants

        iter1 = x.begin();
        ++iter1;
        --iter1;
        if (iter1 != x.begin())
            passed = false;

        iter1 = x.end();
        --iter1;
        ++iter1;
        if (iter1 != x.end())
            passed = false;
    }

    {
        typedef iterator_traits<X::iterator>::value_type value_type;
        typedef iterator_traits<X::iterator>::difference_type difference_type;

        double darray[5] = {0, 1, 2, 3, 4};

    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(darray, 5);

        X::iterator iter1 = x.begin();
        X::iterator iter2 = x.begin();
        X::iterator iter3 = x.begin();

    // Iterator addition

        iter1 += 0;
        if (iter1 != iter2)
            passed = false;

        iter1 += 3;
        ++iter2;
        ++iter2;
        ++iter2;
        if (iter1 != iter2)
            passed = false;

        iter1 += -3;
        --iter2;
        --iter2;
        --iter2;
        if (iter1 != iter2)
            passed = false;

        iter1 = x.begin();
        iter2 = x.begin();
        iter3 = iter1 + 3;
        iter2 += 3;
        if (iter3 != iter2)
            passed = false;
        if (iter1 != x.begin())
            passed = false;

        iter1 = x.begin();
        iter2 = x.begin();
        iter3 = 3 + iter1;
        iter2 += 3;
        if (iter3 != iter2)
            passed = false;
        if (iter1 != x.begin())
            passed = false;

    // Iterator subtraction

        iter1 = x.end();
        iter2 = x.end();
        iter1 -= 0;
        if (iter1 != iter2)
            passed = false;

        iter1 -= 3;
        iter2 += -3;
        if (iter1 != iter2)
            passed = false;

        iter1 -= -3;
        iter2 += -(-3);
        if (iter1 != iter2)
            passed = false;

        iter1 = x.end();
        iter2 = x.end();
        iter3 = iter1 - 3;
        iter2 -= 3;
        if (iter3 != iter2)
            passed = false;
        if (iter1 != x.end())
            passed = false;

    // Iterator difference.

        iter1 = x.begin();
        iter2 = x.end();
        difference_type d = iter2 - iter1;
        if (!(iter2 == iter1 + d))
            passed = false;

    // Element access and assignment

        iter1 = x.begin();
        if (iter1[2] != *(iter1 + 2))
            passed = false;

        iter1[2] = 12.;
        if (*(iter1 + 2) != 12.)
            passed = false;

    // Invariants

        iter1 = x.begin();
        iter1 += 3;
        iter1 -= 3;
        if (iter1 != x.begin())
            passed = false;
        iter2 = (iter1 + 3) - 3;
        if (iter2 != x.begin())
            passed = false;

        iter1 = x.end();
        iter1 -= 3;
        iter1 += 3;
        if (iter1 != x.end())
            passed = false;
        iter2 = (iter1 - 3) + 3;
        if (iter2 != x.end())
            passed = false;

        iter1 = x.begin();
        iter2 = x.end();
        if (!(iter2 == iter1 + (iter2 - iter1)))
            passed = false;
        if (!(iter2 - iter1 >= 0))
            passed = false;
    }

    cout << "t2: end\n";
}




// Test the X::const_iterator functionality

void t3()
{
    cout << "t3: beginning.\n";

    {
        typedef iterator_traits<X::const_iterator>::value_type value_type;
        typedef iterator_traits<X::const_iterator>::difference_type
                                                    difference_type;
        typedef iterator_traits<X::const_iterator>::reference reference;
        typedef iterator_traits<X::const_iterator>::pointer pointer;
        typedef iterator_traits<X::const_iterator>::iterator_category
                                                    iterator_category;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(10, value);

    // Test the default constructor.

        f4(X::const_iterator());
        X::const_iterator iter1;

    // Test the copy constructor.

        iter1 = x.begin();
        f5(iter1, X::const_iterator(iter1));
        X::const_iterator iter2(iter1);
        if (iter2 != iter1)
            passed = false;
        X::const_iterator iter3 = iter1;
        if (iter3 != iter1)
            passed = false;

    // Test assignment.

        X::const_iterator iter4;
        iter4 = iter1;
        if (iter4 != iter1)
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(10, value);

        X::const_iterator iter1, iter2, iter3;
        iter1 = x.begin();

    // Test equivalence relations.

        iter2 = iter1;
        if (!(iter1 == iter2))
            passed = false;
        if ((iter1 != iter2) != !(iter1 == iter2))
            passed = false;

    // Invariants

        iter2 = iter1;
        iter3 = iter2;
        X::const_iterator* iter2p = &iter2;
        if ((iter2p == &iter2) && !(*iter2p == iter2))
            passed = false;
        if (iter2 != iter2)
            passed = false;
        if ((iter1 == iter2) && !(iter2 == iter1))
            passed = false;
        if (((iter1 == iter2) && (iter2 == iter3)) && !(iter1 == iter3))
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(10, value);

        X::const_iterator iter1, iter2, iter3;
        iter1 = x.begin();

    // Test ordering relations.

        iter2 = iter1;
        if (iter1 < iter2)
            passed = false;
        if ((iter1 > iter2) != (iter2 < iter1))
            passed = false;
        if ((iter1 <= iter2) != !(iter2 < iter1))
            passed = false;
        if ((iter1 >= iter2) != !(iter1 < iter2))
            passed = false;

        iter3 = iter1;
        ++iter3;
        if (iter3 < iter1)
            passed = false;
        if ((iter3 > iter1) != (iter1 < iter3))
            passed = false;
        if ((iter3 <= iter1) != !(iter1 < iter3))
            passed = false;
        if ((iter3 >= iter1) != !(iter3 < iter1))
            passed = false;

    // Invariants

        if (iter1 < iter1)
            passed = false;
        iter2 = iter1;
        iter2++;
        if ((iter1 < iter2) != !(iter2 < iter1))
            passed = false;
        iter3 = iter2;
        iter3++;
        if (((iter1 < iter2) && (iter2 < iter3)) && !(iter1 < iter3))
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(10, value);

        X::const_iterator iter1, iter2, iter3;
        iter1 = x.begin();
        iter2 = iter1;
        iter3 = iter2;

    // Invariants

        if ((!(iter1 < iter2) && !(iter2 < iter1) &&
             !(iter2 < iter3) && !(iter3 < iter2))
          && !(!(iter1 < iter3) && !(iter3 < iter1)))
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(10, value);

        X::const_iterator iter = x.begin();

    // Test dereferenceability.

        if (*iter != *(x.begin()))
            passed = false;
    }

    {
        DoubleContainer dc(value);
        DoubleContainer dcarray[5] = {dc, dc, dc, dc, dc};

    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const XDC x(dcarray, 5);

        XDC::const_iterator iter = x.begin();

    // Test member access

        if ((*iter).data != iter->data)
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(10, value);

        X::const_iterator iter1 = x.begin();
        X::const_iterator iter2 = x.begin();

    // Invariant

        if ((iter1 == iter2) != (&(*iter1) == &(*iter2)))
            passed = false;
        iter1++;
        if ((iter1 == iter2) != (&(*iter1) == &(*iter2)))
            passed = false;
    }

    {
        typedef iterator_traits<X::const_iterator>::value_type value_type;

        double darray[5] = {0, 1, 2, 3, 4};

    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(darray, 5);

        X::const_iterator iter1 = x.begin();
        X::const_iterator iter2 = x.begin();

    // Test increments

        ++iter1;

        iter1 = iter2;
        iter1++;
        ++iter2;
        if (iter1 != iter2)
            passed = false;

        iter2 = x.begin();
        iter1 = iter2;
        value_type t = *iter2;
        ++iter2;
        if (*iter1++ != t)
            passed = false;
        if (iter1 != iter2)
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(10, value);

        X::const_iterator iter1 = x.begin();
        X::const_iterator iter2 = x.begin();

        if (!(&iter1 == &++iter1))
            passed = false;
        iter1 = iter2;
        if (!(++iter1 == ++iter2))
            passed = false;
    }

    {
        typedef iterator_traits<X::const_iterator>::value_type value_type;

        double darray[5] = {0, 1, 2, 3, 4};

    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(darray, 5);

        X::const_iterator iter1 = x.end();
        X::const_iterator iter2 = x.end();

    // Test decrements

        --iter1;
        if (!(&iter1 == &--iter1))
            passed = false;
        iter1 = iter2;
        if (!(--iter1 == --iter2))
            passed = false;
        iter1 = iter2;
        ++iter1;
        if (!(--iter1 == iter2))
            passed = false;

        iter1 = x.end();
        iter2 = iter1;
        X::const_iterator iter3 = iter2;
        --iter2;
        if (iter1-- != iter3)
            passed = false;
        if (iter1 != iter2)
            passed = false;

    // Invariants

        iter1 = x.begin();
        ++iter1;
        --iter1;
        if (iter1 != x.begin())
            passed = false;

        iter1 = x.end();
        --iter1;
        ++iter1;
        if (iter1 != x.end())
            passed = false;
    }

    {
        typedef iterator_traits<X::const_iterator>::value_type value_type;
        typedef iterator_traits<X::const_iterator>::difference_type
                                                    difference_type;

        double darray[5] = {0, 1, 2, 3, 4};

    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(darray, 5);

        X::const_iterator iter1 = x.begin();
        X::const_iterator iter2 = x.begin();
        X::const_iterator iter3 = x.begin();

    // Iterator addition

        iter1 += 0;
        if (iter1 != iter2)
            passed = false;

        iter1 += 3;
        ++iter2;
        ++iter2;
        ++iter2;
        if (iter1 != iter2)
            passed = false;

        iter1 += -3;
        --iter2;
        --iter2;
        --iter2;
        if (iter1 != iter2)
            passed = false;

        iter1 = x.begin();
        iter2 = x.begin();
        iter3 = iter1 + 3;
        iter2 += 3;
        if (iter3 != iter2)
            passed = false;
        if (iter1 != x.begin())
            passed = false;

        iter1 = x.begin();
        iter2 = x.begin();
        iter3 = 3 + iter1;
        iter2 += 3;
        if (iter3 != iter2)
            passed = false;
        if (iter1 != x.begin())
            passed = false;

    // Iterator subtraction

        iter1 = x.end();
        iter2 = x.end();
        iter1 -= 0;
        if (iter1 != iter2)
            passed = false;

        iter1 -= 3;
        iter2 += -3;
        if (iter1 != iter2)
            passed = false;

        iter1 -= -3;
        iter2 += -(-3);
        if (iter1 != iter2)
            passed = false;

        iter1 = x.end();
        iter2 = x.end();
        iter3 = iter1 - 3;
        iter2 -= 3;
        if (iter3 != iter2)
            passed = false;
        if (iter1 != x.end())
            passed = false;

    // Iterator difference.

        iter1 = x.begin();
        iter2 = x.end();
        difference_type d = iter2 - iter1;
        if (!(iter2 == iter1 + d))
            passed = false;

    // Element access

        iter1 = x.begin();
        if (iter1[2] != *(iter1 + 2))
            passed = false;

    // Invariants

        iter1 = x.begin();
        iter1 += 3;
        iter1 -= 3;
        if (iter1 != x.begin())
            passed = false;
        iter2 = (iter1 + 3) - 3;
        if (iter2 != x.begin())
            passed = false;

        iter1 = x.end();
        iter1 -= 3;
        iter1 += 3;
        if (iter1 != x.end())
            passed = false;
        iter2 = (iter1 - 3) + 3;
        if (iter2 != x.end())
            passed = false;

        iter1 = x.begin();
        iter2 = x.end();
        if (!(iter2 == iter1 + (iter2 - iter1)))
            passed = false;
        if (!(iter2 - iter1 >= 0))
            passed = false;
    }

    cout << "t3: end\n";
}




// Test the X::reverse_iterator functionality

void t4()
{
    cout << "t4: beginning.\n";

    {
        typedef iterator_traits<X::reverse_iterator>::value_type value_type;
        typedef iterator_traits<X::reverse_iterator>::difference_type
                                                      difference_type;
        typedef iterator_traits<X::reverse_iterator>::reference reference;
        typedef iterator_traits<X::reverse_iterator>::pointer pointer;
        typedef iterator_traits<X::reverse_iterator>::iterator_category
                                                      iterator_category;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value);

    // Test the default constructor.

        f6(X::reverse_iterator());
        X::reverse_iterator iter1;

    // Test the copy constructor.

        iter1 = x.rbegin();
        f7(iter1, X::reverse_iterator(iter1));
        X::reverse_iterator iter2(iter1);
        if (iter2 != iter1)
            passed = false;
        X::reverse_iterator iter3 = iter1;
        if (iter3 != iter1)
            passed = false;

    // Test assignment.

        X::reverse_iterator iter4;
        iter4 = iter1;
        if (iter4 != iter1)
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value);

        X::reverse_iterator iter1, iter2, iter3;
        iter1 = x.rbegin();

    // Test equivalence relations.

        iter2 = iter1;
        if (!(iter1 == iter2))
            passed = false;
        if ((iter1 != iter2) != !(iter1 == iter2))
            passed = false;

    // Invariants

        iter2 = iter1;
        iter3 = iter2;
        X::reverse_iterator* iter2p = &iter2;
        if ((iter2p == &iter2) && !(*iter2p == iter2))
            passed = false;
        if (iter2 != iter2)
            passed = false;
        if ((iter1 == iter2) && !(iter2 == iter1))
            passed = false;
        if (((iter1 == iter2) && (iter2 == iter3)) && !(iter1 == iter3))
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value);

        X::reverse_iterator iter1, iter2, iter3;
        iter1 = x.rbegin();

    // Test ordering relations.

        iter2 = iter1;
        if (iter1 < iter2)
            passed = false;
        if ((iter1 > iter2) != (iter2 < iter1))
            passed = false;
        if ((iter1 <= iter2) != !(iter2 < iter1))
            passed = false;
        if ((iter1 >= iter2) != !(iter1 < iter2))
            passed = false;

        iter3 = iter1;
        ++iter3;
        if (iter3 < iter1)
            passed = false;
        if ((iter3 > iter1) != (iter1 < iter3))
            passed = false;
        if ((iter3 <= iter1) != !(iter1 < iter3))
            passed = false;
        if ((iter3 >= iter1) != !(iter3 < iter1))
            passed = false;

    // Invariants

        if (iter1 < iter1)
            passed = false;
        iter2 = iter1;
        iter2++;
        if ((iter1 < iter2) != !((iter2 < iter1)))
            passed = false;
        iter3 = iter2;
        iter3++;
        if (((iter1 < iter2) && (iter2 < iter3)) && !(iter1 < iter3))
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value);

        X::reverse_iterator iter1, iter2, iter3;
        iter1 = x.rbegin();
        iter2 = iter1;
        iter3 = iter2;

    // Invariants

        if ((!(iter1 < iter2) && !((iter2 < iter1)) &&
             !(iter2 < iter3) && !(iter3 < iter2))
          && !(!(iter1 < iter3) && !(iter3 < iter1)))
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value);

        X::reverse_iterator iter = x.rbegin();

    // Test dereferenceability.

        if (*iter != *(x.begin()))
            passed = false;
        *iter = value - 1.;
        if (*iter != value - 1.)
            passed = false;
    }

    {
        DoubleContainer dc(value);
        DoubleContainer dcarray[5] = {dc, dc, dc, dc, dc};

    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        XDC x(dcarray, 5);

        XDC::reverse_iterator iter = x.rbegin();

    // Test member access

        iter->data = value + 1.;
        if ((*iter).data != value + 1.)
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value);

        X::reverse_iterator iter1 = x.rbegin();
        X::reverse_iterator iter2 = x.rbegin();

    // Invariant

        if ((iter1 == iter2) != (&(*iter1) == &(*iter2)))
            passed = false;
        iter1++;
        if ((iter1 == iter2) != (&(*iter1) == &(*iter2)))
            passed = false;
    }

    {
        typedef iterator_traits<X::reverse_iterator>::value_type value_type;

        double darray[5] = {0, 1, 2, 3, 4};

    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(darray, 5);

        X::reverse_iterator iter1 = x.rbegin();
        X::reverse_iterator iter2 = x.rbegin();

    // Test increments

        ++iter1;

        iter1 = iter2;
        iter1++;
        ++iter2;
        if (iter1 != iter2)
            passed = false;

        iter2 = x.rbegin();
        iter1 = iter2;
        value_type t = *iter2;
        ++iter2;
        if (*iter1++ != t)
            passed = false;
        if (iter1 != iter2)
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value);

        X::reverse_iterator iter1 = x.rbegin();
        X::reverse_iterator iter2 = x.rbegin();

        if (!(&iter1 == &++iter1))
            passed = false;
        iter1 = iter2;
        if (!(++iter1 == ++iter2))
            passed = false;
    }

    {
        typedef iterator_traits<X::reverse_iterator>::value_type value_type;

        double darray[5] = {0, 1, 2, 3, 4};

    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(darray, 5);

        X::reverse_iterator iter1 = x.rend();
        X::reverse_iterator iter2 = x.rend();

    // Test decrements

        --iter1;
        if (!(&iter1 == &--iter1))
            passed = false;
        iter1 = iter2;
        if (!(--iter1 == --iter2))
            passed = false;
        iter1 = iter2;
        ++iter1;
        if (!(--iter1 == iter2))
            passed = false;

        iter1 = x.rend();
        iter2 = iter1;
        X::reverse_iterator iter3 = iter2;
        --iter2;
        if (iter1-- != iter3)
            passed = false;
        if (iter1 != iter2)
            passed = false;

    // Invariants

        iter1 = x.rbegin();
        ++iter1;
        --iter1;
        if (iter1 != x.rbegin())
            passed = false;

        iter1 = x.rend();
        --iter1;
        ++iter1;
        if (iter1 != x.rend())
            passed = false;
    }

    {
        typedef iterator_traits<X::reverse_iterator>::value_type value_type;
        typedef iterator_traits<X::reverse_iterator>::difference_type
                                                      difference_type;

        double darray[5] = {0, 1, 2, 3, 4};

    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(darray, 5);

        X::reverse_iterator iter1 = x.rbegin();
        X::reverse_iterator iter2 = x.rbegin();
        X::reverse_iterator iter3 = x.rbegin();

    // Iterator addition

        iter1 += 0;
        if (iter1 != iter2)
            passed = false;

        iter1 += 3;
        ++iter2;
        ++iter2;
        ++iter2;
        if (iter1 != iter2)
            passed = false;

        iter1 += -3;
        --iter2;
        --iter2;
        --iter2;
        if (iter1 != iter2)
            passed = false;

        iter1 = x.rbegin();
        iter2 = x.rbegin();
        iter3 = iter1 + 3;
        iter2 += 3;
        if (iter3 != iter2)
            passed = false;
        if (iter1 != x.rbegin())
            passed = false;

        iter1 = x.rbegin();
        iter2 = x.rbegin();
        iter3 = 3 + iter1;
        iter2 += 3;
        if (iter3 != iter2)
            passed = false;
        if (iter1 != x.rbegin())
            passed = false;

    // Iterator subtraction

        iter1 = x.rend();
        iter2 = x.rend();
        iter1 -= 0;
        if (iter1 != iter2)
            passed = false;

        iter1 -= 3;
        iter2 += -3;
        if (iter1 != iter2)
            passed = false;

        iter1 -= -3;
        iter2 += -(-3);
        if (iter1 != iter2)
            passed = false;

        iter1 = x.rend();
        iter2 = x.rend();
        iter3 = iter1 - 3;
        iter2 -= 3;
        if (iter3 != iter2)
            passed = false;
        if (iter1 != x.rend())
            passed = false;

    // Iterator difference.

        iter1 = x.rbegin();
        iter2 = x.rend();
        difference_type d = iter2 - iter1;
        if (!(iter2 == iter1 + d))
            passed = false;

    // Element access and assignment

        iter1 = x.rbegin();
        if (iter1[2] != *(iter1 + 2))
            passed = false;

        iter1[2] = 12.;
        if (*(iter1 + 2) != 12.)
            passed = false;

    // Invariants

        iter1 = x.rbegin();
        iter1 += 3;
        iter1 -= 3;
        if (iter1 != x.rbegin())
            passed = false;
        iter2 = (iter1 + 3) - 3;
        if (iter2 != x.rbegin())
            passed = false;

        iter1 = x.rend();
        iter1 -= 3;
        iter1 += 3;
        if (iter1 != x.rend())
            passed = false;
        iter2 = (iter1 - 3) + 3;
        if (iter2 != x.rend())
            passed = false;

        iter1 = x.rbegin();
        iter2 = x.rend();
        if (!(iter2 == iter1 + (iter2 - iter1)))
            passed = false;
        if (!(iter2 - iter1 >= 0))
            passed = false;
    }

    cout << "t4: end\n";
}




// Test the X::const_reverse_iterator functionality

void t5()
{
    cout << "t5: beginning.\n";

    {
        typedef iterator_traits<X::const_reverse_iterator>::value_type
                                                            value_type;
        typedef iterator_traits<X::const_reverse_iterator>::difference_type
                                                            difference_type;
        typedef iterator_traits<X::const_reverse_iterator>::reference
                                                            reference;
        typedef iterator_traits<X::const_reverse_iterator>::pointer pointer;
        typedef iterator_traits<X::const_reverse_iterator>::iterator_category
                                                            iterator_category;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(10, value);

    // Test the default constructor.

        f8(X::const_reverse_iterator());
        X::const_reverse_iterator iter1;

    // Test the copy constructor.

        iter1 = x.rbegin();
        f9(iter1, X::const_reverse_iterator(iter1));
        X::const_reverse_iterator iter2(iter1);
        if (iter2 != iter1)
            passed = false;
        X::const_reverse_iterator iter3 = iter1;
        if (iter3 != iter1)
            passed = false;

    // Test assignment.

        X::const_reverse_iterator iter4;
        iter4 = iter1;
        if (iter4 != iter1)
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(10, value);

        X::const_reverse_iterator iter1, iter2, iter3;
        iter1 = x.rbegin();

    // Test equivalence relations.

        iter2 = iter1;
        if (!(iter1 == iter2))
            passed = false;
        if ((iter1 != iter2) != !(iter1 == iter2))
            passed = false;

    // Invariants

        iter2 = iter1;
        iter3 = iter2;
        X::const_reverse_iterator* iter2p = &iter2;
        if ((iter2p == &iter2) && !(*iter2p == iter2))
            passed = false;
        if (iter2 != iter2)
            passed = false;
        if ((iter1 == iter2) && !(iter2 == iter1))
            passed = false;
        if (((iter1 == iter2) && (iter2 == iter3)) && !(iter1 == iter3))
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(10, value);

        X::const_reverse_iterator iter1, iter2, iter3;
        iter1 = x.rbegin();

    // Test ordering relations.

        iter2 = iter1;
        if (iter1 < iter2)
            passed = false;
        if ((iter1 > iter2) != (iter2 < iter1))
            passed = false;
        if ((iter1 <= iter2) != !(iter2 < iter1))
            passed = false;
        if ((iter1 >= iter2) != !(iter1 < iter2))
            passed = false;

        iter3 = iter1;
        ++iter3;
        if (iter3 < iter1)
            passed = false;
        if ((iter3 > iter1) != (iter1 < iter3))
            passed = false;
        if ((iter3 <= iter1) != !(iter1 < iter3))
            passed = false;
        if ((iter3 >= iter1) != !(iter3 < iter1))
            passed = false;

    // Invariants

        if (iter1 < iter1)
            passed = false;
        iter2 = iter1;
        iter2++;
        if ((iter1 < iter2) != !(iter2 < iter1))
            passed = false;
        iter3 = iter2;
        iter3++;
        if (((iter1 < iter2) && (iter2 < iter3)) && !(iter1 < iter3))
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(10, value);

        X::const_reverse_iterator iter1, iter2, iter3;
        iter1 = x.rbegin();
        iter2 = iter1;
        iter3 = iter2;

    // Invariants

        if ((!(iter1 < iter2) && !(iter2 < iter1) &&
             !(iter2 < iter3) && !(iter3 < iter2))
          && !(!(iter1 < iter3) && !(iter3 < iter1)))
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(10, value);

        X::const_reverse_iterator iter = x.rbegin();

    // Test dereferenceability.

        if (*iter != *(x.begin()))
            passed = false;
    }

    {
        DoubleContainer dc(value);
        DoubleContainer dcarray[5] = {dc, dc, dc, dc, dc};

    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const XDC x(dcarray, 5);

        XDC::const_reverse_iterator iter = x.rbegin();

    // Test member access

        if ((*iter).data != iter->data)
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(10, value);

        X::const_reverse_iterator iter1 = x.rbegin();
        X::const_reverse_iterator iter2 = x.rbegin();

    // Invariant

        if ((iter1 == iter2) != (&(*iter1) == &(*iter2)))
            passed = false;
        iter1++;
        if ((iter1 == iter2) != (&(*iter1) == &(*iter2)))
            passed = false;
    }

    {
        typedef iterator_traits<X::const_reverse_iterator>::value_type
                                                            value_type;

        double darray[5] = {0, 1, 2, 3, 4};

    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(darray, 5);

        X::const_reverse_iterator iter1 = x.rbegin();
        X::const_reverse_iterator iter2 = x.rbegin();

    // Test increments

        ++iter1;

        iter1 = iter2;
        iter1++;
        ++iter2;
        if (iter1 != iter2)
            passed = false;

        iter2 = x.rbegin();
        iter1 = iter2;
        value_type t = *iter2;
        ++iter2;
        if (*iter1++ != t)
            passed = false;
        if (iter1 != iter2)
            passed = false;
    }

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(10, value);

        X::const_reverse_iterator iter1 = x.rbegin();
        X::const_reverse_iterator iter2 = x.rbegin();

        if (!(&iter1 == &++iter1))
            passed = false;
        iter1 = iter2;
        if (!(++iter1 == ++iter2))
            passed = false;
    }

    {
        typedef iterator_traits<X::const_reverse_iterator>::value_type
                                                            value_type;

        double darray[5] = {0, 1, 2, 3, 4};

    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(darray, 5);

        X::const_reverse_iterator iter1 = x.rend();
        X::const_reverse_iterator iter2 = x.rend();

    // Test decrements

        --iter1;
        if (!(&iter1 == &--iter1))
            passed = false;
        iter1 = iter2;
        if (!(--iter1 == --iter2))
            passed = false;
        iter1 = iter2;
        ++iter1;
        if (!(--iter1 == iter2))
            passed = false;

        iter1 = x.rend();
        iter2 = iter1;
        X::const_reverse_iterator iter3 = iter2;
        --iter2;
        if (iter1-- != iter3)
            passed = false;
        if (iter1 != iter2)
            passed = false;

    // Invariants

        iter1 = x.rbegin();
        ++iter1;
        --iter1;
        if (iter1 != x.rbegin())
            passed = false;

        iter1 = x.rend();
        --iter1;
        ++iter1;
        if (iter1 != x.rend())
            passed = false;
    }

    {
        typedef iterator_traits<X::const_reverse_iterator>::value_type
                                                            value_type;
        typedef iterator_traits<X::const_reverse_iterator>::difference_type
                                                            difference_type;

        double darray[5] = {0, 1, 2, 3, 4};

    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        const X x(darray, 5);

        X::const_reverse_iterator iter1 = x.rbegin();
        X::const_reverse_iterator iter2 = x.rbegin();
        X::const_reverse_iterator iter3 = x.rbegin();

    // Iterator addition

        iter1 += 0;
        if (iter1 != iter2)
            passed = false;

        iter1 += 3;
        ++iter2;
        ++iter2;
        ++iter2;
        if (iter1 != iter2)
            passed = false;

        iter1 += -3;
        --iter2;
        --iter2;
        --iter2;
        if (iter1 != iter2)
            passed = false;

        iter1 = x.rbegin();
        iter2 = x.rbegin();
        iter3 = iter1 + 3;
        iter2 += 3;
        if (iter3 != iter2)
            passed = false;
        if (iter1 != x.rbegin())
            passed = false;

        iter1 = x.rbegin();
        iter2 = x.rbegin();
        iter3 = 3 + iter1;
        iter2 += 3;
        if (iter3 != iter2)
            passed = false;
        if (iter1 != x.rbegin())
            passed = false;

    // Iterator subtraction

        iter1 = x.rend();
        iter2 = x.rend();
        iter1 -= 0;
        if (iter1 != iter2)
            passed = false;

        iter1 -= 3;
        iter2 += -3;
        if (iter1 != iter2)
            passed = false;

        iter1 -= -3;
        iter2 += -(-3);
        if (iter1 != iter2)
            passed = false;

        iter1 = x.rend();
        iter2 = x.rend();
        iter3 = iter1 - 3;
        iter2 -= 3;
        if (iter3 != iter2)
            passed = false;
        if (iter1 != x.rend())
            passed = false;

    // Iterator difference.

        iter1 = x.rbegin();
        iter2 = x.rend();
        difference_type d = iter2 - iter1;
        if (!(iter2 == iter1 + d))
            passed = false;

    // Element access

        iter1 = x.rbegin();
        if (iter1[2] != *(iter1 + 2))
            passed = false;

    // Invariants

        iter1 = x.rbegin();
        iter1 += 3;
        iter1 -= 3;
        if (iter1 != x.rbegin())
            passed = false;
        iter2 = (iter1 + 3) - 3;
        if (iter2 != x.rbegin())
            passed = false;

        iter1 = x.rend();
        iter1 -= 3;
        iter1 += 3;
        if (iter1 != x.rend())
            passed = false;
        iter2 = (iter1 - 3) + 3;
        if (iter2 != x.rend())
            passed = false;

        iter1 = x.rbegin();
        iter2 = x.rend();
        if (!(iter2 == iter1 + (iter2 - iter1)))
            passed = false;
        if (!(iter2 - iter1 >= 0))
            passed = false;
    }

    cout << "t5: end\n";
}




// Test conversions between mutable and const iterators.

void t6()
{
    cout << "t6: beginning.\n";

    {
    // The following constructor is not required by the Random Access
    // Container concept, but we need to get an object somehow.

        X x(10, value);

        X::iterator iter = x.begin();
        X::reverse_iterator riter = x.rbegin();
        X::const_iterator citer;
        X::const_reverse_iterator criter;

        citer = iter;
        if (citer != x.begin())
            passed = false;

    // The static_cast below is currently required because of a compiler
    // error.

        criter = riter;
        if (criter != static_cast<X::const_reverse_iterator>(x.rbegin()))
            passed = false;
    }

    cout << "t6: end\n";
}

//---------------------------------------------------------------------------//
// Main
//---------------------------------------------------------------------------//
int main( int /*argc*/, char * /*argv*/[] )
{
    cout << "Initiating test of the Mat1 container.\n";

    try {
        t1();
        t2();
        t3();
        t4();
        t5();
        t6();
    }
    catch( rtt_dsxx::assertion& a )
    {
	cout << "Failed assertion: " << a.what() << endl;
    }
    catch( ... )
    {
        cout << "Unknown exception caught." << endl;
    }

    // Print the status of the test.

    cout <<     "\n******************************************";
    if (passed) 
        cout << "\n**** Mat1 Container Self Test: PASSED ****";
    else
        cout << "\n**** Mat1 Container Self Test: FAILED ****";
    cout <<     "\n******************************************\n\n"
         << "Done testing Mat1 container." << endl;

    return passed ? 0 : 1;
}

//---------------------------------------------------------------------------//
//                              end of tstMat1RA.cc
//---------------------------------------------------------------------------//