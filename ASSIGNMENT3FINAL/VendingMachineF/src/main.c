/**
 * @file main.c
 * @brief Main file for vending machine control.
 * @authors     João Modesto, 97943;
 *              Luis Araujo, 93287.
 */

//Include 
#include <zephyr/kernel.h>      
#include <zephyr/device.h>      
#include <zephyr/devicetree.h>	
#include <zephyr/drivers/gpio.h>    
#include "../../assig3/src-gen/Statechart.h"
#include "../../assig3/src/sc_types.h"
#include "../../assig3/src-gen/Statechart_required.h"

/**
 * @brief Definitions of LED and Button nodes.
 */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

#define BUT0_NODE DT_ALIAS(sw0)
#define BUT1_NODE DT_ALIAS(sw1)
#define BUT2_NODE DT_ALIAS(sw2)
#define BUT3_NODE DT_ALIAS(sw3)

#define Euro1 1
#define Euro2 2
#define Balance 3
#define ReturnCrd 4
#define Prod1 5
#define Prod2 6
#define Prod3 7
#define NotEnoughCrd 8
#define DispProd 9
#define Refund 10

/**
 * @brief GPIO specifications for LEDs.
 */
static const struct gpio_dt_spec leds[]={
        GPIO_DT_SPEC_GET(LED0_NODE,gpios),
        GPIO_DT_SPEC_GET(LED1_NODE,gpios),
        GPIO_DT_SPEC_GET(LED2_NODE,gpios),
        GPIO_DT_SPEC_GET(LED3_NODE,gpios),
};

/**
 * @brief GPIO specifications for Buttons.
 */
static const struct gpio_dt_spec buts[]={
        GPIO_DT_SPEC_GET(BUT0_NODE,gpios),
        GPIO_DT_SPEC_GET(BUT1_NODE,gpios),
        GPIO_DT_SPEC_GET(BUT2_NODE,gpios),
        GPIO_DT_SPEC_GET(BUT3_NODE,gpios),
};

/**
 * @brief Statechart instance.
 */
struct Statechart statechart;

/**
 * @brief Callback function for button press event.
 * 
 * @param dev Pointer to the device structure.
 * @param cb Pointer to the callback structure.
 * @param pins Pins that triggered the interrupt.
 */
void buttonPressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins);

/**
 * @brief Initialize the system.
 */
void startconfig();

/**
 * @brief Main function.
 * 
 * @return int Return status.
 */
int main()
{
        statechart_init(&statechart);
        statechart_enter(&statechart);
        startconfig();
        while(1)
        {
                statechart_trigger_without_event(&statechart);  
        }
        return 0;
}

void startconfig(){
        int ret =0;
        static struct gpio_callback buttonCbData; 

        for (int i = 0; i < 4; i++) {
                if (!device_is_ready(leds[i].port) || !device_is_ready(buts[i].port)) {
                        return 0;
                }
        }

        for (int i = 0; i < 4; i++) {
                
                ret = gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT_INACTIVE);
                if (ret < 0) {
                return 0;
                }

                ret = gpio_pin_configure_dt(&buts[i], GPIO_INPUT);
                if (ret < 0) {
                return 0;
                }

                ret = gpio_pin_interrupt_configure_dt(&buts[i], GPIO_INT_EDGE_TO_ACTIVE);
                if (ret < 0) {
                return 0;
                }
        }

        gpio_init_callback(&buttonCbData, buttonPressed, BIT(buts[0].pin) | BIT(buts[1].pin) | BIT(buts[2].pin) | BIT(buts[3].pin));

        for (int i = 0; i < 4; i++) {
                gpio_add_callback(buts[i].port, &buttonCbData);
        }
        printk("\e[1;1H\e[2J");
        printk("Hello! Insert coins or try to browse our products");
}

void buttonPressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
        for (int i = 0; i < 4; i++) {
                if (dev == buts[i].port) {
                        if (pins & BIT(buts[i].pin)) {
                                switch (i) {
                                case 0:
                                        statechart_but_raise_e1(&statechart);
                                        break;
                                case 1:
                                        statechart_but_raise_e2(&statechart);
                                        break;
                                case 2:
                                        statechart_but_raise_browse(&statechart);
                                        break;
                                case 3:
                                        statechart_but_raise_enter(&statechart);
                                        break;
                                default:
                                        break;
                                }
                        }
                }
        }
}

/**
 * @brief Turn off all LEDs.
 * 
 * @param handle Pointer to the statechart instance.
 */
void statechart_ledsoff( Statechart* handle){
        gpio_pin_set(leds[0].port, leds[0].pin, 0);
        gpio_pin_set(leds[1].port, leds[1].pin, 0);
        gpio_pin_set(leds[2].port, leds[2].pin, 0);
        gpio_pin_set(leds[3].port, leds[3].pin, 0);
}

/**
 * @brief Turn on the specified LED.
 * 
 * @param handle Pointer to the statechart instance.
 * @param led LED index.
 */
void statechart_ledon( Statechart* handle, const sc_integer led){
        switch(led){
                case 0:
                        gpio_pin_set(leds[1].port, leds[1].pin, 1);     //Product 1
                        break;
                case 1:
                        gpio_pin_set(leds[0].port, leds[0].pin, 1);     //Product 2
                        break;
                case 2:
                        gpio_pin_set(leds[2].port, leds[2].pin, 1);     //Product Dispensed
                        break;
                case 3:
                        gpio_pin_set(leds[3].port, leds[3].pin, 1);     //Return Credit
                        break;
                case 4:
                        gpio_pin_set(leds[0].port, leds[0].pin, 1);     //Not enough credit ERROR
                        gpio_pin_set(leds[1].port, leds[1].pin, 1);     //All leds on
                        gpio_pin_set(leds[2].port, leds[2].pin, 1);
                        gpio_pin_set(leds[3].port, leds[3].pin, 1);
                        break;
                default:
                        break;
        }

}

/**
 * @brief Print messages based on statechart events.
 * 
 * @param handle Pointer to the statechart instance.
 * @param k Event ID.
 */
void statechart_print( Statechart* handle, const sc_integer k){
        switch(k){
                case Euro1:
                        printk("\e[1;1H\e[2J");
                        printk("\n1 euro inserted");
                        printk("\nBalance: %d euros", statechart.internal.Saldo);
                        break;
                case Euro2:
                        printk("\e[1;1H\e[2J");
                        printk("\n2 euros inserted");
                        printk("\nBalance: %d euros", statechart.internal.Saldo);
                        break;
                case Balance:
                        printk("\nBalance: %d euros", statechart.internal.Saldo);
                        break;
                case ReturnCrd:
                        printk("\e[1;1H\e[2J");
                        printk("\nPress Enter to get a refund");
                        break;
                case Prod1:
                        printk("\e[1;1H\e[2J");
                        printk("\nProduct 1 -> 1 euro");
                        break;
                case Prod2:
                        printk("\e[1;1H\e[2J");
                        printk("\nProduct 2 -> 2 euros");
                        break;
                case Prod3:
                        printk("\e[1;1H\e[2J");
                        printk("\nProduct 3 -> 3 euros");
                        break;
                case NotEnoughCrd:
                        printk("\e[1;1H\e[2J");
                        printk("\nNot Enough credit");
                        break;
                case DispProd:
                        printk("\e[1;1H\e[2J");
                        printk("\nProduct %d Dispensed", statechart.internal.option);
                        break;
                case Refund:
                        printk("\e[1;1H\e[2J");
                        printk("\nYou got refunded!");
                        break;
                default:
                        break;
        }

}