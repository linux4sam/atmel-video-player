/*
 * Copyright (C) 2013-2015 Atmel Corporation.
 *
 * This file is licensed under the terms of the Atmel LIMITED License Agreement
 * as written in the COPYING file that is delivered in the project root directory.
 *
 * DISCLAIMER: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
 * See the COPYING license file for more details.
 */

#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include <gst/gst.h>
#include <stdio.h>


/**
 * @file videoplayer.h
 * @brief Player  gstreamer class
 */


class VideoPlayer : public QObject
{
    Q_OBJECT
public:
    VideoPlayer(int gem = -1, int width = -1, int height = -1);
    ~VideoPlayer();


    /**
     * @brief Sets the media file URI to the current pipeline
     * @param uri file URI
     * @return true if success
     */
    bool setMedia(QString uri);

signals:
    void durationChanged(qint64 duration);      // Media Duration changed
    void positionChanged(qint64 position);      // Media position changed
    void fpsChanged(QString fps);      // Media position changed
    void playState(int state);                 // pipeline play state (true = onPlay, false = onPause)

public slots:
    /**
     * @brief play Send pipeline to play state
     * @return  true if success
     */
    bool play(bool mute,int volume);
    /**
     * @brief pause Send Pipeline to pause state
     * @return true is success
     */
    bool pause();
    /**
     * @brief null Send pipeline to null state
     * @return true if success
     */
    bool null();

    /**
     * @brief Adjusts the volume of the audio in the video being played
     * @param volume desired volume in the range of 0 (no sound) to 100 (normal sound)
     * @return true if success
     */
    bool setVolume(int volume);

    /**
     * @brief Mutes the audio of the video being played
     * @param mute true if the audio is to be muted
     * @return true if success
     */
    bool setMute(bool mute);

private:
    GstElement *_videoPipeline;      // Video Pipeline
    GstElement *_src;                // Source element
    GstElement *_volume;             // Volume controller
    int _gem, _width, _height;
    bool setState(GstState state);
    bool createPipeline();
    void destroyPipeline();

};

#endif // VIDEOPLAYER_H
