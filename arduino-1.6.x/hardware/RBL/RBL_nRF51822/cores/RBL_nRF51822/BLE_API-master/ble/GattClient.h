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

#ifndef __GATT_CLIENT_H__
#define __GATT_CLIENT_H__

#include "Gap.h"
#include "GattAttribute.h"
#include "ServiceDiscovery.h"

#include "GattCallbackParamTypes.h"

class GattClient {
public:
    typedef void (*ReadCallback_t)(const GattReadCallbackParams *params);

    enum WriteOp_t {
        GATT_OP_WRITE_REQ = 0x01,  /**< Write Request. */
        GATT_OP_WRITE_CMD = 0x02,  /**< Write Command. */
    };

    typedef void (*WriteCallback_t)(const GattWriteCallbackParams *params);

    typedef void (*HVXCallback_t)(const GattHVXCallbackParams *params);

    /*
     * The following functions are meant to be overridden in the platform-specific sub-class.
     */
public:
    /**
     * Launch service discovery. Once launched, application callbacks will be
     * invoked for matching services/characteristics. isServiceDiscoveryActive()
     * can be used to determine status; and a termination callback (if setup)
     * will be invoked at the end. Service discovery can be terminated prematurely
     * if needed using terminateServiceDiscovery().
     *
     * @param  connectionHandle
     *           Handle for the connection with the peer.
     * @param  sc
     *           This is the application callback for matching service. Taken as
     *           NULL by default. Note: service discovery may still be active
     *           when this callback is issued; calling asynchronous BLE-stack
     *           APIs from within this application callback might cause the
     *           stack to abort service discovery. If this becomes an issue, it
     *           may be better to make local copy of the discoveredService and
     *           wait for service discovery to terminate before operating on the
     *           service.
     * @param  cc
     *           This is the application callback for matching characteristic.
     *           Taken as NULL by default. Note: service discovery may still be
     *           active when this callback is issued; calling asynchronous
     *           BLE-stack APIs from within this application callback might cause
     *           the stack to abort service discovery. If this becomes an issue,
     *           it may be better to make local copy of the discoveredCharacteristic
     *           and wait for service discovery to terminate before operating on the
     *           characteristic.
     * @param  matchingServiceUUID
     *           UUID based filter for specifying a service in which the application is
     *           interested. By default it is set as the wildcard UUID_UNKNOWN,
     *           in which case it matches all services. If characteristic-UUID
     *           filter (below) is set to the wildcard value, then a service
     *           callback will be invoked for the matching service (or for every
     *           service if the service filter is a wildcard).
     * @param  matchingCharacteristicUUIDIn
     *           UUID based filter for specifying characteristic in which the application
     *           is interested. By default it is set as the wildcard UUID_UKNOWN
     *           to match against any characteristic. If both service-UUID
     *           filter and characteristic-UUID filter are used with non- wildcard
     *           values, then only a single characteristic callback is
     *           invoked for the matching characteristic.
     *
     * @note     Using wildcard values for both service-UUID and characteristic-
     *           UUID will result in complete service discovery--callbacks being
     *           called for every service and characteristic.
     *
     * @note     Providing NULL for the characteristic callback will result in
     *           characteristic discovery being skipped for each matching
     *           service. This allows for an inexpensive method to discover only
     *           services.
     *
     * @return
     *           BLE_ERROR_NONE if service discovery is launched successfully; else an appropriate error.
     */
    virtual ble_error_t launchServiceDiscovery(Gap::Handle_t                               connectionHandle,
                                               ServiceDiscovery::ServiceCallback_t         sc                           = NULL,
                                               ServiceDiscovery::CharacteristicCallback_t  cc                           = NULL,
                                               const UUID                                 &matchingServiceUUID          = UUID::ShortUUIDBytes_t(BLE_UUID_UNKNOWN),
                                               const UUID                                 &matchingCharacteristicUUIDIn = UUID::ShortUUIDBytes_t(BLE_UUID_UNKNOWN)) {
        /* avoid compiler warnings about unused variables */
        (void)connectionHandle;
        (void)sc;
        (void)cc;
        (void)matchingServiceUUID;
        (void)matchingCharacteristicUUIDIn;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /**
     * Launch service discovery for services. Once launched, service discovery will remain
     * active with service-callbacks being issued back into the application for matching
     * services. isServiceDiscoveryActive() can be used to
     * determine status; and a termination callback (if setup) will be invoked
     * at the end. Service discovery can be terminated prematurely if needed
     * using terminateServiceDiscovery().
     *
     * @param  connectionHandle
     *           Handle for the connection with the peer.
     * @param  sc
     *           This is the application callback for matching service. Note: service discovery may still be active
     *           when this callback is issued; calling asynchronous BLE-stack
     *           APIs from within this application callback might cause the
     *           stack to abort service discovery. If this becomes an issue, it
     *           may be better to make local copy of the discoveredService and
     *           wait for service discovery to terminate before operating on the
     *           service.
     * @param  matchingServiceUUID
     *           UUID based filter for specifying a service in which the application is
     *           interested. By default it is set as the wildcard UUID_UNKNOWN,
     *           in which case it matches all services.
     *
     * @return
     *           BLE_ERROR_NONE if service discovery is launched successfully; else an appropriate error.
     */
    virtual ble_error_t discoverServices(Gap::Handle_t                        connectionHandle,
                                         ServiceDiscovery::ServiceCallback_t  callback,
                                         const UUID                          &matchingServiceUUID = UUID::ShortUUIDBytes_t(BLE_UUID_UNKNOWN)) {
        /* avoid compiler warnings about unused variables */
        (void)connectionHandle;
        (void)callback;
        (void)matchingServiceUUID;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /**
     * Launch service discovery for services. Once launched, service discovery will remain
     * active with service-callbacks being issued back into the application for matching
     * services. isServiceDiscoveryActive() can be used to
     * determine status; and a termination callback (if setup) will be invoked
     * at the end. Service discovery can be terminated prematurely if needed
     * using terminateServiceDiscovery().
     *
     * @param  connectionHandle
     *           Handle for the connection with the peer.
     * @param  sc
     *           This is the application callback for matching service. Note: service discovery may still be active
     *           when this callback is issued; calling asynchronous BLE-stack
     *           APIs from within this application callback might cause the
     *           stack to abort service discovery. If this becomes an issue, it
     *           may be better to make local copy of the discoveredService and
     *           wait for service discovery to terminate before operating on the
     *           service.
     * @param  startHandle, endHandle
     *           Handle range within which to limit the search
     *
     * @return
     *           BLE_ERROR_NONE if service discovery is launched successfully; else an appropriate error.
     */
    virtual ble_error_t discoverServices(Gap::Handle_t                        connectionHandle,
                                         ServiceDiscovery::ServiceCallback_t  callback,
                                         GattAttribute::Handle_t              startHandle,
                                         GattAttribute::Handle_t              endHandle) {
        /* avoid compiler warnings about unused variables */
        (void)connectionHandle;
        (void)callback;
        (void)startHandle;
        (void)endHandle;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /**
     * Is service-discovery currently active?
     */
    virtual bool isServiceDiscoveryActive(void) const {
        return false; /* default implementation; override this API if this capability is supported. */
    }

    /**
     * Terminate an ongoing service-discovery. This should result in an
     * invocation of the TerminationCallback if service-discovery is active.
     */
    virtual void terminateServiceDiscovery(void) {
        /* default implementation; override this API if this capability is supported. */
    }

    /* Initiate a Gatt Client read procedure by attribute-handle. */
    virtual ble_error_t read(Gap::Handle_t connHandle, GattAttribute::Handle_t attributeHandle, uint16_t offset) const {
        /* avoid compiler warnings about unused variables */
        (void)connHandle;
        (void)attributeHandle;
        (void)offset;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /**
     * Initiate a GATT Client write procedure.
     *
     * @param[in] cmd
     *              Command can be either a write-request (which generates a
     *              matching response from the peripheral), or a write-command,
     *              which doesn't require the connected peer to respond.
     * @param[in] connHandle
     *              Connection handle.
     * @param[in] attributeHandle
     *              handle for the target attribtue on the remote GATT server.
     * @param[in] length
     *              length of the new value.
     * @param[in] value
     *              new value being written.
     */
    virtual ble_error_t write(GattClient::WriteOp_t    cmd,
                              Gap::Handle_t            connHandle,
                              GattAttribute::Handle_t  attributeHandle,
                              size_t                   length,
                              const uint8_t           *value) const {
        /* avoid compiler warnings about unused variables */
        (void)cmd;
        (void)connHandle;
        (void)attributeHandle;
        (void)length;
        (void)value;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }

    /* Event callback handlers. */
public:
    /**
     * Setup a callback for read response events.
     */
    void onDataRead(ReadCallback_t callback) {
        onDataReadCallback = callback;
    }

    /**
     * Setup a callback for write response events.
     * @Note: write commands (issued using writeWoResponse) don't generate a response.
     */
    void onDataWrite(WriteCallback_t callback) {
        onDataWriteCallback = callback;
    }

    /**
     * Setup callback for when serviceDiscovery terminates.
     */
    virtual void onServiceDiscoveryTermination(ServiceDiscovery::TerminationCallback_t callback) {
        (void)callback; /* avoid compiler warnings about ununsed variables */

        /* default implementation; override this API if this capability is supported. */
    }

    /**
     * Setup a callback for when GattClient receives an update event
     * corresponding to a change in value of a characteristic on the remote
     * GattServer.
     */
    void onHVX(HVXCallback_t callback) {
        onHVXCallback = callback;
    }

protected:
    GattClient() {
        /* empty */
    }

    /* Entry points for the underlying stack to report events back to the user. */
public:
    void processReadResponse(const GattReadCallbackParams *params) {
        if (onDataReadCallback) {
            onDataReadCallback(params);
        }
    }

    void processWriteResponse(const GattWriteCallbackParams *params) {
        if (onDataWriteCallback) {
            onDataWriteCallback(params);
        }
    }

    void processHVXEvent(const GattHVXCallbackParams *params) {
        if (onHVXCallback) {
            onHVXCallback(params);
        }
    }

protected:
    ReadCallback_t  onDataReadCallback;
    WriteCallback_t onDataWriteCallback;
    HVXCallback_t   onHVXCallback;

private:
    /* disallow copy and assignment */
    GattClient(const GattClient &);
    GattClient& operator=(const GattClient &);
};

#endif // ifndef __GATT_CLIENT_H__
