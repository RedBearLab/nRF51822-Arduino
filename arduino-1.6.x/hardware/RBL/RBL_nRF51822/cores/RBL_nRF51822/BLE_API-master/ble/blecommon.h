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

#ifndef __BLE_COMMON_H__
#define __BLE_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup BLE_UUID_VALUES Assigned Values for BLE UUIDs
 * @{ */
/* Generic UUIDs, applicable to all services */
enum {
    BLE_UUID_UNKNOWN                             = 0x0000, /**< Reserved UUID. */
    BLE_UUID_SERVICE_PRIMARY                     = 0x2800, /**< Primary Service. */
    BLE_UUID_SERVICE_SECONDARY                   = 0x2801, /**< Secondary Service. */
    BLE_UUID_SERVICE_INCLUDE                     = 0x2802, /**< Include. */
    BLE_UUID_CHARACTERISTIC                      = 0x2803, /**< Characteristic. */
    BLE_UUID_DESCRIPTOR_CHAR_EXT_PROP            = 0x2900, /**< Characteristic Extended Properties Descriptor. */
    BLE_UUID_DESCRIPTOR_CHAR_USER_DESC           = 0x2901, /**< Characteristic User Description Descriptor. */
    BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG       = 0x2902, /**< Client Characteristic Configuration Descriptor. */
    BLE_UUID_DESCRIPTOR_SERVER_CHAR_CONFIG       = 0x2903, /**< Server Characteristic Configuration Descriptor. */
    BLE_UUID_DESCRIPTOR_CHAR_PRESENTATION_FORMAT = 0x2904, /**< Characteristic Presentation Format Descriptor. */
    BLE_UUID_DESCRIPTOR_CHAR_AGGREGATE_FORMAT    = 0x2905, /**< Characteristic Aggregate Format Descriptor. */

/* GATT specific UUIDs */
    BLE_UUID_GATT                                = 0x1801, /**< Generic Attribute Profile. */
    BLE_UUID_GATT_CHARACTERISTIC_SERVICE_CHANGED = 0x2A05, /**< Service Changed Characteristic. */

/* GAP specific UUIDs */
    BLE_UUID_GAP                                 = 0x1800, /**< Generic Access Profile. */
    BLE_UUID_GAP_CHARACTERISTIC_DEVICE_NAME      = 0x2A00, /**< Device Name Characteristic. */
    BLE_UUID_GAP_CHARACTERISTIC_APPEARANCE       = 0x2A01, /**< Appearance Characteristic. */
    BLE_UUID_GAP_CHARACTERISTIC_PPF              = 0x2A02, /**< Peripheral Privacy Flag Characteristic. */
    BLE_UUID_GAP_CHARACTERISTIC_RECONN_ADDR      = 0x2A03, /**< Reconnection Address Characteristic. */
    BLE_UUID_GAP_CHARACTERISTIC_PPCP             = 0x2A04, /**< Peripheral Preferred Connection Parameters Characteristic. */
};
/** @} */

/** @defgroup BLE_APPEARANCES Bluetooth Appearance values
 *  @note Retrieved from http://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.gap.appearance.xml
 * @{ */
enum {
    BLE_APPEARANCE_UNKNOWN                             =    0, /**< Unknown. */
    BLE_APPEARANCE_GENERIC_PHONE                       =   64, /**< Generic Phone. */
    BLE_APPEARANCE_GENERIC_COMPUTER                    =  128, /**< Generic Computer. */
    BLE_APPEARANCE_GENERIC_WATCH                       =  192, /**< Generic Watch. */
    BLE_APPEARANCE_WATCH_SPORTS_WATCH                  =  193, /**< Watch: Sports Watch. */
    BLE_APPEARANCE_GENERIC_CLOCK                       =  256, /**< Generic Clock. */
    BLE_APPEARANCE_GENERIC_DISPLAY                     =  320, /**< Generic Display. */
    BLE_APPEARANCE_GENERIC_REMOTE_CONTROL              =  384, /**< Generic Remote Control. */
    BLE_APPEARANCE_GENERIC_EYE_GLASSES                 =  448, /**< Generic Eye-glasses. */
    BLE_APPEARANCE_GENERIC_TAG                         =  512, /**< Generic Tag. */
    BLE_APPEARANCE_GENERIC_KEYRING                     =  576, /**< Generic Keyring. */
    BLE_APPEARANCE_GENERIC_MEDIA_PLAYER                =  640, /**< Generic Media Player. */
    BLE_APPEARANCE_GENERIC_BARCODE_SCANNER             =  704, /**< Generic Barcode Scanner. */
    BLE_APPEARANCE_GENERIC_THERMOMETER                 =  768, /**< Generic Thermometer. */
    BLE_APPEARANCE_THERMOMETER_EAR                     =  769, /**< Thermometer: Ear. */
    BLE_APPEARANCE_GENERIC_HEART_RATE_SENSOR           =  832, /**< Generic Heart rate Sensor. */
    BLE_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT   =  833, /**< Heart Rate Sensor: Heart Rate Belt. */
    BLE_APPEARANCE_GENERIC_BLOOD_PRESSURE              =  896, /**< Generic Blood Pressure. */
    BLE_APPEARANCE_BLOOD_PRESSURE_ARM                  =  897, /**< Blood Pressure: Arm. */
    BLE_APPEARANCE_BLOOD_PRESSURE_WRIST                =  898, /**< Blood Pressure: Wrist. */
    BLE_APPEARANCE_GENERIC_HID                         =  960, /**< Human Interface Device (HID). */
    BLE_APPEARANCE_HID_KEYBOARD                        =  961, /**< Keyboard (HID Subtype). */
    BLE_APPEARANCE_HID_MOUSE                           =  962, /**< Mouse (HID Subtype). */
    BLE_APPEARANCE_HID_JOYSTICK                        =  963, /**< Joystiq (HID Subtype). */
    BLE_APPEARANCE_HID_GAMEPAD                         =  964, /**< Gamepad (HID Subtype). */
    BLE_APPEARANCE_HID_DIGITIZERSUBTYPE                =  965, /**< Digitizer Tablet (HID Subtype). */
    BLE_APPEARANCE_HID_CARD_READER                     =  966, /**< Card Reader (HID Subtype). */
    BLE_APPEARANCE_HID_DIGITAL_PEN                     =  967, /**< Digital Pen (HID Subtype). */
    BLE_APPEARANCE_HID_BARCODE                         =  968, /**< Barcode Scanner (HID Subtype). */
    BLE_APPEARANCE_GENERIC_GLUCOSE_METER               = 1024, /**< Generic Glucose Meter. */
    BLE_APPEARANCE_GENERIC_RUNNING_WALKING_SENSOR      = 1088, /**< Generic Running Walking Sensor. */
    BLE_APPEARANCE_RUNNING_WALKING_SENSOR_IN_SHOE      = 1089, /**< Running Walking Sensor: In-Shoe. */
    BLE_APPEARANCE_RUNNING_WALKING_SENSOR_ON_SHOE      = 1090, /**< Running Walking Sensor: On-Shoe. */
    BLE_APPEARANCE_RUNNING_WALKING_SENSOR_ON_HIP       = 1091, /**< Running Walking Sensor: On-Hip. */
    BLE_APPEARANCE_GENERIC_CYCLING                     = 1152, /**< Generic Cycling. */
    BLE_APPEARANCE_CYCLING_CYCLING_COMPUTER            = 1153, /**< Cycling: Cycling Computer. */
    BLE_APPEARANCE_CYCLING_SPEED_SENSOR                = 1154, /**< Cycling: Speed Sensor. */
    BLE_APPEARANCE_CYCLING_CADENCE_SENSOR              = 1155, /**< Cycling: Cadence Sensor. */
    BLE_APPEARANCE_CYCLING_POWER_SENSOR                = 1156, /**< Cycling: Power Sensor. */
    BLE_APPEARANCE_CYCLING_SPEED_CADENCE_SENSOR        = 1157, /**< Cycling: Speed and Cadence Sensor. */
    BLE_APPEARANCE_GENERIC_PULSE_OXIMETER              = 3136, /**< Generic Pulse Oximeter. */
    BLE_APPEARANCE_PULSE_OXIMETER_FINGERTIP            = 3137, /**< Fingertip (Pulse Oximeter subtype). */
    BLE_APPEARANCE_PULSE_OXIMETER_WRIST_WORN           = 3138, /**< Wrist Worn(Pulse Oximeter subtype). */
    BLE_APPEARANCE_GENERIC_WEIGHT_SCALE                = 3200, /**< Generic Weight Scale. */
    BLE_APPEARANCE_GENERIC_OUTDOOR_SPORTS_ACT          = 5184, /**< Generic Outdoor Sports Activity. */
    BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_DISP         = 5185, /**< Location Display Device (Outdoor Sports Activity subtype). */
    BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_AND_NAV_DISP = 5186, /**< Location and Navigation Display Device (Outdoor Sports Activity subtype). */
    BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_POD          = 5187, /**< Location Pod (Outdoor Sports Activity subtype). */
    BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_AND_NAV_POD  = 5188, /**< Location and Navigation Pod (Outdoor Sports Activity subtype). */
};
/** @} */

/**************************************************************************/
/*!
    \brief  Error codes for the BLE API
*/
/**************************************************************************/
enum ble_error_t {
    BLE_ERROR_NONE                    = 0, /**< No error */
    BLE_ERROR_BUFFER_OVERFLOW         = 1, /**< The requested action would cause a buffer overflow and has been aborted */
    BLE_ERROR_NOT_IMPLEMENTED         = 2, /**< Requested a feature that isn't yet implement or isn't supported by the target HW */
    BLE_ERROR_PARAM_OUT_OF_RANGE      = 3, /**< One of the supplied parameters is outside the valid range */
    BLE_ERROR_INVALID_PARAM           = 4, /**< One of the supplied parameters is invalid */
    BLE_STACK_BUSY                    = 5, /**< The stack is busy */
    BLE_ERROR_INVALID_STATE           = 6, /**< Invalid state. */
    BLE_ERROR_NO_MEM                  = 7, /**< Out of Memory */
    BLE_ERROR_OPERATION_NOT_PERMITTED = 8,
    BLE_ERROR_UNSPECIFIED             = 9, /**< Unknown error. */
};

/** @brief Default MTU size. */
static const unsigned BLE_GATT_MTU_SIZE_DEFAULT = 23;

enum HVXType_t {
    BLE_HVX_NOTIFICATION = 0x01,  /**< Handle Value Notification. */
    BLE_HVX_INDICATION   = 0x02,  /**< Handle Value Indication. */
};

#ifdef __cplusplus
}
#endif

#endif // ifndef __BLE_COMMON_H__
