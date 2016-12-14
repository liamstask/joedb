#ifndef joedb_Multiplexer_declared
#define joedb_Multiplexer_declared

#include "joedb/Writeable.h"

#include <vector>
#include <memory>

namespace joedb
{
 class Multiplexer;

 class Internal_Writeable: public Writeable
 {
  private:
   Multiplexer * const multiplexer;
   const size_t id;

  public:
   Internal_Writeable(Multiplexer *multiplexer, size_t id);

   void create_table(const std::string &name) override;
   void drop_table(table_id_t table_id) override;
   void rename_table(table_id_t table_id,
                     const std::string &name) override;

   void add_field(table_id_t table_id,
                  const std::string &name,
                  Type type) override;
   void drop_field(table_id_t table_id,
                   field_id_t field_id) override;
   void rename_field(table_id_t table_id,
                     field_id_t field_id,
                     const std::string &name) override;

   void custom(const std::string &name) override;

   void comment(const std::string &comment) override;
   void timestamp(int64_t timestamp) override;
   void valid_data() override;

   void insert(table_id_t table_id, record_id_t record_id) override;
   void insert_vector(table_id_t table_id,
                      record_id_t record_id,
                      record_id_t size) override;
   void delete_record(table_id_t table_id, record_id_t record_id) override;

   #define TYPE_MACRO(type, return_type, type_id, R, W)\
   void update_##type_id(table_id_t table_id,\
                         record_id_t record_id,\
                         field_id_t field_id,\
                         return_type value) override;\
   void update_vector_##type_id(table_id_t table_id,\
                                record_id_t record_id,\
                                field_id_t field_id,\
                                record_id_t size,\
                                const type *value) override;
   #include "joedb/TYPE_MACRO.h"
   #undef TYPE_MACRO
 };

 class Multiplexer
 {
  friend class Internal_Writeable;

  private:
   std::vector<Writeable *> external_writeables;
   std::vector<std::unique_ptr<Internal_Writeable>> internal_writeables;
   bool multiplexing = false;

  public:
   Writeable &add_writeable(Writeable &external_writeable);
 };
}

#endif
