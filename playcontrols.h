/*
 * Copyright (C) 2013-2015 Atmel Corporation.
 *
 * This file is licensed under the terms of the Atmel LIMITED License Agreement
 * as written in the COPYING file that is delivered in the project root directory.
 *
 * DISCLAIMER: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
 * See the COPYING license file for more details.
 */

#ifndef PLAYCONTROLS_H
#define PLAYCONTROLS_H

#include <QWidget>

// Button images
#define PLAY_IMG    ":/playcontrols/images/Play.png"
#define PAUSE_IMG   ":/playcontrols/images/Pause.png"
#define MUTE_IMG    ":/playcontrols/images/speaker.png"
#define UNMUTE_IMG  ":/playcontrols/images/speaker_mute.png"

/**
 * @file playcontrols.h
 * @brief Video player play controls implementation
 */

namespace Ui {
class PlayControls;
}

class PlayControls : public QWidget
{
    Q_OBJECT

public:
    explicit PlayControls(QWidget *parent = 0);
    ~PlayControls();

    /**
     * @brief setCPUusage Sets the value in the CPU usage progress bar
     * @param usage Percentage of CPU utilization
     */
    void setCPUusage(int usage);

    /**
     * @brief allowSeek Allow the controls to emit a positionChanged signal when the slider is moved
     * @param allow true = allow seeking, fasle = don´t allow seeking (default true)
     */
    void allowSeek(bool allow);

    /**
     * @brief enableVolumeControl Enable/Disable the volume controls
     * @param enable True = volume controls enabled, false = volume controls disabled (default state enabled)
     */
    void enableVolumeControl(bool enable);

    /**
     * @brief getCurrentVolume
     * @return the current volume configured on the volume slider
     */
    int getCurrentVolume();

    /**
     * @brief getCurrentMute
     * @return the current mute state
     */
    bool getCurrentMute();

signals:
    void play();
    void pause();
    void setMute(bool mute);
    void setVolume(int volume);
    void positionChanged(int position);
    void setFullScreen();
    void showOptions();

private slots:
    void on_PlayPause_clicked();
    void on_muteButton_clicked();
    /**
     * @brief durationChanged UPdates the Media progress status bar maximum value and media duration label
     * @param duration Media duration
     */
    void durationChanged(qint64 duration);
    /**
     * @brief positionChanged Updates the Media progress status bar current position and Media position label
     * @param position Current meda position (position < = duration)
     */
    void positionChanged(qint64 position);

    void on_fullScreenButtonn_clicked();

    void on_exitButton_clicked();

public slots:
    /**
      * @brief set the controls to the current play state
      *
      * @param state true = onPlay, false = onPause
      */
    void setPlayState(bool state);

private:
    Ui::PlayControls *ui;

    // internal funcions
    void updatePositionLabel(int position);
    QString secondsToString(int seconds);
    void setPlayIcon(bool state);


    // Internal variables
    bool _onPlay;               // Video is playing
    bool _onMute;               // Video is paused
    int _totalDuration;         // Video total duration
    QString _duration;          // Video total duration (hh:mm:ss formated)
    int _currentPosition;       // Current position of the stream
    bool _allowSeek;            // Allow seek
};

#endif // PLAYCONTROLS_H
