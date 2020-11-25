//
// Created by shawn lv on 2020-11-24.
//

#include "util.h"

float time_interval(struct timeval start, struct timeval end) {
  long timeInterval = (end.tv_sec - start.tv_sec) * 1000000L + end.tv_usec - start.tv_usec;
  return timeInterval / 1000000.0f;
}