#pragma once 

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/inotify.h>

class Utils {
private:
    #define CORE_COUNT 8
    std::string logFilePath = "/storage/emulated/0/Android/MW/log.txt";

    void clearLogFile() {
        std::ofstream logFile(logFilePath, std::ofstream::out | std::ofstream::trunc);
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void writeLog(const std::string& message) {
        std::ofstream logFile(logFilePath, std::ofstream::app);
        if (logFile.is_open()) {
            logFile << message << std::endl;
            logFile.close();
        }
    }

public:
    Utils() {
        clearLogFile();
    }

    std::vector<std::vector<long>> readCpuStats() {
        std::ifstream file("/proc/stat");
        std::string line;
        std::vector<std::vector<long>> cpuStats(CORE_COUNT, std::vector<long>(4));

        int core = 0;
        while (std::getline(file, line)) {
            if (line.find("cpu") == 0 && line.find("cpu ") != 0) { 
                std::istringstream iss(line);
                std::string cpu;
                iss >> cpu;
                for (int i = 0; i < 4; ++i) {
                    iss >> cpuStats[core][i];
                }
                core++;
                if (core >= CORE_COUNT) break;
            }
        }
        return cpuStats;
    }

    int calculateTotalCpuLoad(const std::vector<std::vector<long>>& stats1, const std::vector<std::vector<long>>& stats2) {
        long total1 = 0, idle1 = 0;
        long total2 = 0, idle2 = 0;

        for (size_t i = 0; i < stats1.size(); ++i) {
            const long* coreStats1 = stats1[i].data(); 
            const long* coreStats2 = stats2[i].data(); 
            total1 += *(coreStats1) + *(coreStats1 + 1) + *(coreStats1 + 2) + *(coreStats1 + 3);
            idle1 += *(coreStats1 + 3);

            total2 += *(coreStats2) + *(coreStats2 + 1) + *(coreStats2 + 2) + *(coreStats2 + 3);
            idle2 += *(coreStats2 + 3);
        }

        long totalDiff = total2 - total1;
        long idleDiff = idle2 - idle1;

        if (totalDiff == 0) {
            return 0;
        }

        return static_cast<int>(((totalDiff - idleDiff) * 100) >> 1) / 50;
    }

    void WriteFile(const std::string& filePath, const std::string& content) noexcept {
        int fd = open(filePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd >= 0) {
            write(fd, content.data(), content.size());
            close(fd);
        }
    }

    void MaxCpuFreq() {
        for(int i = 0; i <= 7; ++i){   
            WriteFile("/sys/devices/system/cpu/cpufreq/policy" + std::to_string(i) + "/scaling_max_freq", "2147483647");
        }
    }

    void MixCpuFreq() {
        WriteFile("/sys/devices/system/cpu/cpufreq/policy0/scaling_max_freq", "1800000");
        for(int i = 1; i <= 7; ++i){
            WriteFile("/sys/devices/system/cpu/cpufreq/policy" + std::to_string(i) + "/scaling_max_freq", "1700000");
        }
    }

    void Disable_Eas_Scheduler() {
        WriteFile("/proc/sys/kernel/sched_energy_aware", "0");
        writeLog("EAS scheduler disabled.  EAS 已关闭。");
        writeLog("running......  运行中......");
    }
};