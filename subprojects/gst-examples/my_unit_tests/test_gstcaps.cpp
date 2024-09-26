//
// Created by user on 9/25/24.
//
#include "ScopeGuard.h"
#include <gmock/gmock.h>
#include <gst/gstcaps.h>

using namespace testing;

class AGstCaps : public Test {
public:
};

TEST_F(AGstCaps, CanCreateEmptyCaps) {
  auto *e = gst_caps_new_empty();
  ON_SCOPE_EXIT([e] { gst_caps_unref(e); });

  ASSERT_THAT(e, NotNull());
}

TEST_F(AGstCaps, EmptypCapsNoSetFlag) {
  auto *e = gst_caps_new_empty();
  ON_SCOPE_EXIT([e] { gst_caps_unref(e); });

  ASSERT_THAT(GST_CAPS_FLAGS(e), Eq(0));
}

TEST_F(AGstCaps, CanCreateAnyCaps) {
  auto *e = gst_caps_new_any();
  ON_SCOPE_EXIT([e] { gst_caps_unref(e); });

  ASSERT_THAT(e, NotNull());
}

TEST_F(AGstCaps, AnyCapsHasFlag) {
  auto *e = gst_caps_new_any();
  ON_SCOPE_EXIT([e] { gst_caps_unref(e); });

  ASSERT_THAT(GST_CAPS_FLAGS(e), Eq(GST_CAPS_FLAG_ANY));
}

TEST_F(AGstCaps, CanCheckIsAnyOrNot) {
  auto *e0 = gst_caps_new_empty();
  ON_SCOPE_EXIT([e0] { gst_caps_unref(e0); });
  ASSERT_THAT(gst_caps_is_any(e0), Eq(FALSE));

  auto *e1 = gst_caps_new_any();
  ON_SCOPE_EXIT([e1] { gst_caps_unref(e1); });
  ASSERT_THAT(gst_caps_is_any(e1), Eq(TRUE));
}

TEST_F(AGstCaps, CanCheckIsEmptyOrNot) {
  auto *e0 = gst_caps_new_empty();
  ON_SCOPE_EXIT([e0] { gst_caps_unref(e0); });
  ASSERT_THAT(gst_caps_is_empty(e0), Eq(TRUE));

  auto *e1 = gst_caps_new_any();
  ON_SCOPE_EXIT([e1] { gst_caps_unref(e1); });
  ASSERT_THAT(gst_caps_is_empty(e1), Eq(FALSE));
}

TEST_F(AGstCaps, EmptyCapsHasNoStructureAfterCreated) {
  auto *e = gst_caps_new_empty();
  ON_SCOPE_EXIT([e] { gst_caps_unref(e); });

  ASSERT_THAT(gst_caps_get_size(e), Eq(0));
}

TEST_F(AGstCaps, AnyCapsHasNoStructureAfterCreated) {
  auto *e = gst_caps_new_any();
  ON_SCOPE_EXIT([e] { gst_caps_unref(e); });

  ASSERT_THAT(gst_caps_get_size(e), Eq(0));
}

TEST_F(AGstCaps, CreateEmptyCapsWithMediaTypeHasOneStructure) {
  auto *e = gst_caps_new_empty_simple("video/x-raw");
  ON_SCOPE_EXIT([e] { gst_caps_unref(e); });

  ASSERT_THAT(e, NotNull());
  ASSERT_THAT(gst_caps_get_size(e), Eq(1));
  auto *s = gst_caps_get_structure(e, 0);
  ASSERT_THAT(s, NotNull());
}

TEST_F(AGstCaps, CreateEmptyCapsWithMediaTypeHasCorrectMediaType) {
  auto *e = gst_caps_new_empty_simple("video/x-raw");
  ON_SCOPE_EXIT([e] { gst_caps_unref(e); });

  auto *s = gst_caps_get_structure(e, 0);
  auto *mediaType = gst_structure_get_name(s);
  ASSERT_THAT(mediaType, StrEq("video/x-raw"));
}

