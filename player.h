/*
 * Copyright (C) 2013-2015 Atmel Corporation.
 *
 * This file is licensed under the terms of the Atmel LIMITED License Agreement
 * as written in the COPYING file that is delivered in the project root directory.
 *
 * DISCLAIMER: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
 * See the COPYING license file for more details.
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <QMainWindow>
#include <QToolBar>
#include <QTimer>
#include <QFileDialog>
#include <QLabel>
#include <QKeyEvent>

#include "tools.h"
#include "playcontrols.h"
#include "videoplayer.h"


/**
 * @file player.h
 * @brief Video player main class implementation
 */

/**
  * @mainpage Video Player Demo
 *  @details
 *  @version   1.0
 *  @date      August 2014
 *
 *
 * \section contents Contents
 * - \ref introsec
 *
 * \section introsec Introduction
 *
 * Library for the managment of common tasks
 *
 */

namespace Ui {
class Player;
}

class Player : public QMainWindow
{
    Q_OBJECT

public:
    explicit Player(QWidget *parent = 0);
    ~Player();

private:
    Ui::Player *ui;

    Tools *_tools;

    QToolBar        *_toolBar;           // To hold the play controls
    QLabel          *_fpsL;          // To display video statistics
    PlayControls    *_controls;          // Play controls
    VideoPlayer     *_videoplayer;       // gstreamer implementation
    QTimer          *_cpuTimer;          // To update CPU usage
    QTimer *_hideControlsTimer;          // Hide controls after some time with no activity
    int            _hideTimeout;         // Time to hide controls (in seconds)
    QFileDialog     *_openFileDialog;    // select media file


    void setUpPlayControls();
    void setUpFpsLabel();
    void mousePressEvent (QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);


private slots:
    void updateCPUusage();
    void fpsChanged(QString perf);
    void onControlsTimeout();
    void onSetFullScreen();
    void onOpenFile();

};

#endif // PLAYER_H
