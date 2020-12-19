#ifndef SEVEN_SEGMENT_H_
#define SEVEN_SEGMENT_H_ 1

#include <stdbool.h>

#include "time.h"

void
seg_init(void);

void
seg_display_time_props_async(struct time_props *props, bool *done);

void
seg_display_time_props_async_continue();

#endif
