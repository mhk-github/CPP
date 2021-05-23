///////////////////////////////////////////////////////////////////////////////
// FILE     : WallpaperChanger.cpp
// SYNOPSIS : Implementation file for console WIN32 application to change 
//            the Windows desktop background to one randomly selected image 
//            file each time an interval has elapsed.
// LICENSE  : MIT
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// HEADER FILES
///////////////////////////////////////////////////////////////////////////////

// PRECOMPILED HEADER FILE ////////////////////////////////////////////////////

#include "PCH.h"

// LINKER DIRECTIVES //////////////////////////////////////////////////////////

#pragma comment(lib, "boost_log_setup-vc142-mt-x32-1_76.lib")
#pragma comment(lib, "boost_log-vc142-mt-x32-1_76.lib")



///////////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////////

const auto ARG_COUNT{ 2 };

const auto *INI_APP_NAME{ L"WALLPAPERCHANGER" };
const auto *INI_SEARCH_KEY{ L"SEARCH_TERM" };
const auto *INI_EXTENSION_KEY{ L"FILE_EXTENSION" };
const auto *INI_SLEEP_KEY{ L"SLEEP_TIME" };
const auto *INI_DEFAULT_EXTENSION{ L".jpg" };
const auto INI_DEFAULT_SLEEP_TIME_MS{ 120'000 };
const auto INI_MAX_STRING_LENGTH{ 1'024 };



///////////////////////////////////////////////////////////////////////////////
// LOGGING SYSTEM
///////////////////////////////////////////////////////////////////////////////

using namespace boost::log::trivial;
boost::log::sources::severity_logger< severity_level > logwc;



///////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
///////////////////////////////////////////////////////////////////////////////

inline void init_logging ()
{
  boost::log::add_common_attributes ();
  boost::log::add_console_log (
      std::cout,
      boost::log::keywords::format = "%Message%",
      boost::log::keywords::auto_flush = true
  );
#ifdef _DEBUG
  boost::log::core::get ()->set_filter ( severity >= trace );
#else
  boost::log::core::get ()->set_filter ( severity >= debug );
#endif /* _DEBUG */
}



///////////////////////////////////////////////////////////////////////////////
// DRIVER
///////////////////////////////////////////////////////////////////////////////

int wmain ( int argc, wchar_t *argv[], wchar_t *envp[] ) 
{
  init_logging ();
  BOOST_LOG_SEV(logwc,info) << "WallpaperChanger - Start";
  BOOST_LOG_SEV(logwc,debug) << "  Process ID = " << GetCurrentProcessId ();
  const auto process{ GetCurrentProcess () };
  BOOST_LOG_SEV(logwc,debug) << "    Process priority = "
      << GetPriorityClass ( process );

  if ( argc != ARG_COUNT )
  {
    BOOST_LOG_SEV(logwc,fatal)
        << "  A valid INI configuration file must be given !";
    return ( 1 );
  }
  const auto *ini_file{ argv[1] };
  BOOST_LOG_SEV(logwc,debug) << "  INI configuration file = " << ini_file;

  wchar_t ini_string_buffer[INI_MAX_STRING_LENGTH]{};
  if ( GetPrivateProfileStringW ( 
           INI_APP_NAME, 
           INI_SEARCH_KEY, 
           nullptr, 
           ini_string_buffer, 
           INI_MAX_STRING_LENGTH, 
           ini_file 
       ) == 0 )
  {
    BOOST_LOG_SEV(logwc,error) << "    Failed to find value for '" 
        << INI_SEARCH_KEY << "' in INI file '" << ini_file << "' !";
    return ( 2 );
  }
  const std::wstring directory_search_term{ ini_string_buffer };
  BOOST_LOG_SEV(logwc,debug) << "    Directory search term = " 
      << directory_search_term;

  if ( GetPrivateProfileStringW (
         INI_APP_NAME, 
         INI_EXTENSION_KEY, 
         INI_DEFAULT_EXTENSION, 
         ini_string_buffer, 
         INI_MAX_STRING_LENGTH, 
         ini_file 
     ) == 0 )
  {
    BOOST_LOG_SEV(logwc,error) << "    Failed to find value for '" 
        << INI_EXTENSION_KEY << "' in INI file '" << ini_file << "' !";
    return ( 3 );
  }
  const std::wstring file_extension{ ini_string_buffer };
  BOOST_LOG_SEV(logwc,debug) << "    File extension = " << file_extension;

  const auto sleep_time{ 
      GetPrivateProfileIntW ( 
          INI_APP_NAME, 
          INI_SLEEP_KEY, 
          INI_DEFAULT_SLEEP_TIME_MS, 
          ini_file
      ) 
  };
  BOOST_LOG_SEV(logwc,debug) << "    Sleep time (ms) = " << sleep_time;

  // String substring constructor uses source string, start point and length
  const std::wstring image_directory{ 
      directory_search_term, 
      0, 
      directory_search_term.rfind ( L"\\" ) + 1 
  };

  WIN32_FIND_DATAW data{};
  HANDLE finder{ FindFirstFileW ( directory_search_term.c_str (), &data ) };
  if ( finder != INVALID_HANDLE_VALUE )
  {
    std::vector<std::wstring> file_list{};
    do
    {
      const std::wstring file_name{ data.cFileName };
      if ( file_name.rfind ( file_extension ) != std::wstring::npos )
      {
        file_list.emplace_back ( image_directory + data.cFileName );
      }
      } while ( FindNextFileW ( finder, &data ) );
      FindClose ( finder );

      BOOST_LOG_SEV(logwc,debug) << "  Files:";
      for ( const auto &x : file_list )
      {
        BOOST_LOG_SEV(logwc,debug) << "    " << x;
      }

      const auto num_files{ file_list.size() };
      if ( num_files == 0 )
      {
        BOOST_LOG_SEV(logwc,error) << "    No files with extension '" 
            << file_extension << "' found in directory '" << image_directory 
            << "' !";
        return ( 4 );
      }
      BOOST_LOG_SEV(logwc,debug) << "  Total files found = " << num_files;

      std::mt19937_64 mt_engine{ static_cast<uint64_t>(
          std::chrono::system_clock::now ().time_since_epoch ().count () ) 
      };
      BOOST_LOG_SEV(logwc,debug) << "  Set up random number generator";

      SetPriorityClass ( process, IDLE_PRIORITY_CLASS );
      BOOST_LOG_SEV(logwc,debug) << "  Set process priority to " 
          << IDLE_PRIORITY_CLASS;

      BOOST_LOG_SEV(logwc,debug) << "  Set background:";
      for ( ;; )
      {
        const auto *file_chosen{ 
            file_list[mt_engine () % num_files].c_str ()
        };
        SystemParametersInfoW ( 
            SPI_SETDESKWALLPAPER, 
            0, 
            static_cast<PVOID>( const_cast<wchar_t *>( file_chosen ) ), 
            0 
        );
        BOOST_LOG_SEV(logwc,debug) << "    " << file_chosen;
        Sleep ( sleep_time );
      }
    }

    return ( 0 );
}



///////////////////////////////////////////////////////////////////////////////
// END
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @brief Implementation file for this application.
 */
 // Local variables:
 // mode: c++
 // End:
