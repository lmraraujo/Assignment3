#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <sys/printk.h>
#include <sys/__assert.h>
#include <string.h>
#include <timing/timing.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define GPIO0_NID DT_NODELABEL(gpio0)
#define BOARDBUT1 11
#define BOARDBUT2 12
#define BOARDBUT3 24
#define BOARDBUT4 25
