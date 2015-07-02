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

#ifndef __UUID_H__
#define __UUID_H__

#include <stdint.h>
#include <string.h>

#include "blecommon.h"

class UUID {
public:
    enum UUID_Type_t {
        UUID_TYPE_SHORT = 0,    // Short BLE UUID
        UUID_TYPE_LONG  = 1     // Full 128-bit UUID
    };

    typedef uint16_t      ShortUUIDBytes_t;

    static const unsigned LENGTH_OF_LONG_UUID = 16;
    typedef uint8_t       LongUUIDBytes_t[LENGTH_OF_LONG_UUID];

public:
    /**
     * Creates a new 128-bit UUID
     *
     * @note   The UUID is a unique 128-bit (16 byte) ID used to identify
     *         different service or characteristics on the BLE device.
     *
     * @param longUUID
     *          The 128-bit (16-byte) UUID value, MSB first (big-endian).
     */
    UUID(const LongUUIDBytes_t longUUID) : type(UUID_TYPE_LONG), baseUUID(), shortUUID(0) {
        setupLong(longUUID);
    }

    /**
     * Creates a new 16-bit UUID
     *
     * @note The UUID is a unique 16-bit (2 byte) ID used to identify
     *       different service or characteristics on the BLE device.
     *
     * For efficiency, and because 16 bytes would take a large chunk of the
     * 27-byte data payload length of the Link Layer, the BLE specification adds
     * two additional UUID formats: 16-bit and 32-bit UUIDs. These shortened
     * formats can be used only with UUIDs that are defined in the Bluetooth
     * specification (i.e., that are listed by the Bluetooth SIG as standard
     * Bluetooth UUIDs).
     *
     * To reconstruct the full 128-bit UUID from the shortened version, insert
     * the 16-bit short value (indicated by xxxx, including leading zeros) into
     * the Bluetooth Base UUID:
     *
     *  0000xxxx-0000-1000-8000-00805F9B34FB
     *
     * @note Shortening is not available for UUIDs that are not derived from the
     *       Bluetooth Base UUID. Such non-standard UUIDs are commonly called
     *       vendor-specific UUIDs. In these cases, you’ll need to use the full
     *       128-bit UUID value at all times.
     *
     * @note we don't yet support 32-bit shortened UUIDs.
     */
    UUID(ShortUUIDBytes_t shortUUID) : type(UUID_TYPE_SHORT), baseUUID(), shortUUID(shortUUID) {
        /* empty */
    }

    UUID(const UUID &source) {
        type      = source.type;
        shortUUID = source.shortUUID;
        memcpy(baseUUID, source.baseUUID, LENGTH_OF_LONG_UUID);
    }

    UUID(void) : type(UUID_TYPE_SHORT), shortUUID(BLE_UUID_UNKNOWN) {
        /* empty */
    }

    /**
     * Fill in a 128-bit UUID; this is useful when UUID isn't known at the time of object construction.
     */
    void setupLong(const LongUUIDBytes_t longUUID) {
        type      = UUID_TYPE_LONG;
        memcpy(baseUUID, longUUID, LENGTH_OF_LONG_UUID);
        shortUUID = (uint16_t)((longUUID[2] << 8) | (longUUID[3]));
    }

public:
    UUID_Type_t       shortOrLong(void)  const {return type;     }
    const uint8_t    *getBaseUUID(void)  const {
        if (type == UUID_TYPE_SHORT) {
            return (const uint8_t*)&shortUUID;
        } else {
            return baseUUID;
        }
    }

    ShortUUIDBytes_t  getShortUUID(void) const {return shortUUID;}
    uint8_t           getLen(void)       const {
        return ((type == UUID_TYPE_SHORT) ? sizeof(ShortUUIDBytes_t) : LENGTH_OF_LONG_UUID);
    }

    bool operator== (const UUID &other) const {
        if ((this->type == UUID_TYPE_SHORT) && (other.type == UUID_TYPE_SHORT) &&
            (this->shortUUID == other.shortUUID)) {
            return true;
        }

        if ((this->type == UUID_TYPE_LONG) && (other.type == UUID_TYPE_LONG) &&
            (memcmp(this->baseUUID, other.baseUUID, LENGTH_OF_LONG_UUID) == 0)) {
            return true;
        }

        return false;
    }

    bool operator!= (const UUID &other) const {
        return !(*this == other);
    }

private:
    UUID_Type_t      type;      // UUID_TYPE_SHORT or UUID_TYPE_LONG
    LongUUIDBytes_t  baseUUID;  /* the base of the long UUID (if
                            * used). Note: bytes 12 and 13 (counting from LSB)
                            * are zeroed out to allow comparison with other long
                            * UUIDs which differ only in the 16-bit relative
                            * part.*/
    ShortUUIDBytes_t shortUUID; // 16 bit uuid (byte 2-3 using with base)
};

#endif // ifndef __UUID_H__
