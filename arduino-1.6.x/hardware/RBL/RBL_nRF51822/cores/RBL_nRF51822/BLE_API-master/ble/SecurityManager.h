/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
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

#ifndef __SECURITY_MANAGER_H__
#define __SECURITY_MANAGER_H__

#include <stdint.h>

#include "Gap.h"

class SecurityManager {
public:
    enum SecurityMode_t {
        SECURITY_MODE_NO_ACCESS,
        SECURITY_MODE_ENCRYPTION_OPEN_LINK, /**< require no protection, open link. */
        SECURITY_MODE_ENCRYPTION_NO_MITM,   /**< require encryption, but no MITM protection. */
        SECURITY_MODE_ENCRYPTION_WITH_MITM, /**< require encryption and MITM protection. */
        SECURITY_MODE_SIGNED_NO_MITM,       /**< require signing or encryption, but no MITM protection. */
        SECURITY_MODE_SIGNED_WITH_MITM,     /**< require signing or encryption, and MITM protection. */
    };

    /**
     * @brief Defines possible security status/states.
     *
     * @details Defines possible security status/states of a link when requested by getLinkSecurity().
     */
    enum LinkSecurityStatus_t {
        NOT_ENCRYPTED,          /**< The link is not secured. */
        ENCRYPTION_IN_PROGRESS, /**< Link security is being established.*/
        ENCRYPTED               /**< The link is secure.*/
    };

    enum SecurityIOCapabilities_t {
      IO_CAPS_DISPLAY_ONLY     = 0x00,   /**< Display Only. */
      IO_CAPS_DISPLAY_YESNO    = 0x01,   /**< Display and Yes/No entry. */
      IO_CAPS_KEYBOARD_ONLY    = 0x02,   /**< Keyboard Only. */
      IO_CAPS_NONE             = 0x03,   /**< No I/O capabilities. */
      IO_CAPS_KEYBOARD_DISPLAY = 0x04,   /**< Keyboard and Display. */
    };

    enum SecurityCompletionStatus_t {
        SEC_STATUS_SUCCESS              = 0x00,  /**< Procedure completed with success. */
        SEC_STATUS_TIMEOUT              = 0x01,  /**< Procedure timed out. */
        SEC_STATUS_PDU_INVALID          = 0x02,  /**< Invalid PDU received. */
        SEC_STATUS_PASSKEY_ENTRY_FAILED = 0x81,  /**< Passkey entry failed (user canceled or other). */
        SEC_STATUS_OOB_NOT_AVAILABLE    = 0x82,  /**< Out of Band Key not available. */
        SEC_STATUS_AUTH_REQ             = 0x83,  /**< Authentication requirements not met. */
        SEC_STATUS_CONFIRM_VALUE        = 0x84,  /**< Confirm value failed. */
        SEC_STATUS_PAIRING_NOT_SUPP     = 0x85,  /**< Pairing not supported.  */
        SEC_STATUS_ENC_KEY_SIZE         = 0x86,  /**< Encryption key size. */
        SEC_STATUS_SMP_CMD_UNSUPPORTED  = 0x87,  /**< Unsupported SMP command. */
        SEC_STATUS_UNSPECIFIED          = 0x88,  /**< Unspecified reason. */
        SEC_STATUS_REPEATED_ATTEMPTS    = 0x89,  /**< Too little time elapsed since last attempt. */
        SEC_STATUS_INVALID_PARAMS       = 0x8A,  /**< Invalid parameters. */
    };

    /**
     * Declaration of type containing a passkey to be used during pairing. This
     * is passed into initializeSecurity() to specify a pre-programmed passkey
     * for authentication instead of generating a random one.
     */
    static const unsigned PASSKEY_LEN = 6;
    typedef uint8_t Passkey_t[PASSKEY_LEN];         /**< 6-digit passkey in ASCII ('0'-'9' digits only). */

public:
    typedef void (*HandleSpecificEvent_t)(Gap::Handle_t handle);
    typedef void (*SecuritySetupInitiatedCallback_t)(Gap::Handle_t, bool allowBonding, bool requireMITM, SecurityIOCapabilities_t iocaps);
    typedef void (*SecuritySetupCompletedCallback_t)(Gap::Handle_t, SecurityCompletionStatus_t status);
    typedef void (*LinkSecuredCallback_t)(Gap::Handle_t handle, SecurityMode_t securityMode);
    typedef void (*PasskeyDisplayCallback_t)(Gap::Handle_t handle, const Passkey_t passkey);

