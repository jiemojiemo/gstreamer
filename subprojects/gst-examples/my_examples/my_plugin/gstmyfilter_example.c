//
// Created by user on 1/18/25.
//

#include <gst/gst.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

int
tutorial_main (int argc, char *argv[])
{
  GstElement *pipeline;
  GstBus *bus;
  GstMessage *msg;

  /* Initialize GStreamer */
  gst_init (&argc, &argv);

  /* Build the pipeline */
  pipeline = gst_pipeline_new("pipeline");
  GstElement* audio_test_src = gst_element_factory_make("audiotestsrc", "audio_test_src");
  GstElement* my_filter = gst_element_factory_make("my_filter", "my_filter");
  GstElement* audio_sink = gst_element_factory_make("autoaudiosink", "audio_sink");

  if(!pipeline || !audio_test_src || !my_filter || !audio_sink) {
    g_printerr ("Not all elements could be created.\n");
    return -1;
  }

  gst_bin_add_many(GST_BIN(pipeline), audio_test_src, my_filter, audio_sink, NULL);
  gboolean ok = gst_element_link_many(audio_test_src, my_filter, audio_sink, NULL);
  if(!ok) {
    g_printerr ("Elements could not be linked.\n");
    gst_object_unref (pipeline);
    return -1;
  }

  g_object_set(audio_test_src, "wave", 12, NULL);
  g_object_set(my_filter, "delay", 1000.0f, NULL);
  g_object_set(my_filter, "feedback", 0.2f, NULL);

  /* Start playing */
  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  /* Wait until error or EOS */
  bus = gst_element_get_bus (pipeline);
  msg =
      gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
      GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

  /* See next tutorial for proper error message handling/parsing */
  if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR) {
    g_printerr ("An error occurred! Re-run with the GST_DEBUG=*:WARN "
        "environment variable set for more details.\n");
  }

  /* Free resources */
  gst_message_unref (msg);
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  return 0;
}

int
main (int argc, char *argv[])
{
#if defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE
  return gst_macos_main ((GstMainFunc) tutorial_main, argc, argv, NULL);
#else
  return tutorial_main (argc, argv);
#endif
}
