/**
 * @file printer.c
 * @brief printer flow
 * @author Li Ming <Lee.dhucst@gmail.com>
 * @url www.freereaper.com
 * @version v1.0.0
 * @date 2014-12-14
 * @Copyright (c) DHU, CST.  All Rights Reserved
 */
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "common.h"
#include "log.h"
#include "musb.h"
#include "config.h"
#include "printer.h"


static usb_printer_t printer = {
	.libusb = {
		.device      = NULL,
		.handle      = NULL,
		.config      = -1,
		.interface   = -1,
		.alt_setting = -1
	},
	.w_buf = NULL,
	.r_buf = NULL,
	.open  = open_device,
	.write = device_write,
	.read  = device_read,
	.close = close_device
};

/* special command for pjl type printers */
static const char pjl_ustatus_cmd[] = "\e%-12345X@PJL USTATUS DEVICE = ON \r\n@PJL USTATUS JOB = ON \r\n@PJL JOB \r\n\e%-12345X";
static const char pjl_job_end_cmd[] = "\e%-12345X@PJL EOJ \r\n\e%-12345X";
static const char pjl_ustatus_off_cmd[] = "\e%-12345X@PJL USTATUSOFF \r\n\e%-12345X";



app_status_t printer_param_init(void)
{
	printer.w_buf = calloc(WR_BUFFER_SIZE, 1);
	printer.r_buf = calloc(RD_BUFFER_SIZE, 1);

	if (printer.w_buf == NULL || printer.r_buf == NULL) {
		sys_log(LOGS_ERROR, "can not allocate memory\n");
		if (printer.w_buf) free(printer.w_buf);
		if (printer.r_buf) free(printer.r_buf);
		return APP_NOMEM_ERR;
	}	
	
	return APP_STATUS_OK;
}

int main(int argc, char *argv)
{
	int len = 0;
	int status_len = 0;
	int fd;
	void *write_buf, *read_buf;
	
	struct app_config *config = &printer.config;

	if (init_server_config(config) != APP_STATUS_OK ) {
		 goto bugout;
	}

	log_sys_init(config);
	
	if (printer_param_init()!= APP_STATUS_OK ) {
		goto bugout;
	}

	if (printer.open(&printer.libusb)) {
		sys_log(LOGS_ERROR, "can not open printer\n");
		return APP_NODEV_ERR;
	}
	
	//fprintf(stderr, "%s , %d\n", __FUNCTION__, __LINE__);

	
#if 0
	fd = open(config->firmware, O_RDONLY);
	if (fd < 0) {
		sys_log(LOGS_ERROR, "unable to open firmware file \n");
		return SERVICE_ERR;
	}

	while ((len = read(fd, write_buf, sizeof(write_buf))) > 0) {
		int size = len;
		int total = 0;
		int write_bytes = 0;

		while (size > 0) {
			write_bytes = printer.write(&printer.libusb, write_buf+total, size, 5*1000);

			fprintf(stderr, "the rest length: %d, write length: %d\n", size, write_bytes);

			total += write_bytes;
			size  -= write_bytes;
		}

	}
	close(fd);

//	printf("%s, %d\n", __FUNCTION__, __LINE__);
	int	total_len = 0;
	while (total_len < (sizeof(pjl_ustatus_cmd) -1)) {
		len =	printer.write(&printer.libusb, pjl_ustatus_cmd+total_len, sizeof(pjl_ustatus_cmd)-1-total_len, 5*1000);
		total_len += len;
		fprintf(stderr, "write length : %d\n", len);
	}

	fd = open("/project/test.zc", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "unable to open firmware file \n");
		return SERVICE_ERR;
	}

	while ((len = read(fd, write_buf, sizeof(write_buf))) > 0) {
		int size = len;
		int total = 0;
		int write_bytes = 0;

		while (size > 0) {
			write_bytes = printer.write(&printer.libusb, write_buf+total, size, 5*1000);

			fprintf(stderr, "the rest length: %d, write length: %d\n", size, write_bytes);

			total += write_bytes;
			size  -= write_bytes;
		}

	}
	close(fd);



//	printf("%s, %d\n", __FUNCTION__, __LINE__);

	int i = 0;
	while ( i++ < 10) {
		status_len = printer.read(&printer.libusb, buff, sizeof(buff), 5*1000);
		buff[status_len] =	0 ;
		fprintf(stderr, "read buf length: %d, %s\n", status_len, buff);
	}

	total_len = 0;
	while (total_len < (sizeof(pjl_ustatus_cmd) -1)) {
		len =	printer.write(&printer.libusb, pjl_ustatus_cmd+total_len, sizeof(pjl_ustatus_cmd)-1-total_len, 5*1000);
		total_len += len;
		fprintf(stderr, "write length : %d\n", len);
	}

	printer.close(&printer.libusb);

	return SERVICE_OK;

#endif

bugout:
		return APP_STATUS_OK;

}
