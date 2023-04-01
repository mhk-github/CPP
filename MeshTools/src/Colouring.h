#pragma once
///////////////////////////////////////////////////////////////////////////////
// FILE     : Colouring.h
// SYNOPSIS : Utilities for setting colours in mesh data.
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

//! A namespace for colouring functionality.
namespace Colouring
{
  /////////////////////////////////////////////////////////////////////////////
  // STRUCTS
  /////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Represents a colour as \c red , \c green , \c blue , and \c alpha
   * components.
   *
   * @ingroup STRUCT
   */
  struct rgba_struct
  {
    //! The \c red colour component.
    float red{};
    //! The \c green colour component.
    float green{};
    //! The \c blue colour component.
    float blue{};
    /**
     * @brief The \c alpha colour component.
     *
     * @internal @note A material is assumed opaque by default.
     */    
    float alpha{ 1.0f };
  };

  
  /**
   * @brief Uses ADS colour values and a shininess value to represent a
   * material.
   *
   * @ingroup STRUCT
   */
  struct material_struct
  {
    //! The \c ambient colour (A).
    rgba_struct ambient{};
    //! The \c diffuse colour (D).
    rgba_struct diffuse{};
    //! The \c specular colour (S).
    rgba_struct specular{};
    //! The shininess value for the material.
    float shininess{};
  };
  
  
  /////////////////////////////////////////////////////////////////////////////
  // ENUMS
  /////////////////////////////////////////////////////////////////////////////

  //! An enumeration of all materials catered for in this application.
  enum class MaterialEnum 
  {
    Gold,     ///< An identifier for gold.
    Jade,     ///< An identifier for jade.
    Pearl,    ///< An identifier for pearl.
    Silver,   ///< An identifier for silver.
    Landscape ///< An identifier for landscapes.
  };
  

  /////////////////////////////////////////////////////////////////////////////
  // CONSTANTS
  /////////////////////////////////////////////////////////////////////////////

  /**
   * @defgroup LANDSCAPE Landscape Colour Data
   *
   * @brief Colouring data for landscapes.
   *
   * @{
   */

  //! Landscape ambient colour.
  const rgba_struct LANDSCAPE_AMBIENT_COLOUR{
      .red{ 0.1f },
      .green{ 0.1f },
      .blue{ 0.1f },
      .alpha{ 1.0f }
  };

  //! Landscape specular colour.
  const rgba_struct LANDSCAPE_SPECULAR_COLOUR{
      .red{ 0.1f },
      .green{ 0.1f },
      .blue{ 0.1f },
      .alpha{ 1.0f }
  };

  //! Landscape shininess value.
  const auto LANDSCAPE_SHININESS{ 0.01f };
  
  /**
   * @}
   */

  /**
   * @defgroup MATERIAL Material Colours
   *
   * @brief Colouring data for different materials.
   *
   * @{
   */

  //! Colour data for gold.
  const material_struct GOLD{
      .ambient{ 0.2473f, 0.1995f, 0.0745f, 1.0f },
      .diffuse{ 0.7516f, 0.6065f, 0.2265f, 1.0f },
      .specular{ 0.6283f, 0.5558f, 0.3661f, 1.0f },
      .shininess{ 51.2f }
  };

  //! Colour data for jade.
  const material_struct JADE{
      .ambient{ 0.135f, 0.2225f, 0.1575f, 0.95f },
      .diffuse{ 0.54f, 0.89f, 0.63f, 0.95f },
      .specular{ 0.3162f, 0.3162f, 0.3162f, 0.95f },
      .shininess{ 12.8f }
  };

  //! Colour data for pearl.
  const material_struct PEARL{
      .ambient{ 0.25f, 0.2073f, 0.2073f, 0.922f },
      .diffuse{ 1.0f, 0.829f, 0.829f, 0.922f },
      .specular{ 0.2966f, 0.2966f, 0.2966f, 0.922f },
      .shininess{ 51.2f }
  };

  //! Colour data for silver.
  const material_struct SILVER{
      .ambient{ 0.1923f, 0.1923f, 0.1923f, 1.0f },
      .diffuse{ 0.5075f, 0.5075f, 0.5075f, 1.0f },
      .specular{ 0.5083f, 0.5083f, 0.5083f, 1.0f },
      .shininess{ 51.2f }
  };

  /**
   * @}
   */

    
  /////////////////////////////////////////////////////////////////////////////
  // FUNCTIONS
  /////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Returns diffuse colour data for a landscape.
   *
   * @param[in] height The value of a vertex \c y coordinate.
   * @returns rgba_struct A diffuse colour for the given height value.
   *
   * @internal @note Though this function will not be needed once texturing
   * code is in place, there is the possibility to enhance it with
   * randomization.
   */
  rgba_struct landscape_diffuse_colour ( float height )
  {
    float red{};
    float green{};
    float blue{};
    
    if ( height < 0.32f )
    {
      red = 0.0f;
      green = 0.3922f;
      blue = 0.0f;
    }
    else if ( height < 0.35f )
    {
      red = 0.0f;
      green = 0.5f;
      blue = 0.0f;
    }
    else if ( height < 0.4f )
    {
      red = 0.4196f;
      green = 0.5569f;
      blue = 0.1373f;
    }
    else if ( height < 0.45f )
    {
      red = 0.8672f;
      green = 0.7216f;
      blue = 0.5294f;
    }
    else if ( height < 0.5f )
    {
      red = 0.5f;
      green = 0.5f;
      blue = 0.5f;
    }
    else
    {
      red = 1.0f;
      green = 1.0f;
      blue = 1.0f;
    }

    return ( rgba_struct{ .red{ red }, .green{ green }, .blue{ blue } } );
  }
  
}


///////////////////////////////////////////////////////////////////////////////
// END
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @brief Utilities for setting colours in mesh data.
 *
 * @author Mohammad Haroon Khaliq
 * @date @showdate "%d %B %Y"
 * @copyright MIT License.
 */
 // Local variables:
 // mode: c++
 // End:
