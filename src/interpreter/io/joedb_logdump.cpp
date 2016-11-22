#include <iostream>

#include "File.h"
#include "Journal_File.h"
#include "Interpreter_Dump_Listener.h"
#include "SQL_Dump_Listener.h"
#include "file_error_message.h"
#include "diagnostics.h"

/////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
/////////////////////////////////////////////////////////////////////////////
{
 if (argc <= 1)
 {
  std::cerr << "usage: " << argv[0] << " [--sql|--header] <file.joedb>\n";
  return 1;
 }
 else
 {
  bool sql = false;
  bool header = false;
  int file_index = 1;

  if (std::string(argv[1]) == "--sql")
  {
   sql = true;
   file_index = 2;
  }

  if (std::string(argv[1]) == "--header")
  {
   header = true;
   file_index = 2;
  }

  joedb::File file(argv[file_index], joedb::File::mode_t::read_existing);

  if (joedb::file_error_message(std::cerr, file))
   return 1;

  joedb::Journal_File journal(file);
  if (!journal.is_good())
  {
   std::cerr << "Error opening file\n";
   return 1;
  }

  if (sql)
  {
   joedb::SQL_Dump_Listener dump_listener(std::cout);
   journal.replay_log(dump_listener);
  }
  else if (header)
  {
   std::cout << '\n';
   joedb::dump_header(std::cout, file);
   std::cout << '\n';
   joedb::about_joedb(std::cout);
   std::cout << '\n';
  }
  else
  {
   joedb::Interpreter_Dump_Listener dump_listener(std::cout);
   journal.replay_log(dump_listener);
  }

  static char const * const status_string[]
  {
   "no_error",
   "bad_file",
   "unsupported_version",
   "bad_format",
   "crash_check",
   "listener_threw"
  };

  static_assert(sizeof(status_string) / sizeof(*status_string) ==
   size_t(joedb::Journal_File::state_t::journal_errors),
   "size of status_string is wrong");

  std::cerr << "---> ";
  std::cerr << status_string[int(journal.get_state())] << '\n';
 }

 return 0;
}
