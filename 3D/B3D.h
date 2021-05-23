#pragma once
///////////////////////////////////////////////////////////////////////////////
// FILE     : B3D.h
// SYNOPSIS : Functionality for processing B3D files.
// LICENSE  : MIT
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// HEADER FILES
///////////////////////////////////////////////////////////////////////////////

// PRECOMPILED HEADER FILE ////////////////////////////////////////////////////

#include "PCH.h"

// LOCAL /////////////////////////////////////////////////////////////////////.

#include "Data3D.h"
#include "Log.h"



///////////////////////////////////////////////////////////////////////////////
// NAMESPACE
///////////////////////////////////////////////////////////////////////////////

namespace B3D
{
  /////////////////////////////////////////////////////////////////////////////
  // CONSTANTS
  /////////////////////////////////////////////////////////////////////////////

  const unsigned char B3D_HEADER_B0{ 'B' };
  const unsigned char B3D_HEADER_B1{ '3' };
  const unsigned char B3D_HEADER_B2{ 'D' };
  const unsigned char B3D_HEADER_B3{ '\n' };
  const unsigned char B3D_INDICES_TYPE_UCHAR{ '\x01' };
  const unsigned char B3D_INDICES_TYPE_USHORT{ '\x02' };
  const unsigned char B3D_INDICES_TYPE_UINT{ '\x04' };
  const auto B3D_HEADER_SIZE{ 16U };
  const auto ELEMENTS_IN_VERTEX{ 3 };
  const auto ELEMENTS_IN_NORMAL{ 3 };
  const auto ELEMENTS_IN_UV{ 2 };



  /////////////////////////////////////////////////////////////////////////////
  // STRUCTURES
  /////////////////////////////////////////////////////////////////////////////

  struct B3DHeader {
      unsigned char b0;
      unsigned char b1;
      unsigned char b2;
      unsigned char b3;
      unsigned long num_vertices;
      unsigned long num_indices;
      unsigned char has_normals;
      unsigned char has_uv;
      unsigned char has_colours;
      unsigned char indices_type_hint;
  };
  static_assert ( sizeof ( B3DHeader ) == B3D_HEADER_SIZE,
      L"B3DHeader must be B3D_HEADER_SIZE bytes in size !" );



  /////////////////////////////////////////////////////////////////////////////
  // UNIONS
  /////////////////////////////////////////////////////////////////////////////

  union float_4char_union
  {
      char chars[4];
      float float_val;
  };



  /////////////////////////////////////////////////////////////////////////////
  // FUNCTIONS
  /////////////////////////////////////////////////////////////////////////////

