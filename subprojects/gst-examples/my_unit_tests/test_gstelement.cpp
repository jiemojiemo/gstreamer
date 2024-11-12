//
// Created by user on 10/10/24.
//
#include "ScopeGuard.h"
#include <gmock/gmock.h>
#include <gst/gstelement.h>

using namespace testing;

class AGstElement : public Test {
public:
  void SetUp() override {
    fakesrc = gst_element_factory_make("fakesrc", "source");
    fakesink = gst_element_factory_make("fakesink", "sink");
    test_src_pad = gst_pad_new("test_src", GST_PAD_SRC);
    test_sink_pad = gst_pad_new("test_sink", GST_PAD_SINK);
  }
  void TearDown() override {
    gst_object_unref(fakesrc);
    gst_object_unref(fakesink);
    gst_object_unref(test_src_pad);
    gst_object_unref(test_sink_pad);
  }

  GstElement* fakesrc;
  GstElement* fakesink;
  GstPad * test_src_pad;
  GstPad * test_sink_pad;
};

TEST_F(AGstElement, AddSrcPadIncreaseSrcPadCount) {
  ASSERT_THAT(fakesrc->numsrcpads, Eq(1));

  auto *p = gst_pad_new("test_src", GST_PAD_SRC);
  ON_SCOPE_EXIT([p] { gst_object_unref(p); });

  auto ok = gst_element_add_pad(fakesrc, test_src_pad);
  ASSERT_TRUE(ok);
  ASSERT_THAT(fakesrc->numsrcpads, Eq(2));
}

TEST_F(AGstElement, AddSrcPadFailedIfPadNameIsNotUnique) {

  auto first_ok = gst_element_add_pad(fakesrc, test_src_pad);
  ASSERT_TRUE(first_ok);

  auto second_ok = gst_element_add_pad(fakesrc, test_src_pad);
  ASSERT_FALSE(second_ok);
}

TEST_F(AGstElement, AddSinkPadIncreaseSinkPadCount) {
  ASSERT_THAT(fakesink->numsinkpads, Eq(1));

  auto ok = gst_element_add_pad(fakesink, test_sink_pad);
  ASSERT_TRUE(ok);
  ASSERT_THAT(fakesink->numsinkpads, Eq(2));
}

TEST_F(AGstElement, AddSinkPadFailedIfPadNameIsNotUnique) {
  auto first_ok = gst_element_add_pad(fakesink, test_sink_pad);
  ASSERT_TRUE(first_ok);

  auto second_ok = gst_element_add_pad(fakesink, test_sink_pad);
  ASSERT_FALSE(second_ok);
}

TEST_F(AGstElement, RemoveSrcPadDecreaseSrcPadCount) {
  auto ok = gst_element_add_pad(fakesrc, test_src_pad);
  ASSERT_TRUE(ok);
  ASSERT_THAT(fakesrc->numsrcpads, Eq(2));

  gst_element_remove_pad(fakesrc, test_src_pad);
  ASSERT_THAT(fakesrc->numsrcpads, Eq(1));
}

TEST_F(AGstElement, RemoveSinkPadDecreaseSinkPadCount) {
  auto ok = gst_element_add_pad(fakesink, test_sink_pad);
  ASSERT_TRUE(ok);
  ASSERT_THAT(fakesink->numsinkpads, Eq(2));

  gst_element_remove_pad(fakesink, test_sink_pad);
  ASSERT_THAT(fakesink->numsinkpads, Eq(1));
}

TEST_F(AGstElement, RemovePadFailedIfPadNotAdded) {
  ASSERT_THAT(fakesrc->numsrcpads, Eq(1));

  auto ok = gst_element_remove_pad(fakesrc, test_src_pad);
  ASSERT_FALSE(ok);
}

TEST_F(AGstElement, InitStateIsNull) {
  gst_element_set_state(fakesrc, GST_STATE_PLAYING);
}