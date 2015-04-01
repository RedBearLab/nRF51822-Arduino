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

#ifndef __BLE_DEVICE__
#define __BLE_DEVICE__

#include "Arduino.h"
#include "blecommon.h"
#include "Gap.h"
#include "GattServer.h"

class BLEDeviceInstanceBase; /* forward declaration */

/**
 * BLEDevice uses composition to hide an interface object encapsulating the
 * backend transport.
 *
 * The following API is used to create the singleton interface object. An
 * implementation for this function must be provided by the device-specific
 * library, otherwise there will be a linker error.
 */
extern BLEDeviceInstanceBase *createBLEDeviceInstance(void);

/**
 * The base class used to abstract away BLE capable radio transceivers or SOCs,
 * to enable this BLE API to work with any radio transparently.
 */
class BLEDevice
{
public:
    /**
     * Initialize the BLE controller. This should be called before using
     * anything else in the BLE_API.
     */
    ble_error_t init();
    ble_error_t reset(void);

    /* GAP specific APIs */
public:
    /**
     * Set the BTLE MAC address and type.
     * @return
     */
    ble_error_t setAddress(Gap::addr_type_t type, const uint8_t address[6]);

    /**
     * @param[in] advType
     *              The GAP advertising mode to use for this device. Valid
     *              values are defined in AdvertisingType:
     *
     *              \par ADV_NON_CONNECTABLE_UNDIRECTED
     *              All connections to the peripheral device will be refused.
     *
     *              \par ADV_CONNECTABLE_DIRECTED
     *              Only connections from a pre-defined central device will be
     *              accepted.
     *
     *              \par ADV_CONNECTABLE_UNDIRECTED
     *              Any central device can connect to this peripheral.
     *
     *              \par ADV_SCANNABLE_UNDIRECTED
     *              Any central device can connect to this peripheral, and
     *              the secondary Scan Response payload will be included or
     *              available to central devices.
     *
     *              \par
     *              See Bluetooth Core Specification 4.0 (Vol. 3), Part C,
     *              Section 9.3 and Core Specification 4.0 (Vol. 6), Part B,
     *              Section 2.3.1 for further information on GAP connection
     *              modes
     */
    void        setAdvertisingType(GapAdvertisingParams::AdvertisingType);

    /**
     * @param[in] interval
     *              Advertising interval between 0x0020 and 0x4000 in 0.625ms
     *              units (20ms to 10.24s).  If using non-connectable mode
     *              (ADV_NON_CONNECTABLE_UNDIRECTED) this min value is
     *              0x00A0 (100ms). To reduce the likelihood of collisions, the
     *              link layer perturbs this interval by a pseudo-random delay
     *              with a range of 0 ms to 10 ms for each advertising event.
     *
     *              \par
     *              Decreasing this value will allow central devices to detect
     *              your peripheral faster at the expense of more power being
     *              used by the radio due to the higher data transmit rate.
     *
     *              \par
     *              This field must be set to 0 if connectionMode is equal
     *              to ADV_CONNECTABLE_DIRECTED
     *
     *              \par
     *              See Bluetooth Core Specification, Vol 3., Part C,
     *              Appendix A for suggested advertising intervals.
     */
    void        setAdvertisingInterval(uint16_t interval);

    /**
     * @param[in] timeout
     *              Advertising timeout between 0x1 and 0x3FFF (1 and 16383)
     *              in seconds.  Enter 0 to disable the advertising timeout.
     */
    void        setAdvertisingTimeout(uint16_t timeout);

    /**
     * Please refer to the APIs above.
     */
    void        setAdvertisingParams(const GapAdvertisingParams &advParams);

    /**
     * Reset any advertising payload prepared from prior calls to
     * accumulateAdvertisingPayload().
     */
    void        clearAdvertisingPayload(void);

    /**
     * Accumulate an AD structure in the advertising payload. Please note that
     * the payload is limited to 31 bytes. The SCAN_RESPONSE message may be used
     * as an additional 31 bytes if the advertising payload proves to be too
     * small.
     *
     * @param  flags
     *         The flags to be added. Multiple flags may be specified in
     *         combination.
     */
    ble_error_t accumulateAdvertisingPayload(uint8_t flags);

    /**
     * Accumulate an AD structure in the advertising payload. Please note that
     * the payload is limited to 31 bytes. The SCAN_RESPONSE message may be used
     * as an additional 31 bytes if the advertising payload proves to be too
     * small.
     *
     * @param  app
     *         The appearance of the peripheral.
     */
    ble_error_t accumulateAdvertisingPayload(GapAdvertisingData::Appearance app);

    /**
     * Accumulate an AD structure in the advertising payload. Please note that
     * the payload is limited to 31 bytes. The SCAN_RESPONSE message may be used
     * as an additional 31 bytes if the advertising payload proves to be too
     * small.
     *
     * @param  app
     *         The max transmit power to be used by the controller. This is
     *         only a hint.
     */
    ble_error_t accumulateAdvertisingPayloadTxPower(int8_t power);

