#include <psp2/kernel/clib.h>
#include <psp2/io/fcntl.h>
#include <psp2/kernel/threadmgr.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define LOG_FILE "ux0:data/gbemu.log"

/*
    Logging file specifically for logging to a file in the Vita filesystem and psp2shell for debugging.
*/

// Mutex for thread safety
static SceUID log_mutex = -1;

// Initialize the log system (create mutex)
void log_init() {
    log_mutex = sceKernelCreateMutex("log_mutex", 0, 0, NULL);
}

// Logging to a file in the Vita filesystem and psp2shell for debugging.
void log_write(const char *fmt, ...) {
    if (log_mutex < 0) return; // Ensure mutex is initialized

    sceKernelLockMutex(log_mutex, 1, NULL); // Lock the log file

    va_list args;
    va_start(args, fmt);

    // Format the message
    char log_msg[512];
    vsnprintf(log_msg, sizeof(log_msg), fmt, args);

    // Print to PSP2Shell
    sceClibPrintf("[%s] %s\n", "INFO", log_msg);

    // Append to log file
    SceUID fd = sceIoOpen(LOG_FILE, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_APPEND, 0777);
    if (fd >= 0) {
        char file_msg[512];
        snprintf(file_msg, sizeof(file_msg), "[%s] %s\n", "INFO", log_msg);
        sceIoWrite(fd, file_msg, strlen(file_msg));
        sceIoClose(fd);
    }

    va_end(args);
    sceKernelUnlockMutex(log_mutex, 1);
}

// Cleanup function
void log_cleanup() {
    if (log_mutex >= 0) {
        sceKernelDeleteMutex(log_mutex);
    }
}

