#pragma once
///////////////////////////////////////////////////////////////////////////////
// FILE     : Log.h
// SYNOPSIS : Code for logging functionality
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

//! A namespace for logging functionality.
namespace Log
{
  /////////////////////////////////////////////////////////////////////////////
  // CONSTANTS
  /////////////////////////////////////////////////////////////////////////////

  /**
   * @defgroup LOGGING_LEVELS Logging Levels
   *
   * @brief The set of logging levels catered for in this application.
   *
   * @{ 
   */
 
  //! A value for when \c trace level logging is wanted.
  const std::string LOG_LEVEL_TRACE{ "trace" };
  
  //! A value for when \c debug level logging is wanted.
  const std::string LOG_LEVEL_DEBUG{ "debug" };

  //! A value for when \c info level logging is wanted.
  const std::string LOG_LEVEL_INFO{ "info" };

  //! A value for when \c warning level logging is wanted.
  const std::string LOG_LEVEL_WARNING{ "warning" };

  //! A value for when \c error level logging is wanted.
  const std::string LOG_LEVEL_ERROR{ "error" };

  //! A value for when \c fatal level logging is wanted.
  const std::string LOG_LEVEL_FATAL{ "fatal" };

  /**
   * @}
   */

  //! The default logging level.
  const auto DEFAULT_LOG_LEVEL{ LOG_LEVEL_INFO };


  /////////////////////////////////////////////////////////////////////////////
  // TYPES
  /////////////////////////////////////////////////////////////////////////////

  //! The type of logging object required.
  typedef boost::log::sources::severity_logger<
      boost::log::trivial::severity_level
  > Logger;


  /////////////////////////////////////////////////////////////////////////////
  // FUNCTIONS
  /////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Initializes the logging system.
   *
   * @param[in] log_level The logging level required.
   */
  void init_logging ( const std::string& log_level ) noexcept
  {
    boost::log::add_common_attributes ();
    boost::log::add_console_log (
        std::cout,
        boost::log::keywords::format =
            "[%TimeStamp%] %Severity%: %Message%",
        boost::log::keywords::auto_flush = true
    );
    
    if ( boost::iequals ( log_level, LOG_LEVEL_TRACE ) )
    {
      boost::log::core::get ()->set_filter ( 
          boost::log::trivial::severity >= boost::log::trivial::trace 
      );
    }
    else if ( boost::iequals ( log_level, LOG_LEVEL_DEBUG ) )
    {
      boost::log::core::get ()->set_filter ( 
          boost::log::trivial::severity >= boost::log::trivial::debug 
      );
    }
    else if ( boost::iequals ( log_level, LOG_LEVEL_INFO ) )
    {
      boost::log::core::get ()->set_filter ( 
          boost::log::trivial::severity >= boost::log::trivial::info 
      );
    }
    else if ( boost::iequals ( log_level, LOG_LEVEL_WARNING ) )
    {
      boost::log::core::get ()->set_filter ( 
          boost::log::trivial::severity >= boost::log::trivial::warning 
      );
    }
    else if ( boost::iequals ( log_level, LOG_LEVEL_ERROR ) )
    {
      boost::log::core::get ()->set_filter ( 
          boost::log::trivial::severity >= boost::log::trivial::error 
      );
    }
    else if ( boost::iequals ( log_level, LOG_LEVEL_FATAL ) )
    {
      boost::log::core::get ()->set_filter ( 
          boost::log::trivial::severity >= boost::log::trivial::fatal 
      );
    }

  }

}


///////////////////////////////////////////////////////////////////////////////
// END
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @brief Header file for creating a precompiled header.
 *
 * @author Mohammad Haroon Khaliq
 * @date @showdate "%d %B %Y"
 * @copyright MIT License.
 */
 // Local variables:
 // mode: c++
 // End:
