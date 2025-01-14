//
// Created by user on 1/10/25.
//
#include <gmock/gmock.h>
#include <gst/gst.h>

using namespace testing;

class AGstMyFilter : public Test {
public:
  void SetUp() override {
    myfilter = gst_element_factory_make("my_filter", "myfilter");
  }

  void TearDown() override {
    gst_element_set_state(myfilter, GST_STATE_NULL);
    gst_object_unref(myfilter);
  }

  GstElement *myfilter{nullptr};
};

TEST_F(AGstMyFilter, canCreatedByFactory) {
  GstElement *filter = gst_element_factory_make("my_filter", "myfilter");

  ASSERT_THAT(filter, NotNull());
  gst_object_unref(filter);
}

TEST_F(AGstMyFilter, hasSinkPadTemplate) {
  auto *pad_template = gst_element_get_pad_template(myfilter, "sink");

  ASSERT_THAT(pad_template, NotNull());
}

TEST_F(AGstMyFilter, sinkPadTemplateAsExpected) {
  auto *pad_template = gst_element_get_pad_template(myfilter, "sink");

  ASSERT_THAT(pad_template->name_template, StrEq("sink"));
  ASSERT_THAT(pad_template->direction, Eq(GstPadDirection::GST_PAD_SINK));
  ASSERT_THAT(pad_template->presence, Eq(GstPadPresence::GST_PAD_ALWAYS));
  ASSERT_TRUE(gst_caps_is_any(pad_template->caps));
}

TEST_F(AGstMyFilter, hasSourcePadTemplate) {
  auto *pad_template = gst_element_get_pad_template(myfilter, "src");

  ASSERT_THAT(pad_template, NotNull());
}

TEST_F(AGstMyFilter, sourcePadTemplateAsExpected) {
  auto *pad_template = gst_element_get_pad_template(myfilter, "src");

  ASSERT_THAT(pad_template->name_template, StrEq("src"));
  ASSERT_THAT(pad_template->direction, Eq(GstPadDirection::GST_PAD_SRC));
  ASSERT_THAT(pad_template->presence, Eq(GstPadPresence::GST_PAD_ALWAYS));
  ASSERT_TRUE(gst_caps_is_any(pad_template->caps));
}

TEST_F(AGstMyFilter, hasStaicSinkPad) {
  auto *sink_pad = gst_element_get_static_pad(myfilter, "sink");

  ASSERT_THAT(sink_pad, NotNull());
  gst_object_unref(sink_pad);
}

TEST_F(AGstMyFilter, hasStaicSourcePad) {
  auto *src_pad = gst_element_get_static_pad(myfilter, "src");

  ASSERT_THAT(src_pad, NotNull());
  gst_object_unref(src_pad);
}

TEST_F(AGstMyFilter, SilentPropertyAsExpected) {
  auto *object_class = G_OBJECT_GET_CLASS(myfilter);

  auto *silent_property = g_object_class_find_property(object_class, "silent");
  ASSERT_THAT(silent_property, NotNull());

  ASSERT_THAT(g_param_spec_get_name(silent_property), StrEq("silent"));
  ASSERT_THAT(g_param_spec_get_nick(silent_property), StrEq("Silent"));
  ASSERT_THAT(g_param_spec_get_blurb(silent_property),
              StrEq("Produce verbose output ?"));
  ASSERT_TRUE(silent_property->flags & G_PARAM_READWRITE);
  gboolean default_value =
      g_value_get_boolean(g_param_spec_get_default_value(silent_property));
  ASSERT_FALSE(default_value);
}

TEST_F(AGstMyFilter, CanSetSilentProperty) {
  g_object_set(myfilter, "silent", TRUE, nullptr);

  gboolean silent;
  g_object_get(myfilter, "silent", &silent, nullptr);

  ASSERT_TRUE(silent);
}

TEST_F(AGstMyFilter, MetadataIsAsExpected) {
  auto *longname =
      gst_element_get_metadata(myfilter, GST_ELEMENT_METADATA_LONGNAME);
  ASSERT_THAT(longname, StrEq("MyFilter"));

  auto *auth = gst_element_get_metadata(myfilter, GST_ELEMENT_METADATA_AUTHOR);
  ASSERT_THAT(auth, StrEq(" <<user@hostname.org>>"));

  auto *classification =
      gst_element_get_metadata(myfilter, GST_ELEMENT_METADATA_KLASS);
  ASSERT_THAT(classification, StrEq("Generic"));
}

TEST_F(AGstMyFilter, isNullStateAfterCreated) {
  GstState current;
  GstState pending;
  auto state_change_ret =
      gst_element_get_state(myfilter, &current, &pending, 0);

  ASSERT_THAT(state_change_ret, Eq(GST_STATE_CHANGE_SUCCESS));
  ASSERT_THAT(current, Eq(GST_STATE_NULL));
}

TEST_F(AGstMyFilter, canChangeStateToPlaying) {
  auto ret = gst_element_set_state(myfilter, GST_STATE_PLAYING);
  ASSERT_THAT(ret, Eq(GST_STATE_CHANGE_SUCCESS));

  GstState current;
  GstState pending;
  ret = gst_element_get_state(myfilter, &current, &pending, 0);

  ASSERT_THAT(current, Eq(GST_STATE_PLAYING));
}

TEST_F(AGstMyFilter, sinkPadCanHandleBufferDataWhenPlaying) {
    auto buffer_size = 1024;
    auto* buffer = gst_buffer_new_allocate(NULL, buffer_size, NULL);
    auto* sink_pad = gst_element_get_static_pad(myfilter, "sink");
    gst_element_set_state(myfilter, GST_STATE_PLAYING);

    auto ret = gst_pad_chain(sink_pad, buffer);

    ASSERT_THAT(ret, Eq(GST_FLOW_NOT_LINKED));
    gst_object_unref(sink_pad);
}

TEST_F(AGstMyFilter, canCanHandleEventWhenPlaying) {
  gst_element_set_state(myfilter, GST_STATE_PLAYING);
  auto* event = gst_event_new_eos();
  auto* sink_pad = gst_element_get_static_pad(myfilter, "sink");
  gboolean ret = gst_pad_send_event(sink_pad, event);

  ASSERT_THAT(ret, Eq(FALSE));
  gst_object_unref(sink_pad);
}