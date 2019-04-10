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
//    return (unsigned int) round((((x * -.714) - 59.29) * -27.78) + 833.33);
    if (strikerMode == Normal) return (int) round((x * -4.762 - 95.238) * -5.83 + 916.67);
    if (strikerMode == Medium) return (int) round((x * -1.35 - 78.65) * -29.4 - 1352.94);
    return 0;
}

int Striker::getTargetPosition(unsigned int x) {
//    return (int) round(-((x * .714) + 59.29));
    if (strikerMode == Normal) return (int) round(x * -4.762 - 95.238);
    if (strikerMode == Medium) return (int) round(x * -1.35 - 78.65);
    return 0;
}

void Striker::LogError(const string &functionName, int p_lResult, unsigned int p_ulErrorCode) {
    cerr << functionName << " failed arm " << armID << ", motor " << motorID << " (result=" << p_lResult
         << ", errorCode=0x" << std::hex << p_ulErrorCode << ")" << endl;
}

void Striker::LogInfo(const string &message) {
    cout << message << endl;
}

void Striker::SetDefaultParameters() {
    g_usNodeId = 1;
    g_deviceName = "EPOS4";
    g_protocolStackName = "MAXON SERIAL V2";
    g_interfaceName = "USB";

    g_portName = "USB" + to_string(getID());
    cout << g_portName << endl;
    g_baudrate = 1000000;
}

int Striker::OpenDevice() {
    lResult = MMC_FAILED;

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

    if (g_pKeyHandle != nullptr && *p_pErrorCode == 0) {
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
        g_pKeyHandle = nullptr;
    }

    delete[]pDeviceName;
    delete[]pProtocolStackName;
    delete[]pInterfaceName;
    delete[]pPortName;

    return lResult;
}

int Striker::CloseDevice() {
    lResult = MMC_FAILED;

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

int Striker::tremolo(int m_velocity) {
    while (strikerMode == Tremolo) {
        lResult = MMC_SUCCESS;
        int current = getCurrent(m_velocity);
        if (setCurrent(current) != MMC_SUCCESS) {
            LogError("setCurrent", lResult, *p_pErrorCode);
            lResult = MMC_FAILED;
            return lResult;
        }

        sleep_ms(5);

        if (setCurrent(-current) != MMC_SUCCESS) {
            LogError("setCurrent", lResult, *p_pErrorCode);
            lResult = MMC_FAILED;
            return lResult;
        }

        sleep_ms(15);

        lResult = waitTillHit(30);
    }
    return lResult;
}

int Striker::hit(unsigned int m_velocity, StrikerModes mode) {
    lResult = MMC_SUCCESS;
    strikerMode = mode;

    if (strikerMode == Normal || strikerMode == Medium) {
        int current = getCurrent(m_velocity);
        if (setCurrent(current) != MMC_SUCCESS) {
            LogError("setCurrent", lResult, *p_pErrorCode);
            lResult = MMC_FAILED;
            return lResult;
        }

        sleep_ms(20);

        lResult = waitTillHit(20);

        unsigned int acc = getAcceleration(m_velocity);
        int position = getTargetPosition(m_velocity);

        if (moveToPosition(position, acc) != MMC_SUCCESS) {
            LogError("moveToPosition", lResult, *p_pErrorCode);
            lResult = MMC_FAILED;
        }

    } else if (strikerMode == Tremolo) { // tremolo
        thread{pTremolo, m_velocity}.detach();
    }
    return lResult;
}

int Striker::moveToPosition(int position, unsigned int acc, bool absolute) {
    lResult = MMC_SUCCESS;
    if (VCS_ActivateProfilePositionMode(g_pKeyHandle, g_usNodeId, p_pErrorCode) == 0) {
        lResult = MMC_FAILED;
        LogError("VCS_ActivateProfilePositionMode", lResult, *p_pErrorCode);
    } else {
        stringstream msg;
        msg << "move to position = " << position << ", acc = " << acc;
        LogInfo(msg.str());

        if (VCS_SetPositionProfile(g_pKeyHandle, g_usNodeId, velocity, acc, acc, p_pErrorCode) == 0) {
            lResult = MMC_FAILED;
            LogError("VCS_SetPositionProfile", lResult, *p_pErrorCode);
            return lResult;
        }

        if (VCS_MoveToPosition(g_pKeyHandle, g_usNodeId, position, absolute, 1, p_pErrorCode) == 0) {
            lResult = MMC_FAILED;
            LogError("VCS_MoveToPosition", lResult, *p_pErrorCode);
            return lResult;
        }

        if (VCS_WaitForTargetReached(g_pKeyHandle, g_usNodeId, 1000, p_pErrorCode) == 0) {
            lResult = MMC_FAILED;
            LogError("VCS_WaitForTargetReached", lResult, *p_pErrorCode);
            return lResult;
        }
    }
    return lResult;
}

int Striker::getCurrent(int m_velocity) {
    if (strikerMode == Normal) return round(((m_velocity * -4.76) - 95.24) * -7.33 + 133.33);
    if (strikerMode == Medium) return round(((m_velocity * -1.35) - 78.65) * -29.4 - 1352.94);
    if (strikerMode == Tremolo) return round((m_velocity * -71.249) - 928.57);
    return 0;
}

int Striker::waitTillHit(int velocityThreshold) {
    while (true) {
        int velocityIs = 6000;
        if (VCS_GetVelocityIs(g_pKeyHandle, g_usNodeId, &velocityIs, p_pErrorCode) == 0) {
            LogError("VCS_GetVelocityIs", lResult, *p_pErrorCode);
            lResult = MMC_FAILED;
            return lResult;
        }
        if (velocityIs < velocityThreshold) {
            return MMC_SUCCESS;
        }
    }
}

