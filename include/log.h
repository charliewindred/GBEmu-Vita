#pragma once

#include <stdio.h>
#include <psp2/kernel/clib.h>
#include <psp2/io/fcntl.h>
#include <psp2/kernel/threadmgr.h>
#include <stdarg.h>
#include <string.h>

#define LOG_FILE "ux0:data/gbemu.log"

// Declare mutex (defined in log.c)
extern SceUID log_mutex;

void log_init();

void log_write(const char *fmt, ...);

void log_cleanup();

