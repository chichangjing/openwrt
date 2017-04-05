/*
 * Copyright (C) 2013 Felix Fietkau <nbd@openwrt.org>
 * Copyright (C) 2013 John Crispin <blogic@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __LOG_H
#define __LOG_H

#include <libubox/ulog.h>

#define LOG_NORMAL(fmt,...)     ulog(LOG_INFO,      "[INFO][%s][%d]: "fmt,  __func__,__LINE__, ## __VA_ARGS__)
#define LOG_ERROR(fmt,...)      ulog(LOG_ERR,       "[ERROR][%s][%d]: "fmt, __func__,__LINE__, ## __VA_ARGS__)
#define LOG_WARN(fmt,...)       ulog(LOG_WARNING,   "[WARN][%s][%d]: "fmt,  __func__,__LINE__, ## __VA_ARGS__)

#define LOG_SET_THRESHOLD(v) ulog_threshold(v & LOG_PRIMASK)

#endif
