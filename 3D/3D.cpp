///////////////////////////////////////////////////////////////////////////////
// FILE     : 3D.cpp
// SYNOPSIS : Template file for all 3D coding prototypes.
// LICENSE  : MIT 
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// HEADER FILES
///////////////////////////////////////////////////////////////////////////////

// PRECOMPILED HEADER FILE ////////////////////////////////////////////////////

#include "PCH.h"

// LINKER DIRECTIVES //////////////////////////////////////////////////////////

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "OpenGL32.lib")

#pragma comment(lib, "boost_filesystem-vc142-mt-x32-1_77.lib")
#pragma comment(lib, "boost_log_setup-vc142-mt-x32-1_77.lib")
#pragma comment(lib, "boost_log-vc142-mt-x32-1_77.lib")

#ifdef _DEBUG
#pragma comment(lib, "glad_mt_Debug_x86_0_1_34.lib")
#else
#pragma comment(lib, "glad_mt_x86_0_1_34.lib")
#endif /* _DEBUG */


// LOCAL //////////////////////////////////////////////////////////////////////

#include "Configuration.h"
#include "B3D.h"



///////////////////////////////////////////////////////////////////////////////
// LOGGING SYSTEM
///////////////////////////////////////////////////////////////////////////////

using namespace boost::log::trivial;
Log::Logger log3d;



///////////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////////

// Related to command line processing
const auto MIN_ARGS{ 2 };
const auto MAX_ARGS{ 3 };
const auto INPUT_FILE_POSITION{ 1 };
const auto INI_FILE_POSITION{ 2 };

// Related to creating animations
const auto TOTAL_DEGREES{ 360.0f };

// Related to creating colour values in the range [0.0,1.0]
const auto COLOUR_DIVISOR{ 256.0f };

// Related to setting the title do the 3D display window
const auto* GLAD_VERSION_STRING{ L"0.1.34" };
#ifdef _DEBUG
const auto* BASE_TITLE{ "3D [Debug] " };
#else
const auto* BASE_TITLE{ "3D " };
#endif /* _DEBUG */
static_assert(
    std::is_same<decltype( BASE_TITLE ), const char*>::value,
    L"GLFW only works with type char !"
);
const auto INT_PAD_WIDTH{ 4 };
const auto FLOAT_PRECISION{ 3 };

// Related to the process priority to set
const auto PROCESS_PRIORITY{ HIGH_PRIORITY_CLASS };

// Related to frame rate calculation
const auto MS_PER_S{ 1000.0 };



///////////////////////////////////////////////////////////////////////////////
// SHADERS
///////////////////////////////////////////////////////////////////////////////

// VERTEX /////////////////////////////////////////////////////////////////////

const auto *SPINNING_OBJECT_VERTEX_SHADER_SOURCE{
"#version 450 core                                                          \n"
"                                                                           \n"
"in vec4 position;                                                          \n"
"                                                                           \n"
"out VS_OUT                                                                 \n"
"{                                                                          \n"
"    vec4 color;                                                            \n"
"} vs_out;                                                                  \n"
"                                                                           \n"
"uniform mat4 mvp_matrix;                                                   \n"
"                                                                           \n"
"void main(void)                                                            \n"
"{                                                                          \n"
"    gl_Position = mvp_matrix * position;                                   \n"
"    vs_out.color = position * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);              \n"
"}                                                                          \n" 
};


// FRAGMENT ///////////////////////////////////////////////////////////////////

const auto *SPINNING_OBJECT_FRAGMENT_SHADER_SOURCE{
"#version 450 core                                                          \n"
"                                                                           \n"
"out vec4 color;                                                            \n"
"                                                                           \n"
"in VS_OUT                                                                  \n"
"{                                                                          \n"
"    vec4 color;                                                            \n"
"} fs_in;                                                                   \n"
"                                                                           \n"
"void main(void)                                                            \n"
"{                                                                          \n"
"    color = fs_in.color;                                                   \n"
"}                                                                          \n"
};



