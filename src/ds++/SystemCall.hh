//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   ds++/SystemCall.hh
 * \brief  Wrapper for system calls. Hide differences between Unix/Windows 
 *         system calls.
 * \note   Copyright (C) 2012 Los Alamos National Security, LLC.
 *         All rights reserved.
 * \version $Id$
 */
//---------------------------------------------------------------------------//

#ifndef rtt_dsxx_SystemCall_hh
#define rtt_dsxx_SystemCall_hh

#include "ds++/config.h"
#include <string>

namespace rtt_dsxx
{

//===========================================================================//
// General discussion.  See .cc file for detailed implementation discussion
// (mostly Linux vs. Windows issues).
//===========================================================================//
/*! \section HOST_NAME_MAX
 *
 * The selection of a value for HOST_NAME_MAX is completed by 
   ds++/CMakeLists.txt and ds++/config.h.in.
 *
 * - For most Linux platforms, \c HOST_NAME_MAX is defined in \c <limits.h>.  
 *   However, according to the POSIX standard, \c HOST_NAME_MAX is a 
 *   \em possibly \em indeterminate definition meaning that it
 *
 * \note ...shall be omitted from \c <limits.h> on specific implementations 
 *       where the corresponding value is equal to or greater than the stated
 *       minimum, but is unspecified. 
 * 
 * - The minumum POSIX guarantee is \c HOST_NAME_MAX = \c 256.
 * - An alternate value used by some Unix systems is \c MAXHOSTNAMELEN as 
 *   defined in \c <sys/param.h>
 * - On Windows, the variable \c MAX_COMPUTERNAME_LENGTH from \c <windows.h>
 *   can be used. See http://msdn.microsoft.com/en-us/library/windows/desktop/ms738527%28v=vs.85%29.aspx
 *  - On Mac OSX, we use \c _POSIX_HOST_NAME_MAX.
 */

//===========================================================================//
// FREE FUNCTIONS
//===========================================================================//

//! Return the local hostname
DLL_PUBLIC std::string draco_gethostname( void );

//! Return the local process id
DLL_PUBLIC int draco_getpid( void );

//! Return the current working directory
DLL_PUBLIC std::string draco_getcwd( void );

//! Return the stat value for a file
DLL_PUBLIC int draco_getstat( std::string const &  fqName );

} // end of rtt_dsxx

#endif // rtt_dsxx_SystemCall_hh

//---------------------------------------------------------------------------//
// end of SystemCall.hh
//---------------------------------------------------------------------------//