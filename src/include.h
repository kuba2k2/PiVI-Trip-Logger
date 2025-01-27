// Copyright (c) Kuba Szczodrzyński 2025-1-25.

#pragma once

#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <errno.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#if __has_include(<linux/can.h>)
#include <linux/can.h>
#include <linux/can/raw.h>
#else
struct can_frame {
	uint32_t can_id;
	uint8_t can_dlc;
	uint8_t data[8];
};
#endif

#include <sqlite3.h>

#include "core/config.h"
#include "core/errmacros.h"
#include "core/logger.h"
#include "core/utils.h"

#include "data/measurement.h"
#include "data/record.h"
#include "data/trip.h"
#include "db.h"
#include "frames.h"
