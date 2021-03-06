#ifndef HC_DEVICE_INFO_H_
#define HC_DEVICE_INFO_H_

#include "pstorage.h"
#include "app_error.h"
#include "ble_dis.h"
#include <stdint.h>
#include <string.h>
#include "HC_data_flash.h"

// Device Information
#define DEVICE_NAME       "Muse"             /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME "HZhuiche Inc."        /**< Manufacturer. Will be passed to Device Information Service. */
#define FW_REV_STR        "1.2.0"                /**< software Revision String. */
#define HW_REV_STR        "1.0.2"                /**< Hardware Revision String. */

void mac_get(void);
void ble_devinfo_serv_init(void);

#endif
