//
// Created by user on 9/22/24.
//
#include "ScopeGuard.h"
#include <gmock/gmock.h>
#include <gst/gstatomicqueue.h>
#include <gst/gstbus.h>
#include <thread>

using namespace testing;

class AGstBus : public Test {
public:
  void SetUp() override {
    bus = gst_bus_new();
    auto *s =
        gst_structure_new("test", "message", G_TYPE_STRING, "hello", NULL);
    app_msg = gst_message_new_application(NULL, s);

    auto *s1 =
        gst_structure_new("test", "message", G_TYPE_STRING, "hello", NULL);
    other_msg = gst_message_new_custom(GST_MESSAGE_EOS, NULL, s1);
  }

  void TearDown() override {
    gst_object_unref(bus);
    //    gst_message_unref(app_msg);
  }
  GstBus *bus = nullptr;
  GstMessage *app_msg = nullptr;
  GstMessage *other_msg = nullptr;
  std::function<GstBusSyncReply(GstBus *bus, GstMessage *message,
                                gpointer user_data)>
      handler;
  std::atomic<int> count{0};
};

TEST_F(AGstBus, PopNullIfNoMessage) { ASSERT_THAT(gst_bus_pop(bus), IsNull()); }

TEST_F(AGstBus, NotPendingIfNoMessage) {
  ASSERT_FALSE(gst_bus_have_pending(bus));
}

TEST_F(AGstBus, PopNullIfBusHasNoMessage) {
  ASSERT_THAT(gst_bus_pop(bus), IsNull());
}

TEST_F(AGstBus, PendingAfterPostAMessageButNoPopped) {
  gst_bus_post(bus, app_msg);

  ASSERT_TRUE(gst_bus_have_pending(bus));
}

TEST_F(AGstBus, CanPopAMessageAfterPostMessage) {
  gst_bus_post(bus, app_msg);

  auto *popped_m = gst_bus_pop(bus);
  ON_SCOPE_EXIT([popped_m] { gst_message_unref(popped_m); });

  ASSERT_THAT(popped_m, NotNull());
  ASSERT_THAT(GST_MESSAGE_TYPE(popped_m), Eq(GST_MESSAGE_APPLICATION));
  const auto *message_str =
      gst_structure_get_string(gst_message_get_structure(popped_m), "message");
  ASSERT_THAT(message_str, StrEq("hello"));
}

TEST_F(AGstBus, PopMessageWillClearPending) {
  gst_bus_post(bus, app_msg);
  gst_bus_pop(bus);

  ASSERT_FALSE(gst_bus_have_pending(bus));
}

TEST_F(AGstBus, PopMessageDecreasePendingCount) {
  gst_bus_post(bus, app_msg);

  ASSERT_THAT(gst_bus_pop(bus), NotNull());
  ASSERT_THAT(gst_bus_pop(bus), IsNull());
}

TEST_F(AGstBus, PeekReturnsNullIsNoMessage) {
  ASSERT_THAT(gst_bus_peek(bus), IsNull());
}

TEST_F(AGstBus, CanPeekAMessageAfterPostMessage) {
  gst_bus_post(bus, app_msg);

  auto *peak_m = gst_bus_peek(bus);
  ON_SCOPE_EXIT([peak_m] { gst_message_unref(peak_m); });

  ASSERT_THAT(peak_m, NotNull());
  ASSERT_THAT(GST_MESSAGE_TYPE(peak_m), Eq(GST_MESSAGE_APPLICATION));
  const auto *message_str =
      gst_structure_get_string(gst_message_get_structure(peak_m), "message");
  ASSERT_THAT(message_str, StrEq("hello"));
}

TEST_F(AGstBus, PeekNotDecreasePendingCount) {
  gst_bus_post(bus, app_msg);

  auto *m0 = gst_bus_peek(bus);
  ON_SCOPE_EXIT([m0] { gst_message_unref(m0); });

  auto *m1 = gst_bus_peek(bus);
  ON_SCOPE_EXIT([m1] { gst_message_unref(m1); });

  ASSERT_THAT(m0, NotNull());
  ASSERT_THAT(m1, NotNull());
  ASSERT_THAT(m0, Eq(m1));
}

