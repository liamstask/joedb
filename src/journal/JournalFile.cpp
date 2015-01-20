#include "JournalFile.h"

using namespace joedb;

/////////////////////////////////////////////////////////////////////////////
JournalFile::JournalFile(const char *file_name, File::open_mode_t open_mode):
/////////////////////////////////////////////////////////////////////////////
 file(file_name, open_mode),
 checkpoint_index(0)
{
}

/////////////////////////////////////////////////////////////////////////////
void JournalFile::checkpoint()
/////////////////////////////////////////////////////////////////////////////
{
 checkpoint_index ^= 1;
 file.commit();
}

/////////////////////////////////////////////////////////////////////////////
JournalFile::~JournalFile()
/////////////////////////////////////////////////////////////////////////////
{
 checkpoint();
}
