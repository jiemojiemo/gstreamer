#include <gst/gst.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

int tutorial_main(int argc, char *argv[]) {
  GstElement *pipeline;
  GstElement *audio_source;
  GstElement *queue;
  GstElement *visual;
  GstElement *video_convert;
  GstElement *video_sink;

  /* Initialize GStreamer */
  gst_init(&argc, &argv);

  // create elements
  audio_source = gst_element_factory_make("audiotestsrc", "audio_source");
  queue = gst_element_factory_make("queue", "queue");
  visual = gst_element_factory_make("wavescope", "visual");
  video_convert = gst_element_factory_make("videoconvert", "video_convert");
  video_sink = gst_element_factory_make("autovideosink", "video_sink");

  // create pipeline
  pipeline = gst_pipeline_new("test-pipeline");

  if (!pipeline || !audio_source || !queue ||!visual || !video_convert || !video_sink) {
    g_printerr("Not all elements could be created.\n");
    return -1;
  }

  // configure elements
  g_object_set(audio_source, "freq", 215.0f, NULL);
  g_object_set(visual, "shader", 0, "style", 1, NULL);

  // link elements
  gst_bin_add_many(GST_BIN(pipeline), audio_source, queue, visual, video_convert, video_sink, NULL);
  if(gst_element_link_many(audio_source, queue, visual, video_convert, video_sink, NULL) != TRUE) {
    g_printerr("Elements could not be linked.\n");
    gst_object_unref(pipeline);
    return -1;
  }

  // start playing
  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  // wait until error or EOS
  GstBus* bus = gst_element_get_bus(pipeline);
  GstMessage* msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

  // release resources
  if(msg != NULL) {
    gst_message_unref(msg);
  }
  gst_object_unref(bus);
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);

  return 0;
}

int main(int argc, char *argv[]) {
#if defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE
  return gst_macos_main((GstMainFunc)tutorial_main, argc, argv, NULL);
#else
  return tutorial_main(argc, argv);
#endif
}
