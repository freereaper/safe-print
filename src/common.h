/**
 * @file common.h
 * @brief header files
 * @author Li Ming <Lee.dhucst@gmail.com>
 * @url www.freereaper.com
 * @version v1.0.0
 * @date 2014-12-09
 * @Copyright (c) DHU, CST.  All Rights Reserved
 */

#ifndef __COMMON_H
#define __COMMON_H


/**-----------------------------------------------
**  define the application status code
*-------------------------------------------------*/
typedef enum app_status{
	APP_STATUS_OK  = 0,
	APP_CFG_ERR = -1,
	APP_IO_ERR = -2,
	APP_NOENT_ERR = -3,       /* no such file */
	APP_NOMEM_ERR = -4,
	APP_NODEV_ERR = -5,
	APP_USB_ERR   = -6,


}app_status_t;



#endif
