#ifndef joedb_DBListener_declared
#define joedb_DBListener_declared

#include "SchemaListener.h"

namespace joedb
{
 class DBListener: public SchemaListener
 {
  public:
   DBListener(Database &db): SchemaListener(db) {}

   void after_insert(table_id_t table_id, record_id_t record_id)
   {
    error |= !db.insert_into(table_id, record_id);
   }

   void after_delete(table_id_t table_id, record_id_t record_id)
   {
    error |= !db.delete_from(table_id, record_id);
   }

   void after_update(table_id_t table_id,
                     record_id_t record_id,
                     field_id_t field_id,
                     const Value &value)
   {
    error |= !db.update(table_id, record_id, field_id, value);
   }
 };
}

#endif
