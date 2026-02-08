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

inline const auto * REGISTRY_KEY{L"Control Panel\\Desktop"};
inline const auto * TILE_WALLPAPER_KEY{L"TileWallpaper"};
inline const auto * WALLPAPER_STYLE_KEY{L"WallpaperStyle"};
inline const auto * TILE_ON{L"1"};
inline const auto * TILE_OFF{L"0"};
inline const auto * WALLPAPER_FILL{L"10"};
inline const auto * WALLPAPER_FIT{L"6"};
inline const auto * WALLPAPER_STRETCH{L"2"};
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
std::vector<JSONData> ProcessJSONFile (
  const std::string&,
  const std::string&
);
void ChangeDesktopBackground (const std::wstring&);
void UpdateRegistry (LPCWSTR, bool);
/**
 * @}
 */


//////////////////////////////////////////////////////////////////////////////
// END
//////////////////////////////////////////////////////////////////////////////
