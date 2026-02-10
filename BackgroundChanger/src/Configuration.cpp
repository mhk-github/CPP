/**
 * @file Configuration.cpp
 * @brief Implementation file for the configuration module of this project.
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


//////////////////////////////////////////////////////////////////////////////
// USING
//////////////////////////////////////////////////////////////////////////////

// BOOST
using namespace boost::log::trivial;


//////////////////////////////////////////////////////////////////////////////
// FUNCTION IMPLEMENTATIONS
//////////////////////////////////////////////////////////////////////////////

std::unique_ptr<wxFileConfig> GetConfiguration ()
{
  const wxFileName exePath{wxStandardPaths::Get ().GetExecutablePath ()};
  const wxString iniFile{exePath.GetPathWithSep () + INI_FILE_NAME};
  if (!wxFileName::FileExists (iniFile))
  {
    const auto error{
      wxString::Format (
        "Configuration file '%s' does not exist !",
        iniFile.c_str ()
      )
    };
    wxLogError (error);
    BOOST_LOG_SEV(g_Log,fatal) << "    " << error;

    return nullptr;
  }

  return std::make_unique<wxFileConfig> (
    wxEmptyString,
    wxEmptyString,
    iniFile,
    wxEmptyString,
    wxCONFIG_USE_LOCAL_FILE
    );
}


//////////////////////////////////////////////////////////////////////////////
// END
//////////////////////////////////////////////////////////////////////////////
