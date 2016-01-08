#-----------------------------*-cmake-*----------------------------------------#
# file   config/compilerEnv.cmake
# brief  Default CMake build parameters
# note   Copyright (C) 2010-2015 Los Alamos National Security, LLC.
#        All rights reserved.
#------------------------------------------------------------------------------#
# $Id$
#------------------------------------------------------------------------------#

include( FeatureSummary )

# ----------------------------------------
# PAPI
# ----------------------------------------
if( EXISTS $ENV{PAPI_HOME} )

  set( HAVE_PAPI 1 CACHE BOOL "Is PAPI available on this machine?" )
  set( PAPI_INCLUDE $ENV{PAPI_INCLUDE} CACHE PATH "PAPI headers at this location" )
  set( PAPI_LIBRARY $ENV{PAPI_LIBDIR}/libpapi.so CACHE FILEPATH "PAPI library." )
endif()

# PAPI 4.2 on CT uses a different setup.
if( $ENV{PAPI_VERSION} MATCHES "[45].[0-9].[0-9]")
  set( HAVE_PAPI 1 CACHE BOOL "Is PAPI available on this machine?" )
  string( REGEX REPLACE ".*[ ][-]I(.*)$" "\\1" PAPI_INCLUDE $ENV{PAPI_INCLUDE_OPTS} )
  string( REGEX REPLACE ".*[ ][-]L(.*)[ ].*" "\\1" PAPI_LIBDIR $ENV{PAPI_POST_LINK_OPTS} )
endif()

if( HAVE_PAPI )
  set( PAPI_INCLUDE ${PAPI_INCLUDE} CACHE PATH "PAPI headers at this location" )
  set( PAPI_LIBRARY ${PAPI_LIBDIR}/libpapi.so CACHE FILEPATH "PAPI library." )
  if( NOT EXISTS ${PAPI_LIBRARY} )
    message( FATAL_ERROR "PAPI requested, but library not found.  Set PAPI_LIBDIR to correct path." )
  endif()
  mark_as_advanced( PAPI_INCLUDE PAPI_LIBRARY )
  add_feature_info( HAVE_PAPI HAVE_PAPI "Provide PAPI hardware counters if available." )
endif()

# ----------------------------------------
# MIC processors
# ----------------------------------------
if( "${HAVE_MIC}x" STREQUAL "x" )

  # default to OFF
  set( HAVE_MIC OFF)

  # This was the old mechanism.  It fails to work because we might be
  # targeting a haswell node that also has MIC processors. Still, we
  # might want to use this in the future to determine if MICs are on
  # the local node.
  message( STATUS "Looking for availability of MIC hardware")
  set( mic_found FALSE )
  if( EXISTS /usr/sbin/micctrl )
    exec_program(
      /usr/sbin/micctrl
      ARGS -s
      OUTPUT_VARIABLE mic_status
      OUTPUT_STRIP_TRAILING_WHITESPACE
      OUTPUT_QUIET
      )
    if( mic_status MATCHES online )
      # set( HAVE_MIC ON CACHE BOOL "Does the local machine have MIC chips?" )
      # If we areusing MIC, then disable CUDA.
      # set( USE_CUDA OFF CACHE BOOL "Compile against Cuda libraries?")
      set( mic_found TRUE )
    endif()
  endif()
  if( mic_found )
    message( STATUS "Looking for availability of MIC hardware - found")
  else()
    message( STATUS "Looking for availability of MIC hardware - not found")
  endif()

  if( ${mic_found} )
    # Should we cross compile for the MIC?
    # Look at the environment variable SLURM_JOB_PARTITION to determine
    # if we should cross compile for the MIC processor.
    message(STATUS "Enable cross compiling for MIC (HAVE_MIC) ...")
    # See https://darwin.lanl.gov/darwin_hw/report.html for a list
    # of string designators used as partition names:
    if( NOT "$ENV{SLURM_JOB_PARTITION}x" STREQUAL "x" AND "$ENV{SLURM_JOB_PARTITION}" STREQUAL "knc-mic")
      set( HAVE_MIC ON )
      set( USE_CUDA OFF CACHE BOOL "Compile against Cuda libraries?")
    endif()

    # Store the result in the cache.
    set( HAVE_MIC ${HAVE_MIC} CACHE BOOL "Should we cross compile for the MIC processor?" )
    message(STATUS "Enable cross compiling for MIC (HAVE_MIC) ... ${HAVE_MIC}")
    unset( mic_found)
  endif()

