#pragma once
/**
 * @file Configuration.h
 * @brief Header file for the configuration module of this project.
 *
 * @date @showdate "%d %B %Y"
 * @copyright MIT License.
 */


//////////////////////////////////////////////////////////////////////////////
// HEADER FILES
//////////////////////////////////////////////////////////////////////////////

// PRECOMPILED HEADER FILE ///////////////////////////////////////////////////
#include "PCH.h"


//////////////////////////////////////////////////////////////////////////////
// CONSTANTS
//////////////////////////////////////////////////////////////////////////////

/**
 * @addtogroup CONSTANTS
 *
 * @{
 */

/**
 * @brief This project's Windows INI configuration file name.
 */
inline const auto * INI_FILE_NAME{L"BackgroundChanger.x86.ini"};

const wxString INI_FILE_ERROR{
  L"Failed to process the configuration INI file !"
};

const wxString WIDTH_KEY{L"WINDOW/Width"};
const auto DEFAULT_WIDTH{715};

const wxString HEIGHT_KEY{L"WINDOW/Height"};
const auto DEFAULT_HEIGHT{650};

const wxString TOP_LEFT_X_KEY{L"WINDOW/TopLeftX"};
const auto DEFAULT_TOP_LEFT_X{1'205};

const wxString TOP_LEFT_Y_KEY{L"WINDOW/TopLeftY"};
const auto DEFAULT_TOP_LEFT_Y{390};

const wxString LOG_DIRECTORY_KEY{L"LOGGING/LogDirectory"};
const wxString DEFAULT_LOG_DIRECTORY{L""};

const wxString MANIFEST_KEY{L"MANIFEST/ManifestFile"};
const wxString DEFAULT_MANIFEST{L""};

const wxString BATCH_SIZE_KEY{L"WORKER/BatchSize"};
const auto DEFAULT_BATCH_SIZE{25};

const wxString SLEEP_TIME_KEY{L"WORKER/SleepMilliseconds"};
const auto DEFAULT_SLEEP_TIME{50};

/**
 * @}
 */


//////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS
//////////////////////////////////////////////////////////////////////////////

std::unique_ptr<wxFileConfig> GetConfiguration ();


//////////////////////////////////////////////////////////////////////////////
// END
//////////////////////////////////////////////////////////////////////////////
