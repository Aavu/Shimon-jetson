//
// Created by nvidia on 4/1/19.
//

#include "Striker.h"

int Striker::getID() {
    return 2 * armID + motorID;
}

void Striker::sleep_ms(unsigned int time) {
    chrono::milliseconds timespan(time);
    this_thread::sleep_for(timespan);
}

unsigned int Striker::getAcceleration(unsigned int x) {
    return (((x * -.714) - 59.29) * -27.78) + 833.33;
}

int Striker::getTargetPosition(unsigned int x) {
    return -((x * .714) + 59.29);
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

int Striker::OpenDevice() {
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

int Striker::CloseDevice() {
    int lResult = MMC_FAILED;

    LogInfo("Close device");

    if (VCS_SetDisableState(g_pKeyHandle, g_usNodeId, p_pErrorCode) == 0) {
        LogError("VCS_SetDisableState", lResult, *p_pErrorCode);
        lResult = MMC_FAILED;
    }

    if (VCS_CloseDevice(g_pKeyHandle, p_pErrorCode) != 0 && *p_pErrorCode == 0) {
        lResult = MMC_SUCCESS;
    }

    return lResult;
}

int Striker::Prepare() {
    BOOL oIsFault = 0;
    lResult = MMC_SUCCESS;
    if (VCS_GetFaultState(g_pKeyHandle, g_usNodeId, &oIsFault, p_pErrorCode) == 0) {
        LogError("VCS_GetFaultState", lResult, *p_pErrorCode);
        lResult = MMC_FAILED;
    }

    if (lResult == MMC_SUCCESS) {
        if (oIsFault) {
            stringstream msg;
            msg << "clear fault, node = '" << g_usNodeId << "'";
            LogInfo(msg.str());
            if (VCS_ClearFault(g_pKeyHandle, g_usNodeId, p_pErrorCode) == 0) {
                LogError("VCS_ClearFault", lResult, *p_pErrorCode);
                lResult = MMC_FAILED;
            }
        }

        if (lResult == MMC_SUCCESS) {
            BOOL oIsEnabled = 0;

            if (VCS_GetEnableState(g_pKeyHandle, g_usNodeId, &oIsEnabled, p_pErrorCode) == 0) {
                LogError("VCS_GetEnableState", lResult, *p_pErrorCode);
                lResult = MMC_FAILED;
            }

            if (lResult == MMC_SUCCESS) {
                if (!oIsEnabled) {
                    if (VCS_SetEnableState(g_pKeyHandle, g_usNodeId, p_pErrorCode) == 0) {
                        LogError("VCS_SetEnableState", lResult, *p_pErrorCode);
                        lResult = MMC_FAILED;
                    } else if (setHome() != MMC_SUCCESS) {
                        LogError("setHome", lResult, *p_pErrorCode);
                        lResult = MMC_FAILED;
                    }
                }
            }

            if (lResult == MMC_SUCCESS) {
                if (moveToPosition(-150, 5000) != MMC_SUCCESS) {
                    LogError("moveToPosition", lResult, *p_pErrorCode);
                    lResult = MMC_FAILED;
                }
            }
        }
    }
    return lResult;
}

Striker::Striker(short armID, bool motorID) {
    this->armID = armID;
    this->motorID = motorID;
    SetDefaultParameters();
    if ((lResult = OpenDevice()) != MMC_SUCCESS) {
        LogError("OpenDevice", lResult, *p_pErrorCode);
    }
}

int Striker::setHome() {
    LogInfo("Setting Home..");
    lResult = MMC_SUCCESS;
    int lastPosition = 10000;
    int currentPosition = -10000;
    while (true) {
        if (moveToPosition(25, 1000, 0) != MMC_SUCCESS) {
            LogError("moveToPosition", lResult, *p_pErrorCode);
            lResult = MMC_FAILED;
            return lResult;
        }
        if (VCS_GetPositionIs(g_pKeyHandle, g_usNodeId, &currentPosition, p_pErrorCode) == 0) {
            LogError("VCS_GetPositionIs", lResult, *p_pErrorCode);
            lResult = MMC_FAILED;
            return lResult;
        }
        if (abs(currentPosition - lastPosition) < 10) {
            break;
        }
        cout << "moving" << endl;
        lastPosition = currentPosition;
    }
    if (VCS_DefinePosition(g_pKeyHandle, g_usNodeId, 0, p_pErrorCode) == 0) {
        lResult = MMC_FAILED;
    }
    return lResult;
}

int Striker::setCurrent(short value) {
    lResult = MMC_SUCCESS;
    if (VCS_ActivateCurrentMode(g_pKeyHandle, g_usNodeId, p_pErrorCode) == 0) {
        LogError("VCS_ActivateProfilePositionMode", lResult, *p_pErrorCode);
        lResult = MMC_FAILED;
        return lResult;
    }

    stringstream msg;
    msg << "current = " << value;
    LogInfo(msg.str());

    if (VCS_SetCurrentMust(g_pKeyHandle, g_usNodeId, value, p_pErrorCode) == 0) {
        LogError("VCS_SetCurrentMust", lResult, *p_pErrorCode);
        lResult = MMC_FAILED;
        return lResult;
    }

    return lResult;
}

int Striker::hit(int m_velocity, int mode) {
    lResult = MMC_SUCCESS;
    if (mode == 0) {
        short current = getCurrent(m_velocity);
        if (setCurrent(current) != MMC_SUCCESS) {
            LogError("setCurrent", lResult, *p_pErrorCode);
            lResult = MMC_FAILED;
            return lResult;
        }

        sleep_ms(20);

        while (true) {
            int velocityIs = 6000;
            if (VCS_GetVelocityIs(g_pKeyHandle, g_usNodeId, &velocityIs, p_pErrorCode) == 0) {
                LogError("VCS_GetVelocityIs", lResult, *p_pErrorCode);
                lResult = MMC_FAILED;
                return lResult;
            }
            if (velocityIs < 20) {
                break;
            }
        }

        unsigned int acc = getAcceleration(m_velocity);
        int position = getTargetPosition(m_velocity);

        if (moveToPosition(position, acc) != MMC_SUCCESS) {
            LogError("moveToPosition", lResult, *p_pErrorCode);
            lResult = MMC_FAILED;
        }

    }
    return lResult;
}

int Striker::moveToPosition(int position, unsigned int acc, bool absolute) {
    lResult = MMC_SUCCESS;
    if (VCS_ActivateProfilePositionMode(g_pKeyHandle, g_usNodeId, p_pErrorCode) == 0) {
        LogError("VCS_ActivateProfilePositionMode", lResult, *p_pErrorCode);
        lResult = MMC_FAILED;
    } else {
        stringstream msg;
        msg << "move to position = " << position << ", acc = " << acc;
        LogInfo(msg.str());

        if (VCS_SetPositionProfile(g_pKeyHandle, g_usNodeId, velocity, acc, acc, p_pErrorCode) == 0) {
            LogError("VCS_SetPositionProfile", lResult, *p_pErrorCode);
            lResult = MMC_FAILED;
            return lResult;
        }

        if (VCS_MoveToPosition(g_pKeyHandle, g_usNodeId, position, absolute, 1, p_pErrorCode) == 0) {
            LogError("VCS_MoveToPosition", lResult, *p_pErrorCode);
            lResult = MMC_FAILED;
            return lResult;
        }

        if (VCS_WaitForTargetReached(g_pKeyHandle, g_usNodeId, 500, p_pErrorCode) == 0) {
            LogError("VCS_WaitForTargetReached", lResult, *p_pErrorCode);
            lResult = MMC_FAILED;
            return lResult;
        }
    }
    return lResult;
}

short Striker::getCurrent(int m_velocity) {
    return ((m_velocity * -.714) - 59.29) * -44.44 + 333.33;
}

