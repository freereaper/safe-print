/**
 * @file printer.h
 * @brief printer flow
 * @author Li Ming <Lee.dhucst@gmail.com>
 * @url www.freereaper.com
 * @version v1.0.0
 * @date 2014-12-14
 * @Copyright (c) DHU, CST.  All Rights Reserved
 */
#ifndef __PRINTER_H
#define __PRINTER_H

#include <libusb.h>
#include <stdbool.h>
#include <pthread.h>
#include "common.h"
#include "musb.h"
#include "config.h"

#define WR_BUFFER_SIZE   16*1024
#define RD_BUFFER_SIZE    1*1024
#define WRITE_RETRY_MAX   3




struct printer_task {
	int done;
	int abort;
	int end_page;
	pthread_t tid;
	pthread_mutex_t mutex;
	pthread_cond_t read_done_cond;
};

typedef struct usb_printer{
	struct libusb_component libusb;
	struct app_config config;
	struct printer_task current_task;
	void *r_buf;
	void *w_buf;
	int status_code;

	app_status_t (*open)(struct libusb_component  *libusb_component);
	int (*write)(struct libusb_component *com, const void *buff, int size, int usec);
	int (*read)(struct libusb_component *com, void *buff, int size, int usec);
	app_status_t (*close)(struct libusb_component *libusb_component);

}usb_printer_t;

#endif
