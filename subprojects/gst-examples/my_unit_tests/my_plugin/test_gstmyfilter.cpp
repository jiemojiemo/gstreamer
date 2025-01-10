//
// Created by user on 1/10/25.
//
#include <gst/gst.h>
#include <gmock/gmock.h>

using namespace testing;

class AGstMyFilter : public Test {
public:
    void SetUp() override {
        myfilter = gst_element_factory_make("my_filter", "myfilter");
    }

    void TearDown() override {
        gst_object_unref(myfilter);
    }

    GstElement* myfilter{nullptr};
};

TEST_F(AGstMyFilter, canCreatedByFactory) {
    GstElement* filter = gst_element_factory_make("my_filter", "myfilter");

    ASSERT_THAT(filter, NotNull());
    gst_object_unref(filter);
}

TEST_F(AGstMyFilter, hasSinkPadTemplate) {
    auto* pad_template = gst_element_get_pad_template(myfilter, "sink");

    ASSERT_THAT(pad_template, NotNull());
}

TEST_F(AGstMyFilter, sinkPadTemplateAsExpected)
{
    auto* pad_template = gst_element_get_pad_template(myfilter, "sink");

    ASSERT_THAT(pad_template->name_template, StrEq("sink"));
    ASSERT_THAT(pad_template->direction, Eq(GstPadDirection::GST_PAD_SINK));
    ASSERT_THAT(pad_template->presence, Eq(GstPadPresence::GST_PAD_ALWAYS));
    ASSERT_TRUE(gst_caps_is_any(pad_template->caps));
}

TEST_F(AGstMyFilter, hasSourcePadTemplate) {
    auto* pad_template = gst_element_get_pad_template(myfilter, "src");

    ASSERT_THAT(pad_template, NotNull());
}

TEST_F(AGstMyFilter, sourcePadTemplateAsExpected)
{
    auto* pad_template = gst_element_get_pad_template(myfilter, "src");

    ASSERT_THAT(pad_template->name_template, StrEq("src"));
    ASSERT_THAT(pad_template->direction, Eq(GstPadDirection::GST_PAD_SRC));
    ASSERT_THAT(pad_template->presence, Eq(GstPadPresence::GST_PAD_ALWAYS));
    ASSERT_TRUE(gst_caps_is_any(pad_template->caps));
}

TEST_F(AGstMyFilter, hasStaicSinkPad) {
    auto* sink_pad = gst_element_get_static_pad(myfilter, "sink");

    ASSERT_THAT(sink_pad, NotNull());
}

TEST_F(AGstMyFilter, hasStaicSourcePad) {
    auto* sink_pad = gst_element_get_static_pad(myfilter, "src");

    ASSERT_THAT(sink_pad, NotNull());
}

TEST_F(AGstMyFilter, SilentPropertyAsExpected) {
    auto* object_class = G_OBJECT_GET_CLASS(myfilter);

    auto* silent_property = g_object_class_find_property(object_class, "silent");
    ASSERT_THAT(silent_property, NotNull());

    ASSERT_THAT(g_param_spec_get_name(silent_property), StrEq("silent"));
    ASSERT_THAT(g_param_spec_get_nick(silent_property), StrEq("Silent"));
    ASSERT_THAT(g_param_spec_get_blurb(silent_property), StrEq("Produce verbose output ?"));
    ASSERT_TRUE(silent_property->flags & G_PARAM_READWRITE);
    gboolean default_value = g_value_get_boolean(g_param_spec_get_default_value(silent_property));
    ASSERT_FALSE(default_value);
}

TEST_F(AGstMyFilter, CanSetSilentProperty) {
    g_object_set(myfilter, "silent", TRUE, nullptr);

    gboolean silent;
    g_object_get(myfilter, "silent", &silent, nullptr);

    ASSERT_TRUE(silent);
}
