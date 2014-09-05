/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __GAP_H__
#define __GAP_H__

#include "arduino.h"
#include "blecommon.h"
#include "GapAdvertisingData.h"
#include "GapAdvertisingParams.h"
#include "GapEvents.h"

/**************************************************************************/
/*!
    \brief
    The base class used to abstract GAP functionality to a specific radio
    transceiver, SOC or BLE Stack.
*/
/**************************************************************************/
class Gap
{
public:
    typedef enum addr_type_e {
        ADDR_TYPE_PUBLIC = 0,
        ADDR_TYPE_RANDOM_STATIC,
        ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE,
        ADDR_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE
    } addr_type_t;

    /* These functions must be defined in the sub-class */
    virtual ble_error_t setAddress(addr_type_t type, const uint8_t address[6]) = 0;
    virtual ble_error_t setAdvertisingData(const GapAdvertisingData &, const GapAdvertisingData &) = 0;
    virtual ble_error_t startAdvertising(const GapAdvertisingParams &) = 0;
    virtual ble_error_t stopAdvertising(void)                    = 0;
    virtual ble_error_t disconnect(void)                         = 0;

    /* Describes the current state of the device (more than one bit can be
     *set) */
    typedef struct GapState_s {
        unsigned advertising : 1; /**< peripheral is currently advertising */
        unsigned connected   : 1; /**< peripheral is connected to a central */
    } GapState_t;

    /* Event callback handlers */
    typedef void (*EventCallback_t)(void);
    void setOnTimeout(EventCallback_t callback) {
        onTimeout = callback;
    }
    void setOnConnection(EventCallback_t callback) {
        onConnection = callback;
    }
    void setOnDisconnection(EventCallback_t callback) {
        onDisconnection = callback;
    }

    void handleEvent(GapEvents::gapEvent_e type) {
        switch (type) {
            case GapEvents::GAP_EVENT_TIMEOUT:
                state.advertising = 0;
                if (onTimeout) {
                    onTimeout();
                }
                break;
            case GapEvents::GAP_EVENT_CONNECTED:
                state.connected = 1;
                if (onConnection) {
                    onConnection();
                }
                break;
            case GapEvents::GAP_EVENT_DISCONNECTED:
                state.connected = 0;
                if (onDisconnection) {
                    onDisconnection();
                }
                break;
        }
    }

    GapState_t getState(void) const {
        return state;
    }

protected:
    Gap() : state(), onTimeout(NULL), onConnection(NULL), onDisconnection(NULL) {
        /* empty */
    }

protected:
    GapState_t state;

private:
    EventCallback_t onTimeout;
    EventCallback_t onConnection;
    EventCallback_t onDisconnection;
};

#endif // ifndef __GAP_H__
