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

#include "GapAdvertisingData.h"
#include "GapAdvertisingParams.h"
#include "GapScanningParams.h"
#include "GapEvents.h"
#include "CallChain.h"
#include "FunctionPointerWithContext.h"

using namespace mbed;

/* Forward declarations for classes which will only be used for pointers or references in the following. */
class GapAdvertisingParams;
class GapScanningParams;
class GapAdvertisingData;

class Gap {
public:
    enum AddressType_t {
        ADDR_TYPE_PUBLIC = 0,
        ADDR_TYPE_RANDOM_STATIC,
        ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE,
        ADDR_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE
    };
    typedef enum AddressType_t addr_type_t; /* @Note: deprecated. Use AddressType_t instead. */

    static const unsigned ADDR_LEN = 6;
    typedef uint8_t Address_t[ADDR_LEN]; /* 48-bit address, LSB format. */
    typedef Address_t address_t;         /* @Note: deprecated. Use Address_t instead. */

    enum TimeoutSource_t {
        TIMEOUT_SRC_ADVERTISING      = 0x00, /**< Advertising timeout. */
        TIMEOUT_SRC_SECURITY_REQUEST = 0x01, /**< Security request timeout. */
        TIMEOUT_SRC_SCAN             = 0x02, /**< Scanning timeout. */
        TIMEOUT_SRC_CONN             = 0x03, /**< Connection timeout. */
    };

    /**
     * Enumeration for disconnection reasons. The values for these reasons are
     * derived from Nordic's implementation; but the reasons are meant to be
     * independent of the transport. If you are returned a reason which is not
     * covered by this enumeration, then please refer to the underlying
     * transport library.
     */
    enum DisconnectionReason_t {
        CONNECTION_TIMEOUT                          = 0x08,
        REMOTE_USER_TERMINATED_CONNECTION           = 0x13,
        REMOTE_DEV_TERMINATION_DUE_TO_LOW_RESOURCES = 0x14,  /**< Remote Device Terminated Connection due to low resources.*/
        REMOTE_DEV_TERMINATION_DUE_TO_POWER_OFF     = 0x15,  /**< Remote Device Terminated Connection due to power off. */
        LOCAL_HOST_TERMINATED_CONNECTION            = 0x16,
        CONN_INTERVAL_UNACCEPTABLE                  = 0x3B,
    };

    /* Describes the current state of the device (more than one bit can be set) */
    struct GapState_t {
        unsigned advertising : 1; /**< peripheral is currently advertising */
        unsigned connected   : 1; /**< peripheral is connected to a central */
    };

    typedef uint16_t Handle_t; /* Type for connection handle. */

    typedef struct {
        uint16_t minConnectionInterval;      /**< Minimum Connection Interval in 1.25 ms units, see @ref BLE_GAP_CP_LIMITS.*/
        uint16_t maxConnectionInterval;      /**< Maximum Connection Interval in 1.25 ms units, see @ref BLE_GAP_CP_LIMITS.*/
        uint16_t slaveLatency;               /**< Slave Latency in number of connection events, see @ref BLE_GAP_CP_LIMITS.*/
        uint16_t connectionSupervisionTimeout; /**< Connection Supervision Timeout in 10 ms units, see @ref BLE_GAP_CP_LIMITS.*/
    } ConnectionParams_t;

    enum Role_t {
        PERIPHERAL  = 0x1, /**< Peripheral Role. */
        CENTRAL     = 0x2, /**< Central Role.    */
    };

    struct AdvertisementCallbackParams_t {
        Address_t            peerAddr;
        int8_t               rssi;
        bool                 isScanResponse;
        GapAdvertisingParams::AdvertisingType_t type;
        uint8_t              advertisingDataLen;
        const uint8_t       *advertisingData;
    };
    typedef FunctionPointerWithContext<const AdvertisementCallbackParams_t *> AdvertisementReportCallback_t;

