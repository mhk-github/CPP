#pragma once
///////////////////////////////////////////////////////////////////////////////
// FILE     : Configuration.h
// SYNOPSIS : Code for configuration functionality.
// LICENSE  : MIT
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// HEADER FILES
///////////////////////////////////////////////////////////////////////////////

// PRECOMPILED HEADER FILE ////////////////////////////////////////////////////

#include "PCH.h"

// LOCAL //////////////////////////////////////////////////////////////////////

#include "Log.h"



///////////////////////////////////////////////////////////////////////////////
// NAMESPACE
///////////////////////////////////////////////////////////////////////////////

namespace Config
{
  /////////////////////////////////////////////////////////////////////////////
  // CONSTANTS
  /////////////////////////////////////////////////////////////////////////////

  // Related to parsing a configuration file
  const auto INI_MAX_STRING_LENGTH{ 1'024 };
  const auto* INI_APP_NAME{ L"3D" };
  const auto* INI_APPLICATION_IDENTIFIER_KEY{ L"APPLICATION_IDENTIFIER" };
  const auto* INI_LOGGING_LEVEL_STRING_KEY{ L"LOGGING_LEVEL" };
  const auto* INI_LOG_DIRECTORY_STRING_KEY{ L"LOG_DIRECTORY" };
  const auto* INI_WINDOW_WIDTH_INT_KEY{ L"WINDOW_WIDTH" };
  const auto* INI_WINDOW_HEIGHT_INT_KEY{ L"WINDOW_HEIGHT" };
  const auto* INI_PERSPECTIVE_PROJECTION_FOV_DEGREES_FLOAT_KEY{ L"PERSPECTIVE_PROJECTION_FOV_DEGREES" };
  const auto* INI_PERSPECTIVE_PROJECTION_NEAR_PLANE_FLOAT_KEY{ L"PERSPECTIVE_PROJECTION_NEAR_PLANE" };
  const auto* INI_PERSPECTIVE_PROJECTION_FAR_PLANE_FLOAT_KEY{ L"PERSPECTIVE_PROJECTION_FAR_PLANE" };
  const auto* INI_VIEW_TRANSFORMATION_X_FLOAT_KEY{ L"VIEW_TRANSFORMATION_X" };
  const auto* INI_VIEW_TRANSFORMATION_Y_FLOAT_KEY{ L"VIEW_TRANSFORMATION_Y" };
  const auto* INI_VIEW_TRANSFORMATION_Z_FLOAT_KEY{ L"VIEW_TRANSFORMATION_Z" };
  const auto* INI_TRANSLATION_DIVISOR_INT_KEY{ L"TRANSLATION_DIVISOR" };
  const auto* INI_ROTATION_DIVISOR_INT_KEY{ L"ROTATION_DIVISOR" };
  const auto* INI_CLEAR_COLOUR_RED_FLOAT_KEY{ L"CLEAR_COLOUR_RED" };
  const auto* INI_CLEAR_COLOUR_GREEN_FLOAT_KEY{ L"CLEAR_COLOUR_GREEN" };
  const auto* INI_CLEAR_COLOUR_BLUE_FLOAT_KEY{ L"CLEAR_COLOUR_BLUE" };
  const auto* INI_CLEAR_COLOUR_ALPHA_FLOAT_KEY{ L"CLEAR_COLOUR_ALPHA" };

  // Related to the identifier for the executable
  const auto* DEFAULT_APPLICATION_IDENTIFIER{ INI_APP_NAME };

  // Related to display window
  const auto DEFAULT_WINDOW_WIDTH{ 1'280 };
  const auto DEFAULT_WINDOW_HEIGHT{ 720 };
  const auto MIN_WINDOW_WIDTH{ 1 };
  const auto MIN_WINDOW_HEIGHT{ 1 };

  // Related to perspective projection
  const auto DEFAULT_PERSPECTIVE_PROJECTION_FOV_DEGREES{ 50.0f };
  const auto DEFAULT_PERSPECTIVE_PROJECTION_NEAR_PLANE{ 0.1f };
  const auto DEFAULT_PERSPECTIVE_PROJECTION_FAR_PLANE{ 1000.0f };
  const auto MIN_PERSPECTIVE_PROJECTION_FOV_DEGREES{ 40.0f };
  const auto MAX_PERSPECTIVE_PROJECTION_FOV_DEGREES{ 60.0f };
  const auto MIN_PERSPECTIVE_PROJECTION_NEAR_PLANE{ 0.1f };

  // Related to the view transformation
  const auto DEFAULT_VIEW_TRANSFORMATION_X{ 0.0f };
  const auto DEFAULT_VIEW_TRANSFORMATION_Y{ 0.0f };
  const auto DEFAULT_VIEW_TRANSFORMATION_Z{ -7.0f };

  // Related to configuring smoothness of animations
  const auto DEFAULT_TRANSLATION_DIVISOR{ 500 };
  const auto DEFAULT_ROTATION_DIVISOR{ 10'000 };
  const auto MIN_TRANSLATION_DIVISOR{ 1 };
  const auto MIN_ROTATION_DIVISOR{ 1 };

  // Related to the background colour
  const auto DEFAULT_CLEAR_COLOUR_RED{ 0 };
  const auto DEFAULT_CLEAR_COLOUR_GREEN{ 0 };
  const auto DEFAULT_CLEAR_COLOUR_BLUE{ 0 };
  const auto DEFAULT_CLEAR_COLOUR_ALPHA{ 1.0f };
  const auto MIN_COLOUR_VALUE{ 0 };
  const auto MAX_COLOUR_VALUE{ 255 };
  const auto MIN_ALPHA_VALUE{ 0.0f };
  const auto MAX_ALPHA_VALUE{ 1.0f };



  /////////////////////////////////////////////////////////////////////////////
  // STRUCTURES
  /////////////////////////////////////////////////////////////////////////////

  struct configuration {
      std::wstring application_identifier{ DEFAULT_APPLICATION_IDENTIFIER };
      std::wstring logging_level{ Log::LOG_DEFAULT_LEVEL };
      std::wstring log_directory{ Log::LOG_DEFAULT_DIRECTORY };
      int window_width{ DEFAULT_WINDOW_WIDTH };
      int window_height{ DEFAULT_WINDOW_HEIGHT };
      float perspective_projection_fov_degrees{ DEFAULT_PERSPECTIVE_PROJECTION_FOV_DEGREES };
      float perspective_projection_near_plane{ DEFAULT_PERSPECTIVE_PROJECTION_NEAR_PLANE };
      float perspective_projection_far_plane{ DEFAULT_PERSPECTIVE_PROJECTION_FAR_PLANE };
      float view_transformation_x{ DEFAULT_VIEW_TRANSFORMATION_X };
      float view_transformation_y{ DEFAULT_VIEW_TRANSFORMATION_Y };
      float view_transformation_z{ DEFAULT_VIEW_TRANSFORMATION_Z };
      int translation_divisor{ DEFAULT_TRANSLATION_DIVISOR };
      int rotation_divisor{ DEFAULT_ROTATION_DIVISOR };
      int clear_colour_red{ DEFAULT_CLEAR_COLOUR_RED };
      int clear_colour_green{ DEFAULT_CLEAR_COLOUR_GREEN };
      int clear_colour_blue{ DEFAULT_CLEAR_COLOUR_BLUE };
      float clear_colour_alpha{ DEFAULT_CLEAR_COLOUR_ALPHA };
  };



  /////////////////////////////////////////////////////////////////////////////
  // FUNCTIONS
  /////////////////////////////////////////////////////////////////////////////

  inline bool init_configuration (
      const std::wstring& config_file,
      configuration& config 
  ) noexcept
  {
    try
    {
      const auto* ini_file{ config_file.c_str () };
      wchar_t ini_string_buffer[INI_MAX_STRING_LENGTH]{};

      std::wstring app_identifier{};
      if ( GetPrivateProfileStringW (
          INI_APP_NAME,
          INI_APPLICATION_IDENTIFIER_KEY,
          nullptr,
          ini_string_buffer,
          INI_MAX_STRING_LENGTH,
          ini_file
      ) != 0 )
      {
        app_identifier = ini_string_buffer;
      }

      std::wstring logging_level_cfg{};
      if ( GetPrivateProfileStringW (
          INI_APP_NAME,
          INI_LOGGING_LEVEL_STRING_KEY,
          nullptr,
          ini_string_buffer,
          INI_MAX_STRING_LENGTH,
          ini_file
      ) != 0 )
      {
        logging_level_cfg = ini_string_buffer;
        assert (
            boost::iequals ( logging_level_cfg, Log::LOG_LEVEL_TRACE ) ||
            boost::iequals ( logging_level_cfg, Log::LOG_LEVEL_DEBUG ) ||
            boost::iequals ( logging_level_cfg, Log::LOG_LEVEL_INFO ) ||
            boost::iequals ( logging_level_cfg, Log::LOG_LEVEL_WARNING ) ||
            boost::iequals ( logging_level_cfg, Log::LOG_LEVEL_ERROR ) ||
            boost::iequals ( logging_level_cfg, Log::LOG_LEVEL_FATAL )
        );
        if ( !(
            boost::iequals ( logging_level_cfg, Log::LOG_LEVEL_TRACE ) ||
            boost::iequals ( logging_level_cfg, Log::LOG_LEVEL_DEBUG ) ||
            boost::iequals ( logging_level_cfg, Log::LOG_LEVEL_INFO ) ||
            boost::iequals ( logging_level_cfg, Log::LOG_LEVEL_WARNING ) ||
            boost::iequals ( logging_level_cfg, Log::LOG_LEVEL_ERROR ) ||
            boost::iequals ( logging_level_cfg, Log::LOG_LEVEL_FATAL ) )
        )
        {
          throw std::invalid_argument ( "Logging level can only be one of '{trace|debug|info|warning|error|fatal}' !" );
        }
      }

      std::wstring log_directory_cfg{};
      if ( GetPrivateProfileStringW (
          INI_APP_NAME,
          INI_LOG_DIRECTORY_STRING_KEY,
          nullptr,
          ini_string_buffer,
          INI_MAX_STRING_LENGTH,
          ini_file
      ) != 0 )
      {
        log_directory_cfg = ini_string_buffer;
        assert ( boost::filesystem::is_directory ( log_directory_cfg ) );
        if ( !boost::filesystem::is_directory ( log_directory_cfg ) )
        {
          throw std::runtime_error ( "Value provided is not a valid directory !" );
        }
      }

      auto window_width_cfg{ DEFAULT_WINDOW_WIDTH };
      if ( GetPrivateProfileStringW (
          INI_APP_NAME,
          INI_WINDOW_WIDTH_INT_KEY,
          nullptr,
          ini_string_buffer,
          INI_MAX_STRING_LENGTH,
          ini_file
      ) != 0 )
      {
        window_width_cfg = std::stoi ( ini_string_buffer );
        assert ( window_width_cfg >= MIN_WINDOW_WIDTH );
        if ( window_width_cfg < MIN_WINDOW_WIDTH )
        {
          throw std::invalid_argument ( "Window width less than minimum size !" );
        }
      }

      auto window_height_cfg{ DEFAULT_WINDOW_HEIGHT };
      if ( GetPrivateProfileStringW (
          INI_APP_NAME,
          INI_WINDOW_HEIGHT_INT_KEY,
          nullptr,
          ini_string_buffer,
          INI_MAX_STRING_LENGTH,
          ini_file
      ) != 0 )
      {
        window_height_cfg = std::stoi ( ini_string_buffer );
        assert ( window_height_cfg >= MIN_WINDOW_HEIGHT );
        if ( window_height_cfg < MIN_WINDOW_HEIGHT )
        {
          throw std::invalid_argument ( "Window height less than minimum size !" );
        }
      }

      auto perspective_projection_fov_degrees_cfg{ DEFAULT_PERSPECTIVE_PROJECTION_FOV_DEGREES };
      if ( GetPrivateProfileStringW (
          INI_APP_NAME,
          INI_PERSPECTIVE_PROJECTION_FOV_DEGREES_FLOAT_KEY,
          nullptr,
          ini_string_buffer,
          INI_MAX_STRING_LENGTH,
          ini_file
      ) != 0 )
      {
        perspective_projection_fov_degrees_cfg = std::stof ( ini_string_buffer );
        assert (
          perspective_projection_fov_degrees_cfg >= MIN_PERSPECTIVE_PROJECTION_FOV_DEGREES
        );
        if ( perspective_projection_fov_degrees_cfg < MIN_PERSPECTIVE_PROJECTION_FOV_DEGREES )
        {
          throw std::invalid_argument ( "Perspective projection FOV below minimum size !" );
        }
        assert (
          perspective_projection_fov_degrees_cfg <= MAX_PERSPECTIVE_PROJECTION_FOV_DEGREES
        );
        if ( perspective_projection_fov_degrees_cfg > MAX_PERSPECTIVE_PROJECTION_FOV_DEGREES )
        {
          throw std::invalid_argument ( "Perspective projection FOV above maximum size !" );
        }
      }

      auto perspective_projection_near_plane_cfg{ DEFAULT_PERSPECTIVE_PROJECTION_NEAR_PLANE };
      if ( GetPrivateProfileStringW (
          INI_APP_NAME,
          INI_PERSPECTIVE_PROJECTION_NEAR_PLANE_FLOAT_KEY,
          nullptr,
          ini_string_buffer,
          INI_MAX_STRING_LENGTH,
          ini_file
      ) != 0 )
      {
        perspective_projection_near_plane_cfg = std::stof ( ini_string_buffer );
        assert ( perspective_projection_near_plane_cfg >= MIN_PERSPECTIVE_PROJECTION_NEAR_PLANE );
        if ( perspective_projection_near_plane_cfg < MIN_PERSPECTIVE_PROJECTION_NEAR_PLANE )
        {
          throw std::invalid_argument ( "Perspective projection near plane below minimum size !" );
        }
      }

      auto perspective_projection_far_plane_cfg{ DEFAULT_PERSPECTIVE_PROJECTION_FAR_PLANE };
      if ( GetPrivateProfileStringW (
          INI_APP_NAME,
          INI_PERSPECTIVE_PROJECTION_FAR_PLANE_FLOAT_KEY,
          nullptr,
          ini_string_buffer,
          INI_MAX_STRING_LENGTH,
          ini_file
      ) != 0 )
      {
        perspective_projection_far_plane_cfg = std::stof ( ini_string_buffer );
        assert ( perspective_projection_far_plane_cfg > perspective_projection_near_plane_cfg );
        if ( perspective_projection_far_plane_cfg <= perspective_projection_near_plane_cfg )
        {
          throw std::invalid_argument ( "Perspective projection far plane is less than or equal to near plane location !" );
        }
      }

      auto view_transformation_x_cfg{ DEFAULT_VIEW_TRANSFORMATION_X };
      if ( GetPrivateProfileStringW (
          INI_APP_NAME,
          INI_VIEW_TRANSFORMATION_X_FLOAT_KEY,
          nullptr,
          ini_string_buffer,
          INI_MAX_STRING_LENGTH,
          ini_file
      ) != 0 )
      {
        view_transformation_x_cfg = std::stof ( ini_string_buffer );
      }

      auto view_transformation_y_cfg{ DEFAULT_VIEW_TRANSFORMATION_Y };
      if ( GetPrivateProfileStringW (
          INI_APP_NAME,
          INI_VIEW_TRANSFORMATION_Y_FLOAT_KEY,
          nullptr,
          ini_string_buffer,
          INI_MAX_STRING_LENGTH,
          ini_file
      ) != 0 )
      {
        view_transformation_y_cfg = std::stof ( ini_string_buffer );
      }

      auto view_transformation_z_cfg{ DEFAULT_VIEW_TRANSFORMATION_Z };
      if ( GetPrivateProfileStringW (
          INI_APP_NAME,
          INI_VIEW_TRANSFORMATION_Z_FLOAT_KEY,
          nullptr,
          ini_string_buffer,
          INI_MAX_STRING_LENGTH,
          ini_file
      ) != 0 )
      {
        view_transformation_z_cfg = std::stof ( ini_string_buffer );
      }

      auto translation_divisor_cfg{ DEFAULT_TRANSLATION_DIVISOR };
      if ( GetPrivateProfileStringW (
          INI_APP_NAME,
          INI_TRANSLATION_DIVISOR_INT_KEY,
          nullptr,
          ini_string_buffer,
          INI_MAX_STRING_LENGTH,
          ini_file
      ) != 0 )
      {
        translation_divisor_cfg = std::stoi ( ini_string_buffer );
        assert ( translation_divisor_cfg >= MIN_TRANSLATION_DIVISOR );
        if ( translation_divisor_cfg < MIN_TRANSLATION_DIVISOR )
        {
          throw std::invalid_argument ( "Translation divisor less than minimum value !" );
        }
      }

      auto rotation_divisor_cfg{ DEFAULT_ROTATION_DIVISOR };
      if ( GetPrivateProfileStringW (
          INI_APP_NAME,
          INI_ROTATION_DIVISOR_INT_KEY,
          nullptr,
          ini_string_buffer,
          INI_MAX_STRING_LENGTH,
          ini_file
      ) != 0 )
      {
        rotation_divisor_cfg = std::stoi ( ini_string_buffer );
        assert ( rotation_divisor_cfg >= MIN_ROTATION_DIVISOR );
        if ( rotation_divisor_cfg < MIN_ROTATION_DIVISOR )
        {
          throw std::invalid_argument ( "Rotation divisor less than minimum value !" );
        }
      }

      auto clear_colour_red_cfg{ DEFAULT_CLEAR_COLOUR_RED };
      if ( GetPrivateProfileStringW (
          INI_APP_NAME,
          INI_CLEAR_COLOUR_RED_FLOAT_KEY,
          nullptr,
          ini_string_buffer,
          INI_MAX_STRING_LENGTH,
          ini_file
      ) != 0 )
      {
        clear_colour_red_cfg = std::stoi ( ini_string_buffer );
        assert ( clear_colour_red_cfg >= MIN_COLOUR_VALUE );
        if ( clear_colour_red_cfg < MIN_COLOUR_VALUE )
        {
          throw std::invalid_argument ( "Red colour below minimum value !" );
        }
        assert ( clear_colour_red_cfg <= MAX_COLOUR_VALUE );
        if ( clear_colour_red_cfg > MAX_COLOUR_VALUE )
        {
          throw std::invalid_argument ( "Red colour above maximum value !" );
        }
      }

      auto clear_colour_green_cfg{ DEFAULT_CLEAR_COLOUR_GREEN };
      if ( GetPrivateProfileStringW (
          INI_APP_NAME,
          INI_CLEAR_COLOUR_GREEN_FLOAT_KEY,
          nullptr,
          ini_string_buffer,
          INI_MAX_STRING_LENGTH,
          ini_file
      ) != 0 )
      {
        clear_colour_green_cfg = std::stoi ( ini_string_buffer );
        assert ( clear_colour_green_cfg >= MIN_COLOUR_VALUE );
        if ( clear_colour_green_cfg < MIN_COLOUR_VALUE )
        {
          throw std::invalid_argument ( "Green colour below minimum value !" );
        }
        assert ( clear_colour_green_cfg <= MAX_COLOUR_VALUE );
        if ( clear_colour_green_cfg > MAX_COLOUR_VALUE )
        {
          throw std::invalid_argument ( "Green colour above maximum value !" );
        }
      }

      auto clear_colour_blue_cfg{ DEFAULT_CLEAR_COLOUR_BLUE };
      if ( GetPrivateProfileStringW (
          INI_APP_NAME,
          INI_CLEAR_COLOUR_BLUE_FLOAT_KEY,
          nullptr,
          ini_string_buffer,
          INI_MAX_STRING_LENGTH,
          ini_file
      ) != 0 )
      {
        clear_colour_blue_cfg = std::stoi ( ini_string_buffer );
        assert ( clear_colour_blue_cfg >= MIN_COLOUR_VALUE );
        if ( clear_colour_blue_cfg < MIN_COLOUR_VALUE )
        {
          throw std::invalid_argument ( "Blue colour below minimum value !" );
        }
        assert ( clear_colour_blue_cfg <= MAX_COLOUR_VALUE );
        if ( clear_colour_blue_cfg > MAX_COLOUR_VALUE )
        {
          throw std::invalid_argument ( "Blue colour above maximum value !" );
        }
      }

      auto clear_colour_alpha_cfg{ DEFAULT_CLEAR_COLOUR_ALPHA };
      if ( GetPrivateProfileStringW (
          INI_APP_NAME,
          INI_CLEAR_COLOUR_ALPHA_FLOAT_KEY,
          nullptr,
          ini_string_buffer,
          INI_MAX_STRING_LENGTH,
          ini_file
      ) != 0 )
      {
        clear_colour_alpha_cfg = std::stof ( ini_string_buffer );
        assert ( clear_colour_alpha_cfg >= MIN_ALPHA_VALUE );
        if ( clear_colour_alpha_cfg < MIN_ALPHA_VALUE )
        {
          throw std::invalid_argument ( "Alpha below minimum value !" );
        }
        assert ( clear_colour_alpha_cfg <= MAX_ALPHA_VALUE );
        if ( clear_colour_alpha_cfg > MAX_ALPHA_VALUE )
        {
          throw std::invalid_argument ( "Alpha above maximum value !" );
        }
      }

      if ( !app_identifier.empty () )
      {
        config.application_identifier.swap ( app_identifier );
      }

      if ( !logging_level_cfg.empty () )
      {
        config.logging_level.swap ( logging_level_cfg );
      }

      if ( !log_directory_cfg.empty () )
      {
        config.log_directory.swap ( log_directory_cfg );
      }

      if ( window_width_cfg != DEFAULT_WINDOW_WIDTH )
      {
        config.window_width = window_width_cfg;
      }

      if ( window_height_cfg != DEFAULT_WINDOW_HEIGHT )
      {
        config.window_height = window_height_cfg;
      }

      if ( perspective_projection_fov_degrees_cfg != DEFAULT_PERSPECTIVE_PROJECTION_FOV_DEGREES )
      {
        config.perspective_projection_fov_degrees = perspective_projection_fov_degrees_cfg;
      }

      if ( perspective_projection_near_plane_cfg != DEFAULT_PERSPECTIVE_PROJECTION_NEAR_PLANE )
      {
        config.perspective_projection_near_plane = perspective_projection_near_plane_cfg;
      }

      if ( perspective_projection_far_plane_cfg != DEFAULT_PERSPECTIVE_PROJECTION_FAR_PLANE )
      {
        config.perspective_projection_far_plane = perspective_projection_far_plane_cfg;
      }

      if ( view_transformation_x_cfg != DEFAULT_VIEW_TRANSFORMATION_X )
      {
        config.view_transformation_x = view_transformation_x_cfg;
      }

      if ( view_transformation_y_cfg != DEFAULT_VIEW_TRANSFORMATION_Y )
      {
        config.view_transformation_y = view_transformation_y_cfg;
      }

      if ( view_transformation_z_cfg != DEFAULT_VIEW_TRANSFORMATION_Z )
      {
        config.view_transformation_z = view_transformation_z_cfg;
      }

      if ( translation_divisor_cfg != DEFAULT_TRANSLATION_DIVISOR )
      {
        config.translation_divisor = translation_divisor_cfg;
      }

      if ( rotation_divisor_cfg != DEFAULT_ROTATION_DIVISOR )
      {
        config.rotation_divisor = rotation_divisor_cfg;
      }

      if ( clear_colour_red_cfg != DEFAULT_CLEAR_COLOUR_RED )
      {
        config.clear_colour_red = clear_colour_red_cfg;
      }

      if ( clear_colour_green_cfg != DEFAULT_CLEAR_COLOUR_GREEN )
      {
        config.clear_colour_green = clear_colour_green_cfg;
      }

      if ( clear_colour_blue_cfg != DEFAULT_CLEAR_COLOUR_BLUE )
      {
        config.clear_colour_blue = clear_colour_blue_cfg;
      }

      if ( clear_colour_alpha_cfg != DEFAULT_CLEAR_COLOUR_ALPHA )
      {
        config.clear_colour_alpha = clear_colour_alpha_cfg;
      }

    }
    catch ( std::exception& e )
    {
      std::wcerr << "Exception during parsing of configuration file '"
        << config_file << "': " << e.what () << "\n";
      return ( false );
    }

    return ( true );
  }

}



///////////////////////////////////////////////////////////////////////////////
// END
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @brief Code for parsing configuration data for this application.
 */
// Local variables:
// mode: c++
// End:
