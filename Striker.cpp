//
// Created by nvidia on 4/1/19.
//

#include "Striker.h"

void Striker::sleep_ms(unsigned int time) {
    chrono::milliseconds timespan(time);
    this_thread::sleep_for(timespan);
}

unsigned int Striker::getAcceleration(unsigned int x) {
    return ((x * 7) + 100) * 10;
}

int Striker::getTargetPosition(unsigned int x) {
    return -((x * 5) + 100);
}

void Striker::LogError(string functionName, int p_lResult, unsigned int p_ulErrorCode) {
    cerr << functionName << " failed arm " << armID << ", motor " << motorID << " (result=" << p_lResult
         << ", errorCode=0x" << std::hex << p_ulErrorCode << ")" << endl;
}

void Striker::LogInfo(string message) {
    cout << message << endl;
}

void Striker::SetDefaultParameters() {
    g_usNodeId = 1;
    g_deviceName = "EPOS4";
    g_protocolStackName = "MAXON SERIAL V2";
    g_interfaceName = "USB";
    g_portName = "USB0";
    g_baudrate = 1000000;
}

int Striker::OpenDevice(unsigned int *p_pErrorCode) {
    int lResult = MMC_FAILED;

    char *pDeviceName = new char[255];
    char *pProtocolStackName = new char[255];
    char *pInterfaceName = new char[255];
    char *pPortName = new char[255];

    strcpy(pDeviceName, g_deviceName.c_str());
    strcpy(pProtocolStackName, g_protocolStackName.c_str());
    strcpy(pInterfaceName, g_interfaceName.c_str());
    strcpy(pPortName, g_portName.c_str());

    LogInfo("Open device...");

    g_pKeyHandle = VCS_OpenDevice(pDeviceName, pProtocolStackName, pInterfaceName, pPortName, p_pErrorCode);

    if (g_pKeyHandle != 0 && *p_pErrorCode == 0) {
        unsigned int lBaudrate = 0;
        unsigned int lTimeout = 0;

        if (VCS_GetProtocolStackSettings(g_pKeyHandle, &lBaudrate, &lTimeout, p_pErrorCode) != 0) {
            if (VCS_SetProtocolStackSettings(g_pKeyHandle, g_baudrate, lTimeout, p_pErrorCode) != 0) {
                if (VCS_GetProtocolStackSettings(g_pKeyHandle, &lBaudrate, &lTimeout, p_pErrorCode) != 0) {
                    if (g_baudrate == lBaudrate) {
                        lResult = MMC_SUCCESS;
                    }
                }
            }
        }
    } else {
        g_pKeyHandle = 0;
    }

    delete[]pDeviceName;
    delete[]pProtocolStackName;
    delete[]pInterfaceName;
    delete[]pPortName;

    return lResult;
}

int Striker::CloseDevice(unsigned int *p_pErrorCode) {
    int lResult = MMC_FAILED;

    *p_pErrorCode = 0;

    LogInfo("Close device");

    if (VCS_CloseDevice(g_pKeyHandle, p_pErrorCode) != 0 && *p_pErrorCode == 0) {
        lResult = MMC_SUCCESS;
    }

    return lResult;
}

int Striker::DemoProfilePositionMode(HANDLE p_DeviceHandle, unsigned short p_usNodeId, unsigned int &p_rlErrorCode) {
    int lResult = MMC_SUCCESS;
    stringstream msg;

    msg << "set profile position mode, node = " << p_usNodeId;
    LogInfo(msg.str());

    if (VCS_ActivateProfilePositionMode(p_DeviceHandle, p_usNodeId, &p_rlErrorCode) == 0) {
        LogError("VCS_ActivateProfilePositionMode", lResult, p_rlErrorCode);
        lResult = MMC_FAILED;
    } else {
        for (unsigned int i = 0; i < midi_velocity.size(); i++) {
            long targetPosition = getTargetPosition(midi_velocity.at(i));
            unsigned int acc = getAcceleration(midi_velocity.at(i));
            stringstream msg;
            msg << "move to position = " << targetPosition << ", acc = " << acc << ", time = " << timeInterval.at(i);
            LogInfo(msg.str());

            if (VCS_SetPositionProfile(p_DeviceHandle, p_usNodeId, velocity, acc, acc, &p_rlErrorCode) == 0) {
                LogError("VCS_SetPositionProfile", lResult, p_rlErrorCode);
                lResult = MMC_FAILED;
                break;
            }

            if (VCS_MoveToPosition(p_DeviceHandle, p_usNodeId, hit, 1, 1, &p_rlErrorCode) == 0) {
                LogError("VCS_MoveToPosition", lResult, p_rlErrorCode);
                lResult = MMC_FAILED;
                break;
            }

            if (VCS_WaitForTargetReached(p_DeviceHandle, p_usNodeId, 500, &p_rlErrorCode) == 0) {
                LogError("VCS_WaitForTargetReached", lResult, p_rlErrorCode);
                lResult = MMC_FAILED;
                break;
            }

            if (VCS_MoveToPosition(p_DeviceHandle, p_usNodeId, targetPosition, 1, 1, &p_rlErrorCode) == 0) {
                LogError("VCS_MoveToPosition", lResult, p_rlErrorCode);
                lResult = MMC_FAILED;
                break;
            }

            sleep_ms(timeInterval[i]);
        }
    }

    sleep_ms(1000);

    if (VCS_MoveToPosition(p_DeviceHandle, p_usNodeId, 0, 1, 1, &p_rlErrorCode) == 0) {
        LogError("VCS_MoveToPosition", lResult, p_rlErrorCode);
        lResult = MMC_FAILED;
    }

    return lResult;
}


