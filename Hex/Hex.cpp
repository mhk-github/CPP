///////////////////////////////////////////////////////////////////////////////
// FILE     : Hex.cpp
// SYNOPSIS : Outputs a file's contents in hexadecimal form.
// LICENSE  : MIT
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// HEADER FILES
///////////////////////////////////////////////////////////////////////////////

// PRECOMPILED HEADER FILE ////////////////////////////////////////////////////

#include "PCH.h"

// LINKER DIRECTIVES //////////////////////////////////////////////////////////

#pragma comment(lib, "boost_program_options-vc142-mt-x32-1_77.lib")



///////////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////////

const char *HEX_VALUES[]{
    "00", "01", "02", "03", "04", "05", "06", "07",
    "08", "09", "0A", "0B", "0C", "0D", "0E", "0F",
    "10", "11", "12", "13", "14", "15", "16", "17",
    "18", "19", "1A", "1B", "1C", "1D", "1E", "1F",
    "20", "21", "22", "23", "24", "25", "26", "27",
    "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
    "30", "31", "32", "33", "34", "35", "36", "37",
    "38", "39", "3A", "3B", "3C", "3D", "3E", "3F",
    "40", "41", "42", "43", "44", "45", "46", "47",
    "48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
    "50", "51", "52", "53", "54", "55", "56", "57",
    "58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
    "60", "61", "62", "63", "64", "65", "66", "67",
    "68", "69", "6A", "6B", "6C", "6D", "6E", "6F",
    "70", "71", "72", "73", "74", "75", "76", "77",
    "78", "79", "7A", "7B", "7C", "7D", "7E", "7F",
    "80", "81", "82", "83", "84", "85", "86", "87",
    "88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
    "90", "91", "92", "93", "94", "95", "96", "97",
    "98", "99", "9A", "9B", "9C", "9D", "9E", "9F",
    "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7",
    "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF",
    "B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7",
    "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",
    "C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7",
    "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF",
    "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7",
    "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF",
    "E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7",
    "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
    "F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7",
    "F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF"
};

const char PRINTABLES[]{
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    ' ', '!', '"', '#', '$', '%', '&', '\'',
    '(', ')', '*', '+', ',', '-', '.', '/',
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', ':', ';', '<', '=', '>', '?',
    '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
    'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
    '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
    'x', 'y', 'z', '{', '|', '}', '~', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.'
};

const char HEX_CHARS[]{
    '0', '1', '2', '3', '4', '5', '6', '7', 
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

const auto BIT_SHIFT_AMOUNT{ 4 };
constexpr auto READ_SIZE{ 1 << BIT_SHIFT_AMOUNT };

const auto OFFSET_LIMIT_POSITION{ 7 };
const auto HEX_START_POSITION{ 10 };
const auto PRINTABLES_START_POSITION{ 59 };
const auto HEX_SKIP_AMOUNT{ 2 };

const auto FILE_ARG_POSITION{ 1 };



///////////////////////////////////////////////////////////////////////////////
// DRIVER
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char** argv )
{ 
  bool is_help{};

  boost::program_options::options_description description{ 
      "Hex [options] file" 
  };
  description.add_options ()
      ( 
          "help,h", 
          boost::program_options::bool_switch ( &is_help ), 
          "Display a help dialog" 
      )
      ( 
          "file,f", 
          boost::program_options::value<std::string> (), 
          "File to show in hexadecimal form"
      );

  boost::program_options::positional_options_description positional{};
  positional.add ( "file", FILE_ARG_POSITION );

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
  catch ( const std::exception &e )
  {
    std::cerr << e.what () << "\n";
    return ( 1 );
  }

  if ( is_help )
  {
    std::cout << description;
    return ( 0 );
  }
  if ( vm["file"].empty () )
  {
    std::cerr << "No input file given !\n";
    return ( 2 );
  }
  const auto &filename{ vm["file"].as<std::string> () };

  std::ifstream input{ filename, std::ios::binary };
  if ( !input.is_open () )
  {
    std::cerr << "Cannot open input file '" << filename 
        << "' in binary mode for reading !\n";
    return ( 3 );
  }
  
  char output[]{ 
      "00000000  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................\n" 
  };
  char * const offset_limit{ output + OFFSET_LIMIT_POSITION };
  char * const hex_start{ output + HEX_START_POSITION };
  char * const printables_start{ output + PRINTABLES_START_POSITION };

  char read_buffer[READ_SIZE]{};
  auto byte_ctr{ 0 };
  while ( !input.eof () )
  {
    input.read ( read_buffer, READ_SIZE );

    auto temp_byte_ctr{ byte_ctr };
    char *offset_ptr{ offset_limit };
    while ( temp_byte_ctr ) {
      *offset_ptr = *( HEX_CHARS + ( temp_byte_ctr % READ_SIZE ) );
      temp_byte_ctr >>= BIT_SHIFT_AMOUNT;
      --offset_ptr;
    }

    char *hex_ptr{ hex_start };
    char *printables_ptr{ printables_start };
    auto *byte_ptr{ read_buffer };
    const auto bytes_read{ input.gcount () };
    for ( auto i{ 0 }; i < bytes_read; ++i )
    {
      const auto byte_value{ static_cast<unsigned char>( *byte_ptr ) };
      const auto hex_representation{ *( HEX_VALUES + byte_value ) };
      *hex_ptr = *hex_representation;
      ++hex_ptr;
      *hex_ptr = *( hex_representation + 1 );
        
      *printables_ptr = *( PRINTABLES + byte_value );

      hex_ptr += HEX_SKIP_AMOUNT;
      ++printables_ptr;
      ++byte_ptr;
    }

    if ( bytes_read < READ_SIZE )
    {
      for ( auto i{ READ_SIZE - bytes_read }; i > 0; --i )
      {
        *hex_ptr = ' ';
        ++hex_ptr;
        *hex_ptr = ' ';

        *printables_ptr = ' ';

        hex_ptr += HEX_SKIP_AMOUNT;
        ++printables_ptr;
      }
    }

    byte_ctr += READ_SIZE;

    std::cout << output;
  }
  
  input.close ();


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