    struct ConnectionCallbackParams_t {
        Handle_t      handle;
        Role_t        role;
        AddressType_t peerAddrType;
        Address_t     peerAddr;
        AddressType_t ownAddrType;
        Address_t     ownAddr;
        const ConnectionParams_t *connectionParams;

        ConnectionCallbackParams_t(Handle_t       handleIn,
                                   Role_t         roleIn,
                                   AddressType_t  peerAddrTypeIn,
                                   const uint8_t *peerAddrIn,
                                   AddressType_t  ownAddrTypeIn,
                                   const uint8_t *ownAddrIn,
                                   const ConnectionParams_t *connectionParamsIn) :
            handle(handleIn),
            role(roleIn),
            peerAddrType(peerAddrTypeIn),
            peerAddr(),
            ownAddrType(ownAddrTypeIn),
            ownAddr(),
            connectionParams(connectionParamsIn) {
            memcpy(peerAddr, peerAddrIn, ADDR_LEN);
            memcpy(ownAddr, ownAddrIn, ADDR_LEN);
        }
    };

    static const uint16_t UNIT_1_25_MS  = 1250; /**< Number of microseconds in 1.25 milliseconds. */
    static const uint16_t UNIT_0_625_MS = 625;  /**< Number of microseconds in 0.625 milliseconds. */
    static uint16_t MSEC_TO_GAP_DURATION_UNITS(uint32_t durationInMillis) {
        return (durationInMillis * 1000) / UNIT_1_25_MS;
    }
    static uint16_t MSEC_TO_ADVERTISEMENT_DURATION_UNITS(uint32_t durationInMillis) {
        return (durationInMillis * 1000) / UNIT_0_625_MS;
    }
    static uint16_t ADVERTISEMENT_DURATION_UNITS_TO_MS(uint16_t gapUnits) {
        return (gapUnits * UNIT_0_625_MS) / 1000;
    }

    typedef void (*TimeoutEventCallback_t)(TimeoutSource_t source);
    typedef void (*ConnectionEventCallback_t)(const ConnectionCallbackParams_t *params);
    typedef void (*DisconnectionEventCallback_t)(Handle_t, DisconnectionReason_t);
    typedef void (*RadioNotificationEventCallback_t)(bool radio_active); /* gets passed true for ACTIVE; false for INACTIVE. */

