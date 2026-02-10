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

/// This project's Windows INI configuration file name.
inline const auto * INI_FILE_NAME{L"BackgroundChanger.x86.ini"};

/// Error message for when the INI configuration file cannot be processed.
const wxString INI_FILE_ERROR{
  L"Failed to process the configuration INI file !"
};

/// Key to find the main window width.
const wxString WIDTH_KEY{L"WINDOW/Width"};

/// The default main window width.
const auto DEFAULT_WIDTH{715};

/// Key to find the main window height.
const wxString HEIGHT_KEY{L"WINDOW/Height"};

/// The default main window height.
const auto DEFAULT_HEIGHT{650};

/// Key to find the main window top left x coordinate.
const wxString TOP_LEFT_X_KEY{L"WINDOW/TopLeftX"};

/// The default main window top left x coordinate.
const auto DEFAULT_TOP_LEFT_X{1'205};

/// Key to find the main window top left y coordinate.
const wxString TOP_LEFT_Y_KEY{L"WINDOW/TopLeftY"};

/// The default main window top left y coordinate.
const auto DEFAULT_TOP_LEFT_Y{390};

/// Key for the canonical path to the log files directory.
const wxString LOG_DIRECTORY_KEY{L"LOGGING/LogDirectory"};

/// The default log files directory path.
const wxString DEFAULT_LOG_DIRECTORY{L""};

/// Key for the canonical path to the JSON manifest file for image data.
const wxString MANIFEST_KEY{L"MANIFEST/ManifestFile"};

/// The default path to the JSON manifest file for image data.
const wxString DEFAULT_MANIFEST{L""};

/// Key for the size of data batches the background worker thread returns.
const wxString BATCH_SIZE_KEY{L"WORKER/BatchSize"};

/// Default data batch size for the background worker thread.
const auto DEFAULT_BATCH_SIZE{25};

/// Key for the milliseconds the background worker thread sleeps.
const wxString SLEEP_TIME_KEY{L"WORKER/SleepMilliseconds"};

/// Default sleep time in milliseconds for the background worker thread.
const auto DEFAULT_SLEEP_TIME{50};

/**
 * @}
 */


//////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS
//////////////////////////////////////////////////////////////////////////////

/**
 * @brief Gets the project configuration data.
 *
 * @returns A unique pointer to the project configuration data or \c nullptr.
 */
std::unique_ptr<wxFileConfig> GetConfiguration ();


//////////////////////////////////////////////////////////////////////////////
// END
//////////////////////////////////////////////////////////////////////////////
