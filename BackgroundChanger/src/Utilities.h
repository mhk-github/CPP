#pragma once
/**
 * @file Utilities.h
 * @brief Header file for constants and utility functions in this project.
 *
 * @date @showdate "%d %B %Y"
 * @copyright MIT License.
 */


//////////////////////////////////////////////////////////////////////////////
// HEADER FILES
//////////////////////////////////////////////////////////////////////////////

// PRECOMPILED HEADER FILE ///////////////////////////////////////////////////
#include "PCH.h" 

#include "JSONData.h"


//////////////////////////////////////////////////////////////////////////////
// CONSTANTS
//////////////////////////////////////////////////////////////////////////////

/**
 * @addtogroup CONSTANTS
 *
 * @{
 */

/// Registry key for the desktop background.
inline const auto * REGISTRY_KEY{L"Control Panel\\Desktop"};

/// Key for whether the desktop background wallpaper is tiled.
inline const auto * TILE_WALLPAPER_KEY{L"TileWallpaper"};

/// Key for the desktop background wallpaper style.
inline const auto * WALLPAPER_STYLE_KEY{L"WallpaperStyle"};

/// Value to tile the desktop background wallpaper.
inline const auto * TILE_ON{L"1"};

/// Value to turn off tiling of the desktop background wallpaper.
inline const auto * TILE_OFF{L"0"};

/// Sets the wallpaper style to fill.
inline const auto * WALLPAPER_FILL{L"10"};

/// Sets the wallpaper style to fit.
inline const auto * WALLPAPER_FIT{L"6"};

/// Sets the wallpaper style to stretch.
inline const auto * WALLPAPER_STRETCH{L"2"};

/// Sets the wallpaper style to centre.
inline const auto * WALLPAPER_CENTRE{L"0"};

/**
 * @}
 */


//////////////////////////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * @addtogroup UTILITIES
 *
 * @{
 */

/**
 * @brief Processes a given JSON manifest file.
 *
 * @param[in] json_file Canonical path to the JSON manifest file.
 * @param[in] directory Canonical path to a directory.
 *
 * @returns Items found in the manifest file associated with the directory.
 *
 * @internal @warning Do not change this to std::wstring !
 */
std::vector<JSONData> ProcessJSONFile (
  const std::string &json_file,
  const std::string &directory
);

/**
 * @brief Sets the desktop background to the given image.
 *
 * @param[in] image_file Canonical path to an image file.
 *
 * @internal @warning Do not change this to std::string !
 */
void ChangeDesktopBackground (const std::wstring &image_file);

/**
 * @brief Updates registry entries for the desktop background.
 *
 * @param[in] position Value for the wallpaper position.
 * @param[in] tileOn Set \c true if the background is to be tiled.
 *
 * @internal @warning Leave this as wchar_t !
 */
void UpdateRegistry (LPCWSTR position, bool tileOn);

/**
 * @}
 */


//////////////////////////////////////////////////////////////////////////////
// END
//////////////////////////////////////////////////////////////////////////////
