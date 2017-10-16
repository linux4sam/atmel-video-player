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
#include <xf86drm.h>
#include <engine.h>
#include <qpa/qplatformnativeinterface.h>
#include <kms.h>

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

static int get_fd()
{
    static int dri_fd = -1;
    if (dri_fd == -1)
    {
        void *p = reinterpret_cast<QApplication*>(QApplication::instance())->platformNativeInterface()->nativeResourceForIntegration("dri_fd");
        if (p) {
            dri_fd = (int)(qintptr)p;
            qDebug("dri fd = %d", dri_fd);
        } else {
            qFatal("No dri fd available from platform");
        }
    }
    return dri_fd;
}

static int setup_planes()
{
    struct kms_device *device;
    int fd;
    const char* config_file = "/opt/ApplicationLauncher/applications/xml/default.config";
    uint32_t framedelay = 10000;
    struct plane_data* planes;

    fd = get_fd();

    if (fd < 0) {
        fprintf(stderr, "error: no dri fd\n");
        return 1;
    }

    device = kms_device_open(fd);
    if (!device)
        return 1;

    planes = (struct plane_data*)calloc(device->num_planes, sizeof(struct plane_data));

    if (parse_config(config_file, device, planes, &framedelay))
        return 1;

    return 0;
}

int main(int argc, char *argv[])
{
    // Enable ovr1 layer for Qt
    //Tools *tools = new Tools;
    GMainLoopThread *mainloop = new GMainLoopThread;

    //tools->enableLayer(LCD_WIDTH,LCD_HEIGHT,LAYER);

    QApplication a(argc, argv);

    if (setup_planes())
        fprintf(stderr, "error: setup_planes() failed\n");
    else
        fprintf(stderr, "setup_planes()\n");

    Player w;
    w.show();

    // Start listener for GStreamer messages
    mainloop->start();

    // Run QT event loop
    return a.exec();
}
