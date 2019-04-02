//
// Created by nvidia on 4/1/19.
//

#ifndef SHIMON_STRIKER_H
#define SHIMON_STRIKER_H

#include <iostream>
#include "Definitions.h"
#include <string.h>
#include <sstream>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <thread>
#include <iomanip>

using namespace std;
//using namespace smf;

#ifndef MMC_SUCCESS
#define MMC_SUCCESS 0
#endif

#ifndef MMC_FAILED
#define MMC_FAILED 1
#endif

#ifndef MMC_MAX_LOG_MSG_SIZE
#define MMC_MAX_LOG_MSG_SIZE 512
#endif

class Striker {
public:
    typedef void *HANDLE;
    typedef int BOOL;

    void *g_pKeyHandle = 0;
    unsigned short g_usNodeId = 1;
    string g_deviceName;
    string g_protocolStackName;
    string g_interfaceName;
    string g_portName;
    unsigned int g_baudrate = 0;

    unsigned int velocity = 6000;
    int hit = 50;
    int homePosition = -250;
    short armID;
    bool motorID;
    unsigned int ulErrorCode = 0;
    int lResult = MMC_FAILED;
    vector<unsigned int> midi_velocity;
    vector<unsigned int> timeInterval;

public:
    Striker(short armID, bool motorID);

    void LogError(string functionName, int p_lResult, unsigned int p_ulErrorCode);

    void LogInfo(string message);

    int OpenDevice(unsigned int *p_pErrorCode);

    int CloseDevice(unsigned int *p_pErrorCode);

    void SetDefaultParameters();

    int DemoProfilePositionMode(HANDLE p_DeviceHandle, unsigned short p_usNodeId, unsigned int &p_rlErrorCode);

    int Demo();

    int Prepare(unsigned int *p_pErrorCode);

    unsigned int getAcceleration(unsigned int x);

    int getTargetPosition(unsigned int x);

    void sleep_ms(unsigned int time);

    int setHome(unsigned int *p_pErrorCode);
};


#endif //SHIMON_STRIKER_H
