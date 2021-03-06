/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup ble_sdk_srv_eeg Heart Rate Service
 * @{
 * @ingroup ble_sdk_srv
 * @brief Heart Rate Service module.
 *
 * @details This module implements the Heart Rate Service with the Heart Rate Measurement,
 *          Body Sensor Location and Heart Rate Control Point characteristics.
 *          During initialization it adds the Heart Rate Service and Heart Rate Measurement
 *          characteristic to the BLE stack database. Optionally it also adds the
 *          Body Sensor Location and Heart Rate Control Point characteristics.
 *
 *          If enabled, notification of the Heart Rate Measurement characteristic is performed
 *          when the application calls ble_eeg_send().
 *
 *          The Heart Rate Service also provides a set of functions for manipulating the
 *          various fields in the Heart Rate Measurement characteristic, as well as setting
 *          the Body Sensor Location characteristic value.
 *
 *          If an event handler is supplied by the application, the Heart Rate Service will
 *          generate Heart Rate Service events to the application.
 *
 * @note The application must propagate BLE stack events to the Heart Rate Service module by calling
 *       ble_eeg_on_ble_evt() from the @ref softdevice_handler callback.
 *
 * @note Attention! 
 *  To maintain compliance with Nordic Semiconductor ASA Bluetooth profile 
 *  qualification listings, this section of source code must not be modified.
 */

#ifndef BLE_EEG_H__
#define BLE_EEG_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

// Body Sensor Location values
#define BLE_EEG_BODY_SENSOR_LOCATION_OTHER      0
#define BLE_EEG_BODY_SENSOR_LOCATION_CHEST      1
#define BLE_EEG_BODY_SENSOR_LOCATION_WRIST      2
#define BLE_EEG_BODY_SENSOR_LOCATION_FINGER     3
#define BLE_EEG_BODY_SENSOR_LOCATION_HAND       4
#define BLE_EEG_BODY_SENSOR_LOCATION_EAR_LOBE   5
#define BLE_EEG_BODY_SENSOR_LOCATION_FOOT       6

#define BLE_EEG_MAX_BUFFERED_RR_INTERVALS       20      /**< Size of RR Interval buffer inside service. */
#define BLE_UUID_EEG_SERVICE 0xFF30                      /**< The UUID of the Nordic UART Service. */

#define BLE_com_MAX_DATA_LEN (GATT_MTU_SIZE_DEFAULT - 3) /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */

/**@brief Heart Rate Service event type. */
typedef enum
{
    BLE_EEG_EVT_NOTIFICATION_ENABLED,                   /**< Heart Rate value notification enabled event. */
    BLE_EEG_EVT_NOTIFICATION_DISABLED                   /**< Heart Rate value notification disabled event. */
} ble_EEG_evt_type_t;

/**@brief Heart Rate Service event. */
typedef struct
{
    ble_EEG_evt_type_t evt_type;                        /**< Type of event. */
} ble_eeg_evt_t;

// Forward declaration of the ble_eeg_t type. 
typedef struct ble_eeg_s ble_eeg_t;

/**@brief Heart Rate Service event handler type. */
typedef void (*ble_eeg_evt_handler_t) (ble_eeg_t * p_eeg, ble_eeg_evt_t * p_evt);

/**@brief Heart Rate Service init structure. This contains all options and data needed for
 *        initialization of the service. */
typedef struct
{
    ble_eeg_evt_handler_t        evt_handler;                                          /**< Event handler to be called for handling events in the Heart Rate Service. */
} ble_eeg_init_t;

/**@brief Heart Rate Service structure. This contains various status information for the service. */
struct ble_eeg_s
{
	  uint8_t                      uuid_type;               /**< UUID type for Nordic UART Service Base UUID. */
    ble_eeg_evt_handler_t        evt_handler;                                          /**< Event handler to be called for handling events in the Heart Rate Service. */
    uint16_t                     service_handle;                                       /**< Handle of Heart Rate Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t     ele_state_handles;
  	ble_gatts_char_handles_t     eeg_handles;                                          /**< Handles related to the Heart Rate Measurement characteristic. */
    uint16_t                     conn_handle;                                          /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    uint8_t                      last_state;
  	bool                         is_eeg_notification_enabled;                              /**< Variable to indicate if the peer has enabled notification of the RX characteristic.*/
  	bool                         is_state_notification_enabled;                              /**< Variable to indicate if the peer has enabled notification of the RX characteristic.*/
};

/**@brief Function for initializing the Heart Rate Service.
 *
 * @param[out]  p_eeg       Heart Rate Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_eeg_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_eeg_init(ble_eeg_t * p_eeg, const ble_eeg_init_t * p_eeg_init);

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the Heart Rate Service.
 *
 * @param[in]   p_eeg      Heart Rate Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_eeg_on_ble_evt(ble_eeg_t * p_eeg, ble_evt_t * p_ble_evt);

/**@brief Function for sending heart rate measurement if notification has been enabled.
 *
 * @details The application calls this function after having performed a heart rate measurement.
 *          If notification has been enabled, the heart rate measurement data is encoded and sent to
 *          the client.
 *
 * @param[in]   p_eeg                    Heart Rate Service structure.
 * @param[in]   heart_rate               New heart rate measurement.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_EEG_DATA_send(ble_eeg_t * p_eeg, uint8_t * p_string, uint16_t length);
uint32_t ble_EEG_ELE_STATE_send(ble_eeg_t * p_eeg, uint8_t state, uint16_t length);
/**@brief Function for adding a RR Interval measurement to the RR Interval buffer.
 *
 * @details All buffered RR Interval measurements will be included in the next heart rate
 *          measurement message, up to the maximum number of measurements that will fit into the
 *          message. If the buffer is full, the oldest measurement in the buffer will be deleted.
 *
 * @param[in]   p_eeg        Heart Rate Service structure.
 * @param[in]   rr_interval  New RR Interval measurement (will be buffered until the next
 *                           transmission of Heart Rate Measurement).
 */
void ble_eeg_rr_interval_add(ble_eeg_t * p_eeg, uint16_t rr_interval);

/**@brief Function for checking if RR Interval buffer is full.
 *
 * @param[in]   p_eeg        Heart Rate Service structure.
 *
 * @return      true if RR Interval buffer is full, false otherwise.
 */
bool ble_eeg_rr_interval_buffer_is_full(ble_eeg_t * p_eeg);

/**@brief Function for setting the state of the Sensor Contact Supported bit.
 *
 * @param[in]   p_eeg                        Heart Rate Service structure.
 * @param[in]   is_sensor_contact_supported  New state of the Sensor Contact Supported bit.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_eeg_sensor_contact_supported_set(ble_eeg_t * p_eeg, bool is_sensor_contact_supported);

/**@brief Function for setting the state of the Sensor Contact Detected bit.
 *
 * @param[in]   p_eeg                        Heart Rate Service structure.
 * @param[in]   is_sensor_contact_detected   TRUE if sensor contact is detected, FALSE otherwise.
 */
void ble_eeg_sensor_contact_detected_update(ble_eeg_t * p_eeg, bool is_sensor_contact_detected);

/**@brief Function for setting the Body Sensor Location.
 *
 * @details Sets a new value of the Body Sensor Location characteristic. The new value will be sent
 *          to the client the next time the client reads the Body Sensor Location characteristic.
 *
 * @param[in]   p_eeg                 Heart Rate Service structure.
 * @param[in]   body_sensor_location  New Body Sensor Location.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_eeg_body_sensor_location_set(ble_eeg_t * p_eeg, uint8_t body_sensor_location);

#endif // BLE_eeg_H__

/** @} */
