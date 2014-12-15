/**
 * @file log.c
 * @brief log system init
 * @author Li Ming <Lee.dhucst@gmail.com>
 * @url www.freereaper.com
 * @version v1.0.0
 * @date 2014-12-14
 * @Copyright (c) DHU, CST.  All Rights Reserved
 */
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>
#include "log.h"

static log_cfg_t log_cfg;
static const log_lvl_t log_level[] = {LOGS_DEBUG, LOGS_INFO, LOGS_NOTICE, LOGS_WARNING, LOGS_ERROR};
static const int syslog_map[]      = {LOG_DEBUG, LOG_INFO, LOG_NOTICE, LOG_WARNING, LOG_ERR};
static const char log_symbol[]     = {'.', '-' , '*', '#', '@'};


void log_sys_init(const struct app_config *config)
{
	log_cfg.level          = log_level[config->log_level];
	log_cfg.log_file       = config->log_file;
	log_cfg.verbose        = config->log_verbose;
	log_cfg.syslog_enabled = config->syslog_enabled;
	if (log_cfg.syslog_enabled == true) {
		openlog("printer", LOG_PID | LOG_NDELAY | LOG_NOWAIT, LOG_DAEMON);
	}

}

static void log_output(log_lvl_t level, const char *msg)
{
	FILE *fp;
	int log_to_stdout = log_cfg.log_file[0] == '\0';

	if (log_to_stdout == 1) {
		fp = stdout;
	}
	else {
		fp = fopen(log_cfg.log_file, "a");
		if (fp == NULL) {
			return;
		}
	}

	if (log_cfg.verbose) {
		char buf[100];
		struct timeval tv;
		int off;
		gettimeofday(&tv, NULL);

		off = strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S.", localtime(&tv.tv_sec));
		snprintf(buf+off, sizeof(buf)-off, "%03d", (int)tv.tv_usec/1000);
		fprintf(fp, "[%d] %s %c %s", (int)getpid(), buf, log_symbol[level], msg);
	}
	else {
		fprintf(fp, "%s\n", msg);
	}
	fflush(fp);

	if (!log_to_stdout) {
		 fclose(fp);
	}
	if (log_cfg.syslog_enabled) {
		syslog(syslog_map[level], "%s", msg);
	}

}


void sys_log(log_lvl_t level, const char *fmt, ...)
{
	va_list ap;
	char msg[LOG_MAX_LEN];

	if(level < log_cfg.level) return;
		
	va_start(ap, fmt);
	vsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);

	log_output(level, msg);

}
