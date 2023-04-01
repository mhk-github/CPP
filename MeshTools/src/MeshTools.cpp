///////////////////////////////////////////////////////////////////////////////
// FILE     : MeshTools.cpp
// SYNOPSIS : Implementation file for this application.
// LICENSE  : MIT
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// HEADER FILES
///////////////////////////////////////////////////////////////////////////////

// PRECOMPILED HEADER FILE ////////////////////////////////////////////////////
#include "PCH.h"

// LINKER DIRECTIVES //////////////////////////////////////////////////////////
#ifdef _DEBUG
#pragma comment(lib, "assimp-vc142-mtd.lib")
#pragma comment(lib, "zlibstaticd.lib")
#else
#pragma comment(lib, "assimp-vc142-mt.lib")
#pragma comment(lib, "zlibstatic.lib")
#endif /* _DEBUG */

// LOCAL //////////////////////////////////////////////////////////////////////
#include "Colouring.h"
#include "Log.h"


///////////////////////////////////////////////////////////////////////////////
// USING
///////////////////////////////////////////////////////////////////////////////

using namespace boost::log::trivial;


///////////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////////

/**
 * @defgroup EXIT_CODES Exit Codes
 * 
 * @brief The set of exit codes this application can return.
 *
 * @{ 
 */ 

//! The exit code if there is any problem with command line arguments.
const auto EXIT_COMMAND_LINE_ERROR{ 1 };

//! The exit code if there is no material chosen.
const auto EXIT_NO_MATERIAL_ERROR{ 2 };

//! The exit code if an incorrect material is chosen.
const auto EXIT_INCORRECT_MATERIAL_ERROR{ 3 };

//! The exit code if no 3D data files were specified in the command line.
const auto EXIT_NO_DATA_FILES_ERROR{ 4 };

/**
 * @brief The exit code if no 3D data file could be found from a command line
 * argument.
 */
const auto EXIT_NO_DATA_FILE_FOUND_ERROR{ 5 };

//! The exit code if data could not be imported from a file.
const auto EXIT_DATA_IMPORT_ERROR{ 6 };

//! The exit code if no errors were encountered.
const auto EXIT_SUCCESSFUL{ 0 };

/**
 * @} 
 */

//! The file extension for MT files.
#ifdef _DEBUG
const auto* MT_FILE_EXTENSION{ ".mtd" };
#else
const auto* MT_FILE_EXTENSION{ ".mt" };
#endif /* _DEBUG */

//! The process priority to set in Windows.
const auto PROCESS_PRIORITY{ IDLE_PRIORITY_CLASS };