    /*
     * The following functions are meant to be overridden in the platform-specific sub-class.
     */
public:
    /**
     * Set the BTLE MAC address and type. Please note that the address format is
     * LSB (least significant byte first). Please refer to Address_t.
     *
     * @return BLE_ERROR_NONE on success.
     */
    virtual ble_error_t setAddress(AddressType_t type, const Address_t address) {
        /* avoid compiler warnings about unused variables */
        (void)type;
        (void)address;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /**
     * Fetch the BTLE MAC address and type.
     *
     * @return BLE_ERROR_NONE on success.
     */
    virtual ble_error_t getAddress(AddressType_t *typeP, Address_t address) {
        /* avoid compiler warnings about unused variables */
        (void)typeP;
        (void)address;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /**
     * @return Minimum Advertising interval in milliseconds.
     */
    virtual uint16_t getMinAdvertisingInterval(void) const {
        return 0; /* default implementation; override this API if this capability is supported. */
    }

    /**
     * @return Minimum Advertising interval in milliseconds for non-connectible mode.
     */
    virtual uint16_t getMinNonConnectableAdvertisingInterval(void) const {
        return 0; /* default implementation; override this API if this capability is supported. */
    }

    /**
     * @return Maximum Advertising interval in milliseconds.
     */
    virtual uint16_t getMaxAdvertisingInterval(void) const {
        return 0xFFFF; /* default implementation; override this API if this capability is supported. */
    }

    virtual ble_error_t stopAdvertising(void) {
        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /**
     * Stop scanning. The current scanning parameters remain in effect.
     *
     * @retval BLE_ERROR_NONE if successfully stopped scanning procedure.
     */
    virtual ble_error_t stopScan() {
        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /**
     * Create a connection (GAP Link Establishment).
     *
     * @param peerAddr
     *          48-bit address, LSB format.
     * @param peerAddrType
     *          Address type of the peer.
     * @param connectionParams
     *         Connection parameters.
     * @param scanParams
     *          Paramters to be used while scanning for the peer.
     * @return  BLE_ERROR_NONE if connection establishment procedure is started
     *     successfully. The connectionCallback (if set) will be invoked upon
     *     a connection event.
     */
    virtual ble_error_t connect(const Address_t           peerAddr,
                                Gap::AddressType_t        peerAddrType,
                                const ConnectionParams_t *connectionParams,
                                const GapScanningParams  *scanParams) {
        /* avoid compiler warnings about unused variables */
        (void)peerAddr;
        (void)peerAddrType;
        (void)connectionParams;
        (void)scanParams;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /**
     * This call initiates the disconnection procedure, and its completion will
     * be communicated to the application with an invocation of the
     * disconnectionCallback.
     *
     * @param  reason
     *           The reason for disconnection to be sent back to the peer.
     */
    virtual ble_error_t disconnect(Handle_t connectionHandle, DisconnectionReason_t reason) {
        /* avoid compiler warnings about unused variables */
        (void)connectionHandle;
        (void)reason;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /**
     * This call initiates the disconnection procedure, and its completion will
     * be communicated to the application with an invocation of the
     * disconnectionCallback.
     *
     * @param  reason
     *           The reason for disconnection to be sent back to the peer.
     *
     * @note: this version of disconnect() doesn't take a connection handle. It
     * will work reliably only for stacks which are limited to a single
     * connection. This API should be considered *deprecated* in favour of the
     * altertive which takes a connection handle. It will be dropped in the future.
     */
    virtual ble_error_t disconnect(DisconnectionReason_t reason) {
        /* avoid compiler warnings about unused variables */
        (void)reason;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /**
     * Get the GAP peripheral preferred connection parameters. These are the
     * defaults that the peripheral would like to have in a connection. The
     * choice of the connection parameters is eventually up to the central.
     *
     * @param[out] params
     *               The structure where the parameters will be stored. Memory
     *               for this is owned by the caller.
     *
     * @return BLE_ERROR_NONE if the parameters were successfully filled into
     * the given structure pointed to by params.
     */
    virtual ble_error_t getPreferredConnectionParams(ConnectionParams_t *params) {
        /* avoid compiler warnings about unused variables */
        (void)params;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /**
     * Set the GAP peripheral preferred connection parameters. These are the
     * defaults that the peripheral would like to have in a connection. The
     * choice of the connection parameters is eventually up to the central.
     *
     * @param[in] params
     *               The structure containing the desired parameters.
     */
    virtual ble_error_t setPreferredConnectionParams(const ConnectionParams_t *params) {
        /* avoid compiler warnings about unused variables */
        (void)params;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /**
     * Update connection parameters while in the peripheral role.
     * @details In the peripheral role, this will send the corresponding L2CAP request to the connected peer and wait for
     *          the central to perform the procedure.
     * @param[in] handle
     *              Connection Handle
     * @param[in] params
     *              Pointer to desired connection parameters. If NULL is provided on a peripheral role,
     *              the parameters in the PPCP characteristic of the GAP service will be used instead.
     */
    virtual ble_error_t updateConnectionParams(Handle_t handle, const ConnectionParams_t *params) {
        /* avoid compiler warnings about unused variables */
        (void)handle;
        (void)params;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /**
     * Set the device name characteristic in the GAP service.
     * @param[in] deviceName
     *              The new value for the device-name. This is a UTF-8 encoded, <b>NULL-terminated</b> string.
     */
    virtual ble_error_t setDeviceName(const uint8_t *deviceName) {
        /* avoid compiler warnings about unused variables */
        (void)deviceName;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /**
     * Get the value of the device name characteristic in the GAP service.
     * @param[out]    deviceName
     *                  Pointer to an empty buffer where the UTF-8 *non NULL-
     *                  terminated* string will be placed. Set this
     *                  value to NULL in order to obtain the deviceName-length
     *                  from the 'length' parameter.
     *
     * @param[in/out] lengthP
     *                  (on input) Length of the buffer pointed to by deviceName;
     *                  (on output) the complete device name length (without the
     *                     null terminator).
     *
     * @note If the device name is longer than the size of the supplied buffer,
     *     length will return the complete device name length, and not the
     *     number of bytes actually returned in deviceName. The application may
     *     use this information to retry with a suitable buffer size.
     */
    virtual ble_error_t getDeviceName(uint8_t *deviceName, unsigned *lengthP) {
        /* avoid compiler warnings about unused variables */
        (void)deviceName;
        (void)lengthP;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /**
     * Set the appearance characteristic in the GAP service.
     * @param[in] appearance
     *              The new value for the device-appearance.
     */
    virtual ble_error_t setAppearance(GapAdvertisingData::Appearance appearance) {
        /* avoid compiler warnings about unused variables */
        (void)appearance;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /**
     * Get the appearance characteristic in the GAP service.
     * @param[out] appearance
     *               The new value for the device-appearance.
     */
    virtual ble_error_t getAppearance(GapAdvertisingData::Appearance *appearanceP) {
        /* avoid compiler warnings about unused variables */
        (void)appearanceP;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /**
     * Set the radio's transmit power.
     * @param[in] txPower Radio transmit power in dBm.
     */
    virtual ble_error_t setTxPower(int8_t txPower) {
        /* avoid compiler warnings about unused variables */
        (void)txPower;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /**
     * Query the underlying stack for permitted arguments for setTxPower().
     *
     * @param[out] valueArrayPP
     *                 Out parameter to receive the immutable array of Tx values.
     * @param[out] countP
     *                 Out parameter to receive the array's size.
     */
    virtual void getPermittedTxPowerValues(const int8_t **valueArrayPP, size_t *countP) {
        /* avoid compiler warnings about unused variables */
        (void)valueArrayPP;
        (void)countP;

        *countP = 0; /* default implementation; override this API if this capability is supported. */
    }

protected:
    /* Override the following in the underlying adaptation layer to provide the functionality of scanning. */
    virtual ble_error_t startRadioScan(const GapScanningParams &scanningParams) {
        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /*
     * APIs with non-virtual implementations.
     */
public:
    /**
     * Returns the current GAP state of the device using a bitmask which
     * describes whether the device is advertising and/or connected.
     */
    GapState_t getState(void) const {
        return state;
    }

    /**
     * Set the GAP advertising mode to use for this device.
     */
    void setAdvertisingType(GapAdvertisingParams::AdvertisingType_t advType) {
        _advParams.setAdvertisingType(advType);
    }

    /**
     * @param[in] interval
     *              Advertising interval in units of milliseconds. Advertising
     *              is disabled if interval is 0. If interval is smaller than
     *              the minimum supported value, then the minimum supported
     *              value is used instead. This minimum value can be discovered
     *              using getMinAdvertisingInterval().
     *
     *              This field must be set to 0 if connectionMode is equal
     *              to ADV_CONNECTABLE_DIRECTED.
     *
     * @note: Decreasing this value will allow central devices to detect a
     * peripheral faster at the expense of more power being used by the radio
     * due to the higher data transmit rate.
     *
     * @note: This API is now *deprecated* and will be dropped in the future.
     * You should use the parallel API from Gap directly. A former call to
     * ble.setAdvertisingInterval(...) should now be achieved using
     * ble.gap().setAdvertisingInterval(...).
     *
     * @Note: [WARNING] This API previously used 0.625ms as the unit for its
     * 'interval' argument. That required an explicit conversion from
     * milliseconds using Gap::MSEC_TO_GAP_DURATION_UNITS(). This conversion is
     * no longer required as the new units are milliseconds. Any application
     * code depending on the old semantics would need to be updated accordingly.
     */
    void setAdvertisingInterval(uint16_t interval) {
        if (interval == 0) {
            stopAdvertising();
        } else if (interval < getMinAdvertisingInterval()) {
            interval = getMinAdvertisingInterval();
        }
        _advParams.setInterval(MSEC_TO_ADVERTISEMENT_DURATION_UNITS(interval));
    }

    /**
     * @param[in] timeout
     *              Advertising timeout (in seconds) between 0x1 and 0x3FFF (1
     *              and 16383). Use 0 to disable the advertising timeout.
     */
    void setAdvertisingTimeout(uint16_t timeout) {
        _advParams.setTimeout(timeout);
    }

    /**
     * Start advertising.
     */
    ble_error_t startAdvertising(void) {
        setAdvertisingData(); /* update the underlying stack */
        return startAdvertising(_advParams);
    }

    /**
     * Reset any advertising payload prepared from prior calls to
     * accumulateAdvertisingPayload(). This automatically propagates the re-
     * initialized adv payload to the underlying stack.
     *
     * Note: This should be followed by a call to setAdvertisingPayload() or
     * startAdvertising() before the update takes effect.
     */
    void clearAdvertisingPayload(void) {
        _advPayload.clear();
        setAdvertisingData();
    }

    /**
     * Accumulate an AD structure in the advertising payload. Please note that
     * the payload is limited to 31 bytes. The SCAN_RESPONSE message may be used
     * as an additional 31 bytes if the advertising payload proves to be too
     * small.
     *
     * @param[in] flags
     *              The flags to be added. Please refer to
     *              GapAdvertisingData::Flags for valid flags. Multiple
     *              flags may be specified in combination.
     */
    ble_error_t accumulateAdvertisingPayload(uint8_t flags) {
        ble_error_t rc;
        if ((rc = _advPayload.addFlags(flags)) != BLE_ERROR_NONE) {
            return rc;
        }

        return setAdvertisingData();
    }

    /**
     * Accumulate an AD structure in the advertising payload. Please note that
     * the payload is limited to 31 bytes. The SCAN_RESPONSE message may be used
     * as an additional 31 bytes if the advertising payload proves to be too
     * small.
     *
     * @param  app
     *         The appearance of the peripheral.
     */
    ble_error_t accumulateAdvertisingPayload(GapAdvertisingData::Appearance app) {
        setAppearance(app);

        ble_error_t rc;
        if ((rc = _advPayload.addAppearance(app)) != BLE_ERROR_NONE) {
            return rc;
        }

        return setAdvertisingData();
    }

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
    ble_error_t accumulateAdvertisingPayloadTxPower(int8_t power) {
        ble_error_t rc;
        if ((rc = _advPayload.addTxPower(power)) != BLE_ERROR_NONE) {
            return rc;
        }

        return setAdvertisingData();
    }

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
    ble_error_t accumulateAdvertisingPayload(GapAdvertisingData::DataType type, const uint8_t *data, uint8_t len) {
        if (type == GapAdvertisingData::COMPLETE_LOCAL_NAME) {
            setDeviceName(data);
        }

        ble_error_t rc;
        if ((rc = _advPayload.addData(type, data, len)) != BLE_ERROR_NONE) {
            return rc;
        }

        return setAdvertisingData();
    }

    /**
     * Setup a particular, user-constructed advertisement payload for the
     * underlying stack. It would be uncommon for this API to be used directly;
     * there are other APIs to build an advertisement payload (see above).
     */
    ble_error_t setAdvertisingPayload(const GapAdvertisingData &payload) {
        _advPayload = payload;
        return setAdvertisingData();
    }

    /**
     * @return  Read back advertising data. Useful for storing and
     *          restoring payload.
     */
    const GapAdvertisingData &getAdvertisingPayload(void) const {
        return _advPayload;
    }

    /**
     * Accumulate a variable length byte-stream as an AD structure in the
     * scanResponse payload.
     *
     * @param[in] type The type which describes the variable length data.
     * @param[in] data data bytes.
     * @param[in] len  length of data.
     */
    ble_error_t accumulateScanResponse(GapAdvertisingData::DataType type, const uint8_t *data, uint8_t len) {
        ble_error_t rc;
        if ((rc = _scanResponse.addData(type, data, len)) != BLE_ERROR_NONE) {
            return rc;
        }

        return setAdvertisingData();
    }

    /**
     * Reset any scan response prepared from prior calls to
     * accumulateScanResponse().
     *
     * Note: This should be followed by a call to setAdvertisingPayload() or
     * startAdvertising() before the update takes effect.
     */
    void clearScanResponse(void) {
        _scanResponse.clear();
        setAdvertisingData();
    }

    /**
     * Setup parameters for GAP scanning--i.e. observer mode.
     * @param[in] interval
     *              Scan interval (in milliseconds) [valid values lie between 2.5ms and 10.24s].
     * @param[in] window
     *              Scan Window (in milliseconds) [valid values lie between 2.5ms and 10.24s].
     * @param[in] timeout
     *              Scan timeout (in seconds) between 0x0001 and 0xFFFF, 0x0000 disables timeout.
     * @param[in] activeScanning
     *              Set to True if active-scanning is required. This is used to fetch the
     *              scan response from a peer if possible.
     *
     * The scanning window divided by the interval determines the duty cycle for
     * scanning. For example, if the interval is 100ms and the window is 10ms,
     * then the controller will scan for 10 percent of the time. It is possible
     * to have the interval and window set to the same value. In this case,
     * scanning is continuous, with a change of scanning frequency once every
     * interval.
     *
     * Once the scanning parameters have been configured, scanning can be
     * enabled by using startScan().
     *
     * @Note: The scan interval and window are recommendations to the BLE stack.
     */
    ble_error_t setScanParams(uint16_t interval       = GapScanningParams::SCAN_INTERVAL_MAX,
                              uint16_t window         = GapScanningParams::SCAN_WINDOW_MAX,
                              uint16_t timeout        = 0,
                              bool     activeScanning = false) {
        ble_error_t rc;
        if (((rc = _scanningParams.setInterval(interval)) == BLE_ERROR_NONE) &&
            ((rc = _scanningParams.setWindow(window))     == BLE_ERROR_NONE) &&
            ((rc = _scanningParams.setTimeout(timeout))   == BLE_ERROR_NONE)) {
            _scanningParams.setActiveScanning(activeScanning);
            return BLE_ERROR_NONE;
        }

        return rc;
    }

    /**
     * Setup the scanInterval parameter for GAP scanning--i.e. observer mode.
     * @param[in] interval
     *              Scan interval (in milliseconds) [valid values lie between 2.5ms and 10.24s].
     *
     * The scanning window divided by the interval determines the duty cycle for
     * scanning. For example, if the interval is 100ms and the window is 10ms,
     * then the controller will scan for 10 percent of the time. It is possible
     * to have the interval and window set to the same value. In this case,
     * scanning is continuous, with a change of scanning frequency once every
     * interval.
     *
     * Once the scanning parameters have been configured, scanning can be
     * enabled by using startScan().
     */
    ble_error_t setScanInterval(uint16_t interval) {
        return _scanningParams.setInterval(interval);
    }

    /**
     * Setup the scanWindow parameter for GAP scanning--i.e. observer mode.
     * @param[in] window
     *              Scan Window (in milliseconds) [valid values lie between 2.5ms and 10.24s].
     *
     * The scanning window divided by the interval determines the duty cycle for
     * scanning. For example, if the interval is 100ms and the window is 10ms,
     * then the controller will scan for 10 percent of the time. It is possible
     * to have the interval and window set to the same value. In this case,
     * scanning is continuous, with a change of scanning frequency once every
     * interval.
     *
     * Once the scanning parameters have been configured, scanning can be
     * enabled by using startScan().
     */
    ble_error_t setScanWindow(uint16_t window) {
        return _scanningParams.setWindow(window);
    }

    /**
     * Setup parameters for GAP scanning--i.e. observer mode.
     * @param[in] timeout
     *              Scan timeout (in seconds) between 0x0001 and 0xFFFF, 0x0000 disables timeout.
     *
     * Once the scanning parameters have been configured, scanning can be
     * enabled by using startScan().
     */
    ble_error_t setScanTimeout(uint16_t timeout) {
        return _scanningParams.setTimeout(timeout);
    }

    /**
     * Setup parameters for GAP scanning--i.e. observer mode.
     * @param[in] activeScanning
     *              Set to True if active-scanning is required. This is used to fetch the
     *              scan response from a peer if possible.
     *
     * Once the scanning parameters have been configured, scanning can be
     * enabled by using startScan().
     */
    void setActiveScanning(bool activeScanning) {
        _scanningParams.setActiveScanning(activeScanning);
    }

    /**
     * Start scanning (Observer Procedure) based on the parameters currently in
     * effect.
     *
     * @param[in] callback
     *              The application specific callback to be invoked upon
     *              receiving every advertisement report. This can be passed in
     *              as NULL, in which case scanning may not be enabled at all.
     */
    ble_error_t startScan(void (*callback)(const AdvertisementCallbackParams_t *params)) {
        ble_error_t err = BLE_ERROR_NONE;
        if (callback) {
            if ((err = startRadioScan(_scanningParams)) == BLE_ERROR_NONE) {
                onAdvertisementReport.attach(callback);
            }
        }

        return err;
    }

    /**
     * Same as above, but this takes an (object, method) pair for a callback.
     */
    template<typename T>
    ble_error_t startScan(T *object, void (T::*callbackMember)(const AdvertisementCallbackParams_t *params)) {
        ble_error_t err = BLE_ERROR_NONE;
        if (object && callbackMember) {
            if ((err = startRadioScan(_scanningParams)) == BLE_ERROR_NONE) {
                onAdvertisementReport.attach(object, callbackMember);
            }
        }

        return err;
    }

private:
    ble_error_t setAdvertisingData(void) {
        return setAdvertisingData(_advPayload, _scanResponse);
    }

private:
    virtual ble_error_t setAdvertisingData(const GapAdvertisingData &, const GapAdvertisingData &) = 0;
    virtual ble_error_t startAdvertising(const GapAdvertisingParams &)                             = 0;

public:
    /**
     * Accessors to read back currently active advertising params.
     */
    GapAdvertisingParams &getAdvertisingParams(void) {
        return _advParams;
    }
    const GapAdvertisingParams &getAdvertisingParams(void) const {
        return _advParams;
    }

    /**
     * Setup a particular, user-constructed set of advertisement parameters for
     * the underlying stack. It would be uncommon for this API to be used
     * directly; there are other APIs to tweak advertisement parameters
     * individually.
     */
    void setAdvertisingParams(const GapAdvertisingParams &newParams) {
        _advParams = newParams;
    }

    /* Event callback handlers. */
public:
    /**
     * Setup a callback for timeout events. Refer to TimeoutSource_t for
     * possible event types.
     */
    void onTimeout(TimeoutEventCallback_t callback) {timeoutCallback = callback;}

    /**
     * Setup a callback for connection events. Refer to ConnectionEventCallback_t.
     */
    void onConnection(ConnectionEventCallback_t callback) {connectionCallback = callback;}

    /**
     * Set the application callback for disconnection events.
     * @param callback
     *        Pointer to the unique callback.
     */
    void onDisconnection(DisconnectionEventCallback_t callback) {disconnectionCallback = callback;}

    /**
     * Append to a chain of callbacks to be invoked upon disconnection; these
     * callbacks receive no context and are therefore different from the
     * disconnectionCallback callback.
     * @param callback
     *        function pointer to be invoked upon disconnection; receives no context.
     */
    template<typename T>
    void addToDisconnectionCallChain(T *tptr, void (T::*mptr)(void)) {disconnectionCallChain.add(tptr, mptr);}

    /**
     * Set the application callback for radio-notification events.
     *
     * Radio Notification is a feature that enables ACTIVE and INACTIVE
     * (nACTIVE) signals from the stack that notify the application when the
     * radio is in use. The signal is sent using software interrupt.
     *
     * The ACTIVE signal is sent before the Radio Event starts. The nACTIVE
     * signal is sent at the end of the Radio Event. These signals can be used
     * by the application programmer to synchronize application logic with radio
     * activity. For example, the ACTIVE signal can be used to shut off external
     * devices to manage peak current drawn during periods when the radio is on,
     * or to trigger sensor data collection for transmission in the Radio Event.
     *
     * @param callback
     *          The application handler to be invoked in response to a radio
     *          ACTIVE/INACTIVE event.
     */
    virtual void onRadioNotification(RadioNotificationEventCallback_t callback) {radioNotificationCallback = callback;}

protected:
    Gap() :
        _advParams(),
        _advPayload(),
        _scanningParams(),
        _scanResponse(),
        state(),
        timeoutCallback(NULL),
        connectionCallback(NULL),
        disconnectionCallback(NULL),
        radioNotificationCallback(),
        onAdvertisementReport(),
        disconnectionCallChain() {
        _advPayload.clear();
        _scanResponse.clear();
    }

    /* Entry points for the underlying stack to report events back to the user. */
public:
    void processConnectionEvent(Handle_t                  handle,
                                Role_t                    role,
                                AddressType_t             peerAddrType,
                                const Address_t           peerAddr,
                                AddressType_t             ownAddrType,
                                const Address_t           ownAddr,
                                const ConnectionParams_t *connectionParams) {
        state.connected = 1;
        if (connectionCallback) {
            ConnectionCallbackParams_t callbackParams(handle, role, peerAddrType, peerAddr, ownAddrType, ownAddr, connectionParams);
            connectionCallback(&callbackParams);
        }
    }

    void processDisconnectionEvent(Handle_t handle, DisconnectionReason_t reason) {
        state.connected = 0;
        if (disconnectionCallback) {
            disconnectionCallback(handle, reason);
        }
        disconnectionCallChain.call();
    }

    void processAdvertisementReport(const Address_t    peerAddr,
                                    int8_t             rssi,
                                    bool               isScanResponse,
                                    GapAdvertisingParams::AdvertisingType_t  type,
                                    uint8_t            advertisingDataLen,
                                    const uint8_t     *advertisingData) {
        AdvertisementCallbackParams_t params;
        memcpy(params.peerAddr, peerAddr, ADDR_LEN);
        params.rssi               = rssi;
        params.isScanResponse     = isScanResponse;
        params.type               = type;
        params.advertisingDataLen = advertisingDataLen;
        params.advertisingData    = advertisingData;
        onAdvertisementReport.call(&params);
    }

    void processTimeoutEvent(TimeoutSource_t source) {
        if (timeoutCallback) {
            timeoutCallback(source);
        }
    }

protected:
    GapAdvertisingParams             _advParams;
    GapAdvertisingData               _advPayload;
    GapScanningParams                _scanningParams;
    GapAdvertisingData               _scanResponse;

    GapState_t                       state;

protected:
    TimeoutEventCallback_t           timeoutCallback;
    ConnectionEventCallback_t        connectionCallback;
    DisconnectionEventCallback_t     disconnectionCallback;
    RadioNotificationEventCallback_t radioNotificationCallback;
    AdvertisementReportCallback_t    onAdvertisementReport;
    CallChain                        disconnectionCallChain;

private:
    /* disallow copy and assignment */
    Gap(const Gap &);
    Gap& operator=(const Gap &);
};

#endif // ifndef __GAP_H__
