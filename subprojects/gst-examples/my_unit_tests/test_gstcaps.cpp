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