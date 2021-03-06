#-----------------------------*-cmake-*----------------------------------------#
# file   config/CMakeLists.txt
# author Kelly Thompson <kgt@lanl.gov>
# date   2012 Aug 1
# brief  Generate build project files for c4/test.
# note   Copyright (C) 2016, Los Alamos National Security, LLC.
#        All rights reserved.
#------------------------------------------------------------------------------#
# $Id$
#------------------------------------------------------------------------------#
project( c4_test CXX )

# ---------------------------------------------------------------------------- #
# Source files
# ---------------------------------------------------------------------------- #

file( GLOB test_sources *.cc )

# ---------------------------------------------------------------------------- #
# Directories to search for include directives
# ---------------------------------------------------------------------------- #

include_directories(
   ${PROJECT_SOURCE_DIR}      # headers for tests
   ${PROJECT_SOURCE_DIR}/..   # headers for package
   ${PROJECT_BINARY_DIR}/..   # config.h
   )

# ---------------------------------------------------------------------------- #
# Build Unit tests
# ---------------------------------------------------------------------------- #

# tstAbort doesn't really work as a unit test because it always calls MPI_abort.
# This causes an uncatchable error condition.  I suppose we could wrap the test
# in a python or cmake script - maybe we can do this sometime in the future.
list( REMOVE_ITEM test_sources ${PROJECT_SOURCE_DIR}/tstAbort.cc )

# This test has special arguments and special pass/fail criteria.
set( special_test ${PROJECT_SOURCE_DIR}/phw.cc )
add_parallel_tests(
   SOURCES   "${special_test}"
   DEPS      Lib_c4
   PE_LIST   "1;2;4"
   TEST_ARGS "hello"
   PASS_REGEX "Hello"
   FAIL_REGEX "cruel"
   )
list( REMOVE_ITEM test_sources ${special_test} )

# This tests runs with extra arguments
set( special_test ${PROJECT_SOURCE_DIR}/tstParallelUnitTest.cc )
add_parallel_tests(
   SOURCES   "${special_test}"
   DEPS      Lib_c4
   PE_LIST   "1;2;4"
   TEST_ARGS "--version;--dummy; "
   PASS_REGEX "tstParallelUnitTest[.ex]*: version"
   )
list( REMOVE_ITEM test_sources ${special_test} )

# This test uses MPI+OMP:
# - Bullseye doesn't like some OMP directives.
# - Timing tests are broken if the test has to be pushed to a virtual
#   machine (aprun).
set( special_test ${PROJECT_SOURCE_DIR}/tstOMP.cc )
list( REMOVE_ITEM test_sources ${special_test} )
if( NOT OPENMP_FOUND )
   # no-op
elseif( DBS_CXX_IS_BULLSEYE )
   # no-op
elseif( "${MPIEXEC}" MATCHES "aprun" )
   if( NOT "$ENV{OMP_NUM_THREADS}x" STREQUAL "x" )
      add_parallel_tests(
         SOURCES   "${special_test}"
         DEPS      Lib_c4
         PE_LIST   "1;2"
         MPI_PLUS_OMP
         )
   endif()
else()
   add_parallel_tests(
      SOURCES   "${special_test}"
      DEPS      Lib_c4
      PE_LIST   "1;2"
      MPI_PLUS_OMP
      )
endif()

# Tests that expect exactly 2 PE (omit for DRACO_C4=SCALAR builds):
set( two_pe_tests ${PROJECT_SOURCE_DIR}/tstPingPong.cc )
list( REMOVE_ITEM test_sources ${two_pe_tests} )
if( NOT ${DRACO_C4} STREQUAL "SCALAR" )
  add_parallel_tests(
    SOURCES   "${two_pe_tests}"
    DEPS      Lib_c4
    PE_LIST   "2"
    )
endif()

# Exclude tstTime from valgrind checks.
set( nomemcheck_tests ${PROJECT_SOURCE_DIR}/tstTime.cc )
list( REMOVE_ITEM test_sources ${nomemcheck_tests} )
add_parallel_tests(
  SOURCES   "${nomemcheck_tests}"
  DEPS      Lib_c4
  PE_LIST   "1;2;4"
  LABEL     "nomemcheck"
  )


# The rest of the tests

add_parallel_tests(
   SOURCES   "${test_sources}"
   DEPS      Lib_c4
   PE_LIST   "1;2;4"
   )