TEST_F(AGstCaps, CanCreateEmptyWithMediaTypeAndFieldName) {
  auto *e =
      gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "RGB", NULL);
  ON_SCOPE_EXIT([e] { gst_caps_unref(e); });

  ASSERT_THAT(e, NotNull());
  ASSERT_THAT(gst_caps_get_size(e), Eq(1));
  auto *s = gst_caps_get_structure(e, 0);
  ASSERT_THAT(s, NotNull());
  auto *format = gst_structure_get_string(s, "format");
  ASSERT_THAT(format, StrEq("RGB"));
}

TEST_F(AGstCaps, AppendStructureIncreaseSize) {
  auto *e = gst_caps_new_empty();
  ON_SCOPE_EXIT([e] { gst_caps_unref(e); });

  auto *s =
      gst_structure_new("video/x-raw", "format", G_TYPE_STRING, "RGB", NULL);
  gst_caps_append_structure(e, s);

  ASSERT_THAT(gst_caps_get_size(e), Eq(1));
}

TEST_F(AGstCaps, AppendFailedIfStructureExist) {
  auto *e = gst_caps_new_empty();
  ON_SCOPE_EXIT([e] { gst_caps_unref(e); });

  auto *s =
      gst_structure_new("video/x-raw", "format", G_TYPE_STRING, "RGB", NULL);
  gst_caps_append_structure(e, s);
  gst_caps_append_structure(e, s);

  ASSERT_THAT(gst_caps_get_size(e), Eq(1));
}

TEST_F(AGstCaps, RemoveStructureDecreaseSize) {
  auto *e = gst_caps_new_empty();
  ON_SCOPE_EXIT([e] { gst_caps_unref(e); });

  auto *s =
      gst_structure_new("video/x-raw", "format", G_TYPE_STRING, "RGB", NULL);
  gst_caps_append_structure(e, s);
  ASSERT_THAT(gst_caps_get_size(e), Eq(1));

  gst_caps_remove_structure(e, 0);
  ASSERT_THAT(gst_caps_get_size(e), Eq(0));
}

TEST_F(AGstCaps, RemoveStructureHappendNothingIfIndexNotValid) {
  auto *e = gst_caps_new_empty();
  ON_SCOPE_EXIT([e] { gst_caps_unref(e); });

  auto *s =
      gst_structure_new("video/x-raw", "format", G_TYPE_STRING, "RGB", NULL);
  gst_caps_append_structure(e, s);
  ASSERT_THAT(gst_caps_get_size(e), Eq(1));

  gst_caps_remove_structure(e, 1);
  ASSERT_THAT(gst_caps_get_size(e), Eq(1));
}

TEST_F(AGstCaps, MergeStructureIncreaseCountIfThisStructureNoExpressed) {
  auto *e = gst_caps_new_empty();
  ON_SCOPE_EXIT([e] { gst_caps_unref(e); });

  auto *s =
      gst_structure_new("video/x-raw", "format", G_TYPE_STRING, "RGB", NULL);
  auto *merged = gst_caps_merge_structure(e, s);
  ASSERT_THAT(gst_caps_get_size(merged), Eq(1));

  auto *s1 =
      gst_structure_new("video/x-raw", "format", G_TYPE_STRING, "RGB", NULL);
  auto *merged2 = gst_caps_merge_structure(merged, s1);
  ASSERT_THAT(gst_caps_get_size(merged2), Eq(1));
}

TEST_F(AGstCaps, CanAppendStructureAndFeature) {
  auto *e = gst_caps_new_empty();
  ON_SCOPE_EXIT([e] { gst_caps_unref(e); });

  auto *s =
      gst_structure_new("video/x-raw", "format", G_TYPE_STRING, "RGB", NULL);
  gst_caps_append_structure_full(
      e, s, gst_caps_features_new("m:abc", "m:def", "m:ghi", NULL));

  ASSERT_THAT(gst_caps_get_size(e), Eq(1));
  auto *s1 = gst_caps_get_structure(e, 0);
  ASSERT_THAT(s1, NotNull());
  auto *f = gst_caps_get_features(e, 0);
  ASSERT_THAT(f, NotNull());
}