endif()

# ------------------------------------------------------------------------------
# Identify machine and save name in ds++/config.h
# ------------------------------------------------------------------------------
site_name( SITENAME )
string( REGEX REPLACE "([A-z0-9]+).*" "\\1" SITENAME ${SITENAME} )
if( ${SITENAME} MATCHES "c[it]" )
  set( SITENAME "Cielito" )
elseif( ${SITENAME} MATCHES "ml[0-9]+" OR ${SITENAME} MATCHES "ml-fey" OR
    ${SITENAME} MATCHES "lu[0-9]+" OR ${SITENAME} MATCHES "lu-fey" )
  set( SITENAME "Moonlight" )
elseif( ${SITENAME} MATCHES "tt") #" -login[0-9]+" OR ${SITENAME} MATCHES "tt-fey[0-9]+" )
  set( SITENAME "Trinitite" )
elseif( ${SITENAME} MATCHES "tr") #" -login[0-9]+" OR ${SITENAME} MATCHES "tr-fe[0-9]+" )
  set( SITENAME "Trinity" )
elseif( ${SITENAME} MATCHES "ccscs[0-9]+" )
  # do nothing (keep the fullname)
endif()
set( SITENAME ${SITENAME} CACHE "STRING" "Name of the current machine" FORCE)

#------------------------------------------------------------------------------#
# Setup compilers
#------------------------------------------------------------------------------#
macro(dbsSetupCompilers)

  # Bad platform
  if( NOT WIN32 AND NOT UNIX)
    message( FATAL_ERROR "Unsupported platform (not WIN32 and not UNIX )." )
  endif()

  # Defaults for 1st pass:

  # shared or static libararies?
  if( ${DRACO_LIBRARY_TYPE} MATCHES "STATIC" )
    # message(STATUS "Building static libraries.")
    set( MD_or_MT "MD" )
    set( DRACO_SHARED_LIBS 0 )
    set( DRACO_LIBEXT ".a" )
  elseif( ${DRACO_LIBRARY_TYPE} MATCHES "SHARED" )
    # message(STATUS "Building shared libraries.")
    set( MD_or_MT "MD" )
    # This CPP symbol is used by config.h to signal if we are need to add
    # declspec(dllimport) or declspec(dllexport) for MSVC.
    set( DRACO_SHARED_LIBS 1 )
    mark_as_advanced(DRACO_SHARED_LIBS)
    set( DRACO_LIBEXT ".so" )
  else()
    message( FATAL_ERROR "DRACO_LIBRARY_TYPE must be set to either STATIC or SHARED.")
  endif()
  set( DRACO_SHARED_LIBS ${DRACO_SHARED_LIBS} CACHE STRING
    "This CPP symbol is used by config.h to signal if we are need to add declspec(dllimport) or declspec(dllexport) for MSVC." )

endmacro()

