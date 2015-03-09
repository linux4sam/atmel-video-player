/*
 * Copyright (C) 2013-2015 Atmel Corporation.
 *
 * This file is licensed under the terms of the Atmel LIMITED License Agreement
 * as written in the COPYING file that is delivered in the project root directory.
 *
 * DISCLAIMER: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
 * See the COPYING license file for more details.
 */

#ifndef TOOLS_H
#define TOOLS_H

#include <fstream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cstdlib>
#include <cstring>
#include <QWidget>
#include <QFile>
#include <QTextStream>

#include <string.h> /* for strncpy */
#include <iostream>
#include <string>
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <linux/fb.h>

/**
 * @file tools.h
 * @brief Colection of functions to perform common tasks
 */

/**
 *  @brief
 *  Tools
 * */
class Tools
{
public:
    Tools();

    virtual ~Tools();

    // Stores CPU usage
    float  cpu_usage[4];

    /**
      * Enables the selected layer with the given dimensions
      *
      * @param w Layer width
      * @param h Layer height
      * @param layer Layer number (0 = base, 1 = ovr1, 2 = ovr2)
      *
      * @return True if succeded
      **/
    bool enableLayer(int w, int h, int layer);


    /**
        * Set the nonstd according to the givin dimensions
        *
        * @param w Layer width
        * @param h Layer height
        * @param layer Layer number (0 = base, 1 = ovr1, 2 = ovr2)
        *
        * @return True if succeded
        **/
    bool setStatdard(int w, int h, int layer);

    /**
      * Looks into filepath for key and returns into value the characters between {}.\n
      * sintax: key={value}\n
      *
      *@param filepath complete file path
      *@param key string to be found
      *@param value returned text (if found)
      *@return true if success
      */
    bool getKey(const char* filepath, std::string key, QString &value);

    /**
      * Convert an int value to a std::string
      *
      *@param value int value
      *@return std_string
      */
    std::string to_string(int value);

    /**
      * Print a string to std output
      *
      * @param message Message to be printed
      *
      **/
    void printToStdOUT(QString message);


    /**
      * Updates the CPU Usage
      * The value is stored in the public cpu_usage array
      **/
    void updateCpuUsage();

private:
    QString res_spec;

    // For CPU usage
    float total_cpu_last_time[4];
    float work_cpu_last_time[4];
};

#endif // TOOLS_H
