//
// Created by user on 9/26/24.
//
#include "ScopeGuard.h"
#include <gmock/gmock.h>
#include <gst/gstpad.h>
#include <gst/gstcompat.h>

using namespace testing;

class AGstPad : public Test {
public:
};

TEST_F(AGstPad, CanCreateWithNameAndDirection) {
  auto *pad = gst_pad_new("test", GST_PAD_SRC);
  ON_SCOPE_EXIT([pad] { gst_object_unref(pad); });

  ASSERT_THAT(pad, NotNull());
  ASSERT_THAT(GST_PAD_DIRECTION(pad), Eq(GST_PAD_SRC));
}

TEST_F(AGstPad, CanGetPadName) {
  auto *pad = gst_pad_new("test", GST_PAD_SRC);
  ON_SCOPE_EXIT([pad] { gst_object_unref(pad); });

  ASSERT_THAT(gst_pad_get_name(pad), StrEq("test"));
}

TEST_F(AGstPad, HasNoTemplateByDefault) {
  auto *pad = gst_pad_new("test", GST_PAD_SRC);
  ON_SCOPE_EXIT([pad] { gst_object_unref(pad); });

  auto *t = gst_pad_get_pad_template(pad);
  ASSERT_THAT(t, IsNull());
}

TEST_F(AGstPad, IsNotActivateByDefault) {
  auto *pad = gst_pad_new("test", GST_PAD_SRC);
  ON_SCOPE_EXIT([pad] { gst_object_unref(pad); });

  ASSERT_FALSE(gst_pad_is_active(pad));
}

TEST_F(AGstPad, CanActivatePad) {
  auto *pad = gst_pad_new("test", GST_PAD_SRC);
  ON_SCOPE_EXIT([pad] { gst_object_unref(pad); });

  gst_pad_set_active(pad, TRUE);
  ASSERT_TRUE(gst_pad_is_active(pad));
}

TEST_F(AGstPad, CanLinkSourceAndSink) {
  auto *src = gst_pad_new("src", GST_PAD_SRC);
  ON_SCOPE_EXIT([src] { gst_object_unref(src); });

  auto *sink = gst_pad_new("sink", GST_PAD_SINK);
  ON_SCOPE_EXIT([sink] { gst_object_unref(sink); });

  ASSERT_THAT(gst_pad_link(src, sink), Eq(GST_PAD_LINK_OK));
}

TEST_F(AGstPad, LinkTwoSourceFailed) {
  auto *src1 = gst_pad_new("src1", GST_PAD_SRC);
  ON_SCOPE_EXIT([src1] { gst_object_unref(src1); });

  auto *src2 = gst_pad_new("src2", GST_PAD_SRC);
  ON_SCOPE_EXIT([src2] { gst_object_unref(src2); });

  ASSERT_THAT(gst_pad_link(src1, src2), Eq(GST_PAD_LINK_WRONG_DIRECTION));
}

TEST_F(AGstPad, LinkTwoSinkFailed) {
  auto *sink1 = gst_pad_new("sink1", GST_PAD_SINK);
  ON_SCOPE_EXIT([sink1] { gst_object_unref(sink1); });

  auto *sink2 = gst_pad_new("sink2", GST_PAD_SINK);
  ON_SCOPE_EXIT([sink2] { gst_object_unref(sink2); });

  ASSERT_THAT(gst_pad_link(sink1, sink2), Eq(GST_PAD_LINK_WRONG_DIRECTION));
}

TEST_F(AGstPad, LinkFailedIsAlreadyLinked) {
  auto *src = gst_pad_new("src", GST_PAD_SRC);
  ON_SCOPE_EXIT([src] { gst_object_unref(src); });

  auto *sink = gst_pad_new("sink", GST_PAD_SINK);
  ON_SCOPE_EXIT([sink] { gst_object_unref(sink); });

  gst_pad_link(src, sink);
  ASSERT_THAT(gst_pad_link(src, sink), Eq(GST_PAD_LINK_WAS_LINKED));
}

TEST_F(AGstPad, HasNoCapsByDefault) {
  auto *pad = gst_pad_new("test", GST_PAD_SRC);
  ON_SCOPE_EXIT([pad] { gst_object_unref(pad); });

  auto *caps = gst_pad_get_current_caps(pad);
  ASSERT_THAT(caps, IsNull());
}

TEST_F(AGstPad, SetCapsFaildIfNotActivated) {
  auto *pad = gst_pad_new("test", GST_PAD_SRC);
  ON_SCOPE_EXIT([pad] { gst_object_unref(pad); });

  auto *caps = gst_caps_from_string("foo/bar");
  ASSERT_FALSE(gst_pad_set_caps(pad, caps));
}

TEST_F(AGstPad, SetCapsSuccessIfActivated) {
  auto *pad = gst_pad_new("test", GST_PAD_SRC);
  ON_SCOPE_EXIT([pad] { gst_object_unref(pad); });

  gst_pad_set_active(pad, TRUE);
  auto *caps = gst_caps_from_string("foo/bar");
  ASSERT_TRUE(gst_pad_set_caps(pad, caps));
}

TEST_F(AGstPad, CanGetPadCaps) {
  auto *pad = gst_pad_new("test", GST_PAD_SRC);
  ON_SCOPE_EXIT([pad] { gst_object_unref(pad); });

  gst_pad_set_active(pad, TRUE);
  auto *caps = gst_caps_from_string("foo/bar");
  gst_pad_set_caps(pad, caps);

  auto *c = gst_pad_get_current_caps(pad);
  ASSERT_THAT(c, NotNull());
  ASSERT_THAT(gst_caps_is_equal(caps, c), Eq(TRUE));
}

TEST_F(AGstPad, GetNullAllowedCapsIfNotLinked) {
  auto *pad = gst_pad_new("test", GST_PAD_SRC);
  ON_SCOPE_EXIT([pad] { gst_object_unref(pad); });

  auto* cap = gst_pad_get_allowed_caps(pad);
  ON_SCOPE_EXIT([cap] { gst_caps_unref(cap); });

  ASSERT_THAT(cap, IsNull());
}

TEST_F(AGstPad, GetAllowedCapsIfLinked) {
  auto *src = gst_pad_new("src", GST_PAD_SRC);
  ON_SCOPE_EXIT([src] { gst_object_unref(src); });

  auto *sink = gst_pad_new("sink", GST_PAD_SINK);
  ON_SCOPE_EXIT([sink] { gst_object_unref(sink); });

  gst_pad_link(src, sink);

  auto *cap = gst_pad_get_allowed_caps(src);
  ON_SCOPE_EXIT([cap] { gst_caps_unref(cap); });

  ASSERT_THAT(cap, NotNull());
}

TEST_F(AGstPad, SrcPadAndSinkPadHasSameAllowedCaps) {
  auto *caps = gst_caps_from_string("foo/bar");
  auto *src = gst_pad_new("src", GST_PAD_SRC);
  ON_SCOPE_EXIT([src] { gst_object_unref(src); });
  gst_pad_set_active(src, TRUE);
  gst_pad_set_caps(src, caps);

  auto *sink = gst_pad_new("sink", GST_PAD_SINK);
  ON_SCOPE_EXIT([sink] { gst_object_unref(sink); });
  gst_pad_set_active(sink, TRUE);
  gst_pad_set_caps(sink, caps);

  gst_pad_link(src, sink);

  auto *cap1 = gst_pad_get_allowed_caps(src);
  ON_SCOPE_EXIT([cap1] { gst_caps_unref(cap1); });

  auto *cap2 = gst_pad_get_allowed_caps(sink);
  ON_SCOPE_EXIT([cap2] { gst_caps_unref(cap2); });

  ASSERT_THAT(gst_caps_is_equal(caps, cap1), Eq(TRUE));
  ASSERT_THAT(gst_caps_is_equal(cap1, cap2), Eq(TRUE));
}