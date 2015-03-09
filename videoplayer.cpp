/*
 * Copyright (C) 2013-2015 Atmel Corporation.
 *
 * This file is licensed under the terms of the Atmel LIMITED License Agreement
 * as written in the COPYING file that is delivered in the project root directory.
 *
 * DISCLAIMER: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
 * See the COPYING license file for more details.
 */

// NOTE: this class is using gstreamer naming convention

#include "videoplayer.h"

#define SRC_NAME "srcVideo"
#define PIPE "uridecodebin name=" SRC_NAME " uri=file:///videos/pdahd.h264.800x480.noaudio.avi caps=video/x-h264,stream-format=byte-stream,alignment=au ! \
h264parse ! g1h264dec ! video/x-raw,width=800,height=480,framerate=3000/1001 ! progressreport silent=true do-query=true update-freq=1 format=time name=progress ! perf name=p ! identity sync=true ! fbdevsink sync=false"


static gboolean
busCallback (GstBus *bus, 
	     GstMessage *message,
	     gpointer    data)
{
  VideoPlayer *_this = (VideoPlayer *)data;

  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR: {
      GError *err;
      gchar *debug;

      gst_message_parse_error (message, &err, &debug);
      g_print ("Error: %s\n", err->message);
      g_error_free (err);
      g_free (debug);

     // g_main_loop_quit (loop);
      break;
    }
    case GST_MESSAGE_EOS:
      /* end-of-stream */
      break;
    case GST_MESSAGE_ELEMENT:{
      const GstStructure *info =  gst_message_get_structure (message);
      if(gst_structure_has_name(info, "progress")){
	  const GValue *vcurrent;
          gint64 current = 0;
	  const GValue *vtotal;
          gint64 total = 0;

	  vtotal = gst_structure_get_value (info, "total");
	  total = g_value_get_int64(vtotal);
	  vcurrent = gst_structure_get_value (info, "current");
	  current = g_value_get_int64(vcurrent);

	  emit _this->positionChanged(current);
	  emit _this->durationChanged(total);
      }
      break;
    }
    case GST_MESSAGE_INFO: {
      if (!strncmp(GST_MESSAGE_SRC_NAME(message), "p", 1)) {
	GError *error = NULL;
	gchar *debug = NULL;
	gchar *bps = NULL;
	gchar *fps = NULL;
	gchar *perf = NULL;
	
	gst_message_parse_info (message, &error, &debug);

	fps = g_strrstr (debug, "fps: ") + 5;
    g_debug ("Frames per second: %s\n", fps);

	/* Hack to save up a little performance and tell strrstr to stop 
	 * instead of doing an auxiliary search
	 */
	*((gchar *)(fps-7)) = '\0';
	bps = g_strrstr (debug, "Bps: ") + 5;
    g_debug ("Bytes per second: %s\n", bps);

    perf = g_strdup_printf ("Video Statistics\n\nfps: %s\n\n"
                "bps: %s", fps, bps);
	emit _this->fpsChanged(QString::fromStdString(perf));	

	g_error_free (error);
	g_free (debug);
	g_free (perf);
      }
    }
    break;
    default:
      /* unhandled message */
      break;
  }

  /* we want to be notified again the next time there is a message
   * on the bus, so returning TRUE (FALSE means we want to stop watching
   * for messages on the bus and our callback should not be called again)
   */
  return TRUE;
}

VideoPlayer::VideoPlayer()
{
    gst_init(NULL, NULL);

    this->_videoPipeline = NULL;
    this->_src = NULL;
}

bool
VideoPlayer::createPipeline(){
    GError *error = NULL;
    GstBus *bus;
    guint bus_watch_id;

    /* Make sure we don't leave orphan references */
    destroyPipeline();

    this->_videoPipeline = gst_parse_launch (PIPE, &error);
    if (!this->_videoPipeline) {
        this->_videoPipeline = NULL;
        printf("failed to create video pipeline...\n");
        return false;
    }

    this->_src = gst_bin_get_by_name(GST_BIN(this->_videoPipeline), SRC_NAME);
    if (!this->_src) {
        this->_src = NULL;
        printf("failed to get video src element...\n");
        return false;
    }

    bus = gst_pipeline_get_bus (GST_PIPELINE (this->_videoPipeline));
    bus_watch_id = gst_bus_add_watch (bus, &busCallback, this);
    gst_object_unref (bus);

    return true;
}

void
VideoPlayer::destroyPipeline(){
    if (this->_videoPipeline) {
        this->null();
        g_object_unref(this->_src);
        g_object_unref(this->_videoPipeline);
        this->_src = NULL;
        this->_videoPipeline = NULL;
    }
}

VideoPlayer::~VideoPlayer(){
    destroyPipeline();
}

bool VideoPlayer::play(){
    emit playState(this->setState(GST_STATE_PLAYING));
}

bool VideoPlayer::pause(){
    emit playState(!this->setState(GST_STATE_PAUSED));
}

bool VideoPlayer::null(){
    return this->setState(GST_STATE_NULL);
}


bool VideoPlayer::setMedia(QString uri){
    destroyPipeline();
    createPipeline();
    g_object_set(this->_src, "uri", qPrintable("file://" + uri), NULL);

    return true;
}


bool VideoPlayer::setState(GstState state){
    GstStateChangeReturn ret;
    if(this->_videoPipeline){
        ret = gst_element_set_state(this->_videoPipeline, state);
        if (GST_STATE_CHANGE_FAILURE == ret) {
            printf ("Unable to set video pipeline pipeline to %d!\n", state);
            return false;
        }
    }else return false;

    return true;
}
