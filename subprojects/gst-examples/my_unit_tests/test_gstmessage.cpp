//
// Created by user on 9/24/24.
//
#include "ScopeGuard.h"
#include <gmock/gmock.h>
#include <gst/gstmessage.h>
#include <gst/gst.h>

using namespace testing;

class AGstMessage : public Test {
public:
};

TEST_F(AGstMessage, CanCreateWithType) {
  GstMessageType type = GST_MESSAGE_EOS;
  GstMessage *message = gst_message_new_custom(type, nullptr, nullptr);
  ON_SCOPE_EXIT([message] { gst_message_unref(message); });
  ASSERT_THAT(GST_MESSAGE_TYPE(message), Eq(type));
}

TEST_F(AGstMessage, CanCreateWithTypeAndStructure) {
  GstMessageType type = GST_MESSAGE_APPLICATION;
  auto *s = gst_structure_new("test", "message", G_TYPE_STRING, "hello", NULL);
  GstMessage *message = gst_message_new_custom(type, nullptr, s);
  ON_SCOPE_EXIT([message] { gst_message_unref(message); });
  ASSERT_THAT(GST_MESSAGE_TYPE(message), Eq(type));
}

TEST_F(AGstMessage, SourceIsNullIfCreateWithNullSource) {
  GstMessage *message =
      gst_message_new_custom(GST_MESSAGE_APPLICATION, nullptr, nullptr);
  ON_SCOPE_EXIT([message] { gst_message_unref(message); });
  ASSERT_THAT(GST_MESSAGE_SRC(message), IsNull());
}

TEST_F(AGstMessage, StructureIsNuLlIfCreateWithNullStructure) {
  GstMessage *message =
      gst_message_new_custom(GST_MESSAGE_APPLICATION, nullptr, nullptr);
  ON_SCOPE_EXIT([message] { gst_message_unref(message); });
  ASSERT_THAT(gst_message_get_structure(message), IsNull());
}

TEST_F(AGstMessage, CanGetWritebalStructureIfCreateWithStructure) {
  auto *s = gst_structure_new("test", "message", G_TYPE_STRING, "hello", NULL);
  GstMessage *message =
      gst_message_new_custom(GST_MESSAGE_APPLICATION, nullptr, s);
  ON_SCOPE_EXIT([message] { gst_message_unref(message); });
  ASSERT_THAT(gst_message_writable_structure(message), Eq(s));
}

TEST_F(AGstMessage, CanGetWritebaleStrutureIfCreateWithNullStructure) {
  GstMessage *message =
      gst_message_new_eos(NULL);
  ON_SCOPE_EXIT([message] { gst_message_unref(message); });
  ASSERT_THAT(gst_message_writable_structure(message), NotNull());
}

TEST_F(AGstMessage, GetNullDetailsIfNeverSet) {
  GstMessage *message =
      gst_message_new_custom(GST_MESSAGE_APPLICATION, nullptr, nullptr);
  ON_SCOPE_EXIT([message] { gst_message_unref(message); });
  ASSERT_THAT(gst_message_get_details(message), IsNull());
}

TEST_F(AGstMessage, GetWritebleDetailsEventNotSet) {
  GstMessage *message =
      gst_message_new_custom(GST_MESSAGE_APPLICATION, nullptr, nullptr);
  ON_SCOPE_EXIT([message] { gst_message_unref(message); });
  ASSERT_THAT(gst_message_writable_details(message), NotNull());
}