#------------------------------------------------------------------------------#
# Setup C++ Compiler
#------------------------------------------------------------------------------#
macro(dbsSetupCxx)

  dbsSetupCompilers()

  # Deal with compiler wrappers
  if( ${CMAKE_CXX_COMPILER} MATCHES "tau_cxx.sh" )
    # When using the TAU profiling tool, the actual compiler vendor
    # is hidden under the tau_cxx.sh script.  Use the following
    # command to determine the actual compiler flavor before setting
    # compiler flags (end of this macro).
    execute_process(
      COMMAND ${CMAKE_CXX_COMPILER} -tau:showcompiler
      OUTPUT_VARIABLE my_cxx_compiler )
  else()
    set( my_cxx_compiler ${CMAKE_CXX_COMPILER} )
  endif()

  string( REGEX REPLACE "[^0-9]*([0-9]+).([0-9]+).([0-9]+).*" "\\1"
    DBS_CXX_COMPILER_VER_MAJOR "${CMAKE_CXX_COMPILER_VERSION}" )
  string( REGEX REPLACE "[^0-9]*([0-9]+).([0-9]+).([0-9]+).*" "\\2"
    DBS_CXX_COMPILER_VER_MINOR "${CMAKE_CXX_COMPILER_VERSION}" )

  # C99 support:
  set( CMAKE_C_STANDARD 99 )

  # C++11 support:
  set( CMAKE_CXX_STANDARD 11 )

  # Do not enable extensions (e.g.: --std=gnu++11)
  set( CMAKE_CXX_EXTENSIONS OFF )
  set( CMAKE_C_EXTENSIONS   OFF )

  get_filename_component( my_cxx_compiler ${my_cxx_compiler} NAME )
  if( ${my_cxx_compiler} MATCHES "mpicxx" )
    # MPI wrapper
    execute_process( COMMAND ${my_cxx_compiler} --version
      OUTPUT_VARIABLE mpicxx_version_output
      OUTPUT_STRIP_TRAILING_WHITESPACE )
    # make output safe for regex matching
    string( REPLACE "+" "x" mpicxx_version_output ${mpicxx_version_output} )
    if( ${mpicxx_version_output} MATCHES icpc )
      set( my_cxx_compiler icpc )
    endif()
  endif()
  if( ${my_cxx_compiler} MATCHES "clang" OR
      ${my_cxx_compiler} MATCHES "llvm")
    if( APPLE )
      include( apple-clang )
    else()
      include( unix-clang )
    endif()
  elseif( ${my_cxx_compiler} STREQUAL "pgCC" )
    include( unix-pgi )
  elseif( ${my_cxx_compiler} MATCHES "CC" )
    if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" )
      include( unix-intel )
    elseif( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Cray" )
      include( unix-crayCC )
    else()
      message( FATAL_ERROR "I think the C++ comiler is a Cray compiler wrapper, but I don't know what compiler is wrapped." )
    endif()
  elseif( ${my_cxx_compiler} MATCHES "cl" )
    include( windows-cl )
  elseif( ${my_cxx_compiler} MATCHES "icpc" )
    include( unix-intel )
  elseif( ${my_cxx_compiler} MATCHES "xl" )
    include( unix-xl )
  elseif( ${my_cxx_compiler} MATCHES "[cg][+x]+" )
    include( unix-g++ )
  else()
    message( FATAL_ERROR "Build system does not support CXX=${my_cxx_compiler}" )
  endif()

  # To the greatest extent possible, installed versions of packages
  # should record the configuration options that were used when they
  # were built.  For preprocessor macros, this is usually
  # accomplished via #define directives in config.h files.  A
  # package's installed config.h file serves as both a record of
  # configuration options and a central location for macro
  # definitions that control features in the package.  Defining
  # macros via the -D command-line option to the preprocessor leaves
  # no record of configuration choices (except in a build log, which
  # may not be preserved with the installation).
  #
  # Unfortunately, there are cases where a particular macro must be
  # defined before some particular system header file is included, or
  # before any system header files are included.  In these
  # situations, using the config.h mechanism introduces sensitivity
  # to the order of header files, which can lead to brittleness;
  # defining project-wide language- or system-feature macros via -D,
  # using CMake's add_definitions command, is an acceptable
  # alternative.  Such definitions appear below.

  # Enable the definition of UINT64_C in stdint.h (required by
  # Random123).
  add_definitions(-D__STDC_CONSTANT_MACROS)
  set( CMAKE_REQUIRED_DEFINITIONS
    "${CMAKE_REQUIRED_DEFINITIONS} -D__STDC_CONSTANT_MACROS" )

  # Define _POSIX_C_SOURCE=200112 and _XOPEN_SOURCE=600, to enable
  # definitions conforming to POSIX.1-2001, POSIX.2, XPG4, SUSv2,
  # SUSv3, and C99.  See the feature_test_macros(7) man page for more
  # information.
  add_definitions(-D_POSIX_C_SOURCE=200112 -D_XOPEN_SOURCE=600)
  set( CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS} -D_POSIX_C_SOURCE=200112" )
  set( CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS} -D_XOPEN_SOURCE=600")
  if ( APPLE )
    # Defining the above requires adding POSIX extensions,
    # otherwise, include ordering still goes wrong on Darwin,
    # (i.e., putting fstream before iostream causes problems)
    # see https://code.google.com/p/wmii/issues/detail?id=89
    add_definitions(-D_DARWIN_C_SOURCE)
    set( CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS} -D_DARWIN_C_SOURCE ")
  endif()

endmacro()

#------------------------------------------------------------------------------#
# Setup Fortran Compiler
#
# Use:
#    include( compilerEnv )
#    dbsSetupFortran( [QUIET] )
#
# Returns:
#    BUILD_SHARED_LIBS - bool
#    CMAKE_Fortran_COMPILER - fullpath
#    CMAKE_Fortran_FLAGS
#    CMAKE_Fortran_FLAGS_DEBUG
#    CMAKE_Fortran_FLAGS_RELEASE
#    CMAKE_Fortran_FLAGS_RELWITHDEBINFO
#    CMAKE_Fortran_FLAGS_MINSIZEREL
#    ENABLE_SINGLE_PRECISION - bool
#    DBS_FLOAT_PRECISION     - string (config.h)
#    PRECISION_DOUBLE | PRECISION_SINGLE - bool
#
#------------------------------------------------------------------------------#
macro(dbsSetupFortran)

  dbsSetupCompilers()

  # Toggle if we should try to build Fortran parts of the project.
  # This will be set to true if $ENV{FC} points to a working compiler
  # (e.g.: GNU or Intel compilers with Unix Makefiles) or if the
  # current project doesn't support Fortran but
  # CMakeAddFortranSubdirectory can be used.
  option( HAVE_Fortran "Should we build Fortran parts of the project?" OFF )

  # Is Fortran enabled (it is considered 'optional' for draco)?
  get_property(_LANGUAGES_ GLOBAL PROPERTY ENABLED_LANGUAGES)
  if( _LANGUAGES_ MATCHES Fortran )

    set( HAVE_Fortran ON )
    set( my_fc_compiler ${CMAKE_Fortran_COMPILER} )

    # MPI wrapper
    if( ${my_fc_compiler} MATCHES "mpif90" )
      execute_process( COMMAND ${my_fc_compiler} --version
        OUTPUT_VARIABLE mpifc_version_output
        OUTPUT_STRIP_TRAILING_WHITESPACE )
      if( ${mpifc_version_output} MATCHES ifort )
        set( my_fc_compiler ifort )
      elseif( ${mpifc_version_output} MATCHES GNU )
        set( my_fc_compiler gfortran )
      endif()
    endif()

    if( ${my_fc_compiler} MATCHES "pgf9[05]" OR
        ${my_fc_compiler} MATCHES "pgfortran" )
      include( unix-pgf90 )
    elseif( ${my_fc_compiler} MATCHES "ftn" )
    if( "${CMAKE_Fortran_COMPILER_ID}" STREQUAL "Intel" )
      include( unix-ifort )
    elseif( "${CMAKE_Fortran_COMPILER_ID}" STREQUAL "Cray" )
      include( unix-crayftn )
    else()
      message( FATAL_ERROR "I think the C++ comiler is a Cray compiler wrapper, but I don't know what compiler is wrapped." )
    endif()
    elseif( ${my_fc_compiler} MATCHES "ifort" )
      include( unix-ifort )
    elseif( ${my_fc_compiler} MATCHES "xl" )
      include( unix-xlf )
    elseif( ${my_fc_compiler} MATCHES "gfortran" )
      include( unix-gfortran )
    else()
      message( FATAL_ERROR "Build system does not support FC=${my_fc_compiler}" )
    endif()

    if( _LANGUAGES_ MATCHES "^C$" OR _LANGUAGES_ MATCHES CXX )
      include(FortranCInterface)
    endif()

  else()
    # If CMake doesn't know about a Fortran compiler, $ENV{FC}, then
    # also look for a compiler to use with
    # CMakeAddFortranSubdirectory.
    message( STATUS "Looking for CMakeAddFortranSubdirectory Fortran compiler...")

    # Try to find a Fortran compiler (use MinGW gfortran for MSVC).
    find_program( CAFS_Fortran_COMPILER
      NAMES ${CAFS_Fortran_COMPILER} $ENV{CAFS_Fortran_COMPILER} gfortran
      PATHS
      c:/MinGW/bin
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\MinGW;InstallLocation]/bin" )

    if( EXISTS ${CAFS_Fortran_COMPILER} )
      set( HAVE_Fortran ON )
      message( STATUS "Looking for CMakeAddFortranSubdirectory Fortran compiler... found ${CAFS_Fortran_COMPILER}")
    else()
      message( STATUS "Looking for CMakeAddFortranSubdirectory Fortran compiler... not found")
    endif()

  endif()

  set( HAVE_Fortran ${HAVE_Fortran} CACHE BOOL
    "Should we build Fortran portions of this project?" FORCE )

endmacro()

##---------------------------------------------------------------------------------------##
## Setup profile tools: MAP, PAPI, HPCToolkit, TAU, etc.
##---------------------------------------------------------------------------------------##
macro( dbsSetupProfilerTools )

  # ------------------------------------------------------------
  # Allinea MAP
  # ------------------------------------------------------------
  # Note 1: Allinea MAP should work on regular Linux without this setup.
  # Note 2: I have demonstrated that MAP works under Cray environments only when
  #    (a) compiling with the compiler option '-dynamic',
  #    (b) the Allinea sampler libraries are generated on the same filesystem as
  #        the build, and
  #    (c) These libraries are linked when generated executables.
  # Note 3: Linking the allinea sampler libraries into generated executables
  #        shows up in component_macros near 'add_executable' commands via the
  #        target_link_libraries command.

  option( USE_ALLINEA_MAP
    "If Allinea MAP is available, should we link against those libraries?" OFF )

  if( USE_ALLINEA_MAP )
    # Ref: www.nersc.gov/users/software/performance-and-debugging-tools/MAP
    if( "${SITENAME}" STREQUAL "Trinitite" OR
        "${SITENAME}" STREQUAL "Cielito"   OR
        "${SITENAME}" STREQUAL "Cielo" )
      set( platform_cray "--platform=cray")
    endif()
    if( NOT DEFINED ENV{ALLINEA_LICENSE_DIR} AND NOT DEFINED ENV{DDT_LICENSE_FILE} )
      message( FATAL_ERROR "You must load the Allinea module first!")
    endif()
    if( "${DRACO_LIBRARY_TYPE}" STREQUAL "STATIC")
      if( NOT EXISTS ${PROJECT_BINARY_DIR}/allinea-profiler.ld )
        message( STATUS "Generating allinea-profiler.ld...")
        # message( "make-profiler-libraries ${platform_cray} --lib-type=static")
        execute_process(
          COMMAND make-profiler-libraries ${platform_cray} --lib-type=static
          WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
          OUTPUT_QUIET
          )
        message( STATUS "Generating allinea-profiler.ld...done")
      endif()
      set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,@${PROJECT_BINARY_DIR}/allinea-profiler.ld")

    elseif( USE_ALLINEA_MAP AND "${DRACO_LIBRARY_TYPE}" STREQUAL "SHARED")

      if( NOT EXISTS ${PROJECT_BINARY_DIR}/libmap-sampler.so )
        message( STATUS "Generating allinea-sampler.so...")
        # message( "make-profiler-libraries ${platform_cray}")
        execute_process(
          COMMAND make-profiler-libraries ${platform_cray}
          WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
          OUTPUT_QUIET
          )
        message( STATUS "Generating allinea-sampler.so...done")
      endif()
      find_library( map-sampler-pmpi
        NAMES map-sampler-pmpi
        PATHS ${PROJECT_BINARY_DIR}
        NO_DEFAULT_PATH
        )
      find_library( map-sampler
        NAMES map-sampler
        PATHS ${PROJECT_BINARY_DIR}
        NO_DEFAULT_PATH
        )
      set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--eh-frame-hdr")
      # set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${PROJECT_BINARY_DIR} -lmap-sampler-pmpi -lmap-sampler -Wl,--eh-frame-hdr -Wl,-rpath=${PROJECT_BINARY_DIR}")
    endif()
  endif()

  # ------------------------------------------------------------
  # DMALLOC with Allinea DDT (Memory debugging)
  # ------------------------------------------------------------
  option( USE_ALLINEA_DMALLOC
    "If Allinea DDT is available, should we link against their dmalloc libraries?" OFF )

  if( USE_ALLINEA_DMALLOC )
    if( NOT EXISTS $ENV{DDTROOT} )
      message( FATAL_ERROR "You must load the Allinea module first!")
    endif()
    if( "${SITENAME}" STREQUAL "Trinitite" OR "${SITENAME}" STREQUAL "Cielito" )
      #set( OLD_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES} )
      #set( CMAKE_FIND_LIBRARY_SUFFIXES .a )
      find_library( ddt-dmalloc
        NAMES dmallocthcxx
        PATHS $ENV{DDTROOT}/lib/64
        NO_DEFAULT_PATH
        )
      #set( CMAKE_FIND_LIBRARY_SUFFIXES ${OLD_CMAKE_FIND_LIBRARY_SUFFIXES} )
      #message("ddt-malloc = ${ddt-malloc}")
      set( CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -Wl,--undefined=malloc" )
    endif()
  endif()

endmacro()

##---------------------------------------------------------------------------##
## Toggle a compiler flag based on a bool
##
## Examples:
##   toggle_compiler_flag( GCC_ENABLE_ALL_WARNINGS "-Weffc++" "CXX" "DEBUG" )
##---------------------------------------------------------------------------##
macro( toggle_compiler_flag switch compiler_flag
    compiler_flag_var_names build_modes )

  # generate names that are safe for CMake RegEx MATCHES commands
  string(REPLACE "+" "x" safe_compiler_flag ${compiler_flag})

  # Loop over types of variables to check: CMAKE_C_FLAGS,
  # CMAKE_CXX_FLAGS, etc.
  foreach( comp ${compiler_flag_var_names} )

    # sanity check
    if( NOT ${comp} STREQUAL "C" AND
        NOT ${comp} STREQUAL "CXX" AND
        NOT ${comp} STREQUAL "Fortran" AND
        NOT ${comp} STREQUAL "EXE_LINKER")
      message(FATAL_ERROR "When calling
toggle_compiler_flag(switch, compiler_flag, compiler_flag_var_names),
compiler_flag_var_names must be set to one or more of these valid
names: C;CXX;EXE_LINKER.")
    endif()

    string( REPLACE "+" "x" safe_CMAKE_${comp}_FLAGS
      "${CMAKE_${comp}_FLAGS}" )

    if( "${build_modes}x" STREQUAL "x" ) # set flags for all build modes

      if( ${switch} )
        if( NOT "${safe_CMAKE_${comp}_FLAGS}" MATCHES "${safe_compiler_flag}" )
          set( CMAKE_${comp}_FLAGS "${CMAKE_${comp}_FLAGS} ${compiler_flag} "
            CACHE STRING "compiler flags" FORCE )
        endif()
      else()
        if( "${safe_CMAKE_${comp}_FLAGS}" MATCHES "${safe_compiler_flag}" )
          string( REPLACE "${compiler_flag}" ""
            CMAKE_${comp}_FLAGS ${CMAKE_${comp}_FLAGS} )
          set( CMAKE_${comp}_FLAGS "${CMAKE_${comp}_FLAGS}"
            CACHE STRING "compiler flags" FORCE )
        endif()
      endif()

    else() # build_modes listed

      foreach( bm ${build_modes} )

        string( REPLACE "+" "x" safe_CMAKE_${comp}_FLAGS_${bm}
          ${CMAKE_${comp}_FLAGS_${bm}} )

        if( ${switch} )
          if( NOT "${safe_CMAKE_${comp}_FLAGS_${bm}}" MATCHES
              "${safe_compiler_flag}" )
            set( CMAKE_${comp}_FLAGS_${bm}
              "${CMAKE_${comp}_FLAGS_${bm}} ${compiler_flag} "
              CACHE STRING "compiler flags" FORCE )
          endif()
        else()
          if( "${safe_CMAKE_${comp}_FLAGS_${bm}}" MATCHES
              "${safe_compiler_flag}" )
            string( REPLACE "${compiler_flag}" ""
              CMAKE_${comp}_FLAGS_${bm} ${CMAKE_${comp}_FLAGS_${bm}} )
            set( CMAKE_${comp}_FLAGS_${bm}
              "${CMAKE_${comp}_FLAGS_${bm}}"
              CACHE STRING "compiler flags" FORCE )
          endif()
        endif()

      endforeach()

    endif()

  endforeach()
endmacro()

#------------------------------------------------------------------------------#
# End config/compiler_env.cmake
#------------------------------------------------------------------------------#
