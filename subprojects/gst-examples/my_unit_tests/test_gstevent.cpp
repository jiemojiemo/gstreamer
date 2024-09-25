//
// Created by user on 9/25/24.
//
#include "ScopeGuard.h"
#include <gmock/gmock.h>
#include <gst/gstevent.h>

using namespace testing;

class AGstEvent : public Test {
public:
};

TEST_F(AGstEvent, CanCreateWithType) {
  auto type = GST_EVENT_FLUSH_START;
  auto *event = gst_event_new_custom(type, nullptr);
  ON_SCOPE_EXIT([event] { gst_event_unref(event); });

  ASSERT_THAT(event, NotNull());
  ASSERT_THAT(GST_EVENT_TYPE(event), Eq(type));
}

TEST_F(AGstEvent, CanCreateWithTypeAndStructure) {
  auto type = GST_EVENT_CUSTOM_UPSTREAM;
  auto *s = gst_structure_new("test", "message", G_TYPE_STRING, "hello", NULL);
  auto *event = gst_event_new_custom(type, s);
  ON_SCOPE_EXIT([event] { gst_event_unref(event); });

  ASSERT_THAT(event, NotNull());
  ASSERT_THAT(GST_EVENT_TYPE(event), Eq(type));
  ASSERT_THAT(gst_event_get_structure(event), NotNull());
}

TEST_F(AGstEvent, GetWriteableStructureIfCreateWithStructure) {
  auto *s = gst_structure_new("test", "message", G_TYPE_STRING, "hello", NULL);
  auto *event = gst_event_new_custom(GST_EVENT_CUSTOM_DOWNSTREAM, s);
  ON_SCOPE_EXIT([event] { gst_event_unref(event); });

  ASSERT_THAT(gst_event_writable_structure(event), Eq(s));
}

TEST_F(AGstEvent, GetWriteableStructureCreateNewOneIfInitWithStructure) {
  auto *event = gst_event_new_custom(GST_EVENT_CUSTOM_DOWNSTREAM, NULL);
  ON_SCOPE_EXIT([event] { gst_event_unref(event); });

  auto *s = gst_event_writable_structure(event);

  ASSERT_THAT(s, NotNull());
}