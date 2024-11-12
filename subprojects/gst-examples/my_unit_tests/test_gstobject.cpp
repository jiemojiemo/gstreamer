//
// Created by user on 9/20/24.
//
#include "ScopeGuard.h"
#include <gmock/gmock.h>
#include <gst/gstobject.h>

using namespace testing;

typedef struct _GstFakeObjectClass GstFakeObjectClass;
typedef struct _GstFakeObject GstFakeObject;

struct _GstFakeObject {
  GstObject object;
};

struct _GstFakeObjectClass {
  GstObjectClass parent_class;
};

static GType gst_fake_object_get_type(void) {
  static gsize fake_object_type = 0;

  if (g_once_init_enter(&fake_object_type)) {
    GType type;
    static const GTypeInfo fake_object_info = {
        sizeof(GstFakeObjectClass),
        NULL, // gst_fake_object_base_class_init,
        NULL, // gst_fake_object_base_class_finalize,
        NULL, //(GClassInitFunc) gst_fake_object_class_init,
        NULL,
        NULL,
        sizeof(GstFakeObject),
        0,
        NULL, //(GInstanceInitFunc) gst_fake_object_init,
        NULL};

    type = g_type_register_static(GST_TYPE_OBJECT, "GstFakeObject",
                                  &fake_object_info, (GTypeFlags)0);
    g_once_init_leave(&fake_object_type, type);
  }
  return fake_object_type;
}

static void notify_name(GObject *object, GParamSpec *pspec, gint *out_count) {
  *out_count += 1;
}

static void notify_name2() { printf("xxx"); }

class AGstObject : public Test {
public:
  void SetUp() override {
    object = (GstObject *)g_object_new(gst_fake_object_get_type(), NULL);
  }

  void TearDown() override { gst_object_unref(object); }
  GstObject *object;
};

TEST_F(AGstObject, fakeObjectHasInitialName) {
  auto *name = gst_object_get_name(object);
  ON_SCOPE_EXIT([name] { g_free(name); });

  ASSERT_THAT(name, NotNull());
  ASSERT_THAT(name, StartsWith("fakeobject"));
}

TEST_F(AGstObject, SetNameWillNotify) {
  gint count = 0;
  g_signal_connect(object, "notify::name", G_CALLBACK(notify_name), &count);

  auto ok = gst_object_set_name(object, NULL);

  ASSERT_TRUE(ok);
  ASSERT_THAT(count, Eq(1));
}

TEST_F(AGstObject, SetNullNameWillStartsWithObjectName) {
  auto ok = gst_object_set_name(object, NULL);
  ASSERT_TRUE(ok);

  auto *name = gst_object_get_name(object);
  ON_SCOPE_EXIT([name] { g_free(name); });

  ASSERT_THAT(name, NotNull());
  ASSERT_THAT(name, StartsWith("fakeobject"));
}

TEST_F(AGstObject, CanSetNameViaSetProperty) {
  g_object_set(object, "name", "fake", NULL);

  auto *name = gst_object_get_name(object);
  ON_SCOPE_EXIT([name] { g_free(name); });

  ASSERT_THAT(name, StrEq("fake"));
}

TEST_F(AGstObject, SetNameViaSetPropertyWillNotify) {
  gint count = 0;
  g_signal_connect(object, "notify::name", G_CALLBACK(notify_name), &count);
  g_object_set(object, "name", "fake", NULL);

  ASSERT_THAT(count, Eq(1));
}

TEST_F(AGstObject, ReturnNameIsACopy) {
  auto *name = gst_object_get_name(object);
  ON_SCOPE_EXIT([name] { g_free(name); });
  name[0] = 'm';

  auto *name2 = gst_object_get_name(object);
  ON_SCOPE_EXIT([name2] { g_free(name2); });

  ASSERT_THAT(name2, StartsWith("fakeobject"));
}