    /**
     * @param  app
     *         The appearance of the peripheral.
     */
    /**
     * Accumulate a variable length byte-stream as an AD structure in the
     * advertising payload. Please note that the payload is limited to 31 bytes.
     * The SCAN_RESPONSE message may be used as an additional 31 bytes if the
     * advertising payload proves to be too small.
     *
     * @param  type The type which describes the variable length data.
     * @param  data data bytes.
     * @param  len  length of data.
     */
    ble_error_t accumulateAdvertisingPayload(GapAdvertisingData::DataType type, const uint8_t *data, uint8_t len);

    /**
     * Start advertising (GAP Discoverable, Connectable modes, Broadcast
     * Procedure).
     */
    ble_error_t startAdvertising(void);

    /**
     * Stop advertising (GAP Discoverable, Connectable modes, Broadcast
     * Procedure).
     */
    ble_error_t stopAdvertising(void);

    ble_error_t disconnect(void);

    /* APIs to set GAP callbacks. */
    void onTimeout(Gap::EventCallback_t       timeoutCallback);
    void onConnection(Gap::EventCallback_t    connectionCallback);
    /**
     * Used to setup a callback for GAP disconnection.
     */
    void onDisconnection(Gap::EventCallback_t disconnectionCallback);

    /**
     * Setup a callback for the GATT event DATA_SENT.
     */
    void onDataSent(GattServer::ServerEventCallback_t callback);

    /**
     * Setup a callback for when a characteristic has its value updated by a
     * client.
     */
    void onDataWritten(GattServer::EventCallback_t callback);
    void onUpdatesEnabled(GattServer::EventCallback_t callback);
    void onUpdatesDisabled(GattServer::EventCallback_t callback);
    void onConfirmationReceived(GattServer::EventCallback_t callback);

    /**
     * Add a service declaration to the local server ATT table. Also add the
     * characteristics contained within.
     */
    ble_error_t addService(GattService &service);

    Gap::GapState_t getGapState(void) const;

    ble_error_t readCharacteristicValue(uint16_t handle, uint8_t *const buffer, uint16_t *const lengthP);
    ble_error_t updateCharacteristicValue(uint16_t handle, const uint8_t* value, uint16_t size, bool localOnly = false);

    /**
     * Yield control to the BLE stack or to other tasks waiting for events. This
     * is a sleep function which will return when there is an application
     * specific interrupt, but the MCU might wake up several times before
     * returning (to service the stack). This is not always interchangeable with
     * WFE().
     */
    void waitForEvent(void);

public:
    BLEDevice() : transport(createBLEDeviceInstance()), advParams(), advPayload(), scanResponse(), needToSetAdvPayload(true) {
        advPayload.clear();
        scanResponse.clear();
    }

private:
    BLEDeviceInstanceBase *const transport; /* the device specific backend */

    GapAdvertisingParams advParams;
    GapAdvertisingData   advPayload;
    GapAdvertisingData   scanResponse;

    /* Accumulation of AD structures in the advertisement payload should
     * eventually result in a call to the target's setAdvertisingData() before
     * the server begins advertising. This flag marks the status of the pending update.*/
    bool                 needToSetAdvPayload;

public:
    /**
     * This API is typically used as an internal helper to udpate the transport
     * backend with advertising data before starting to advertise. It may also
     * be explicity used to dynamically reset the accumulated advertising
     * payload; to do this, the application can clear and re-accumulate a new
     * advertising payload before using this API.
     */
    ble_error_t setAdvertisingPayload(void);

    /**
     * DEPRECATED
     */
public:
    ble_error_t setAdvertisingData(const GapAdvertisingData &ADStructures, const GapAdvertisingData &scanResponse);
    ble_error_t setAdvertisingData(const GapAdvertisingData &ADStructures);

    ble_error_t startAdvertising(const GapAdvertisingParams &advParams);
};

/**
 *  The interface for the transport object to be created by the target library's
 *  createBLEDeviceInstance().
 */
class BLEDeviceInstanceBase
{
public:
    virtual Gap&        getGap()           = 0;
    virtual GattServer& getGattServer()    = 0;
    virtual ble_error_t init(void)         = 0;
    virtual ble_error_t reset(void)        = 0;
    virtual void        waitForEvent(void) = 0;
};


/* BLEDevice methods. Most of these simply forward the calls to the underlying
 * transport.*/

inline ble_error_t
BLEDevice::init()
{
    return transport->init();
}

inline ble_error_t
BLEDevice::reset(void)
{
    return transport->reset();
}

inline ble_error_t
BLEDevice::setAddress(Gap::addr_type_t type, const uint8_t address[6])
{
    return transport->getGap().setAddress(type, address);
}

inline void
BLEDevice::setAdvertisingType(GapAdvertisingParams::AdvertisingType advType)
{
    advParams.setAdvertisingType(advType);
}

inline void
BLEDevice::setAdvertisingInterval(uint16_t interval)
{
    advParams.setInterval(interval);
}

