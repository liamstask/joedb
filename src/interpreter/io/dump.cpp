#include "dump.h"
#include "joedb/Database.h"
#include "joedb/Writeable.h"

/////////////////////////////////////////////////////////////////////////////
void joedb::dump(const Readable &db, Writeable &writeable)
/////////////////////////////////////////////////////////////////////////////
{
 //
 // Dump tables
 //
 std::map<table_id_t, table_id_t> table_map;
 {
  table_id_t table_id = 0;
  for (auto table: db.get_tables())
  {
   ++table_id;
   table_map[table.second] = table_id;

   writeable.create_table(table.first);
  }
 }

 //
 // Dump fields
 //
 std::map<table_id_t, std::map<field_id_t, field_id_t>> field_maps;
 {
  table_id_t table_id = 0;
  for (auto table: db.get_tables())
  {
   ++table_id;
   field_id_t field_id = 0;
   for (const auto &field: db.get_table_fields(table.second))
   {
    ++field_id;
    Type type = db.get_field_type(table.second, field.second);
    if (type.get_type_id() == Type::type_id_t::reference)
     type = Type::reference(table_map[type.get_table_id()]);
    field_maps[table.second][field.second] = field_id;

    writeable.add_field(table_map[table.second], field.first, type);
   }
  }
 }

 //
 // Dump records
 //
 for (auto table: db.get_tables())
 {
  const auto &fields = db.get_table_fields(table.second);
  const record_id_t max_record_id = db.get_max_record_id(table.second);

  size_t i = 0;

  while (i < freedom.size())
  {
   while (i < freedom.size() && freedom.is_free(i + 2))
    i++;
   size_t size = 0;
   while (i + size < freedom.size() && !freedom.is_free(i + 2 + size))
    size++;

   if (size)
   {
    writeable.insert_vector(table_map[table.second], i + 1, size);
    i += size;
   }
  }

  for (const auto &field: fields)
  {
   for (size_t i = 0; i < freedom.size(); i++)
    if (!freedom.is_free(i + 2))
    {
     const record_id_t record_id = i + 1;

     switch(field.second.get_type().get_type_id())
     {
      case Type::type_id_t::null:
      break;

      #define TYPE_MACRO(type, return_type, type_id, R, W)\
      case Type::type_id_t::type_id:\
       writeable.update_##type_id(table_map[table.first], record_id, field_maps[table.first][field.first], table.second.get_##type_id(record_id, field.first));\
      break;
      #include "joedb/TYPE_MACRO.h"
      #undef TYPE_MACRO
     }
    }
  }
 }
}

/////////////////////////////////////////////////////////////////////////////
void joedb::dump_data(const Readable &db, Writeable &writeable)
/////////////////////////////////////////////////////////////////////////////
{
 for (auto table: db.get_tables())
 {
  const auto &fields = table.second.get_fields();
  const auto &freedom = table.second.get_freedom();

  size_t i = 0;

  while (i < freedom.size())
  {
   while (i < freedom.size() && freedom.is_free(i + 2))
    i++;
   size_t size = 0;
   while (i + size < freedom.size() && !freedom.is_free(i + 2 + size))
    size++;

   if (size)
   {
    writeable.insert_vector(table.first, i + 1, size);

    for (const auto &field: fields)
    {
     switch(field.second.get_type().get_type_id())
     {
      case Type::type_id_t::null:
      break;

      #define TYPE_MACRO(type, return_type, type_id, R, W)\
      case Type::type_id_t::type_id:\
       writeable.update_vector_##type_id(table.first, i + 1, field.first, size, field.second.get_vector_##type_id() + i);\
      break;
      #include "joedb/TYPE_MACRO.h"
      #undef TYPE_MACRO
     }
    }

    i += size;
   }
  }
 }
}

#include "Selective_Writeable.h"
#include "Multiplexer.h"
#include "joedb/Journal_File.h"

/////////////////////////////////////////////////////////////////////////////
void joedb::pack(Journal_File &input_journal, Writeable &writeable)
/////////////////////////////////////////////////////////////////////////////
{
 Database db;

 Selective_Writeable schema_filter(writeable, Selective_Writeable::Mode::schema);
 Multiplexer multiplexer;
 multiplexer.add_writeable(db);
 multiplexer.add_writeable(schema_filter);

 input_journal.replay_log(multiplexer);

 dump_data(db, writeable);
}
