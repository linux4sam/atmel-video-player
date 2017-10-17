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
    GMainLoopThread *mainloop = new GMainLoopThread;

    QApplication a(argc, argv);

    Player w;
    w.show();

    // Start listener for GStreamer messages
    mainloop->start();

    // Run QT event loop
    return a.exec();
}
