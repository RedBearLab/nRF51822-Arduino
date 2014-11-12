
#include "RBL_API.h"

uint32_t RBL_SetDevName(const char *pstr)
{	
	uint32_t err_code;
	
	ble_gap_conn_sec_mode_t sec_mode;
	
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode); 
    err_code = sd_ble_gap_device_name_set(&sec_mode,  (const uint8_t *)pstr, strlen(pstr));

	return err_code;
}







