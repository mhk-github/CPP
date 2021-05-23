#pragma once
#pragma message("... producing a precompiled header file")
///////////////////////////////////////////////////////////////////////////////
// FILE     : PCH.h
// SYNOPSIS : Project-wide precompiled header file
// LICENSE  : MIT
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// WINDOWS-SPECIFIC
///////////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN
#include <windows.h>



///////////////////////////////////////////////////////////////////////////////
// HEADER FILES
///////////////////////////////////////////////////////////////////////////////

// SYSTEM HEADER FILES ////////////////////////////////////////////////////////

#include <random>

// BOOST //////////////////////////////////////////////////////////////////////

#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>



///////////////////////////////////////////////////////////////////////////////
// END
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @brief Header file for creating a precompiled header.
 */
 // Local variables:
 // mode: c++
 // End:
