/*
 * Copyright (C) 2013-2015 Atmel Corporation.
 *
 * This file is licensed under the terms of the Atmel LIMITED License Agreement
 * as written in the COPYING file that is delivered in the project root directory.
 *
 * DISCLAIMER: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
 * See the COPYING license file for more details.
 */

#include "tools.h"
#include <unistd.h> 

Tools::Tools()
{
}


Tools::~Tools()
{
}


bool Tools::enableLayer(int w, int h, int layer){

    struct fb_var_screeninfo info;
    QString dev = QString("/dev/fb%1").arg(layer); // Frame buffer device
    int fb_fd;
    QRect r(0,0,w,h);

    // Get fb fd
    if ((fb_fd = open(qPrintable(dev), O_RDWR)) == -1) {
        printf("Could not open %s...\n",qPrintable(dev));
        return false;
    }

    // Get screen info
    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &info) == -1) {
        printf("Could not get screen info...\n");
        goto closefd;
    }

    // Set new screen info
    info.xres = w;
    info.yres = h;
    info.xres_virtual = w;
    info.yres_virtual = h;
    info.bits_per_pixel = 32;
    info.nonstd =  (1<< 31) | (r.x() << 10) | r.y();
    info.activate = FB_ACTIVATE_NOW|FB_ACTIVATE_FORCE;

    if (ioctl(fb_fd, FBIOPUT_VSCREENINFO, &info) == -1) {
        printf("Could not write screen info..");
        goto closefd;
    }

    return true;
closefd:
    close(fb_fd);
    return false;
}

bool Tools::setStatdard(int w, int h, int layer){
    struct fb_var_screeninfo info;
    QString dev = QString("/dev/fb%1").arg(layer); // Frame buffer device
    int fbfd;
    QRect r(0,0,w,h);

    // Get fb fd
    if ((fbfd = open(qPrintable(dev), O_RDWR)) == -1) {
        printf("Could not open %s...\n",qPrintable(dev));
        return false;
    }

    // Get screen info
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &info) == -1) {
        printf("Could not get screen info...\n");
        goto closefd;
    }

    unsigned int nonstd;
    nonstd = (1<< 31) | (r.x() << 10) | r.y();
    info.nonstd = nonstd;

    if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &info) == -1) {
        printf("Could not write screen info..");
        goto closefd;
    }

    return true;
closefd:
    ::close(fbfd);
    return false;
}


bool Tools::getKey(const char* filepath, std::string key, QString& value){

    std::ifstream file;
    std::string read;
    size_t  start_pos;
    size_t  end_pos;

    file.open(filepath);

    if(!file.is_open()) {
        printf("\nCould not open file...");
        return false;
    }

    while(file.good()){

        getline(file,read); // Read Line

        start_pos = read.find(key); // Find Key

        if(start_pos == std::string::npos) continue;    // Key not fount

        start_pos = read.find("{"); // First delimiter
        end_pos = read.find("}"); // Last delimiter

        if(start_pos == std::string::npos || end_pos == std::string::npos){
            printf("\nDelimiters not found...");
            break;
        }

        read = read.substr(start_pos + 1, end_pos - start_pos - 1); // Read value

        value = QString::fromUtf8(read.c_str());
        return true;    // Key found

    }

    return false;   // Key not found

}


void Tools::printToStdOUT(QString message){
    QTextStream(stdout) << message << endl;
}


std::string Tools::to_string(int value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}

void Tools::updateCpuUsage()
{
    std::ifstream *fp = new std::ifstream("/proc/stat");
    char cpu_id[5]; // CPU to search
    char* line_buffer;
    int i = 0;      // CPU counter
    int times[7];

    // Time variables
    int total_time;
    int work_time;

    std::string line;
    if (fp->is_open())
    {
        while ( std::getline (*fp,line) && i<1 ) // End of file or 4 cpus founded
        {
            sprintf(cpu_id, "cpu%d", i);  // Get CPU ID

            if(line.find(cpu_id) != std::string::npos){

                // Pass line to line buffer
                line_buffer = new char[line.size() + 1];
                strcpy (line_buffer, line.c_str());

                // Split data and get total and work time
                strtok(line_buffer, " ");
                // Get all 7 values
                times[0] = atoi(strtok(NULL, " "));
                times[1] = atoi(strtok(NULL, " "));
                times[2] = atoi(strtok(NULL, " "));
                times[3] = atoi(strtok(NULL, " "));
                times[4] = atoi(strtok(NULL, " "));
                times[5] = atoi(strtok(NULL, " "));
                times[6] = atoi(strtok(NULL, " "));
                work_time = times[0] + times[1] + times[2];
                total_time = work_time + times[3] + times[4] + times[5] + times[6];

                // Update CPU Usage
                cpu_usage[i] = ((float)work_time - work_cpu_last_time[i]) / ((float)total_time -total_cpu_last_time[i]) * 100;

                // Update last values
                work_cpu_last_time[i] = (float)work_time;
                total_cpu_last_time[i] = (float)total_time;

                i++; // Next CPU
            }

        }
        fp->close();
    }
}
