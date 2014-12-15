/**
 * @file log.h
 * @brief log system header file
 * @author Li Ming <Lee.dhucst@gmail.com>
 * @url www.freereaper.com
 * @version v1.0.0
 * @date 2014-12-14
 * @Copyright (c) DHU, CST.  All Rights Reserved
 */
#ifndef __LOG_H
#define __LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "printer.h"

#define LOG_MAX_LEN  1024


typedef enum log_level{
	LOGS_DEBUG = 0,
	LOGS_INFO,
	LOGS_NOTICE,
	LOGS_WARNING,
	LOGS_ERROR
}log_lvl_t;


typedef struct log_cfg {
	log_lvl_t level;
	int syslog_enabled;
	int verbose;
	const char *log_file;
}log_cfg_t;

void log_sys_init(const struct app_config *config);

#ifdef __GNUC__
void sys_log(log_lvl_t level, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
#else
void sys_log(log_lvl_t level, const char *fmt, ...);
#endif

#endif
