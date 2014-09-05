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

#include "custom_helper.h"

/*
 * The current version of the soft-device doesn't handle duplicate 128-bit UUIDs
 * very  well. It is therefore necessary to filter away duplicates before
 * passing long UUIDs to sd_ble_uuid_vs_add(). The following types and data
 * structures involved in maintaining a local cache of 128-bit UUIDs.
 */
typedef struct {
    LongUUID_t uuid;
    uint8_t    type;
} converted_uuid_table_entry_t;
static const unsigned UUID_TABLE_MAX_ENTRIES = 8; /* This is the maximum number
                                    * of 128-bit UUIDs with distinct bases that
                                    * we expect to be in use; increase this
                                    * limit if needed. */
static unsigned uuidTableEntries = 0; /* current usage of the table */
converted_uuid_table_entry_t convertedUUIDTable[UUID_TABLE_MAX_ENTRIES];

/**
 * lookup the cache of previously converted 128-bit UUIDs to find a type value.
 * @param  uuid          long UUID
 * @param  recoveredType the type field of the 3-byte nRF's uuid.
 * @return               true if a match is found.
 */
static bool
lookupConvertedUUIDTable(const LongUUID_t uuid, uint8_t *recoveredType)
{
    unsigned i;
    for (i = 0; i < uuidTableEntries; i++) {
        if (memcmp(convertedUUIDTable[i].uuid,
                   uuid,
                   LENGTH_OF_LONG_UUID) == 0) {
            *recoveredType = convertedUUIDTable[i].type;
            return true;
        }
    }

    return false;
}

static void
addToConvertedUUIDTable(const LongUUID_t uuid, uint8_t type)
{
    if (uuidTableEntries == UUID_TABLE_MAX_ENTRIES) {
        return; /* recovery needed; or at least the user should be
                 * warned about this fact.*/
    }

    memcpy(convertedUUIDTable[uuidTableEntries].uuid, uuid,LENGTH_OF_LONG_UUID);
    convertedUUIDTable[uuidTableEntries].type = type;
    uuidTableEntries++;
}

/**
 * The nRF transport has its own 3-byte representation of a UUID. If the user-
 * specified UUID is 128-bits wide, then the UUID base needs to be added to the
 * soft-device and converted to a 3-byte handle before being used further. This
 * function is responsible for this translation of user-specified UUIDs into
 * nRF's representation.
 *
 * @param[in]  uuid
 *                 user-specified UUID
 * @return nRF
 *              3-byte UUID (containing a type and 16-bit UUID) representation
 *              to be used with SVC calls.
 */
ble_uuid_t custom_convert_to_nordic_uuid(const UUID &uuid)
{
    ble_uuid_t nordicUUID;
    
    nordicUUID.uuid = uuid.getShortUUID();
    nordicUUID.type = BLE_UUID_TYPE_UNKNOWN; /* to be set below */
    
    if (uuid.shortOrLong() == UUID::UUID_TYPE_SHORT) {
        nordicUUID.type = BLE_UUID_TYPE_BLE;
    } else {
        if (!lookupConvertedUUIDTable(uuid.getBaseUUID(), &nordicUUID.type)) {
            nordicUUID.type = custom_add_uuid_base(uuid.getBaseUUID());
            addToConvertedUUIDTable(uuid.getBaseUUID(), nordicUUID.type);
        }
    }

    return nordicUUID;
}

