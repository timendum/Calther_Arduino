#ifndef _PAPERDINK_H_
#define _PAPERDINK_H_

#include <Arduino.h>
#include "pin_assignment.h"

// Paperdink Devices
#include "devices/classic.h"

// Conversion factor for seconds to microseconds
#define S_TO_uS_FACTOR (1000000)
#define M_TO_uS_FACTOR (6e7)
#define H_TO_uS_FACTOR (3.6e9)

extern PAPERDINK_DEVICE Paperdink;

#endif /* _PAPERDINK_H_ */