    /*
     * The following functions are meant to be overridden in the platform-specific sub-class.
     */
public:
    /**
     * Enable the BLE stack's Security Manager. The Security Manager implements
     * the actual cryptographic algorithms and protocol exchanges that allow two
     * devices to securely exchange data and privately detect each other.
     * Calling this API is a prerequisite for encryption and pairing (bonding).
     *
     * @param[in]  enableBonding Allow for bonding.
     * @param[in]  requireMITM   Require protection for man-in-the-middle attacks.
     * @param[in]  iocaps        To specify IO capabilities of this peripheral,
     *                           such as availability of a display or keyboard to
     *                           support out-of-band exchanges of security data.
     * @param[in]  passkey       To specify a static passkey.
     *
     * @return BLE_ERROR_NONE on success.
     */
    virtual ble_error_t init(bool                     enableBonding = true,
                             bool                     requireMITM   = true,
                             SecurityIOCapabilities_t iocaps        = IO_CAPS_NONE,
                             const Passkey_t          passkey       = NULL) {
        /* avoid compiler warnings about unused variables */
        (void)enableBonding;
        (void)requireMITM;
        (void)iocaps;
        (void)passkey;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if security is supported. */
    }

    /**
     * Get the security status of a connection.
     *
     * @param[in]  connectionHandle   Handle to identify the connection.
     * @param[out] securityStatusP    security status.
     *
     * @return BLE_SUCCESS Or appropriate error code indicating reason for failure.
     */
    virtual ble_error_t getLinkSecurity(Gap::Handle_t connectionHandle, LinkSecurityStatus_t *securityStatusP) {
        /* avoid compiler warnings about unused variables */
        (void)connectionHandle;
        (void)securityStatusP;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if security is supported. */
    }

    /**
     * Delete all peer device context and all related bonding information from
     * the database within the security manager.
     *
     * @retval BLE_ERROR_NONE             On success, else an error code indicating reason for failure.
     * @retval BLE_ERROR_INVALID_STATE    If the API is called without module initialization and/or
     *                                    application registration.
     */
    virtual ble_error_t purgeAllBondingState(void) {
        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if security is supported. */
    }

    /* Event callback handlers. */
public:
    /**
     * To indicate that security procedure for link has started.
     */
    virtual void onSecuritySetupInitiated(SecuritySetupInitiatedCallback_t callback) {securitySetupInitiatedCallback = callback;}

    /**
     * To indicate that security procedure for link has completed.
     */
    virtual void onSecuritySetupCompleted(SecuritySetupCompletedCallback_t callback) {securitySetupCompletedCallback = callback;}

    /**
     * To indicate that link with the peer is secured. For bonded devices,
     * subsequent re-connections with bonded peer will result only in this callback
     * when the link is secured and setup procedures will not occur unless the
     * bonding information is either lost or deleted on either or both sides.
     */
    virtual void onLinkSecured(LinkSecuredCallback_t callback) {linkSecuredCallback = callback;}

    /**
     * To indicate that device context is stored persistently.
     */
    virtual void onSecurityContextStored(HandleSpecificEvent_t callback) {securityContextStoredCallback = callback;}

    /**
     * To set the callback for when the passkey needs to be displayed on a peripheral with DISPLAY capability.
     */
    virtual void onPasskeyDisplay(PasskeyDisplayCallback_t callback) {passkeyDisplayCallback = callback;}

    /* Entry points for the underlying stack to report events back to the user. */
public:
    void processSecuritySetupInitiatedEvent(Gap::Handle_t handle, bool allowBonding, bool requireMITM, SecurityIOCapabilities_t iocaps) {
        if (securitySetupInitiatedCallback) {
            securitySetupInitiatedCallback(handle, allowBonding, requireMITM, iocaps);
        }
    }

    void processSecuritySetupCompletedEvent(Gap::Handle_t handle, SecurityCompletionStatus_t status) {
        if (securitySetupCompletedCallback) {
            securitySetupCompletedCallback(handle, status);
        }
    }

    void processLinkSecuredEvent(Gap::Handle_t handle, SecurityMode_t securityMode) {
        if (linkSecuredCallback) {
            linkSecuredCallback(handle, securityMode);
        }
    }

    void processSecurityContextStoredEvent(Gap::Handle_t handle) {
        if (securityContextStoredCallback) {
            securityContextStoredCallback(handle);
        }
    }

    void processPasskeyDisplayEvent(Gap::Handle_t handle, const Passkey_t passkey) {
        if (passkeyDisplayCallback) {
            passkeyDisplayCallback(handle, passkey);
        }
    }

protected:
    SecurityManager() :
        securitySetupInitiatedCallback(),
        securitySetupCompletedCallback(),
        linkSecuredCallback(),
        securityContextStoredCallback(),
        passkeyDisplayCallback() {
        /* empty */
    }

protected:
    SecuritySetupInitiatedCallback_t securitySetupInitiatedCallback;
    SecuritySetupCompletedCallback_t securitySetupCompletedCallback;
    LinkSecuredCallback_t            linkSecuredCallback;
    HandleSpecificEvent_t            securityContextStoredCallback;
    PasskeyDisplayCallback_t         passkeyDisplayCallback;
};

#endif /*__SECURITY_MANAGER_H__*/
