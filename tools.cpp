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
#include <sstream>
#include <fstream>
#include <cstring>

using namespace std;

Tools::Tools()
{
}


Tools::~Tools()
{
}

void Tools::updateCpuUsage()
{
    std::ifstream fp("/proc/stat");

    if (fp.is_open())
    {
        int i = 0;      // CPU counter
        std::string line;
        while ( std::getline (fp,line) && i<1 ) // End of file or 4 cpus founded
        {
            istringstream ss(line);
            string field0;
            char cpu_id[5]; // CPU to search
            ss >> field0;

            sprintf(cpu_id, "cpu%d", i);  // Get CPU ID

            if (field0.find(cpu_id) != std::string::npos){
                int times[7];
                for (auto &x : times)
                    ss >> x;
                int work_time = times[0] + times[1] + times[2];
                int total_time = work_time + times[3] + times[4] + times[5] + times[6];

                // Update CPU Usage
                cpu_usage[i] = ((float)work_time - work_cpu_last_time[i]) / ((float)total_time -total_cpu_last_time[i]) * 100;

                // Update last values
                work_cpu_last_time[i] = (float)work_time;
                total_cpu_last_time[i] = (float)total_time;

                i++; // Next CPU
            }

        }
        fp.close();
    }
}
