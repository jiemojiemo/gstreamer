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
  GstElement *tee;
  GstElement *queue2;
  GstElement *visual2;
  GstElement *video_convert2;
  GstElement *video_sink2;

  /* Initialize GStreamer */
  gst_init(&argc, &argv);

  // create elements
  audio_source = gst_element_factory_make("audiotestsrc", "audio_source");
  tee = gst_element_factory_make("tee", "tee");

  queue = gst_element_factory_make("queue", "queue");
  visual = gst_element_factory_make("wavescope", "visual");
  video_convert = gst_element_factory_make("videoconvert", "video_convert");
  video_sink = gst_element_factory_make("autovideosink", "video_sink");

  queue2 = gst_element_factory_make("queue", "queue2");
  visual2 = gst_element_factory_make("wavescope", "visual2");
  video_convert2 = gst_element_factory_make("videoconvert", "video_convert2");
  video_sink2 = gst_element_factory_make("autovideosink", "video_sink2");

  // create pipeline
  pipeline = gst_pipeline_new("test-pipeline");

  if (!pipeline || !audio_source || !tee || !queue || !visual ||
      !video_convert || !video_sink || !queue2 || !visual2 || !video_convert2 ||
      !video_sink2) {
    g_printerr("Not all elements could be created.\n");
    return -1;
  }

  // configure elements
  g_object_set(audio_source, "freq", 215.0f, NULL);
  g_object_set(visual, "shader", 0, "style", 1, NULL);
  g_object_set(visual2, "shader", 0, "style", 3, NULL);

  // link elements
  gst_bin_add_many(GST_BIN(pipeline), audio_source, tee, queue, visual,
                   video_convert, video_sink, queue2, visual2, video_convert2,
                   video_sink2, NULL);

  if (gst_element_link_many(audio_source, tee, NULL) != TRUE) {
    g_printerr("Elements could not be linked.\n");
    gst_object_unref(pipeline);
    return -1;
  }

  // link first branch
  if (gst_element_link_many(queue, visual, video_convert, video_sink, NULL) !=
      TRUE) {
    g_printerr("Elements could not be linked.\n");
    gst_object_unref(pipeline);
    return -1;
  }

  // link second branch
  if (gst_element_link_many(queue2, visual2, video_convert2, video_sink2,
                            NULL) != TRUE) {
    g_printerr("Elements could not be linked.\n");
    gst_object_unref(pipeline);
    return -1;
  }

  /* Manually link the Tee, which has "Request" pads */
  GstPad *tee_audio_pad = gst_element_request_pad_simple(tee, "src_%u");
  g_print("Obtained request pad %s for first audio branch.\n",
          gst_pad_get_name(tee_audio_pad));
  GstPad *queue_audio_pad = gst_element_get_static_pad(queue, "sink");

  GstPad *tee_audio_pad2 = gst_element_request_pad_simple(tee, "src_%u");
  g_print("Obtained request pad %s for second audio branch.\n",
          gst_pad_get_name(tee_audio_pad));
  GstPad *queue_audio_pad2 = gst_element_get_static_pad(queue2, "sink");
  if (gst_pad_link(tee_audio_pad, queue_audio_pad) != GST_PAD_LINK_OK ||
      gst_pad_link(tee_audio_pad2, queue_audio_pad2) != GST_PAD_LINK_OK) {
    g_printerr("Tee could not be linked.\n");
    gst_object_unref(pipeline);
    return -1;
  }
  gst_object_unref(queue_audio_pad);
  gst_object_unref(queue_audio_pad2);

  // start playing
  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  // wait until error or EOS
  GstBus *bus = gst_element_get_bus(pipeline);
  GstMessage *msg = gst_bus_timed_pop_filtered(
      bus, GST_CLOCK_TIME_NONE,
      (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

  gst_element_release_request_pad(tee, tee_audio_pad);
  gst_element_release_request_pad(tee, tee_audio_pad2);
  gst_object_unref(tee_audio_pad);
  gst_object_unref(tee_audio_pad2);

  // release resources
  if (msg != NULL) {
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