#ifdef __VERTEX_AND_TRIANGLE_LIMITS__
//! The maximum number of vertices processed before a mesh is split.
const auto VERTEX_LIMIT{ 262'144 };

//! The maximum number of triangles processed before a mesh is split.
const auto TRIANGLE_LIMIT{ 786'432 };
#endif /* __VERTEX_AND_TRIANGLE_LIMITS__ */


///////////////////////////////////////////////////////////////////////////////
// DRIVER
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief The application driver function.
 *
 * @param[in] argc The argument count.
 * @param[in] argv The command-line arguments.
 * @param[in] envp The system environment values.
 * @returns An integer exit code.
 *
 * @note \c envp is not used.
 */
int main ( int argc, char* argv[], char* envp[] )
{
  UNREFERENCED_PARAMETER(envp);

  /////////////////////////////////////
  // COMMAND LINE PROCESSING - Start //
  /////////////////////////////////////
  boost::program_options::options_description description{
      "MeshTools [options] file ... "
  };
  bool is_help{ false };
  description.add_options ()
    (
      "help,h",
      boost::program_options::bool_switch ( &is_help ),
      "Display help message\n"
    )
    (
      "log-level,l",
      boost::program_options::value<std::string> ()->default_value ( 
          Log::DEFAULT_LOG_LEVEL 
      ),
      "Logging level\n"
    )
#ifdef __VERTEX_AND_TRIANGLE_LIMITS__
    (
      "triangle-limit,t",
      boost::program_options::value<int> ()->default_value ( TRIANGLE_LIMIT ),
      "Maximum number of triangles before a mesh is split"
    )
    (
      "vertex-limit,v",
      boost::program_options::value<int> ()->default_value ( VERTEX_LIMIT ),
      "Maximum number of vertices before a mesh is split"
    )
#endif /* __VERTEX_AND_TRIANGLE_LIMITS__ */
    (
      "material,m",
      boost::program_options::value<std::string> (),
      "Choose 'gold', 'jade', 'pearl', 'silver', or 'landscape'"
    )
    (
      "file,f",
      boost::program_options::value<std::vector<std::string>> (),
      "Input files of 3D data"
    );

  boost::program_options::positional_options_description positional{};
  positional.add ( "file", -1 );

  boost::program_options::command_line_parser parser{ argc, argv };
  parser.options ( description );
  parser.positional ( positional );
  boost::program_options::variables_map vm{};
  try
  {
    const auto parsed_result{ parser.run () };
    store ( parsed_result, vm );
    notify ( vm );
  }
  catch ( const std::exception& e )
  {
    std::cerr << e.what () << "\n";
    return ( EXIT_COMMAND_LINE_ERROR );
  }

  if ( is_help )
  {
    std::cout << description;
    return ( EXIT_SUCCESSFUL );
  }

  const auto& material_wanted{ vm["material"].as<std::string> () };
  if ( material_wanted.empty () )
  {
    std::cerr << "No material chosen !\n";
    return ( EXIT_NO_MATERIAL_ERROR );
  }

  Colouring::MaterialEnum material_chosen{};
  if ( boost::iequals ( material_wanted, "gold" ) )
  {
    material_chosen = Colouring::MaterialEnum::Gold;
  }
  else if ( boost::iequals ( material_wanted, "jade" ) )
  {
    material_chosen = Colouring::MaterialEnum::Jade;
  }
  else if ( boost::iequals ( material_wanted, "pearl" ) )
  {
    material_chosen = Colouring::MaterialEnum::Pearl;
  }
  else if ( boost::iequals ( material_wanted, "silver" ) )
  {
    material_chosen = Colouring::MaterialEnum::Silver;
  }
  else if ( boost::iequals ( material_wanted, "landscape" ) )
  {
    material_chosen = Colouring::MaterialEnum::Landscape;
  }
  else
  {
    std::cerr << "The material chosen is not valid !\n";
    return ( EXIT_INCORRECT_MATERIAL_ERROR );
  }

  if ( vm["file"].empty () )
  {
    std::cerr << "At least one 3D data file must be given !\n";
    return ( EXIT_NO_DATA_FILES_ERROR );
  }

  const auto& log_level{ vm["log-level"].as<std::string> () };
  if ( !
      (
          boost::iequals(log_level, Log::LOG_LEVEL_TRACE )
          || boost::iequals(log_level, Log::LOG_LEVEL_DEBUG )
          || boost::iequals(log_level, Log::LOG_LEVEL_INFO )
          || boost::iequals(log_level, Log::LOG_LEVEL_WARNING )
          || boost::iequals(log_level, Log::LOG_LEVEL_ERROR )
          || boost::iequals(log_level, Log::LOG_LEVEL_FATAL )
      )
  )
  {
    std::cerr << "Logging level must be one of 'trace', 'debug', 'info', "
        << "'warning', 'error', or 'fatal' ! [default='" 
        << Log::DEFAULT_LOG_LEVEL << "']\n";
  }

#ifdef __VERTEX_AND_TRIANGLE_LIMITS__
  const auto triangle_limit{ vm["triangle-limit"].as<int> () };
  const auto vertex_limit{ vm["vertex-limit"].as<int> () };
#endif /* __VERTEX_AND_TRIANGLE_LIMITS__ */

  const auto& input_files{ vm["file"].as<std::vector<std::string>> () };
  for ( const auto& f : input_files )
  {
    if ( !boost::filesystem::exists ( f ) )
    {
      std::cerr << "3D data input file '" << f << "' does not exist !\n";
      return ( EXIT_NO_DATA_FILE_FOUND_ERROR );
    }
  }
  ///////////////////////////////////
  // COMMAND LINE PROCESSING - End //
  ///////////////////////////////////
  
  ///////////////////////////
  // LOGGING SETUP - Start //
  ///////////////////////////
  Log::init_logging ( log_level );
  Log::Logger logmt{};
  /////////////////////////
  // LOGGING SETUP - End //
  /////////////////////////

  BOOST_LOG_SEV(logmt,info) << "MeshTools - Start";

  const auto process{ GetCurrentProcess () };
  const auto process_id{ GetProcessId ( process ) };
  const auto process_priority_old{ GetPriorityClass ( process ) };
  SetPriorityClass ( process, PROCESS_PRIORITY );
  const auto process_priority_new{ GetPriorityClass ( process ) };
  {
    BOOST_LOG_SEV(logmt,info) << "  Lowered priority of process ID "
        << process_id << " from " << process_priority_old << " to "
        << process_priority_new;
  }

  BOOST_LOG_SEV(logmt,info) << "  3D data input files = [";
  for ( const auto& f : input_files )
  {
    BOOST_LOG_SEV(logmt,info) << "    '" << f << "'";
  }
  BOOST_LOG_SEV(logmt,info) << "  ]";

  // ...
  const std::unique_ptr<
      aiPropertyStore, 
      decltype( 
          []( aiPropertyStore* p_ps ) 
          { 
            aiReleasePropertyStore ( p_ps ); 
          } 
      )
  > p_property_store{ aiCreatePropertyStore () };
  assert( p_property_store.get () != nullptr );

#ifdef __VERTEX_AND_TRIANGLE_LIMITS__
  aiSetImportPropertyInteger (
      p_property_store.get (),
      AI_CONFIG_PP_SLM_VERTEX_LIMIT,
      vertex_limit
  );
  {
    BOOST_LOG_SEV(logmt,debug) << "  Mesh split vertex limit = "
        << vertex_limit;
  }

  aiSetImportPropertyInteger (
      p_property_store.get (),
      AI_CONFIG_PP_SLM_TRIANGLE_LIMIT,
      triangle_limit
  );
  {
    BOOST_LOG_SEV(logmt,debug) << "  Mesh split triangle limit = "
        << triangle_limit;
  }
#endif /* __VERTEX_AND_TRIANGLE_LIMITS__ */
  
  for ( const auto& f : input_files )
  {
    const std::unique_ptr<
        aiScene,
        decltype(
            []( aiScene* p_s )
            {
              aiReleaseImport ( p_s );
            }
        )
    > p_assimp_scene{ 
          const_cast<aiScene*>(
              aiImportFileExWithProperties (
                  f.c_str (),
                  aiProcessPreset_TargetRealtime_Quality,
                  nullptr,
                  p_property_store.get ()
              )
          )
    };
    const auto* p_scene{ p_assimp_scene.get () };
    if (
        p_scene == nullptr
        || p_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
        || p_scene->mRootNode == nullptr
    )
    {
      {
        BOOST_LOG_SEV(logmt,error) << "  ASSIMP: Failed to import file '"
            << f << "' !";
        Assimp::Importer importer{};
        std::string importable{ "" };
        importer.GetExtensionList ( importable );
        BOOST_LOG_SEV(logmt,error) 
            << "    ASSIMP: Supported file extensions = '" << importable 
            << "'";
      }
      return ( EXIT_DATA_IMPORT_ERROR );
    }
    else
    {
      BOOST_LOG_SEV(logmt,info) << "  ASSIMP: Successfully imported file '"
          << f << "'";
    }

    const auto num_meshes{ p_scene->mNumMeshes };
    {
      BOOST_LOG_SEV(logmt,debug) << "  Number of meshes = " << num_meshes;
    }
    for ( auto i { 0U }; i < num_meshes; ++i )
    {
      {
        BOOST_LOG_SEV(logmt,debug) << "  Mesh " << i << ":";
      }
      const auto* p_mesh{ p_scene->mMeshes[i] };
      const size_t numVertices{ p_mesh->mNumVertices };
      {
        BOOST_LOG_SEV(logmt,debug) << "    Number of vertices = "
            << numVertices;
      }

      //MHK: HACK - Start
      std::vector<float> texture_uvs(
          numVertices * 2,
          0.0f
      );
      //MHK: HACK - End

      const auto num_floats{ numVertices * 3 };
      std::vector<float> vertices{};
      vertices.reserve ( num_floats );
      {
        BOOST_LOG_SEV(logmt,debug) << "    Created vertices vector of "
            << num_floats << " float";
      }
      std::vector<float> vertex_normals{};
      vertex_normals.reserve ( num_floats );
      {
        BOOST_LOG_SEV(logmt,debug) << "    Created normals vector of "
            << num_floats << " float";
      }
      std::vector<Colouring::material_struct> colouring{};
      colouring.reserve ( numVertices );
      {
        BOOST_LOG_SEV(logmt,debug) << "    Created colouring vector of "
            << numVertices << " Colouring::material";
      }

      const auto* p_vertices{ p_mesh->mVertices };
      const auto* p_normals{ p_mesh->mNormals };
      for ( size_t j{ 0 }; j < numVertices; ++j )
      {
        const auto& v{ p_vertices[j] };
        vertices.emplace_back ( v.x );
        vertices.emplace_back ( v.y );
        vertices.emplace_back ( v.z );
        const auto& n{ p_normals[j] };
        vertex_normals.emplace_back ( n.x );
        vertex_normals.emplace_back ( n.y );
        vertex_normals.emplace_back ( n.z );

        // Facilitate creation of a jump table by using an enum class.
        switch ( material_chosen )
        {
          case Colouring::MaterialEnum::Gold:
            {
              colouring.emplace_back ( Colouring::GOLD );
            }
            break;

          case Colouring::MaterialEnum::Jade:
            {
              colouring.emplace_back ( Colouring::JADE );
            }
            break;

          case Colouring::MaterialEnum::Pearl:
            {
              colouring.emplace_back ( Colouring::PEARL );
            }
            break;

          case Colouring::MaterialEnum::Silver:
            {
              colouring.emplace_back ( Colouring::SILVER );
            }
            break;

          case Colouring::MaterialEnum::Landscape:
            {
              colouring.emplace_back (
                  Colouring::material_struct{ 
                      Colouring::LANDSCAPE_AMBIENT_COLOUR, 
                      Colouring::landscape_diffuse_colour ( v.y ), 
                      Colouring::LANDSCAPE_SPECULAR_COLOUR, 
                      Colouring::LANDSCAPE_SHININESS 
                  }
              );
            }
            break;

          // NOTE: A default is not coded deliberately.
        }
      }
      {
        BOOST_LOG_SEV(logmt,debug)
            << "    Vertex, vertex normal, and colouring data loaded";
      }

      const size_t numFaces{ p_mesh->mNumFaces };
      {
        BOOST_LOG_SEV(logmt,debug) << "    Number of faces = " << numFaces;
      }
      const auto numIndices{ numFaces * 3 };
      {
        BOOST_LOG_SEV(logmt,debug) << "    Number of indices = "
            << numFaces * 3;
      }
      std::vector<unsigned int> indices{};
      indices.reserve ( numIndices );
      {
        BOOST_LOG_SEV(logmt,debug) << "    Created indices vector of "
            << numIndices << " unsigned int";
      }
      const auto* p_faces{ p_mesh->mFaces };
      for ( size_t j{ 0 }; j < numFaces; ++j )
      {
        const auto& current_face{ p_faces[j] };
        assert( current_face.mNumIndices == 3U );
        const auto* p_current_face_indices{ current_face.mIndices };
        indices.emplace_back ( *p_current_face_indices );
        indices.emplace_back ( *( p_current_face_indices + 1 ) );
        indices.emplace_back ( *( p_current_face_indices + 2 ) );
      }

      const size_t vertices_bytes{ vertices.size () * sizeof ( float ) };
      const size_t vertex_normals_bytes{ 
          vertex_normals.size () * sizeof ( float ) 
      };
      const size_t texture_uvs_bytes{ 
          texture_uvs.size () * sizeof ( float ) 
      };
      const size_t colouring_bytes{ 
          colouring.size () * sizeof ( Colouring::material_struct) 
      };
      const size_t indices_bytes{ indices.size () * sizeof ( unsigned int ) };
      std::stringstream ss_file_name{};
      ss_file_name << f << "." << i << "." << numVertices << "." << numIndices 
          << MT_FILE_EXTENSION;
      const auto file_name{ ss_file_name.str () };
      std::ofstream output_file{
          file_name, 
          std::ios_base::out | std::ios_base::binary 
      };
      {
        BOOST_LOG_SEV(logmt,info) << "    Opened output file '" << file_name
            << "'";
      }

      output_file.write (
          reinterpret_cast<char*>( vertices.data () ), 
          vertices_bytes
      );
      {
        BOOST_LOG_SEV(logmt,debug) << "      Wrote " << vertices_bytes
            << " bytes for vertex data";
      }
      
      output_file.write ( 
          reinterpret_cast<char*>( vertex_normals.data () ), 
          vertex_normals_bytes
      );
      {
        BOOST_LOG_SEV(logmt,debug) << "      Wrote " << vertex_normals_bytes
            << " bytes for vertex normal data";
      }

      output_file.write ( 
          reinterpret_cast<char*>( texture_uvs.data () ), 
          texture_uvs_bytes
      );
      {
        BOOST_LOG_SEV(logmt,debug) << "      Wrote " << texture_uvs_bytes
            << " bytes for texture UV data";
      }

      output_file.write (
          reinterpret_cast<char*>( colouring.data () ),
          colouring_bytes
      );
      {
        BOOST_LOG_SEV(logmt,debug) << "      Wrote " << colouring_bytes
          << " bytes for colouring data";
      }

      output_file.write ( 
          reinterpret_cast<char*>( indices.data () ), 
          indices_bytes
      );
      {
        BOOST_LOG_SEV(logmt,debug) << "      Wrote " << indices_bytes
            << " bytes for index data";
      }

      output_file.close ();
      {
        BOOST_LOG_SEV(logmt,info) << "    Closed output file '" << file_name
            << "'";
      }

      //...
    }
  }

  // ...

  BOOST_LOG_SEV(logmt,info) << "MeshTools - End";

  return ( EXIT_SUCCESSFUL );
}


///////////////////////////////////////////////////////////////////////////////
// END
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @brief Implementation file for this application.
 *
 * @author Mohammad Haroon Khaliq
 * @date @showdate "%d %B %Y"
 * @copyright MIT License.
 */
/**
 * @mainpage MeshTools
 *
 * @section SYNOPSIS Synopsis
 *
 * A utility to convert 3D data files with the following file extensions into MT
 * data files :
 *
 * - 3d
 * - 3ds
 * - 3mf
 * - ac
 * - ac3d
 * - acc
 * - amf
 * - ase
 * - ask
 * - assbin
 * - b3d
 * - blend
 * - bsp
 * - bvh
 * - cob
 * - csm
 * - dae
 * - dxf
 * - enff
 * - fbx
 * - glb
 * - gltf
 * - hmp
 * - ifc
 * - ifczip
 * - iqm
 * - irr
 * - irrmesh
 * - lwo
 * - lws
 * - lxo
 * - md2
 * - md3
 * - md5anim
 * - md5camera
 * - md5mesh
 * - mdc
 * - mdl
 * - mesh
 * - mesh.xml
 * - mot
 * - ms3d
 * - ndo
 * - nff
 * - obj
 * - off
 * - ogex
 * - pk3
 * - ply
 * - pmx
 * - prj
 * - q3o
 * - q3s
 * - raw
 * - scn
 * - sib
 * - smd
 * - step
 * - stl
 * - stp
 * - ter
 * - uc
 * - vta
 * - x
 * - x3d
 * - x3db
 * - xgl
 * - xml
 * - zae
 * - zgl
 *
 * @section MT_FILE_FORMAT MT File Format
 *
 * This is a binary 3D data file format with all numerical values in
 * little-endian form. Triangles are the only polygon catered for, and
 * counter-clockwise winding order is used to determine if they face forward.
 *
 * MT files \b must have file extension <tt>.&alpha;.&beta;.mt</tt> where :
 * - \c &alpha; = positive integer number of vertices
 * - \c &beta; = positive integer number of vertex indices
 *
 * @internal @note Development MT files end with \c .mtd not \c .mt !
 *
 * The file is separated internally into the following sections :
 * -# <tt>3&alpha;</tt> \c float for the vertex \c x , \c y , and \c z
 * components
 * -# <tt>3&alpha;</tt> \c float for the vertex normal \c x , \c y , and \c z
 * components
 * -# <tt>2&alpha;</tt> \c float for the texture \c U and \c V values
 * -# <tt>13&alpha;</tt> \c float for vertex material lighting data
 * -# <tt>&beta;</tt> <tt>unsigned int</tt> for vertex indices
 *
 * @sa Colouring::rgba_struct 
 * @sa Colouring::material_struct
 */
/**
 * @defgroup STRUCT C-style Structs
 *
 * @brief C-style structs used in this application.
 */
// Local variables:
// mode: c++
// End:
