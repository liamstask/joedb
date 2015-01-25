#include "JournalFile.h"
#include "File.h"
#include "DBListener.h"
#include "gtest/gtest.h"
#include "dump.h"

using namespace joedb;

class JournalFile_Test: public::testing::Test
{
 protected:
  virtual void TearDown()
  {
   std::remove("test.joedb");
  }
};

/////////////////////////////////////////////////////////////////////////////
TEST_F(JournalFile_Test, basic_operations)
{
 Database db1;

 {
  File file("test.joedb", File::mode_t::create_new);
  JournalFile journal(file);
  db1.set_listener(journal);
  db1.drop_table(db1.create_table("deleted"));
  const table_id_t table_id = db1.create_table("table_test");
  db1.insert_into(table_id, 1);
  const field_id_t field_id = db1.add_field(table_id, "field", Type::int32());
  db1.update(table_id, 1, field_id, Value(int32_t(1234)));
  db1.delete_from(table_id, 1);
  db1.insert_into(table_id, 2);
  db1.update(table_id, 2, field_id, Value(int32_t(4567)));
  db1.drop_field(table_id, field_id);

  const field_id_t big_field_id = db1.add_field(table_id, "big_field", Type::int64());
  db1.update(table_id, 2, big_field_id, Value(int64_t(1234567)));

  const field_id_t new_field =
   db1.add_field(table_id, "new_field", Type::reference(table_id));
  db1.update(table_id, 2, new_field, Value(record_id_t(2)));
  const field_id_t name_id = db1.add_field(table_id, "name", Type::string());
  db1.update(table_id, 2, name_id, Value("Aristide"));
  journal.checkpoint();
  file.commit();
 }

 Database db2;

 {
  File file("test.joedb", File::mode_t::read_existing);
  JournalFile journal(file);
  DBListener db_listener(db2);
  journal.replay_log(db_listener);
  EXPECT_TRUE(db_listener.is_good());
  EXPECT_EQ(JournalFile::state_t::no_error, journal.get_state());
 }

 std::ostringstream oss1;
 std::ostringstream oss2;

 joedb::dump(oss1, db1);
 joedb::dump(oss2, db2);

 EXPECT_EQ(oss1.str(), oss2.str());
}