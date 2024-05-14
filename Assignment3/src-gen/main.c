/* Includes. Add according to the resources used  */
#include <zephyr/kernel.h>          /* for k_msleep() */
#include <zephyr/device.h>          /* for device_is_ready() and device structure */
#include <zephyr/devicetree.h>      /* for DT_NODELABEL() */
#include <zephyr/drivers/gpio.h>    /* for GPIO api*/
#include "../../assignmemt3/src-gen/Statechart.h"
#include "../../assignmemt3/src/sc_types.h"

/* Get led0 and button0 node IDs. Refer to the DTS file */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

#define BUT0_NODE DT_ALIAS(sw0)
#define BUT1_NODE DT_ALIAS(sw1)
#define BUT2_NODE DT_ALIAS(sw2)
#define BUT3_NODE DT_ALIAS(sw3)

static const struct gpio_dt_spec leds[]={
     GPIO_DT_SPEC_GET(LED0_NODE,gpios),
     GPIO_DT_SPEC_GET(LED1_NODE,gpios),
     GPIO_DT_SPEC_GET(LED2_NODE,gpios),
     GPIO_DT_SPEC_GET(LED3_NODE,gpios),
};
static const struct gpio_dt_spec buts[]={
     GPIO_DT_SPEC_GET(BUT0_NODE,gpios),
 GPIO_DT_SPEC_GET(BUT1_NODE,gpios),
 GPIO_DT_SPEC_GET(BUT2_NODE,gpios),
 GPIO_DT_SPEC_GET(BUT3_NODE,gpios),
};

/*void statechart_but_raise_e1(Statechart* handle);
void statechart_but_raise_e2(Statechart* handle);
void statechart_but_raise_e1(Statechart* handle);*/


struct Statechart statechart;
struct StatechartInternal statechartint;
  
void buttonPressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    for (int i = 0; i < 4; i++) {
        if (dev == buts[i].port) {
            if (pins & BIT(buts[i].pin)) {
                // Button i is pressed
                //printk("Button %d pressed\n", i);
                
                // Depending on your requirements, you can raise events in the statechart here
                switch (i) {
                    case 0:
                        statechart_but_raise_e1(&statechart);
                        gpio_pin_set(leds[0].port, leds[0].pin, 0);
                        gpio_pin_set(leds[1].port, leds[1].pin, 1);
                        printk("\n1€ inserted");
                        printk("\nBalance: %d€", statechartint.Saldo);
                        break;
                    case 1:
                        statechart_but_raise_e2(&statechart);
                        gpio_pin_set(leds[0].port, leds[0].pin, 1);
                        gpio_pin_set(leds[1].port, leds[1].pin, 0);
                        printk("\n2€ inserted");
                        printk("\nBalance: %d€", statechartint.Saldo);
                        break;
                    case 2:
                        statechart_but_raise_browse(&statechart);
                        printk("\nProduct %d -> %d ", statechartint.Saldo, statechartint.Saldo);
                        break;
                    case 3:
                        statechart_but_raise_enter(&statechart);
                        if(statechartint.option==0){
                            gpio_pin_set(leds[0].port, leds[0].pin, 0);
                            gpio_pin_set(leds[1].port, leds[1].pin, 0);
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }
}




int main()
{
        int ret =0;
    static struct gpio_callback buttonCbData; // Define variable for holding callback data

    /* Check if devices are ready */
    for (int i = 0; i < 4; i++) {
        if (!device_is_ready(leds[i].port) || !device_is_ready(buts[i].port)) {
            return 0;
        }
    }

    /* Configure the GPIO pins for input/output and set active logic */
    for (int i = 0; i < 4; i++) {
        
        ret = gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT_INACTIVE);
        if (ret < 0) {
            return 0;
            }

        ret = gpio_pin_configure_dt(&buts[i], GPIO_INPUT);
        if (ret < 0) {
            return 0;
        }

        /* Set interrupt */
        ret = gpio_pin_interrupt_configure_dt(&buts[i], GPIO_INT_EDGE_TO_ACTIVE);
        if (ret < 0) {
            return 0;
        }
    }

    gpio_init_callback(&buttonCbData, buttonPressed, BIT(buts[0].pin) | BIT(buts[1].pin) | BIT(buts[2].pin) | BIT(buts[3].pin));

    /* Add the callback function by calling gpio_add_callback()   */
    for (int i = 0; i < 4; i++) {
    gpio_add_callback(buts[i].port, &buttonCbData);
    }


    statechart_init(&statechart);
    statechart_enter(&statechart);

    printk("Hello! Insert coins or try to browse our products");
    while(1)
    {
        statechart_trigger_without_event(&statechart);
    }
    
   

}