TEST_F(AGstBus, PopWithFilteredWithTypeReturnNullIsNoSuchTypeInQueue) {
  gst_bus_post(bus, other_msg);

  auto *m = gst_bus_pop_filtered(bus, GST_MESSAGE_APPLICATION);

  ASSERT_THAT(m, IsNull());
}

TEST_F(AGstBus, PopWithTypeReturnsFilterdMessage) {
  gst_bus_post(bus, app_msg);
  gst_bus_post(bus, other_msg);

  auto *m = gst_bus_pop_filtered(bus, GST_MESSAGE_APPLICATION);

  ASSERT_THAT(m, NotNull());
  ASSERT_THAT(GST_MESSAGE_TYPE(m), Eq(GST_MESSAGE_APPLICATION));
  ASSERT_THAT(m, Eq(app_msg));
}

TEST_F(AGstBus, TimePopWithTimeNoneWillWaitUntilHasMessage) {
  auto t = std::thread([this] {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    gst_bus_post(bus, app_msg);
  });
  auto *m = gst_bus_timed_pop(bus, GST_CLOCK_TIME_NONE);
  ON_SCOPE_EXIT([m] { gst_message_unref(m); });

  ASSERT_THAT(m, NotNull());
  ASSERT_THAT(m, Eq(app_msg));
  t.join();
}

TEST_F(AGstBus, TimePopWithTimePopNullIfTimeout) {
  auto t = std::thread([this] {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    gst_bus_post(bus, app_msg);
  });
  auto *m = gst_bus_timed_pop(bus, GST_USECOND * 10);
  ASSERT_THAT(m, IsNull());
  t.join();

  auto *m1 = gst_bus_pop(bus);
  ON_SCOPE_EXIT([m1] { gst_message_unref(m1); });
  ASSERT_THAT(m1, NotNull());
}

TEST_F(AGstBus, TimedPopFilterdWithTimeNoneWillWaitUntilHasMessage) {
  auto t = std::thread([this] {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    gst_bus_post(bus, app_msg);
    gst_bus_post(bus, other_msg);
  });
  auto *m = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                       GST_MESSAGE_APPLICATION);
  ON_SCOPE_EXIT([m] { gst_message_unref(m); });

  ASSERT_THAT(m, NotNull());
  ASSERT_THAT(m, Eq(app_msg));
  t.join();
}

TEST_F(AGstBus, TimedPopFilterdWithTimePopNullIfTimeout) {
  auto t = std::thread([this] {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    gst_bus_post(bus, other_msg);
  });
  auto *m = gst_bus_timed_pop_filtered(bus, GST_USECOND * 10,
                                       GST_MESSAGE_APPLICATION);
  ASSERT_THAT(m, IsNull());
  t.join();

  auto *m1 = gst_bus_pop(bus);
  ON_SCOPE_EXIT([m1] { gst_message_unref(m1); });
  ASSERT_THAT(m1, NotNull());
}

TEST_F(AGstBus, SetFlushingWillClearPendingMessages) {
  gst_bus_post(bus, app_msg);
  gst_bus_set_flushing(bus, TRUE);

  ASSERT_FALSE(gst_bus_have_pending(bus));
  ASSERT_THAT(gst_bus_pop(bus), IsNull());
}

GstBusSyncReply test_bus_sync_handler(GstBus *bus, GstMessage *message,
                                      gpointer user_data) {
  auto *self = static_cast<AGstBus *>(user_data);
  return self->handler(bus, message, user_data);
}

TEST_F(AGstBus, SetDropSyncHandlerAndMainThreadWillNotReceiveMessage) {
  handler = [](GstBus *bus, GstMessage *message, gpointer user_data) {
    return GST_BUS_DROP;
  };
  gst_bus_set_sync_handler(bus, test_bus_sync_handler, this, nullptr);
  gst_bus_post(bus, app_msg);

  ASSERT_THAT(gst_bus_pop(bus), IsNull());
}

TEST_F(AGstBus, SetPassSyncHandlerAndMainThreadWillReceiveMessage) {
  handler = [](GstBus *bus, GstMessage *message, gpointer user_data) {
    return GST_BUS_PASS;
  };
  gst_bus_set_sync_handler(bus, test_bus_sync_handler, this, nullptr);
  gst_bus_post(bus, app_msg);

  auto *m = gst_bus_pop(bus);
  ON_SCOPE_EXIT([m] { gst_message_unref(m); });

  ASSERT_THAT(m, NotNull());
  ASSERT_THAT(m, Eq(app_msg));
}

