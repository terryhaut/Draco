//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   c4/test/tstReduction.cc
 * \author Thomas M. Evans
 * \date   Mon Mar 25 15:41:00 2002
 * \brief  C4 Reduction test.
 * \note   Copyright (C) 2016 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "c4/ParallelUnitTest.hh"
#include "ds++/Release.hh"
#include "ds++/Soft_Equivalence.hh"

using namespace std;

using rtt_c4::global_sum;
using rtt_c4::global_prod;
using rtt_c4::global_min;
using rtt_c4::global_max;
using rtt_dsxx::soft_equiv;

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

void elemental_reduction(rtt_dsxx::UnitTest &ut) {
  // test ints
  int xint = rtt_c4::node() + 1;
  global_sum(xint);

  int int_answer = 0;
  for (int i = 0; i < rtt_c4::nodes(); i++)
    int_answer += i + 1;

  if (xint != int_answer)
    ITFAILS;

  // Test with deprecated form of global_sum
  xint = rtt_c4::node() + 1;
  global_sum(xint);
  if (xint != int_answer)
    ITFAILS;

  // test longs
  long xlong = rtt_c4::node() + 1000;
  global_sum(xlong);

  long long_answer = 0;
  for (int i = 0; i < rtt_c4::nodes(); i++)
    long_answer += i + 1000;

  if (xlong != long_answer)
    ITFAILS;

  // test doubles
  double xdbl = static_cast<double>(rtt_c4::node()) + 0.1;
  global_sum(xdbl);

  double dbl_answer = 0.0;
  for (int i = 0; i < rtt_c4::nodes(); i++)
    dbl_answer += static_cast<double>(i) + 0.1;

  if (!soft_equiv(xdbl, dbl_answer))
    ITFAILS;

  // test product
  xlong = rtt_c4::node() + 1;
  global_prod(xlong);

  long_answer = 1;
  for (int i = 0; i < rtt_c4::nodes(); i++)
    long_answer *= (i + 1);

  if (xlong != long_answer)
    ITFAILS;

  // Test with deprecated form of global_prod
  xlong = rtt_c4::node() + 1;
  global_prod(xlong);
  if (xlong != long_answer)
    ITFAILS;

  // test min
  xdbl = 0.5 + rtt_c4::node();
  global_min(xdbl);

  if (!soft_equiv(xdbl, 0.5))
    ITFAILS;

  // Test with deprecated form of global_min
  xdbl = rtt_c4::node() + 0.5;
  global_min(xdbl);
  if (!soft_equiv(xdbl, 0.5))
    ITFAILS;

  // test max
  xdbl = 0.7 + rtt_c4::node();
  global_max(xdbl);

  if (!soft_equiv(xdbl, rtt_c4::nodes() - 0.3))
    ITFAILS;

  // Test with deprecated form of global_max
  xdbl = 0.7 + rtt_c4::node();
  global_max(xdbl);
  if (!soft_equiv(xdbl, rtt_c4::nodes() - 0.3))
    ITFAILS;

  if (ut.numFails == 0)
    PASSMSG("Elemental reductions ok.");
  return;
}

//---------------------------------------------------------------------------//
void array_reduction(rtt_dsxx::UnitTest &ut) {
  // make a vector of doubles
  vector<double> x(100);
  vector<double> prod(100, 1.0);
  vector<double> sum(100, 0.0);
  vector<double> lmin(100, 0.0);
  vector<double> lmax(100, 0.0);

  // fill it
  for (int i = 0; i < 100; i++) {
    x[i] = rtt_c4::node() + 0.11;
    for (int j = 0; j < rtt_c4::nodes(); j++) {
      sum[i] += (j + 0.11);
      prod[i] *= (j + 0.11);
    }
    lmin[i] = 0.11;
    lmax[i] = rtt_c4::nodes() + 0.11 - 1.0;
  }

  vector<double> c;

  {
    c = x;
    global_sum(&c[0], 100);
    if (!soft_equiv(c.begin(), c.end(), sum.begin(), sum.end()))
      ITFAILS;

    c = x;
    global_prod(&c[0], 100);
    if (!soft_equiv(c.begin(), c.end(), prod.begin(), prod.end()))
      ITFAILS;

    c = x;
    global_min(&c[0], 100);
    if (!soft_equiv(c.begin(), c.end(), lmin.begin(), lmin.end()))
      ITFAILS;

    c = x;
    global_max(&c[0], 100);
    if (!soft_equiv(c.begin(), c.end(), lmax.begin(), lmax.end()))
      ITFAILS;
  }

  // Test using deprecated forms of global_sum, global_min, global_max and
  // global_prod.

  {
    c = x;
    global_sum(&c[0], 100);
    if (!soft_equiv(c.begin(), c.end(), sum.begin(), sum.end()))
      ITFAILS;

    c = x;
    global_prod(&c[0], 100);
    if (!soft_equiv(c.begin(), c.end(), prod.begin(), prod.end()))
      ITFAILS;

    c = x;
    global_min(&c[0], 100);
    if (!soft_equiv(c.begin(), c.end(), lmin.begin(), lmin.end()))
      ITFAILS;

    c = x;
    global_max(&c[0], 100);
    if (!soft_equiv(c.begin(), c.end(), lmax.begin(), lmax.end()))
      ITFAILS;
  }

  if (ut.numFails == 0)
    PASSMSG("Array reductions ok.");
  return;
}

//---------------------------------------------------------------------------//
int main(int argc, char *argv[]) {
  rtt_c4::ParallelUnitTest ut(argc, argv, rtt_dsxx::release);
  try {
    elemental_reduction(ut);
    array_reduction(ut);
  }
  UT_EPILOG(ut);
}

//---------------------------------------------------------------------------//
// end of tstReduction.cc
//---------------------------------------------------------------------------//
