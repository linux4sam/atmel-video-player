/*
 * Copyright (C) 2013-2015 Atmel Corporation.
 *
 * This file is licensed under the terms of the Atmel LIMITED License Agreement
 * as written in the COPYING file that is delivered in the project root directory.
 *
 * DISCLAIMER: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
 * See the COPYING license file for more details.
 */

#include "player.h"
#include <QApplication>
#include <QThread>

#define LCD_WIDTH   800
#define LCD_HEIGHT  480
#define LAYER       1

class GMainLoopThread : public QThread
{
    void run()
    {
        GMainLoop *mainloop;
  
        mainloop = g_main_loop_new(NULL, FALSE);
        g_main_loop_run(mainloop);
    }
};

int main(int argc, char *argv[])
{
    // Enable ovr1 layer for Qt
    Tools *tools = new Tools;
    GMainLoopThread *mainloop = new GMainLoopThread;

    tools->enableLayer(LCD_WIDTH,LCD_HEIGHT,LAYER);

    QApplication a(argc, argv);
    Player w;
    w.show();

    // Start listener for GStreamer messages
    mainloop->start();

    // Run QT event loop
    return a.exec();
}