  bool import_b3d_file ( const std::wstring& filename, Data3D::D3D& data_3d )
  {
    using namespace boost::log::trivial;
    Log::Logger log;
    BOOST_LOG_SEV(log,debug) << "  Enter import_b3d_file ('" << filename << "',...)";

    std::ifstream file{ filename, std::ios::binary };
    if ( !file.is_open () )
    {
      BOOST_LOG_SEV(log,error) << "    Cannot open binary file '" << filename << "' for reading !";
      BOOST_LOG_SEV(log,debug) << "  Leave import_b3d_file (value=false)";
      return ( false );
    }
    BOOST_LOG_SEV(log,debug) << "    Opened binary file '" << filename << "' for reading";

    B3DHeader b3d_header{};
    file.read ( reinterpret_cast<char*>( &b3d_header ), sizeof ( B3DHeader ) );
    if ( file.bad () || file.fail () )
    {
      BOOST_LOG_SEV(log,error) << "      Failed to read "
          << sizeof ( B3DHeader ) << " bytes of B3D header data from file '"
          << filename << "' !";
      BOOST_LOG_SEV(log,debug) << "  Leave import_b3d_file (value=false)";
      return ( false );
    }
    BOOST_LOG_SEV(log,trace) << "      Header data bytes read from B3D file = " << file.gcount ();

    if ( !( b3d_header.b0 == B3D_HEADER_B0 &&
        b3d_header.b1 == B3D_HEADER_B1 &&
        b3d_header.b2 == B3D_HEADER_B2 &&
        b3d_header.b3 == B3D_HEADER_B3 ) 
    )
    {
      BOOST_LOG_SEV(log,error) << "      B3D header ID bytes incorrect ['"
          << static_cast<int>( b3d_header.b0 ) << "',"
          << static_cast<int>( b3d_header.b1 ) << "',"
          << static_cast<int>( b3d_header.b2 ) << "',"
          << static_cast<int>( b3d_header.b3 ) << "'] !";
      BOOST_LOG_SEV(log,debug) << "  Leave import_b3d_file (value=false)";
      return ( false );
    }

    const auto num_verts{ b3d_header.num_vertices };
    const auto num_indices{ b3d_header.num_indices };
    const auto has_normals{ b3d_header.has_normals == 1 };
    const auto has_uvs{ b3d_header.has_uv == 1 };
    const auto has_colours{ b3d_header.has_colours == 1 };
    const auto indices_type_hint{ b3d_header.indices_type_hint };
    if ( !( indices_type_hint == B3D_INDICES_TYPE_UCHAR ||
        indices_type_hint == B3D_INDICES_TYPE_USHORT ||
        indices_type_hint == B3D_INDICES_TYPE_UINT ) 
    )
    {
      BOOST_LOG_SEV(log,error) << "      Unrecognised indices type hint '" << static_cast<int>( indices_type_hint ) << "' !";
      BOOST_LOG_SEV(log,debug) << "  Leave import_b3d_file (value=false)";
      return ( false );
    }
    BOOST_LOG_SEV(log,trace) << "      B3D header data: vertices = "
        << num_verts << ", indices = " << num_indices << ", normals? = "
        << has_normals << ", texture UVs? = " << has_uvs << ", colours? = "
        << has_colours << ", indices type hint = " << static_cast<int>( indices_type_hint );

    const auto vertex_bytes_needed{ num_verts * ELEMENTS_IN_VERTEX * sizeof ( float ) };
    boost::scoped_array<char> file_chars_vertices{ new char[vertex_bytes_needed] };
    file.read ( file_chars_vertices.get (), static_cast<std::streamsize>( vertex_bytes_needed ) );
    if ( file.bad () || file.fail () )
    {
      BOOST_LOG_SEV(log,error) << "      Failed to read " << vertex_bytes_needed
          << " bytes from file '" << filename << "' !";
      BOOST_LOG_SEV(log,debug) << "  Leave import_b3d_file (value=false)";
      return ( false );
    }
    BOOST_LOG_SEV(log,trace) << "      Vertex float precursor bytes read = " << file.gcount ();

    const auto* vertex_char_ptr{ file_chars_vertices.get () };
    const auto num_vertex_glm_vec3{ num_verts };
    std::vector<glm::vec3> extracted_vertices{};
    extracted_vertices.reserve ( num_vertex_glm_vec3 );
    for ( auto i{ 0UL }; i < num_vertex_glm_vec3; ++i )
    {
      float_4char_union f4cu{};
      auto* f4cu_char_ptr{ f4cu.chars };
      *f4cu_char_ptr = *vertex_char_ptr;
      ++f4cu_char_ptr;
      ++vertex_char_ptr;
      *f4cu_char_ptr = *vertex_char_ptr;
      ++f4cu_char_ptr;
      ++vertex_char_ptr;
      *f4cu_char_ptr = *vertex_char_ptr;
      ++f4cu_char_ptr;
      ++vertex_char_ptr;
      *f4cu_char_ptr = *vertex_char_ptr;
      ++vertex_char_ptr;
      const auto x{ f4cu.float_val };

      f4cu_char_ptr = f4cu.chars;
      *f4cu_char_ptr = *vertex_char_ptr;
      ++f4cu_char_ptr;
      ++vertex_char_ptr;
      *f4cu_char_ptr = *vertex_char_ptr;
      ++f4cu_char_ptr;
      ++vertex_char_ptr;
      *f4cu_char_ptr = *vertex_char_ptr;
      ++f4cu_char_ptr;
      ++vertex_char_ptr;
      *f4cu_char_ptr = *vertex_char_ptr;
      ++vertex_char_ptr;
      const auto y{ f4cu.float_val };

      f4cu_char_ptr = f4cu.chars;
      *f4cu_char_ptr = *vertex_char_ptr;
      ++f4cu_char_ptr;
      ++vertex_char_ptr;
      *f4cu_char_ptr = *vertex_char_ptr;
      ++f4cu_char_ptr;
      ++vertex_char_ptr;
      *f4cu_char_ptr = *vertex_char_ptr;
      ++f4cu_char_ptr;
      ++vertex_char_ptr;
      *f4cu_char_ptr = *vertex_char_ptr;
      ++vertex_char_ptr;
      const auto z{ f4cu.float_val };

      extracted_vertices.emplace_back ( x, y, z );
    }
    file_chars_vertices.reset ();
    BOOST_LOG_SEV(log,trace) << "      Size of vertex glm::vec3 vector made = "
        << extracted_vertices.size ();

    std::vector<glm::vec3> extracted_normals{};
    if ( has_normals )
    {
      const auto normal_bytes_needed = num_verts * ELEMENTS_IN_NORMAL * sizeof ( float );
      boost::scoped_array<char> file_chars_normals{ new char[normal_bytes_needed] };
      file.read ( file_chars_normals.get (), static_cast<std::streamsize>( normal_bytes_needed ) );
      if ( file.bad () || file.fail () )
      {
        BOOST_LOG_SEV(log,error) << "      Failed to read "
            << normal_bytes_needed << " bytes from file '" << filename << "' !";
        BOOST_LOG_SEV(log,debug) << "  Leave import_b3d_file (value=false)";
        return ( false );
      }
      BOOST_LOG_SEV(log,trace) << "      Normal float precursor bytes read = " << file.gcount ();

      const auto* normal_char_ptr{ file_chars_normals.get () };
      const auto num_normal_glm_vec3{ num_verts };
      extracted_normals.reserve ( num_normal_glm_vec3 );
      for ( auto i{ 0UL }; i < num_normal_glm_vec3; ++i )
      {
        float_4char_union f4cu{};
        auto* f4cu_char_ptr{ f4cu.chars };
        *f4cu_char_ptr = *normal_char_ptr;
        ++f4cu_char_ptr;
        ++normal_char_ptr;
        *f4cu_char_ptr = *normal_char_ptr;
        ++f4cu_char_ptr;
        ++normal_char_ptr;
        *f4cu_char_ptr = *normal_char_ptr;
        ++f4cu_char_ptr;
        ++normal_char_ptr;
        *f4cu_char_ptr = *normal_char_ptr;
        ++normal_char_ptr;
        const auto x{ f4cu.float_val };

        f4cu_char_ptr = f4cu.chars;
        *f4cu_char_ptr = *normal_char_ptr;
        ++f4cu_char_ptr;
        ++normal_char_ptr;
        *f4cu_char_ptr = *normal_char_ptr;
        ++f4cu_char_ptr;
        ++normal_char_ptr;
        *f4cu_char_ptr = *normal_char_ptr;
        ++f4cu_char_ptr;
        ++normal_char_ptr;
        *f4cu_char_ptr = *normal_char_ptr;
        ++normal_char_ptr;
        const auto y{ f4cu.float_val };

        f4cu_char_ptr = f4cu.chars;
        *f4cu_char_ptr = *normal_char_ptr;
        ++f4cu_char_ptr;
        ++normal_char_ptr;
        *f4cu_char_ptr = *normal_char_ptr;
        ++f4cu_char_ptr;
        ++normal_char_ptr;
        *f4cu_char_ptr = *normal_char_ptr;
        ++f4cu_char_ptr;
        ++normal_char_ptr;
        *f4cu_char_ptr = *normal_char_ptr;
        ++normal_char_ptr;
        const auto z{ f4cu.float_val };

        extracted_normals.emplace_back ( x, y, z );
      }
      file_chars_normals.reset ();
      BOOST_LOG_SEV(log,trace) << "      Size of normal glm::vec3 vector made = "
          << extracted_normals.size ();
    }

    std::vector<glm::vec2> extracted_uvs{};
    if ( has_uvs )
    {
      const auto uv_bytes_needed = num_verts * ELEMENTS_IN_UV * sizeof ( float );
      boost::scoped_array<char> file_chars_uvs{ new char[uv_bytes_needed] };
      file.read ( file_chars_uvs.get (), static_cast<std::streamsize>( uv_bytes_needed ) );
      if ( file.bad () || file.fail () )
      {
        BOOST_LOG_SEV(log,error) << "      Failed to read "
            << uv_bytes_needed << " bytes from file '" << filename << "' !";
        BOOST_LOG_SEV(log,debug) << "  Leave import_b3d_file (value=false)";
        return ( false );
      }
      BOOST_LOG_SEV(log,trace) << "      Texture UV float precursor bytes read = " << file.gcount ();

      const auto* uv_char_ptr{ file_chars_uvs.get () };
      const auto num_uv_glm_vec2{ num_verts };
      extracted_uvs.reserve ( num_uv_glm_vec2 );
      for ( auto i{ 0UL }; i < num_uv_glm_vec2; ++i )
      {
        float_4char_union f4cu{};
        auto* f4cu_char_ptr{ f4cu.chars };
        *f4cu_char_ptr = *uv_char_ptr;
        ++f4cu_char_ptr;
        ++uv_char_ptr;
        *f4cu_char_ptr = *uv_char_ptr;
        ++f4cu_char_ptr;
        ++uv_char_ptr;
        *f4cu_char_ptr = *uv_char_ptr;
        ++f4cu_char_ptr;
        ++uv_char_ptr;
        *f4cu_char_ptr = *uv_char_ptr;
        ++uv_char_ptr;
        const auto u{ f4cu.float_val };

        f4cu_char_ptr = f4cu.chars;
        *f4cu_char_ptr = *uv_char_ptr;
        ++f4cu_char_ptr;
        ++uv_char_ptr;
        *f4cu_char_ptr = *uv_char_ptr;
        ++f4cu_char_ptr;
        ++uv_char_ptr;
        *f4cu_char_ptr = *uv_char_ptr;
        ++f4cu_char_ptr;
        ++uv_char_ptr;
        *f4cu_char_ptr = *uv_char_ptr;
        ++uv_char_ptr;
        const auto v{ f4cu.float_val };

        extracted_uvs.emplace_back ( u, v );
      }
      file_chars_uvs.reset ();
      BOOST_LOG_SEV(log,trace) << "      Size of texture UV glm::vec2 vector made = "
          << extracted_uvs.size ();
    }

    std::vector<unsigned int> extracted_rgbas{};
    if ( has_colours )
    {
      boost::scoped_array<unsigned int> file_uints_colours{ new unsigned int[num_verts] };
      file.read (
          reinterpret_cast<char*>( file_uints_colours.get () ),
          static_cast<std::streamsize>( num_verts ) *
          static_cast<std::streamsize>( sizeof ( unsigned int ) )
      );
      if ( file.bad () || file.fail () )
      {
        BOOST_LOG_SEV(log,error) << "      Failed to read " << num_verts
            << " unsigned int data from file '" << filename << "' !";
        BOOST_LOG_SEV(log,debug) << "  Leave import_b3d_file (value=false)";
        return ( false );
      }
      BOOST_LOG_SEV(log,trace) << "      Colours unsigned int bytes read = " << file.gcount ();
      std::vector<unsigned int> rgbas_found{
          file_uints_colours.get (),
          file_uints_colours.get () + num_verts
      };
      file_uints_colours.reset ();
      extracted_rgbas.swap ( rgbas_found );
      BOOST_LOG_SEV(log,trace)
          << "      Size of colours unsigned int vector = "
          << extracted_rgbas.size ();
    }

    boost::scoped_array<unsigned int> file_uints_indices{ new unsigned int[num_indices] };
    file.read (
        reinterpret_cast<char*>( file_uints_indices.get () ),
        static_cast<std::streamsize>( num_indices ) *
        static_cast<std::streamsize>( sizeof ( unsigned int ) )
    );
    if ( file.bad () || file.fail () )
    {
      BOOST_LOG_SEV(log,error) << "      Failed to read " << num_indices
          << " unsigned int data from file '" << filename << "' !";
      BOOST_LOG_SEV(log,debug) << "  Leave import_b3d_file (value=false)";
      return ( false );
    }
    BOOST_LOG_SEV(log,trace) << "      Indices unsigned int bytes read = " << file.gcount ();
    std::vector<unsigned int> extracted_indices{
        file_uints_indices.get (),
        file_uints_indices.get () + num_indices
    };
    file_uints_indices.reset ();
    BOOST_LOG_SEV(log,trace)
        << "      Size of indices unsigned int vector = "
        << extracted_indices.size ();

    file.close ();
    BOOST_LOG_SEV(log,debug) << "    Closed binary file '" << filename << "'";

    data_3d.vertices.swap ( extracted_vertices );
    data_3d.normals.swap ( extracted_normals );
    data_3d.texture_uvs.swap ( extracted_uvs );
    data_3d.colours_rgba8ui.swap ( extracted_rgbas );
    data_3d.indices.swap ( extracted_indices );
    data_3d.index_type_hint = indices_type_hint;

    BOOST_LOG_SEV(log,debug) << "    Vertices extracted = " << data_3d.vertices.size ();
    BOOST_LOG_SEV(log,debug) << "    Normals extracted = " << data_3d.normals.size ();
    BOOST_LOG_SEV(log,debug) << "    UVs extracted = " << data_3d.texture_uvs.size ();
    BOOST_LOG_SEV(log,debug) << "    Colours extracted = " << data_3d.colours_rgba8ui.size ();
    BOOST_LOG_SEV(log,debug) << "    Indices extracted = "
        << data_3d.indices.size () << " (" << data_3d.indices.size () / 3 << " triangles)";
    BOOST_LOG_SEV(log,debug) << "    Indices type hint = " << static_cast<int>( data_3d.index_type_hint );

    BOOST_LOG_SEV(log,debug) << "  Leave import_b3d_file (value=true)";
    return ( true );
  }

}



///////////////////////////////////////////////////////////////////////////////
// END
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @brief All code for working with %B3D format files of 3D data.
 */
// Local variables:
// mode: c++
// End:
