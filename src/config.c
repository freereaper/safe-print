/**
 * @file config.c
 * @brief server configuration
 * @author Li Ming <Lee.dhucst@gmail.com>
 * @url www.freereaper.com
 * @version v1.0.0
 * @date 2014-12-14
 * @Copyright (c) DHU, CST.  All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ini.h>
#include "common.h"
#include "printer.h"
#include "log.h"
#include "config.h"


static char *strdup(const char *str)
{
    int n = strlen(str) + 1;
    char *dup = malloc(n * sizeof(char));
    if(dup) {
        strcpy(dup, str);
    }
    return dup;
}

static bool resolve_bool(const char *val)
{
	if (strcasecmp(val, "false") == 0) {
		return false;
	}
	else if (strcasecmp(val, "true") == 0) {
		return true;
	}
	else {
		sys_log(LOGS_ERROR, "unsupported config files\n");
		exit(-1);
	}

}

static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    struct app_config *config = (struct app_config *)user;

    if (MATCH("log", "log_file")) {
		config->log_file = strdup(value);
    }
    else if (MATCH("log", "verbose")) {
		config->log_verbose = resolve_bool(value);
    }
    else if (MATCH("log", "syslog_enabled")) {
		config->syslog_enabled =  resolve_bool(value);
    }
    else if (MATCH("log", "level")) {
		config->log_level = atoi(value);
    }
    else if (MATCH("app", "firmware")) {
    	config->firmware = strdup(value);
    }
    else {
		return 0;  /* unknown section/name, error */
    }

		return 1;
}


app_status_t init_server_config(struct app_config *config)
{
	/* default settings if config file does not set */
	config->log_file       = DEFAULT_LOG_FILE;
	config->log_level      = 0;
	config->log_verbose    = true;
	config->syslog_enabled = false;


	/* read value from config file */
    if (ini_parse("config.ini", handler, config) < 0) {
        sys_log(LOGS_ERROR, "Can't load config file\n");
        return APP_NOENT_ERR;
    }

	return APP_STATUS_OK;

}