inline void
BLEDevice::setAdvertisingTimeout(uint16_t timeout)
{
    advParams.setTimeout(timeout);
}

inline void
BLEDevice::setAdvertisingParams(const GapAdvertisingParams &newAdvParams)
{
    advParams = newAdvParams;
}

inline void
BLEDevice::clearAdvertisingPayload(void)
{
    needToSetAdvPayload = true;
    advPayload.clear();
}

inline ble_error_t
BLEDevice::accumulateAdvertisingPayload(uint8_t flags)
{
    needToSetAdvPayload = true;
    return advPayload.addFlags(flags);
}

inline ble_error_t
BLEDevice::accumulateAdvertisingPayload(GapAdvertisingData::Appearance app)
{
    needToSetAdvPayload = true;
    return advPayload.addAppearance(app);
}

inline ble_error_t
BLEDevice::accumulateAdvertisingPayloadTxPower(int8_t txPower)
{
    needToSetAdvPayload = true;
    return advPayload.addTxPower(txPower);
}

inline ble_error_t
BLEDevice::accumulateAdvertisingPayload(GapAdvertisingData::DataType  type, const uint8_t *data, uint8_t len)
{
    needToSetAdvPayload = true;
    return advPayload.addData(type, data, len);
}

inline ble_error_t
BLEDevice::setAdvertisingPayload(void) {
    needToSetAdvPayload = false;
    return transport->getGap().setAdvertisingData(advPayload, scanResponse);
}

inline ble_error_t
BLEDevice::startAdvertising(void)
{
    if (needToSetAdvPayload) {
        ble_error_t rc;
        if ((rc = setAdvertisingPayload()) != BLE_ERROR_NONE) {
            return rc;
        }
    }

    return transport->getGap().startAdvertising(advParams);
}

inline ble_error_t
BLEDevice::stopAdvertising(void)
{
    return transport->getGap().stopAdvertising();
}

inline ble_error_t
BLEDevice::disconnect(void)
{
    return transport->getGap().disconnect();
}

inline void
BLEDevice::onTimeout(Gap::EventCallback_t timeoutCallback)
{
    transport->getGap().setOnTimeout(timeoutCallback);
}

inline void
BLEDevice::onConnection(Gap::EventCallback_t connectionCallback)
{
    transport->getGap().setOnConnection(connectionCallback);
}

inline void
BLEDevice::onDisconnection(Gap::EventCallback_t disconnectionCallback)
{
    transport->getGap().setOnDisconnection(disconnectionCallback);
}

inline void
BLEDevice::onDataSent(GattServer::ServerEventCallback_t callback)
{
    transport->getGattServer().setOnDataSent(callback);
}

inline void
BLEDevice::onDataWritten(GattServer::EventCallback_t callback)
{
    transport->getGattServer().setOnDataWritten(callback);
}

inline void
BLEDevice::onUpdatesEnabled(GattServer::EventCallback_t callback)
{
    transport->getGattServer().setOnUpdatesEnabled(callback);
}

inline void
BLEDevice::onUpdatesDisabled(GattServer::EventCallback_t callback)
{
    transport->getGattServer().setOnUpdatesDisabled(callback);
}

inline void
BLEDevice::onConfirmationReceived(GattServer::EventCallback_t callback)
{
    transport->getGattServer().setOnConfirmationReceived(callback);
}

inline ble_error_t
BLEDevice::addService(GattService &service)
{
    return transport->getGattServer().addService(service);
}

inline Gap::GapState_t
BLEDevice::getGapState(void) const
{
    return transport->getGap().getState();
}

inline ble_error_t BLEDevice::readCharacteristicValue(uint16_t handle, uint8_t *const buffer, uint16_t *const lengthP)
{
    return transport->getGattServer().readValue(handle, buffer, lengthP);
}

inline ble_error_t
BLEDevice::updateCharacteristicValue(uint16_t handle, const uint8_t* value, uint16_t size, bool localOnly)
{
    return transport->getGattServer().updateValue(handle, const_cast<uint8_t *>(value), size, localOnly);
}

inline void
BLEDevice::waitForEvent(void)
{
    transport->waitForEvent();
}

/*
 * ALL OF THE FOLLOWING METHODS ARE DEPRECATED
 */

inline ble_error_t
BLEDevice::setAdvertisingData(const GapAdvertisingData &ADStructures, const GapAdvertisingData &scanResponse)
{
    needToSetAdvPayload = false;
    return transport->getGap().setAdvertisingData(ADStructures, scanResponse);
}

inline ble_error_t
BLEDevice::setAdvertisingData(const GapAdvertisingData &ADStructures)
{
    GapAdvertisingData scanResponse;

    needToSetAdvPayload = false;
    return transport->getGap().setAdvertisingData(ADStructures, scanResponse);
}

inline ble_error_t
BLEDevice::startAdvertising(const GapAdvertisingParams &_advParams)
{
    return transport->getGap().startAdvertising(_advParams);
}

#endif // ifndef __BLE_DEVICE__
