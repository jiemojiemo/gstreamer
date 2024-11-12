//
// Created by user on 9/25/24.
//
#include <gtest/gtest.h>
#include <gst/gst.h>

void init_gst_test(int argc, char **argv)
{
  gboolean _gst_check_list_tests = FALSE;
  guint timeout_multiplier = 1;
  GOptionContext *ctx;
  GError *err = NULL;
  GOptionEntry options[] = {{"list-tests", 'l', 0, G_OPTION_ARG_NONE,
                             &_gst_check_list_tests,
                             "List tests present in the testsuite", NULL},
                            {NULL}};
  guint i;
  printf("yyyy");

  ctx = g_option_context_new("gst-check");
  g_option_context_add_main_entries(ctx, options, NULL);
  g_option_context_add_group(ctx, gst_init_get_option_group());

  if (!g_option_context_parse(ctx, &argc, &argv, &err)) {
    if (err)
      g_printerr("Error initializing: %s\n", GST_STR_NULL(err->message));
    else
      g_printerr("Error initializing: Unknown error!\n");
    g_clear_error(&err);
  }
  g_option_context_free(ctx);
}


// 全局初始化代码
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  init_gst_test(argc, argv);

  return RUN_ALL_TESTS();
}