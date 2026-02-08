/**
 * @file Utilities.cpp
 * @brief Implementation file for utility functions in this project.
 *
 * @date @showdate "%d %B %Y"
 * @copyright MIT License.
 */


//////////////////////////////////////////////////////////////////////////////
// HEADER FILES
//////////////////////////////////////////////////////////////////////////////

// PRECOMPILED HEADER FILE ///////////////////////////////////////////////////
#include "PCH.h"

#include "Globals.h"
#include "Utilities.h"


///////////////////////////////////////////////////////////////////////////////
// USING
///////////////////////////////////////////////////////////////////////////////

// BOOST
using namespace boost::log::trivial;


//////////////////////////////////////////////////////////////////////////////
// FUNCTION IMPLEMENTATIONS
//////////////////////////////////////////////////////////////////////////////

std::vector<JSONData> ProcessJSONFile (
  const std::string &json_file,
  const std::string &directory
)
{
  try {
    BOOST_LOG_SEV(g_Log,debug) << "    Enter - ProcessJSONFile('"
      << json_file << "','" << directory << "')";
    std::ifstream file{json_file};
    if (!file)
    {
      throw std::runtime_error{
        std::format (
          "Cannot open JSON manifest file '{}'!",
          json_file
        )
      };
    }

    nlohmann::json manifest{};
    file >> manifest;
    if (!manifest.is_object ())
    {
      throw std::runtime_error{
        std::format (
          "Manifest file '{}' does not give a JSON object !",
          json_file
        )
      };
    }

    std::vector<JSONData> retVec{};
    retVec.reserve (manifest.size ());
    for (const auto & [cachePath, entry] : manifest.items ())
    {
      const auto imagePath{entry.at ("ImagePath").get<std::string> ()};
      const auto width{entry.at ("Width").get<int> ()};
      const auto height{entry.at ("Height").get<int> ()};

      if (imagePath.starts_with (directory))
      {
        retVec.push_back (
          {
            imagePath,
            cachePath,
            width,
            height
          }
        );
      }
    }
    std::ranges::sort (retVec, std::less<>{}, & JSONData::file_path);

    BOOST_LOG_SEV(g_Log,debug) << "    Leave - ProcessJSONFile(...)";
    return retVec;
  }
  catch (const std::runtime_error &e)
  {
    BOOST_LOG_SEV(g_Log,error) << "      EXCEPTION: " << e.what ();
    throw;
  }
}

void ChangeDesktopBackground (const std::wstring &image_file)
{
  BOOST_LOG_SEV(g_Log,debug) << "    Enter - ChangeDesktopBackground('"
    << image_file << "')";
  SystemParametersInfoW (
    SPI_SETDESKWALLPAPER,
    0U,
    (PVOID)image_file.c_str (),
    0U
  );
  BOOST_LOG_SEV(g_Log,debug) << "    Leave - ChangeDesktopBackground(...)";
}

void UpdateRegistry (LPCWSTR position, bool tileOn)
{
  BOOST_LOG_SEV(g_Log,debug) << "    Enter - UpdateRegistry('"
    << position << "'," << tileOn << ")";

  HKEY hKey{};
  if (
    RegOpenKeyExW (
      HKEY_CURRENT_USER,
      REGISTRY_KEY,
      0,
      KEY_SET_VALUE,
      &hKey
    ) == ERROR_SUCCESS
  )
  {
    if (tileOn)
    {
      RegSetValueExW (
        hKey,
        TILE_WALLPAPER_KEY,
        0,
        REG_SZ,
        (const BYTE*)TILE_ON,
        (wcslen (TILE_ON) + 1) * sizeof(WCHAR)
      );
    }
    else
    {
      RegSetValueExW (
        hKey,
        TILE_WALLPAPER_KEY,
        0,
        REG_SZ,
        (const BYTE*)TILE_OFF,
        (wcslen (TILE_OFF) + 1) * sizeof(WCHAR)
      );
      RegSetValueExW (
        hKey,
        WALLPAPER_STYLE_KEY,
        0,
        REG_SZ,
        (const BYTE*)position,
        (wcslen (position) + 1) * sizeof(WCHAR)
      );
    }
    RegCloseKey (hKey);
  }
  BOOST_LOG_SEV(g_Log,debug) << "    Leave - UpdateRegistry(...)";
}


//////////////////////////////////////////////////////////////////////////////
// END
//////////////////////////////////////////////////////////////////////////////
