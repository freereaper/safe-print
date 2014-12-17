/**
 * @file config.h
 * @brief server config
 * @author Li Ming <Lee.dhucst@gmail.com>
 * @url www.freereaper.com
 * @version v1.0.0
 * @date 2014-12-14
 * @Copyright (c) DHU, CST.  All Rights Reserved
 */

#ifndef __CONFIG_H
#define __CONFIG_H
#include <string.h>

struct app_config {
	const char *firmware;

	const char *log_file;
	int log_level;
	bool log_verbose;
	bool syslog_enabled;

};

#define CONFIG_FILE   "config.ini"
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
#define DEFAULT_LOG_FILE ""


app_status_t init_server_config(struct app_config *config);


#endif
