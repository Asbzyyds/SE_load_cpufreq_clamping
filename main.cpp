#include "utils.hpp"
Utils utils;

int main() {
    utils.Disable_Eas_Scheduler();
    while(true){

        auto initialStats = utils.readCpuStats();
        usleep(20000);
        auto finalStats = utils.readCpuStats();
        int totalLoad = utils.calculateTotalCpuLoad(initialStats, finalStats);

        if (totalLoad >= 75){
            utils.MaxCpuFreq(); 
        } 
        else {
            utils.MixCpuFreq();
        }

        sleep(1);
    }
    return 0;
}