///////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
///////////////////////////////////////////////////////////////////////////////

void error_callback ( int error_code, const char *description )
{
  BOOST_LOG_SEV(log3d,error) << "  Error = " << error_code << ", Description = '" << description << "'";
}


void key_callback ( GLFWwindow *window, int key, int scancode, int action, int mods )
{
  if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
  {
    BOOST_LOG_SEV(log3d,debug) << "  GLFW: 'Esc' key pressed";
    glfwSetWindowShouldClose ( window, GLFW_TRUE );
  }
}



///////////////////////////////////////////////////////////////////////////////
// DRIVER
///////////////////////////////////////////////////////////////////////////////

int wmain ( int argc, wchar_t *argv[], wchar_t *envp[] )
{
  if ( argc < MIN_ARGS || argc > MAX_ARGS )
  {
    std::wcerr << "A B3D binary input file must be provided and then an optional INI file !\n";
    return ( 1 );
  }

  const std::wstring input_file{ argv[INPUT_FILE_POSITION] };
  if ( !boost::filesystem::exists ( input_file ) )
  {
    std::wcerr << "3D data input file '" << input_file << "' does not exist !\n";
    return ( 2 );
  }
  else if ( !boost::iends_with ( input_file, L".b3d" ) )
  {
    std::wcerr << "Unknown 3D data file type '" << input_file << "' where B3D file (*.b3d) expected !\n";
    return ( 3 );
  }

  Config::configuration cfg{};
  const std::wstring ini_file{ ( argc == MAX_ARGS ) ? argv[INI_FILE_POSITION] : L"" };
  if ( !ini_file.empty () )
  {
    if ( !boost::filesystem::exists ( ini_file ) )
    {
      std::wcerr << "The configuration file '" << ini_file << "' does not exist !\n";
      return ( 4 );  
    }

    if ( !init_configuration ( ini_file, cfg ) )
    {
      std::wcerr << "Failed to load configuration data from file '" << ini_file << "' !\n";
      return ( 5 );
    } 
  }

  const Log::LogSetup log_cfg{ 
      cfg.application_identifier, 
      cfg.log_directory, 
      cfg.logging_level
  };
  Log::init_logging ( log_cfg );

  BOOST_LOG_SEV(log3d,info) << "3D - start";
  BOOST_LOG_SEV(log3d,debug) << "  3D data input file = '" << input_file << "'";
  if ( !ini_file.empty () )
  {
    BOOST_LOG_SEV(log3d,debug) << "  INI configuration file = '" << ini_file << "'";
  }
  BOOST_LOG_SEV(log3d,debug) << "  Configured values:";
  BOOST_LOG_SEV(log3d,debug) << "    Application identifier = '" << cfg.application_identifier << "'";
  BOOST_LOG_SEV(log3d,debug) << "    Logging level = '" << cfg.logging_level << "'";
  BOOST_LOG_SEV(log3d,debug) << "    Log directory = '" << cfg.log_directory << "'";
  BOOST_LOG_SEV(log3d,debug) << "    Window width = " << cfg.window_width;
  BOOST_LOG_SEV(log3d,debug) << "    Window height = " << cfg.window_height;
  BOOST_LOG_SEV(log3d,debug) << "    Perspective projection FOV = " << cfg.perspective_projection_fov_degrees << " degrees";
  BOOST_LOG_SEV(log3d,debug) << "    Perspective projection near plane = " << cfg.perspective_projection_near_plane;
  BOOST_LOG_SEV(log3d,debug) << "    Perspective projection far plane = " << cfg.perspective_projection_far_plane;
  BOOST_LOG_SEV(log3d,debug) << "    View transformation X = " << cfg.view_transformation_x;
  BOOST_LOG_SEV(log3d,debug) << "    View transformation Y = " << cfg.view_transformation_y;
  BOOST_LOG_SEV(log3d,debug) << "    View transformation Z = " << cfg.view_transformation_z;
  BOOST_LOG_SEV(log3d,debug) << "    Translation divisor = " << cfg.translation_divisor;
  BOOST_LOG_SEV(log3d,debug) << "    Rotation divisor = " << cfg.rotation_divisor;
  BOOST_LOG_SEV(log3d,debug) << "    Clear colour red = " << cfg.clear_colour_red;
  BOOST_LOG_SEV(log3d,debug) << "    Clear colour green = " << cfg.clear_colour_green;
  BOOST_LOG_SEV(log3d,debug) << "    Clear colour blue = " << cfg.clear_colour_blue;
  BOOST_LOG_SEV(log3d,debug) << "    Clear colour alpha = " << cfg.clear_colour_alpha;

  Data3D::D3D data3D{};
  if ( !B3D::import_b3d_file ( input_file, data3D ) )
  {
    BOOST_LOG_SEV(log3d,fatal) << "  import_b3d_file('" << input_file << "',...) returned false !";
    return ( 6 );
  }
  BOOST_LOG_SEV(log3d,info) << "  Successfully loaded 3D data from B3D input file '" << input_file << "'";

  BOOST_LOG_SEV(log3d,debug) << "  GLAD: version " << GLAD_VERSION_STRING;
  BOOST_LOG_SEV(log3d,debug) << "  " << GLM_VERSION_MESSAGE;
  BOOST_LOG_SEV(log3d,debug) << "  BOOST: version " << BOOST_LIB_VERSION;
  auto glfw_major{ 0 };
  auto glfw_minor{ 0 };
  auto glfw_revision{ 0 };
  glfwGetVersion ( &glfw_major, &glfw_minor, &glfw_revision );
  BOOST_LOG_SEV(log3d,debug) << "  GLFW: version " << glfw_major << "." << glfw_minor << "." << glfw_revision;
  BOOST_LOG_SEV(log3d,trace) << "    GLFW: version string = '" << glfwGetVersionString () << "'";

  glfwSetErrorCallback ( error_callback );
  BOOST_LOG_SEV(log3d,info) << "  GLFW: glfwSetErrorCallback(...) succeeded";
  if ( !glfwInit () )
  {
    BOOST_LOG_SEV(log3d,fatal) << "  GLFW: glfwInit() failed !";
    return ( 7 );
  } 
  BOOST_LOG_SEV(log3d,info) << "  GLFW: glfwInit() succeeded";

  auto *window{ glfwCreateWindow ( cfg.window_width, cfg.window_height, BASE_TITLE, nullptr, nullptr ) };
  if ( !window )
  {
    glfwTerminate ();
    BOOST_LOG_SEV(log3d,debug) << "  GLFW: glfwTerminate() succeeded";
    BOOST_LOG_SEV(log3d,fatal) << "  GLFW: glfwCreateWindow(" 
        << cfg.window_width << "," << cfg.window_height << ",'" << BASE_TITLE 
        << "',...) failed !";
    return ( 8 );
  }
  BOOST_LOG_SEV(log3d,info) << "  GLFW: glfwCreateWindow(" << cfg.window_width 
      << "," << cfg.window_height << ",'" << BASE_TITLE << "',...) succeeded";

  glfwSetKeyCallback ( window, key_callback );
  BOOST_LOG_SEV(log3d,info) << "  GLFW: glfwSetKeyCallback(...) succeeded - Key control ready";
  glfwMakeContextCurrent ( window );
  BOOST_LOG_SEV(log3d,info) << "  GLFW: glfwMakeContextCurrent(...) succeeded - OpenGL context of this window on this thread";
  gladLoadGLLoader ( reinterpret_cast<GLADloadproc>( glfwGetProcAddress ) );
  BOOST_LOG_SEV(log3d,info) << "  GLAD: gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) succeeded - OpenGL ready";
  glfwSwapInterval ( 1 );
  BOOST_LOG_SEV(log3d,info) << "  GLFW: glfwSwapInterval(1) succeeded - VSync set";

  // Query the OpenGL system
  BOOST_LOG_SEV(log3d,debug) << "  OPENGL: Vendor = " << glGetString ( GL_VENDOR );
  BOOST_LOG_SEV(log3d,debug) << "  OPENGL: Renderer = " << glGetString ( GL_RENDERER );
  BOOST_LOG_SEV(log3d,debug) << "  OPENGL: Version string = '" << glGetString ( GL_VERSION ) << "'";
  GLint gl_major_version{};
  glGetIntegerv( GL_MAJOR_VERSION, &gl_major_version );
  BOOST_LOG_SEV(log3d,trace) << "    OPENGL: Major version = " << gl_major_version;
  GLint gl_minor_version{};
  glGetIntegerv( GL_MINOR_VERSION, &gl_minor_version );
  BOOST_LOG_SEV(log3d,trace) << "    OPENGL: Minor version = " << gl_minor_version;
  BOOST_LOG_SEV(log3d,trace) << "  OPENGL: Shading language version = " 
      << glGetString ( GL_SHADING_LANGUAGE_VERSION );
  GLint exts{};
  glGetIntegerv ( GL_NUM_EXTENSIONS, &exts );
  BOOST_LOG_SEV(log3d,debug) << "  OPENGL: Extensions = " << exts;
  for ( auto i{ 0 }; i < exts; ++i )
  {
    BOOST_LOG_SEV(log3d,trace) << "    OPENGL: (" 
        << std::setw ( INT_PAD_WIDTH ) << i + 1 << ") " 
        << glGetStringi ( GL_EXTENSIONS, i );
  }

  // Query the monitor
  auto *primary_monitor{ glfwGetPrimaryMonitor () };
  if ( primary_monitor == nullptr )
  {
    glfwTerminate ();
    BOOST_LOG_SEV(log3d,debug) << "  GLFW: glfwTerminate() succeeded";
    BOOST_LOG_SEV(log3d,fatal) << "  GLFW: glfwGetPrimaryMonitor() returned NULL !";
    return ( 9 );
  }
  auto num_modes{ 0 };
  const auto *video_modes{ glfwGetVideoModes ( primary_monitor, &num_modes ) };
  if ( num_modes == 0  )
  {
    glfwTerminate ();
    BOOST_LOG_SEV(log3d,debug) << "  GLFW: glfwTerminate() succeeded";
    BOOST_LOG_SEV(log3d,fatal) << "  GLFW: glfwGetVideoModes(...) returned 0 !";
    return ( 10 );
  }
  BOOST_LOG_SEV(log3d,debug) << "  GLFW: Primary monitor modes detected = " << num_modes;
  for ( auto i{ 0 }; i < num_modes; ++i )
  {
    const auto &video_mode{ *( video_modes + i ) };
    BOOST_LOG_SEV(log3d,trace) << "    GLFW: Mode " << i << ":";
    BOOST_LOG_SEV(log3d,trace) << "      GLFW: Width = " << video_mode.width;
    BOOST_LOG_SEV(log3d,trace) << "      GLFW: Height = " << video_mode.height;
    BOOST_LOG_SEV(log3d,trace) << "      GLFW: Red bits = " << video_mode.redBits;
    BOOST_LOG_SEV(log3d,trace) << "      GLFW: Green bits = " << video_mode.greenBits;
    BOOST_LOG_SEV(log3d,trace) << "      GLFW: Blue bits = " << video_mode.blueBits;
    BOOST_LOG_SEV(log3d,trace) << "      GLFW: Refresh rate = " << video_mode.refreshRate << " Hz";
  }

  // Save the windowed video mode
  const auto *current_video_mode{ glfwGetVideoMode ( primary_monitor ) };
  if ( current_video_mode == nullptr )
  {
    glfwTerminate ();
    BOOST_LOG_SEV(log3d,debug) << "  GLFW: glfwTerminate() succeeded";
    BOOST_LOG_SEV(log3d,fatal) << "  GLFW: glfwGetVideoMode(...) returned NULL !";
    return ( 11 );
  }
  const auto windowed_video_mode{ *current_video_mode };
  BOOST_LOG_SEV(log3d,debug) << "  GLFW: Details of current windowed video mode on primary monitor:";
  BOOST_LOG_SEV(log3d,debug) << "    GLFW: Width = " << windowed_video_mode.width;
  BOOST_LOG_SEV(log3d,debug) << "    GLFW: Height = " << windowed_video_mode.height;
  BOOST_LOG_SEV(log3d,debug) << "    GLFW: Red bits = " << windowed_video_mode.redBits;
  BOOST_LOG_SEV(log3d,debug) << "    GLFW: Green bits = " << windowed_video_mode.greenBits;
  BOOST_LOG_SEV(log3d,debug) << "    GLFW: Blue bits = " << windowed_video_mode.blueBits;
  const auto refresh_rate{ windowed_video_mode.refreshRate };
  BOOST_LOG_SEV(log3d,debug) << "    GLFW: Refresh rate = " << refresh_rate << " Hz";

  // OPENGL: Create a VAO (Vertex Array Object) and bind it to the current context
  GLuint vao{};
  glGenVertexArrays ( 1, &vao );
  BOOST_LOG_SEV(log3d,debug ) << "  OPENGL: Created VAO " << vao;
  glBindVertexArray ( vao );
  BOOST_LOG_SEV(log3d,debug) << "    OPENGL: Bound VAO " << vao;

  // OPENGL: Create a VBO (Vertex Buffer Object) and bind it to the current context
  GLuint vbo{};
  glGenBuffers ( 1, &vbo );
  BOOST_LOG_SEV(log3d,debug) << "  OPENGL: Created VBO " << vbo;
  glBindBuffer ( GL_ARRAY_BUFFER, vbo );
  BOOST_LOG_SEV(log3d,debug) << "    OPENGL: Bound VBO " << vbo;

  // OpenGL: Fill the VBO
  const auto vbo_size_bytes_ptr{ static_cast<GLsizeiptr>( data3D.vertices.size () * sizeof ( glm::vec3 ) ) };
  glNamedBufferData (
      vbo,
      vbo_size_bytes_ptr,
      &data3D.vertices[0],
      GL_STATIC_DRAW
  );
  BOOST_LOG_SEV(log3d,debug) << "  OPENGL: Filled bound VBO " << vbo 
      << " with " << data3D.vertices.size () * sizeof ( glm::vec3 ) << " bytes";

  // OPENGL: Connect shader 'in' variables to a vertex attribut array
  glVertexAttribPointer ( 
      0,        // GLuint    - zero-based index of shader attribute location
      3,        // GLint     - number of components to be updated per vertex (1,2,3,4)
      GL_FLOAT, // GLenum    - the data type
      GL_FALSE, // GLboolean - true id normalization wanted before storage
      0,        // GLsizei   - byte offset between consecutive elements in the array (0=tightly packed)
      0         // GLvoid *  - offest from start of the buffer object
  );
  BOOST_LOG_SEV(log3d,debug) << "  OPENGL: Set vertex attribute pointer";
 
  // OPENGL: Enable the identified vertex array attribute in the context
  glEnableVertexAttribArray ( 
      0 // GLuint - id of vertex array 
  );
  BOOST_LOG_SEV(log3d,debug) << "    OPENGL: Enabled vertex attribute array";

  // OPENGL: Create an IBO (Index Buffer Object) and bind it to the current context
  GLuint ibo{};
  glGenBuffers ( 1, &ibo );
  BOOST_LOG_SEV(log3d,debug) << "  OPENGL: Created IBO " << ibo;
  glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, ibo );
  BOOST_LOG_SEV(log3d,debug) << "    OPENGL: Bound IBO " << ibo;

  // OpenGL: Fill the IBO
  const auto index_buffer_size_ptr{ static_cast<GLsizeiptr>( data3D.indices.size () * sizeof ( unsigned int ) ) };
  glNamedBufferData (
      ibo,
      index_buffer_size_ptr,
      &data3D.indices[0],
      GL_STATIC_DRAW
  );
  const auto index_buffer_size{ static_cast<GLsizei>( data3D.indices.size () * sizeof ( unsigned int ) ) };
  BOOST_LOG_SEV(log3d,debug) << "  OPENGL: Filled bound IBO " << ibo << " with " << index_buffer_size << " bytes";

  // OpenGL: Cull polygons based on their winding in window coordinates (default: CCW winding is front-facing)
  glEnable ( GL_CULL_FACE );
  BOOST_LOG_SEV(log3d,debug) << "  OPENGL: GL_CULL_FACE enabled";

  // OpenGL: DO depth comparisons
  glEnable ( GL_DEPTH_TEST );
  BOOST_LOG_SEV(log3d,debug) << "  OPENGL: GL_DEPTH_TEST enabled";

  // OpenGL: Passes if the incoming value is less than or equal to the stored depth value
  glDepthFunc ( GL_LEQUAL );
  BOOST_LOG_SEV(log3d,debug) << "  OPENGL: GL_LEQUAL set for depth function";

  // OPENGL: Create the vertex shader
  const auto vertex_shader{ glCreateShader ( GL_VERTEX_SHADER ) };
  BOOST_LOG_SEV(log3d,debug) << "  OPENGL: Created vertex shader " << vertex_shader;
  glShaderSource ( vertex_shader, 1, &SPINNING_OBJECT_VERTEX_SHADER_SOURCE, nullptr );
  BOOST_LOG_SEV(log3d,debug) << "    OPENGL: Set vertex shader " 
      << vertex_shader << " source to " 
      << &SPINNING_OBJECT_VERTEX_SHADER_SOURCE;
  glCompileShader ( vertex_shader );
  BOOST_LOG_SEV(log3d,debug) << "    OPENGL: Compiled vertex shader " << vertex_shader;

  // OPENGL: Create the fragment shader
  const auto fragment_shader{ glCreateShader ( GL_FRAGMENT_SHADER ) };
  BOOST_LOG_SEV(log3d,debug) << "  OPENGL: Created fragment shader " 
      << fragment_shader;
  glShaderSource ( fragment_shader, 1, &SPINNING_OBJECT_FRAGMENT_SHADER_SOURCE, nullptr );
  BOOST_LOG_SEV(log3d,debug) << "    OPENGL: Set fragment shader " 
      << fragment_shader << " source to "
      << &SPINNING_OBJECT_FRAGMENT_SHADER_SOURCE;
  glCompileShader ( fragment_shader );
  BOOST_LOG_SEV(log3d,debug) << "    OPENGL: Compiled fragment shader " << fragment_shader;

  // OPENGL: Compile and link the shaders into a program
  const auto shader_program{ glCreateProgram () };
  BOOST_LOG_SEV(log3d,debug) << "  OPENGL: Created program " << shader_program;
  glAttachShader ( shader_program, vertex_shader );
  BOOST_LOG_SEV(log3d,debug) << "    OPENGL: Attached vertex shader " 
      << vertex_shader << " to program " << shader_program;
  glAttachShader ( shader_program, fragment_shader );
  BOOST_LOG_SEV(log3d,debug) << "    OPENGL: Attached fragment shader " 
      << fragment_shader << " to program " << shader_program;
  glLinkProgram ( shader_program );
  BOOST_LOG_SEV(log3d,debug) << "    OPENGL: Linked program " 
      << shader_program;

  // OPENGL: Remove the shaders as they are in the program now
  glDeleteShader ( fragment_shader );
  BOOST_LOG_SEV(log3d,debug) << "  OPENGL: Deleted fragment shader " << fragment_shader;
  glDeleteShader( vertex_shader);
  BOOST_LOG_SEV(log3d,debug) << "  OPENGL: Deleted vertex shader " << vertex_shader;

  // Identify the model view projection 4x4 matrix in the vertex shader
  const auto mvp_matrix_location{ glGetUniformLocation ( shader_program, "mvp_matrix" ) };
  BOOST_LOG_SEV(log3d,debug) << "  OPENGL: Set identifier of MVP matrix in vertex shader to " << mvp_matrix_location;

  // Start using the program
  glUseProgram ( shader_program );
  BOOST_LOG_SEV(log3d,debug) << "  OPENGL: Using program " << shader_program;

  // Set the background colour
  const auto background_red{ cfg.clear_colour_red / COLOUR_DIVISOR };
  const auto background_green{ cfg.clear_colour_green / COLOUR_DIVISOR };
  const auto background_blue{ cfg.clear_colour_blue / COLOUR_DIVISOR };
  glClearColor  ( 
      background_red,
      background_green,
      background_blue,
      cfg.clear_colour_alpha
  );
  BOOST_LOG_SEV(log3d,debug) 
      << "  OPENGL: Background colour: " 
      << "red=" << background_red << " (R=" << cfg.clear_colour_red << "), "
      << "green=" << background_green << " (G=" << cfg.clear_colour_green << "), "
      << "blue=" << background_blue << " (B=" << cfg.clear_colour_blue << "), "
      << "alpha=" << cfg.clear_colour_alpha;

  const auto translation_step{ TOTAL_DEGREES / cfg.translation_divisor };
  BOOST_LOG_SEV(log3d,debug) << "  Translation step = " << translation_step;
  const auto rotation_step{ TOTAL_DEGREES / cfg.rotation_divisor };
  BOOST_LOG_SEV(log3d,debug) << "  Rotation step = " << rotation_step;

  const auto process{ GetCurrentProcess () };
  const auto process_id{ GetProcessId ( process ) };
  const auto process_priority_old{ GetPriorityClass ( process ) };
  SetPriorityClass ( process, PROCESS_PRIORITY );
  const auto process_priority_new{ GetPriorityClass ( process ) };
  BOOST_LOG_SEV(log3d,debug) << "  Raised priority of process ID " 
      << process_id << " from " << process_priority_old << " to " 
      << process_priority_new;
  
  BOOST_LOG_SEV(log3d,info) << "  Entering main loop";
  auto translation_ctr{ 0 };
  auto rotation_ctr{ 0 };
  auto width{ 0 };
  auto height{ 0 };
  LARGE_INTEGER perf_ctr_freq{};
  QueryPerformanceFrequency ( &perf_ctr_freq );
  const auto ticks_per_s{ perf_ctr_freq.QuadPart };
  const auto ticks_per_ms{ ticks_per_s / MS_PER_S };
  BOOST_LOG_SEV(log3d,debug) << "    Performance counter frequency = " 
      << ticks_per_s << " ticks per second (" << ticks_per_ms 
      << " ticks per millisecond)";
  auto frame_count{ 0 };
  LARGE_INTEGER perf_ctr_now{};
  LARGE_INTEGER perf_ctr_last{};
  QueryPerformanceCounter ( &perf_ctr_last );
  while ( !glfwWindowShouldClose ( window ) )
  {
    glfwGetFramebufferSize ( window, &width, &height );
    
    // OpenGL: Set the viewport
    glViewport ( 0, 0, width, height );

    // OpenGL: Clear the colour buffer and the depth buffer
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Model M (I) undergoes the projection then the view transformation Tv ...
    const auto aspect{ ( width / static_cast<float>( height ) ) };
    const auto projection_matrix4x4{ 
        glm::perspective ( 
            glm::radians ( cfg.perspective_projection_fov_degrees ), 
            aspect, 
            cfg.perspective_projection_near_plane,
            cfg.perspective_projection_far_plane
        ) 
    };
    const auto MPTv_matrix4x4{ 
        glm::translate ( 
            projection_matrix4x4,
            glm::vec3{ 
                cfg.view_transformation_x, 
                cfg.view_transformation_y,
                cfg.view_transformation_z
            }
        ) 
    };

    // RST - Translations happen third
    ++translation_ctr;
    translation_ctr %= cfg.translation_divisor;
    const auto translation_radians{ 
        glm::radians ( translation_ctr * translation_step )
    };
    const auto MPTvT1_matrix4x4{
        glm::translate ( 
            MPTv_matrix4x4,
            glm::vec3{
                glm::sin ( translation_radians ),
                glm::cos ( translation_radians ),
                glm::sin ( translation_radians ),
            } 
        )
    };

    // RST - Scalings happen second

    // RST - Rotations happen first
    ++rotation_ctr;
    rotation_ctr %= cfg.rotation_divisor;
    const auto rotate_amount{ rotation_ctr * rotation_step };
    const auto roll{ rotate_amount };
    const auto yaw{ rotate_amount };
    const auto pitch{ rotate_amount };
    const glm::vec3 euler_angles{ roll, yaw, pitch };
    const glm::quat quaternion{ euler_angles };
    const auto rotation_matrix{ glm::toMat4 ( quaternion ) };
    const auto MPTvT1R1_matrix4x4{ MPTvT1_matrix4x4 * rotation_matrix };

    // OpenGL: Update the model view projection matrix in the vertex shader
    glUniformMatrix4fv ( mvp_matrix_location, 1, GL_FALSE, glm::value_ptr ( MPTvT1R1_matrix4x4 ) );

    // OpenGL: Draw using the IBO
    glDrawElements (
        GL_TRIANGLES,
        index_buffer_size,
        GL_UNSIGNED_INT,
        0
    );

    glfwSwapBuffers ( window );
    glfwPollEvents ();

    ++frame_count;
    QueryPerformanceCounter ( &perf_ctr_now );
    const auto ticks_elapsed{ perf_ctr_now.QuadPart - perf_ctr_last.QuadPart };
    if ( ticks_elapsed >= ticks_per_s )
    {
      const auto ms_per_frame{ ticks_elapsed / ( ticks_per_ms * frame_count ) };
      const auto fps{ MS_PER_S / ms_per_frame };
      std::stringstream ss_title_fps{};
      ss_title_fps << std::fixed << std::setprecision ( FLOAT_PRECISION ) 
          << BASE_TITLE << ms_per_frame << " ms/f (" << fps << " fps)";
      glfwSetWindowTitle ( window, ss_title_fps.str ().c_str () );
      BOOST_LOG_SEV(log3d,debug) << "    Statistics: Window size = " << width 
          << "x" << height << ", Frame rate = " << ms_per_frame << " ms/f (" 
          << fps << " fps)";

      frame_count = 0;
      QueryPerformanceCounter ( &perf_ctr_last );
    }
  }
  BOOST_LOG_SEV(log3d,info) << "  Left main loop";

  glfwDestroyWindow ( window );
  BOOST_LOG_SEV(log3d,info) << "  GLFW: glfwDestroyWindow(...) succeeded";
  glfwTerminate ();
  BOOST_LOG_SEV(log3d,info) << "  GLFW: glfwTerminate() succeeded";


  BOOST_LOG_SEV(log3d,info) << "3D - end";
  return ( 0 );
}



///////////////////////////////////////////////////////////////////////////////
// END
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @brief Implementation file for this application.
 */
// Local variables:
// mode: c++
// End:
