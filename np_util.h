#pragma once
#include "np.h"

#include <stddef.h>

void free_array(char **buf, size_t size);
void free_info(struct np_info *info);
