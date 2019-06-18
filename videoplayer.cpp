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
#include <QDesktopWidget>

#define SRC_NAME "srcVideo"
#define PERF_NAME "perf"
#define PROGRESS_NAME "progress"
#define VOLUME_NAME "volume"

#if defined PLANA || defined PLANC
#define PIPE "uridecodebin expose-all-streams=false name=" SRC_NAME " \
caps=video/x-h264\\;audio/x-raw " SRC_NAME ". ! queue ! h264parse ! \
queue ! g1h264dec ! video/x-raw,width=%2,height=%3,format=BGRx ! \
progressreport silent=true do-query=true update-freq=1 format=time \
name=" PROGRESS_NAME " ! perf name=" PERF_NAME " ! \
g1kmssink gem-name=%1 " SRC_NAME ". ! queue ! audioconvert ! \
audio/x-raw,format=S16LE ! volume name=" VOLUME_NAME " ! \
alsasink async=false enable-last-sample=false"
#else
#define PIPE "uridecodebin expose-all-streams=false name=" SRC_NAME " \
caps=video/x-h264\\;audio/x-raw " SRC_NAME ". ! queue ! h264parse ! \
queue ! g1h264dec ! video/x-raw,width=%2,height=%3 ! \
progressreport silent=true do-query=true update-freq=1 format=time \
name=" PROGRESS_NAME " ! perf name=" PERF_NAME " ! \
g1fbdevsink zero-memcpy=true max-lateness=-1 async=false \
enable-last-sample=false "
#endif


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
    case GST_MESSAGE_EOS:{
        /// end-of-stream
#if ((defined PLANA || defined PLAND) && defined FPSVIEW)
          gchar *perf = NULL;
          perf = g_strdup_printf ("fps: 0\n"
                      "bps (Mbit/s): 0");
          emit _this->fpsChanged(QString::fromStdString(perf));
#endif //PLANA
          emit _this->playState(5);
          _this->setMedia(QString("/opt/VideoPlayer/media/Microchip-masters.mp4"));

      break;
    }
    case GST_MESSAGE_ELEMENT:{
#if defined PLANA || defined PLAND
      const GstStructure *info =  gst_message_get_structure (message);
      if(gst_structure_has_name(info, PROGRESS_NAME)){
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
#endif //PLANA
      break;
    }
    case GST_MESSAGE_INFO: {
#if ((defined PLANA || defined PLAND) && defined FPSVIEW)
      if (!strncmp(GST_MESSAGE_SRC_NAME(message), PERF_NAME, 1)) {
	GError *error = NULL;
	gchar *debug = NULL;
	gchar *bps = NULL;
	gchar *fps = NULL;
	gchar *perf = NULL;
	int bpsi;
	
	gst_message_parse_info (message, &error, &debug);

	fps = g_strrstr (debug, "fps: ") + 5;
    g_debug ("Frames per second: %s\n", fps);

	/* Hack to save up a little performance and tell strrstr to stop 
	 * instead of doing an auxiliary search
	 */
	*((gchar *)(fps-7)) = '\0';
	bps = g_strrstr (debug, "Bps: ") + 5;
    g_debug ("Bytes per second: %s\n", bps);
        bpsi = atoi(bps);
	bpsi = bpsi/1000000;
    perf = g_strdup_printf ("fps: %s\n"
                "bps (Mbit/s): %d", fps, bpsi);
	emit _this->fpsChanged(QString::fromStdString(perf));	

	g_error_free (error);
	g_free (debug);
	g_free (perf);
      }
#endif //PLANA

    break;

    }
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

VideoPlayer::VideoPlayer(int gem, int width, int height)
        : _gem(gem),
		  _width(width),
		  _height(height)		
{
    gst_init(NULL, NULL);

    this->_videoPipeline = NULL;
    this->_src = NULL;
    this->_volume = NULL;
}

bool
VideoPlayer::createPipeline(){
    GError *error = NULL;
    GstBus *bus;
    /* Make sure we don't leave orphan references */
    destroyPipeline();

	printf("gem = %d _width= %d _height=%d \n", _gem, _width, _height);

    QString pipe(QString(PIPE).arg(_gem).arg(_width).arg(_height));
    printf("%s\n", pipe.toStdString().c_str());
    this->_videoPipeline = gst_parse_launch (pipe.toStdString().c_str(), &error);
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

    this->_volume = gst_bin_get_by_name(GST_BIN(this->_videoPipeline), VOLUME_NAME);
    if (!this->_volume) {
        this->_volume = NULL;
        printf("failed to get volume element...\n");
        return false;
    }

    bus = gst_pipeline_get_bus (GST_PIPELINE (this->_videoPipeline));
    gst_bus_add_watch (bus, &busCallback, this);
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

bool VideoPlayer::play(bool mute,int volume){

    emit playState(this->setState(GST_STATE_PLAYING));
    setVolume(volume);
    setMute(mute);
    return true;
}

bool VideoPlayer::pause(){
    emit playState(!this->setState(GST_STATE_PAUSED));
    return true;
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

bool VideoPlayer::setVolume(int volume) {
  if (!this->_volume)
    return false;

  g_object_set(this->_volume, "volume", volume/100.0, NULL);
  return true;
}

bool VideoPlayer::setMute(bool mute) {
  if (!this->_volume)
    return false;

  g_object_set(this->_volume, "mute", mute, NULL);
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
