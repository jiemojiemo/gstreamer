//
// Created by user on 9/25/24.
//
#include "ScopeGuard.h"
#include <gmock/gmock.h>
#include <gst/gststructure.h>

using namespace testing;

class AGstStructure : public Test {
public:
};

TEST_F(AGstStructure, CanCreateEmptyStructureWithName) {
  auto *s = gst_structure_new_empty("test");
  ON_SCOPE_EXIT([s] { gst_structure_free(s); });

  ASSERT_THAT(s, NotNull());
}

TEST_F(AGstStructure, CanGetName) {
  auto *s = gst_structure_new_empty("test");
  ON_SCOPE_EXIT([s] { gst_structure_free(s); });

  ASSERT_THAT(gst_structure_get_name(s), StrEq("test"));
}

TEST_F(AGstStructure, CanCreateWithNameAndFields) {
  auto *s = gst_structure_new("test", "message", G_TYPE_STRING, "hello", NULL);
  ON_SCOPE_EXIT([s] { gst_structure_free(s); });

  ASSERT_THAT(s, NotNull());
}

TEST_F(AGstStructure, CanGetField) {
  auto *s = gst_structure_new("test", "message", G_TYPE_STRING, "hello", NULL);
  ON_SCOPE_EXIT([s] { gst_structure_free(s); });

  auto *f = gst_structure_get_value(s, "message");
  ASSERT_THAT(f, NotNull());
  ASSERT_THAT(g_value_get_string(f), StrEq("hello"));
}

TEST_F(AGstStructure, CanSerializeStructureToString) {
  auto *s = gst_structure_new("test", "message", G_TYPE_STRING, "hello", NULL);
  ON_SCOPE_EXIT([s] { gst_structure_free(s); });

  auto *str = gst_structure_to_string(s);

  ASSERT_THAT(str, StrEq("test, message=(string)hello;"));
}

TEST_F(AGstStructure, CanCreateStructureFromString) {
  auto *s0 = gst_structure_new("test", "message", G_TYPE_STRING, "hello", NULL);
  ON_SCOPE_EXIT([s0] { gst_structure_free(s0); });

  auto *s0_str = gst_structure_to_string(s0);
  auto *s1 = gst_structure_new_from_string(s0_str);

  ASSERT_TRUE(gst_structure_is_equal(s0, s1));
}

TEST_F(AGstStructure, CopyOneIsEqualToOriginal) {
  auto *s0 = gst_structure_new("test", "message", G_TYPE_STRING, "hello", NULL);
  ON_SCOPE_EXIT([s0] { gst_structure_free(s0); });

  auto *s1 = gst_structure_copy(s0);

  ASSERT_TRUE(gst_structure_is_equal(s0, s1));
}

TEST_F(AGstStructure, CanCheckHasAName) {
  auto *s = gst_structure_new("test", "message", G_TYPE_STRING, "hello", NULL);
  ON_SCOPE_EXIT([s] { gst_structure_free(s); });

  ASSERT_TRUE(gst_structure_has_name(s, "test"));
  ASSERT_FALSE(gst_structure_has_name(s, "abc"));
}

TEST_F(AGstStructure, CanSetANewName) {
  auto *s = gst_structure_new("test", "message", G_TYPE_STRING, "hello", NULL);
  ON_SCOPE_EXIT([s] { gst_structure_free(s); });

  gst_structure_set_name(s, "abc");

  ASSERT_THAT(gst_structure_get_name(s), StrEq("abc"));
}

TEST_F(AGstStructure, CanUpdateFiledValueAfterSetValueIfNameFound) {
  auto *s = gst_structure_new("test", "message", G_TYPE_STRING, "hello", NULL);
  ON_SCOPE_EXIT([s] { gst_structure_free(s); });

  GValue value = G_VALUE_INIT;
  g_value_init(&value, G_TYPE_STRING);
  g_value_set_string(&value, "world");
  gst_structure_set_value(s, "message", &value);
  g_value_unset(&value);

  auto *f = gst_structure_get_value(s, "message");
  ASSERT_THAT(g_value_get_string(f), StrEq("world"));
}

TEST_F(AGstStructure, SetFieldWillAddNewFieldIfNameNotFound) {
  auto *s = gst_structure_new("test", "message", G_TYPE_STRING, "hello", NULL);
  ON_SCOPE_EXIT([s] { gst_structure_free(s); });

  GValue value = G_VALUE_INIT;
  g_value_init(&value, G_TYPE_STRING);
  g_value_set_string(&value, "world");
  gst_structure_set_value(s, "abc", &value);
  g_value_unset(&value);

  auto *f = gst_structure_get_value(s, "abc");
  ASSERT_THAT(g_value_get_string(f), StrEq("world"));
}

TEST_F(AGstStructure, CanRemoveField) {
  auto *s = gst_structure_new("test", "message", G_TYPE_STRING, "hello", NULL);
  ON_SCOPE_EXIT([s] { gst_structure_free(s); });

  gst_structure_remove_field(s, "message");

  ASSERT_THAT(gst_structure_get_value(s, "message"), IsNull());
}

TEST_F(AGstStructure, IsNotSubsetIfNameIsDifferent) {
  auto *s1 = gst_structure_from_string("test/test, channels=(int)1", NULL);
  ON_SCOPE_EXIT([s1] { gst_structure_free(s1); });
  auto *s2 = gst_structure_from_string("test/test2, channels=(int)1", NULL);
  ON_SCOPE_EXIT([s2] { gst_structure_free(s2); });

  ASSERT_FALSE(gst_structure_is_subset(s1, s2));
}

TEST_F(AGstStructure, IsNotSubsetIfFieldMissing) {
  auto *s1 = gst_structure_from_string(
      "test/test, channels=(int)1, rate=(int)1", NULL);
  ON_SCOPE_EXIT([s1] { gst_structure_free(s1); });
  auto *s2 = gst_structure_from_string("test/test1, channels=(int)1", NULL);
  ON_SCOPE_EXIT([s2] { gst_structure_free(s2); });

  ASSERT_FALSE(gst_structure_is_subset(s1, s2));
}

TEST_F(AGstStructure, IsNotSubsetIfSupersteHasExtraFields) {
  auto *s1 = gst_structure_from_string("test/test, channels=(int)1", NULL);
  ON_SCOPE_EXIT([s1] { gst_structure_free(s1); });
  auto *s2 = gst_structure_from_string(
      "test/test, channels=(int)1, rate=(int)1", NULL);
  ON_SCOPE_EXIT([s2] { gst_structure_free(s2); });

  ASSERT_FALSE(gst_structure_is_subset(s1, s2));
}

TEST_F(AGstStructure, IsSubsetIfSupersetHasExtraValues) {
  auto *s1 = gst_structure_from_string("test/test, channels=(int)1", NULL);
  ON_SCOPE_EXIT([s1] { gst_structure_free(s1); });
  auto *s2 =
      gst_structure_from_string("test/test, channels=(int)[ 1, 2 ]", NULL);
  ON_SCOPE_EXIT([s2] { gst_structure_free(s2); });

  ASSERT_TRUE(gst_structure_is_subset(s1, s2));
}