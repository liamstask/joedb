#include "is_identifier.h"

/////////////////////////////////////////////////////////////////////////////
bool joedb::is_letter(char c)
/////////////////////////////////////////////////////////////////////////////
{
 return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

/////////////////////////////////////////////////////////////////////////////
bool joedb::is_number(char c)
/////////////////////////////////////////////////////////////////////////////
{
 return ('0' <= c && c <= '9');
}

/////////////////////////////////////////////////////////////////////////////
bool joedb::is_identifier(const std::string &s)
/////////////////////////////////////////////////////////////////////////////
{
 if (s.size() == 0)
  return false;

 if (is_number(s[0]))
  return false;

 char previous = 0;
 for (char c: s)
 {
  if (c != '_' && !is_letter(c) && !is_number(c))
   return false;
  if (c == '_' && previous == '_')
   return false;
  previous = c;
 }

 return true;
}
