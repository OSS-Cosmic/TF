#ifndef _TF_TYPES_H
#define _TF_TYPES_H

#include <cstddef>
#include <stdint.h>
#include <cstdarg>

#define TF_ARRAY_COUNT(array) (sizeof(array) / (sizeof(array[0]) * (sizeof(array) != PTR_SIZE || sizeof(array[0]) <= PTR_SIZE)))

#endif