int Striker::Prepare(unsigned int *p_pErrorCode) {
//    int lResult = MMC_SUCCESS;
    BOOL oIsFault = 0;

    if (VCS_GetFaultState(g_pKeyHandle, g_usNodeId, &oIsFault, p_pErrorCode) == 0) {
        LogError("VCS_GetFaultState", lResult, *p_pErrorCode);
        lResult = MMC_FAILED;
    }

    if (lResult == 0) {
        if (oIsFault) {
            stringstream msg;
            msg << "clear fault, node = '" << g_usNodeId << "'";
            LogInfo(msg.str());
            if (VCS_ClearFault(g_pKeyHandle, g_usNodeId, p_pErrorCode) == 0) {
                LogError("VCS_ClearFault", lResult, *p_pErrorCode);
                lResult = MMC_FAILED;
            }
        }

        if (lResult == 0) {
            BOOL oIsEnabled = 0;

            if (VCS_GetEnableState(g_pKeyHandle, g_usNodeId, &oIsEnabled, p_pErrorCode) == 0) {
                LogError("VCS_GetEnableState", lResult, *p_pErrorCode);
                lResult = MMC_FAILED;
            }

            if (lResult == 0) {
                if (!oIsEnabled) {
                    if (VCS_SetEnableState(g_pKeyHandle, g_usNodeId, p_pErrorCode) == 0) {
                        LogError("VCS_SetEnableState", lResult, *p_pErrorCode);
                        lResult = MMC_FAILED;
                    } else if (setHome(p_pErrorCode) != MMC_SUCCESS) {
                        LogError("setHome", lResult, *p_pErrorCode);
                    }
                }
            }
        }
    }
    return lResult;
}



int Striker::Demo() {
//    int lResult = MMC_SUCCESS;
//    unsigned int lErrorCode = 0;

    lResult = DemoProfilePositionMode(g_pKeyHandle, g_usNodeId, ulErrorCode);

    if (lResult != MMC_SUCCESS) {
        LogError("DemoProfilePositionMode", lResult, ulErrorCode);
    } else {
        if (VCS_SetDisableState(g_pKeyHandle, g_usNodeId, &ulErrorCode) == 0) {
            LogError("VCS_SetDisableState", lResult, ulErrorCode);
            lResult = MMC_FAILED;
        }
    }

    return lResult;
}

Striker::Striker(short armID, bool motorID) {
    this->armID = armID;
    this->motorID = motorID;
    SetDefaultParameters();
    if ((lResult = OpenDevice(&ulErrorCode)) != MMC_SUCCESS) {
        LogError("OpenDevice", lResult, ulErrorCode);
    }
}

int Striker::setHome(unsigned int *p_pErrorCode) {
    lResult = VCS_DefinePosition(g_pKeyHandle, g_usNodeId, 0, p_pErrorCode);
    return lResult;
}

int Striker::setCurrent(int value) {
    if (VCS_ActivateProfilePositionMode(p_DeviceHandle, p_usNodeId, &p_rlErrorCode) == 0) {
        LogError("VCS_ActivateProfilePositionMode", lResult, p_rlErrorCode);
        lResult = MMC_FAILED;
    }
}

