#pragma once
#include "types.h"

extern SemaphoreHandle_t xSemaphore_getSpeed;
extern SemaphoreHandle_t xSemaphore_getRosSpeed;
extern EventGroupHandle_t initialization_groupEvent;

extern const int task0_init_done;
extern const int task1_init_done;
