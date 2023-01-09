#ifndef _LOG_H
#define _LOG_H

#include <stdbool.h>

bool LOGGING_ENABLED = true;
void klog(char *fmt, ...);

#endif