/**************************************************************************/
/*!
    @brief      Adds the base UUID to the custom service. All UUIDs used
                by this service are based on this 128-bit UUID.

    @note       This UUID needs to be added to the SoftDevice stack before
                adding the service's primary service via
                'sd_ble_gatts_service_add'

    @param[in]  p_uuid_base   A pointer to the 128-bit UUID array (8*16)

    @returns    The UUID type.
                A return value of 0 should be considered an error.

    @retval     0x00    BLE_UUID_TYPE_UNKNOWN
    @retval     0x01    BLE_UUID_TYPE_BLE
    @retval     0x02    BLE_UUID_TYPE_VENDOR_BEGIN

    @section EXAMPLE
    @code

    // Take note that bytes 2/3 are blank since these are used to identify
    // the primary service and individual characteristics
    #define CFG_CUSTOM_UUID_BASE  "\x6E\x40\x00\x00\xB5\xA3\xF3\x93\xE0\xA9\xE5\x0E\x24\xDC\xCA\x9E"

    uint8_t uuid_type = custom_add_uuid_base(CFG_CUSTOM_UUID_BASE);
    ASSERT(uuid_type > 0, ERROR_NOT_FOUND);

    // We can now safely add the primary service and any characteristics
    // for our custom service ...

    @endcode
*/
/**************************************************************************/
uint8_t custom_add_uuid_base(uint8_t const *const p_uuid_base)
{
    ble_uuid128_t base_uuid;
    uint8_t       uuid_type = 0;

    /* Reverse the bytes since ble_uuid128_t is LSB */
    for (uint8_t i = 0; i<16; i++) {
        base_uuid.uuid128[i] = p_uuid_base[15 - i];
    }

    //ASSERT_INT( ERROR_NONE, sd_ble_uuid_vs_add( &base_uuid, &uuid_type ), 0);  //_modify
    nrf_err_check( sd_ble_uuid_vs_add( &base_uuid, &uuid_type ) );
    
    return uuid_type;
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
error_t custom_decode_uuid_base(uint8_t const *const p_uuid_base,
                                ble_uuid_t          *p_uuid)
{
    LongUUID_t uuid_base_le;

    /* Reverse the bytes since ble_uuid128_t is LSB */
    for (uint8_t i = 0; i<16; i++) {
        uuid_base_le[i] = p_uuid_base[15 - i];
    }

    //ASSERT_STATUS( sd_ble_uuid_decode(16, uuid_base_le, p_uuid));  //_modify
    nrf_err_check( sd_ble_uuid_decode(16, uuid_base_le, p_uuid) );
    
    return ERROR_NONE;
}

/**************************************************************************/
/*!
    @brief      Adds a new characteristic to the custom service, assigning
                properties, a UUID add-on value, etc.

    @param[in]  service_handle
    @param[in]  p_uuid            The 16-bit value to add to the base UUID
                                  for this characteristic (normally >1
                                  since 1 is typically used by the primary
                                  service).
    @param[in]  char_props        The characteristic properties, as
                                  defined by ble_gatt_char_props_t
    @param[in]  max_length        The maximum length of this characeristic
    @param[in]  p_char_handle

    @returns
    @retval     ERROR_NONE        Everything executed normally
*/
/**************************************************************************/
error_t custom_add_in_characteristic(uint16_t    service_handle,
                                     ble_uuid_t *p_uuid,
                                     uint8_t     properties,
                                     uint8_t    *p_data,
                                     uint16_t    min_length,
                                     uint16_t    max_length,
                                     ble_gatts_char_handles_t *p_char_handle)
{
    /* Characteristic metadata */
    ble_gatts_attr_md_t   cccd_md;
    ble_gatt_char_props_t char_props;

    memcpy(&char_props, &properties, 1);

    if (char_props.notify || char_props.indicate) {
        /* Notification requires cccd */
        memclr_( &cccd_md, sizeof(ble_gatts_attr_md_t));
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    }

    ble_gatts_char_md_t char_md = {0};

    char_md.char_props = char_props;
    char_md.p_cccd_md  =
        (char_props.notify || char_props.indicate) ? &cccd_md : NULL;

    /* Attribute declaration */
    ble_gatts_attr_md_t attr_md = {0};

    attr_md.vloc = BLE_GATTS_VLOC_STACK;
    attr_md.vlen = (min_length == max_length) ? 0 : 1;

    if (char_props.read || char_props.notify || char_props.indicate) {
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    }

    if (char_props.write) {
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    }

    ble_gatts_attr_t attr_char_value = {0};

    attr_char_value.p_uuid    = p_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = min_length;
    attr_char_value.max_len   = max_length;
    attr_char_value.p_value   = p_data;

    // ASSERT_STATUS ( sd_ble_gatts_characteristic_add(service_handle,   //_modify
                                                    // &char_md,
                                                    // &attr_char_value,
                                                    // p_char_handle));

    nrf_err_check( sd_ble_gatts_characteristic_add(service_handle,  
                                                    &char_md,
                                                    &attr_char_value,
                                                    p_char_handle));

    return ERROR_NONE;
}