static void test_sync_message(GstBus *bus, GstMessage *message) {
  printf("debug xxxxx");
}

TEST_F(AGstBus, AsyncSyncHandlerWillBlockThreadUntilPostedMsgFreed) {
  handler = [](GstBus *bus, GstMessage *message, gpointer user_data) {
    return GST_BUS_ASYNC;
  };
  gst_bus_set_sync_handler(bus, test_bus_sync_handler, this, nullptr);

  auto t = std::thread([this] {
    // block here until main thread handle this message
    gst_bus_post(bus, app_msg);
  });

  auto *m = gst_bus_timed_pop(bus, GST_CLOCK_TIME_NONE);
  ASSERT_THAT(m, NotNull());
  ASSERT_THAT(m, Eq(app_msg));

  // free this message and unlock the thread
  gst_message_unref(m);
  t.join();
}
static gboolean
message_func_eos (GstBus * bus, GstMessage * message, guint * p_counter)
{
  const GstStructure *s;
  gint i;

  g_return_val_if_fail (GST_MESSAGE_TYPE (message) == GST_MESSAGE_EOS, FALSE);

  s = gst_message_get_structure (message);
  if (!gst_structure_get_int (s, "msg_id", &i))
    g_critical ("Invalid message");

  if (p_counter != NULL)
    *p_counter += 1;

  return i != 9;
}

static gboolean
message_func_app (GstBus * bus, GstMessage * message, guint * p_counter)
{
  const GstStructure *s;
  gint i;

  g_return_val_if_fail (GST_MESSAGE_TYPE (message) == GST_MESSAGE_APPLICATION,
                       FALSE);

  s = gst_message_get_structure (message);
  if (!gst_structure_get_int (s, "msg_id", &i))
    g_critical ("Invalid message");

  if (p_counter != NULL)
    *p_counter += 1;

  return i != 9;
}

static gboolean
send_messages (gpointer data)
{
  auto* self = static_cast<AGstBus*>(data);
  GstMessage *m;
  GstStructure *s;
  gint i;

  for (i = 0; i < 10; i++) {
    s = gst_structure_new ("test_message", "msg_id", G_TYPE_INT, i, NULL);
    m = gst_message_new_application (NULL, s);
    gst_bus_post (self->bus, m);
    s = gst_structure_new ("test_message", "msg_id", G_TYPE_INT, i, NULL);
    m = gst_message_new_custom (GST_MESSAGE_EOS, NULL, s);
    gst_bus_post (self->bus, m);
  }

  return FALSE;
}

TEST_F(AGstBus, xxxx)
{
  guint num_eos = 0;
  guint num_app = 0;
  auto* main_loop = g_main_loop_new(NULL, FALSE);
  auto id = gst_bus_add_watch (bus, gst_bus_async_signal_func, NULL);
  ASSERT_THAT(id, Not(0));

  g_signal_connect (bus, "message::eos", (GCallback) message_func_eos,
                   &num_eos);
  g_signal_connect (bus, "message::application",
                   (GCallback) message_func_app, &num_app);

  g_idle_add ((GSourceFunc) send_messages, this);
  while (g_main_context_pending (NULL))
    g_main_context_iteration (NULL, FALSE);

  ASSERT_THAT(num_eos, Eq(10));
  ASSERT_THAT(num_app, Eq(10));

  gst_bus_remove_watch(bus);
  g_main_loop_unref (main_loop);
}

//
// TEST_F(AGstBus, CanGetSyncMessageSignalsFromSyncSignalHandler)
//{
//  g_signal_connect(bus, "message", G_CALLBACK(test_sync_message), this);
//
//  gst_bus_sync_signal_handler(bus, app_msg, this);
//}

// TEST_F(AGstBus, SetAyncHandlerWillBlockMainUntilMessageAyncHandler)
//{
//   handler = [](GstBus * bus, GstMessage * message, gpointer user_data) {
//     auto* self = static_cast<AGstBus*>(user_data);
//     self->count++;
//     return GST_BUS_ASYNC;
//   };
//   gst_bus_set_sync_handler(bus, test_bus_sync_handler, this, nullptr);
//   gst_bus_post(bus, app_msg);
//
//   std::this_thread::sleep_for(std::chrono::milliseconds(100));
//   ASSERT_THAT(count, Eq(1));
// }