#pragma once
///////////////////////////////////////////////////////////////////////////////
// FILE     : Log.h
// SYNOPSIS : Code for logging functionality.
// LICENSE  : MIT
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// HEADER FILES
///////////////////////////////////////////////////////////////////////////////

// PRECOMPILED HEADER FILE ////////////////////////////////////////////////////

#include "PCH.h"



///////////////////////////////////////////////////////////////////////////////
// NAMESPACE
///////////////////////////////////////////////////////////////////////////////

namespace Log
{
  /////////////////////////////////////////////////////////////////////////////
  // CONSTANTS
  /////////////////////////////////////////////////////////////////////////////

  const std::wstring LOG_LEVEL_TRACE{ L"trace" };
  const std::wstring LOG_LEVEL_DEBUG{ L"debug" };
  const std::wstring LOG_LEVEL_INFO{ L"info" };
  const std::wstring LOG_LEVEL_WARNING{ L"warning" };
  const std::wstring LOG_LEVEL_ERROR{ L"error" };
  const std::wstring LOG_LEVEL_FATAL{ L"fatal" };
  const auto LOG_DEFAULT_LEVEL{ LOG_LEVEL_WARNING };

//  const auto* LOG_FILE_SPECIFICATION_NAME{ L"3D_%Y%m%d_%H%M%S_%N.log" };
  const auto* LOG_FILE_NAME_TAIL{ L"_%Y%m%d_%H%M%S_%N.log" };
  const auto* LOG_FORMAT{ "[%TimeStamp%] (%ProcessID%<%ThreadID%>) %Severity%: %Message%" };
  static_assert(
      std::is_same<decltype( LOG_FORMAT ), const char*>::value,
      L"boost::log::keywords::format only works with type char !"
  );
  const std::wstring LOG_DEFAULT_DIRECTORY{ L"" };
  const std::wstring LOG_DIRECTORY_SEPARATOR{ L"\\" };



  /////////////////////////////////////////////////////////////////////////////
  // TYPES
  /////////////////////////////////////////////////////////////////////////////

  typedef boost::log::sources::severity_logger<boost::log::trivial::severity_level> Logger;



  /////////////////////////////////////////////////////////////////////////////
  //STRUCTURES
  /////////////////////////////////////////////////////////////////////////////

  struct LogSetup
  {
      const std::wstring& application_name;
      const std::wstring& log_directory;
      const std::wstring& log_level;
  };

  /////////////////////////////////////////////////////////////////////////////
  // FUNCTIONS
  /////////////////////////////////////////////////////////////////////////////

  inline void init_logging ( const LogSetup& setup ) noexcept
  {
    boost::log::add_common_attributes ();

    const auto& log_dir{ setup.log_directory };
    const auto fully_specified_file_name{
        ( ( boost::equals ( log_dir, LOG_DEFAULT_DIRECTORY ) ||
        boost::ends_with ( log_dir, LOG_DIRECTORY_SEPARATOR ) ) ?
        log_dir : ( log_dir + LOG_DIRECTORY_SEPARATOR ) ) + 
        setup.application_name + LOG_FILE_NAME_TAIL
    };
    boost::log::add_file_log (
      boost::log::keywords::file_name = fully_specified_file_name,
      boost::log::keywords::format = LOG_FORMAT,
      boost::log::keywords::auto_flush = true
    );

    const auto& level{ setup.log_level };
    if ( boost::iequals ( level, LOG_LEVEL_TRACE ) )
    {
      boost::log::core::get ()->set_filter ( boost::log::trivial::severity >= boost::log::trivial::trace );
    }
    else if ( boost::iequals ( level, LOG_LEVEL_DEBUG ) )
    {
      boost::log::core::get ()->set_filter ( boost::log::trivial::severity >= boost::log::trivial::debug );
    }
    else if ( boost::iequals ( level, LOG_LEVEL_INFO ) )
    {
      boost::log::core::get ()->set_filter ( boost::log::trivial::severity >= boost::log::trivial::info );
    }
    else if ( boost::iequals ( level, LOG_LEVEL_WARNING ) )
    {
      boost::log::core::get ()->set_filter ( boost::log::trivial::severity >= boost::log::trivial::warning );
    }
    else if ( boost::iequals ( level, LOG_LEVEL_ERROR ) )
    {
      boost::log::core::get ()->set_filter ( boost::log::trivial::severity >= boost::log::trivial::error );
    }
    else if ( boost::iequals ( level, LOG_LEVEL_FATAL ) )
    {
      boost::log::core::get ()->set_filter ( boost::log::trivial::severity >= boost::log::trivial::fatal );
    }
  }

}



///////////////////////////////////////////////////////////////////////////////
// END
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @brief Code for logging functionality.
 */
// Local variables:
// mode: c++
// End:
