#include "Listener.h"

namespace joedb
{
 #define TYPE_MACRO(type, return_type, type_id, R, W)\
 void Listener::after_update_vector_##type_id(table_id_t table_id,\
                                              record_id_t record_id,\
                                              field_id_t field_id,\
                                              record_id_t size,\
                                              const type *value)\
 {\
  for (record_id_t i = 0; i < size; i++)\
   after_update_##type_id(table_id, record_id + i, field_id, value[i]);\
 }
 #include "TYPE_MACRO.h"
 #undef TYPE_MACRO
}
