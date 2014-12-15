/**
 * @file musb.h
 * @brief header files
 * @author Li Ming <Lee.dhucst@gmail.com>
 * @url www.freereaper.com
 * @version v1.0.0
 * @date 2014-12-09
 * @Copyright (c) DHU, CST.  All Rights Reserved
 */

#ifndef __MUSB_H
#define __MUSB_H

#include <string.h>
#include <stdio.h>
#include <libusb.h>
#include "common.h"



struct libusb_component {
	struct libusb_device *device;
	struct libusb_device_handle *handle;
	int config;
	int interface;
	int alt_setting;
};


app_status_t open_device(struct libusb_component  *libusb_component);
int device_write(struct libusb_component *com, const void *buff, int size, int usec);
int device_read(struct libusb_component *com, void *buff, int size, int usec);
app_status_t close_device(struct libusb_component *libusb_component);


#endif
