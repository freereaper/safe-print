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
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
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

static app_status_t printer_write(void *buf, int size, int timeout_sec, int *bytes_write);
static app_status_t printer_read(void *buf, int size, int timeout_sec, int *bytes_read);



app_status_t printer_param_init(void)
{
	printer.w_buf = calloc(WR_BUFFER_SIZE, 1);
	printer.r_buf = calloc(RD_BUFFER_SIZE, 1);

	if (printer.w_buf == NULL || printer.r_buf == NULL) {
		sys_log(LOGS_ERROR, "can not allocate memory\n");
		if (printer.w_buf) {
			free(printer.w_buf);
			printer.w_buf = NULL;
		}
		if (printer.r_buf) {
			free(printer.r_buf);
			printer.r_buf = NULL;
		}
		return APP_NOMEM_ERR;
	}

	return APP_STATUS_OK;
}

static app_status_t load_firmware()
{
	int len;
	int fd = open(printer.config.firmware, O_RDONLY);
	void *write_buf = printer.w_buf;
	app_status_t status = APP_NOENT_ERR;
	if (fd < 0) {
		sys_log(LOGS_ERROR, "unable to open firmware file \n");
	}
	else {
		while ((len = read(fd, write_buf, sizeof(write_buf))) > 0) {
			int write_bytes = 0;
			
			status = printer_write(write_buf, len, 10, &write_bytes);
			
			if (status != APP_STATUS_OK) {
				break;
			}
			
		}
	}
	
	close(fd);
	
	return status;
	
}

static void prase_status(char *buff, int *status, int *end_page)
{
	char *p;
	
	if (buff[0] == '\0') {
		return;	
	}
	
	if ((p = strcasestr(buff, "code=")) != NULL) {
		*status = strtol(p+5, NULL, 10);
	}
	
	if ((p = strcasestr(buff, "ustatus job")) != NULL) {
		if (strncasecmp(p+13, "end", 3) == 0) {
			if ((p = strcasestr(p+5+13, "pages=")) != NULL) {
				*end_page = strtol(p+6, NULL, 10);
			}
		}
	}
}



static void read_status_thread(void *args)
{
	struct printer_task *task = &(printer.current_task);
	int bytes_read;
	app_status_t status = APP_STATUS_OK;
	
	pthread_detach(pthread_self());
	sys_log(LOGS_INFO, "Starting read status thread %d\n", (int)printer.current_task.tid);
	
	/* default is ready */
	printer.status_code = 10001;
	task->end_page  	 = 0;
	task->done      	 = 0;
	task->abort     	 = 0;
	
	while (!task->abort) {
		status = printer_read(printer.r_buf, RD_BUFFER_SIZE, 0, &bytes_read);
		
		switch (status) {
		case APP_STATUS_OK:
			prase_status((char *)(printer.r_buf), &printer.status_code, &task->end_page);
			break;
		case APP_TIMEOUT_ERR:
			sleep(1);
			break;
		case APP_IO_ERR:
			printer.status_code = 5000+status;
		default:
			goto io_err;
		}	
	}
	
io_err:
	task->done = 1;
	pthread_cond_signal(&task->read_done_cond);
	
}


static app_status_t printer_start(void)
{
	int bytes_write;
	app_status_t ret = APP_STATUS_OK;
	
	do {
		if ((ret = load_firmware()) != APP_STATUS_OK) {
			break;
		}
		
		printer_write((void *)pjl_ustatus_cmd, sizeof(pjl_ustatus_cmd) - 1, 3, &bytes_write);
		pthread_mutex_init(&printer.current_task.mutex, NULL);
		pthread_cond_init(&printer.current_task.read_done_cond, NULL);
		pthread_create(&printer.current_task.tid, NULL, (void *(*)(void *))read_status_thread, NULL);
	} while (0);
	
	return ret;

}

