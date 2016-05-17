//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   parser/test/parser_test.hh
 * \author Kent G. Budge
 * \date   Feb 18 2003
 * \brief  parser testing utilities.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef __parser_test_hh__
#define __parser_test_hh__

#include <iostream>
#include <string>

namespace rtt_parser_test
{

//===========================================================================//
// PASS/FAILURE LIMIT
//===========================================================================//

// Returns true for pass
// Returns false for fail
// Failure functions also set rtt_parser_test::passed to false

// These can be used in any combination in a test to print output messages  
// if no fail functions are called then the test will pass
// (rtt_parser_test::passed will have its default value of true)

// Needless to say, these can be used in many different combinations or
// ways.  We do not constrain draco tests except that the output must be of
// the form "Test: pass/fail"

bool fail(int line);

bool fail(int line, char *file);

bool pass_msg(const std::string &);

bool fail_msg(const std::string &);

//---------------------------------------------------------------------------//
// PASSING CONDITIONALS
//---------------------------------------------------------------------------//

extern bool passed;

} // end namespace rtt_parser_test

//===========================================================================//
// TEST MACROS
//
// USAGE:
// if (!condition) ITFAILS;
//
// These are a convenience only
//===========================================================================//

#define ITFAILS    rtt_parser_test::fail(__LINE__);
#define FAILURE    rtt_parser_test::fail(__LINE__, __FILE__);
#define PASSMSG(a) rtt_parser_test::pass_msg(a);
#define FAILMSG(a) rtt_parser_test::fail_msg(a);

#endif                          // __parser_test_hh__

//---------------------------------------------------------------------------//
//                              end of parser/test/parser_test.hh
//---------------------------------------------------------------------------//