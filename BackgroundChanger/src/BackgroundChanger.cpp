/**
 * @file BackgroundChanger.cpp
 * @brief Implementation file for this application.
 *
 * @date @showdate "%d %B %Y"
 * @copyright MIT License.
 */


//////////////////////////////////////////////////////////////////////////////
// HEADER FILES
//////////////////////////////////////////////////////////////////////////////

// PRECOMPILED HEADER FILE ///////////////////////////////////////////////////
#include "PCH.h" 

#include "Configuration.h"
#include "Globals.h"
#include "MainFrame.h"


///////////////////////////////////////////////////////////////////////////////
// USING
///////////////////////////////////////////////////////////////////////////////

// BOOST
using namespace boost::log::trivial;


///////////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////////

/**
 * @addtogroup CONSTANTS
 *
 * @{
 */

const auto * LOG_FILE{L"\\BackgroundChanger_x86_%Y%m%d_%H%M%S.log"};

/**
 * @}
 */


///////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS
///////////////////////////////////////////////////////////////////////////////

void init_logging (const std::wstring&);
LONG WINAPI UnhandledExceptionHandler (EXCEPTION_POINTERS*);


///////////////////////////////////////////////////////////////////////////////
// FUNCTION IMPLEMENTATIONS
///////////////////////////////////////////////////////////////////////////////

void init_logging (const std::wstring &logDirectory)
{
  boost::log::add_common_attributes ();
  boost::log::add_file_log (
    boost::log::keywords::file_name = logDirectory + LOG_FILE,
    boost::log::keywords::format =
    "[%TimeStamp%] (%ProcessID%<%ThreadID%>) %Severity%: %Message%",
    boost::log::keywords::auto_flush = true
  );
  boost::log::core::get ()->set_filter (severity >= debug);
}

LONG WINAPI UnhandledExceptionHandler (EXCEPTION_POINTERS*)
{
  BOOST_LOG_SEV(g_Log,fatal) << "Unhandled exception in Windows !";
  boost::log::core::get ()->flush ();
  return EXCEPTION_EXECUTE_HANDLER;
}


//////////////////////////////////////////////////////////////////////////////
// CLASS DECLARATIONS
//////////////////////////////////////////////////////////////////////////////

class BackgroundChangerApp final : public wxApp
{
public:
  bool OnInit () override;
  int OnExit () override;
};


//////////////////////////////////////////////////////////////////////////////
// CLASS IMPLEMENTATIONS
//////////////////////////////////////////////////////////////////////////////

bool BackgroundChangerApp::OnInit ()
{
  std::set_terminate ([]()
    {
      BOOST_LOG_SEV(g_Log,fatal) << "std::terminate called !";
      boost::log::core::get ()->flush ();
      std::abort ();
    }
  );
  SetUnhandledExceptionFilter (UnhandledExceptionHandler);

  SetPriorityClass (GetCurrentProcess (), IDLE_PRIORITY_CLASS);

  const auto directory{wxTheApp->argv[1]};
  if (!wxDirExists (directory))
  {
    wxLogError ("Invalid images directory '%s' !", directory.c_str ());
    return false;
  }

  const auto config{GetConfiguration ()};
  if (config == nullptr)
  {
    wxLogError (INI_FILE_ERROR);
    return false;
  }

  wxString logDir{};
  config->Read (LOG_DIRECTORY_KEY, &logDir, DEFAULT_LOG_DIRECTORY);
  if (!wxDirExists (logDir))
  {
    wxLogError ("Invalid log files directory '%s' !", logDir.c_str ());
    return false;
  }

  wxString manifest{};
  config->Read (MANIFEST_KEY, &manifest, DEFAULT_MANIFEST);
  if (!wxFileName::FileExists (manifest))
  {
    wxLogError ("Cannot find manifest JSON file '%s' !", manifest.c_str ());
    return false;
  }

  int width{};
  config->Read (WIDTH_KEY, &width, DEFAULT_WIDTH);
  int height{};
  config->Read (HEIGHT_KEY, &height, DEFAULT_HEIGHT);
  int topLeftX{};
  config->Read (TOP_LEFT_X_KEY, &topLeftX, DEFAULT_TOP_LEFT_X);
  int topLeftY{};
  config->Read (TOP_LEFT_Y_KEY, &topLeftY, DEFAULT_TOP_LEFT_Y);

  init_logging (logDir.ToStdWstring ());
  BOOST_LOG_SEV(g_Log,info) << "BackgroundChanger - Start";

  auto * frame{new MainFrame{topLeftX, topLeftY, width, height}};
  frame->SetIcon (wxIcon{L"IDI_ICON1"});
  frame->Show (true);

  return true;
}

int BackgroundChangerApp::OnExit ()
{
  BOOST_LOG_SEV(g_Log,info) << "BackgroundChanger - End [OnExit]";
  boost::log::core::get ()->flush ();
  return wxApp::OnExit ();
}


///////////////////////////////////////////////////////////////////////////////
// MAIN
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief This C++ desktop application's program entry point.
 *
 * @ingroup DRIVER
 */
wxIMPLEMENT_APP (BackgroundChangerApp);


///////////////////////////////////////////////////////////////////////////////
// END
///////////////////////////////////////////////////////////////////////////////
/**
 * @defgroup CONSTANTS Constants
 *
 * @brief Constant values used in this project.
 */
/**
 * @defgroup GLOBALS Global Variables
 *
 * @brief Global variables in this project.
 */
/**
 * @defgroup POD Plain Old Data
 *
 * @brief POD in this project.
 */
/**
 * @defgroup EVENTS Events
 *
 * @brief Events that are specific to this application.
 */
/**
 * @defgroup UTILITIES Utility Functions
 *
 * @brief Functions useful across this application.
 */
/**
 * @defgroup DRIVER Driver
 *
 * @brief The application executable's entry point.
 */
/**
 * @defgroup THREADS Threads
 *
 * @brief Threads used in this application.
 */