int main(int argc, char *argv)
{
	int len = 0;
	int status_len = 0;
	int fd;
	int write_bytes = 0;
	app_status_t ret;

	struct app_config *config = &printer.config;

	ret = init_server_config(config);
	if (ret != APP_STATUS_OK ) {
		goto err1_out;
	}

	log_sys_init(config);

	ret = printer_param_init();
	if (ret!= APP_STATUS_OK) {
		goto err1_out;
	}
	
	ret = printer.open(&printer.libusb);
	if ( ret != APP_STATUS_OK) {
		goto err2_out;
	}

	ret = printer_start();
	if (ret != APP_STATUS_OK) {
		goto err3_out;
	}
	
	
	fd = open("/project/test.zc", O_RDONLY);
	if (fd < 0) {
		sys_log(LOGS_ERROR, "unable to open write file \n");
		goto err4_out;
	}

	while ((len = read(fd, printer.w_buf, sizeof(printer.w_buf))) > 0) {
		app_status_t status = printer_write(printer.w_buf, len, 10, &write_bytes);
		if (status != APP_STATUS_OK) {
			close(fd);
			goto err4_out;
		}
	}
	close(fd);
	
	printer_write((void *)pjl_job_end_cmd, sizeof(pjl_job_end_cmd)-1, 3, &write_bytes);
	printer_write((void *)pjl_ustatus_off_cmd, sizeof(pjl_ustatus_off_cmd)-1, 3, &write_bytes);
	

err4_out:
	pthread_mutex_lock(&printer.current_task.mutex);
	printer.current_task.abort = 1;
	while (!printer.current_task.done) {
		pthread_cond_wait(&printer.current_task.read_done_cond, &printer.current_task.mutex);
	}
	pthread_mutex_unlock(&printer.current_task.mutex);
	pthread_mutex_destroy(&printer.current_task.mutex);
	pthread_cond_destroy(&printer.current_task.read_done_cond);

err3_out:
	printer.close(&printer.libusb);
err2_out:
	free(printer.w_buf);
	free(printer.r_buf);
	printer.w_buf = NULL;
	printer.r_buf = NULL;
err1_out:
	return ret;
}

static app_status_t printer_write(void *buf, int size, int timeout_sec, int *bytes_write)
{
	int usec;
	int len = 0;
	int total = 0;
	int retry_cnt = 0;
	app_status_t status = APP_STATUS_OK;
	
	usec = timeout_sec < 5 ? 5 * 1000000 : timeout_sec * 1000000;
	
	while (total < size && retry_cnt < WRITE_RETRY_MAX) {
		len = printer.write(&printer.libusb, buf+total,  size-total, usec);
		
		if (printer.status_code >= 41000) {
			sys_log(LOGS_ERROR, "printer break down \n");
			status = APP_IO_ERR;
			break;
		}
		
		if ( len < 0) {
			sys_log(LOGS_ERROR, "write failed\n");
			status = APP_IO_ERR;
			break;
		}
		total += len;
		retry_cnt++;
	}
	
	
	if (retry_cnt == WRITE_RETRY_MAX) {
		status = APP_TIMEOUT_ERR;
	}
		
	*bytes_write = total;
	
	return status;
}

static app_status_t printer_read(void *buf, int size, int timeout_sec, int *bytes_read)
{
	int usec;
	int len = 0;
	
	*bytes_read = 0;
	
	/* minmum timeout is 1ms for libusb */
	if (timeout_sec == 0) {
		usec = 1000;
	}
	else {
		usec = 1000000*timeout_sec;
	}

	len = printer.read(&printer.libusb, buf, size, usec);
	
	if (len == -ETIMEDOUT) {
		return APP_TIMEOUT_ERR;
	}
	else if (len < 0) {
		return APP_IO_ERR;	
	}
	else {
		*bytes_read = len;
		*((char *)buf + len)= '\0';
		return APP_STATUS_OK;
	}
	
}
