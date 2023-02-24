/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* 1000 msec = 1 sec */
/*Put #define stuff here*/
#define SLEEP_TIME_MS   1000
#define LED_ON_TIME_S 1

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(heartbeat)
#define LED1_NODE DT_ALIAS(buzzer)
#define LED2_NODE DT_ALIAS(ivdrip)
#define LED3_NODE DT_ALIAS(alarm)
#define LED4_NODE DT_ALIAS(error)
/* Button Config*/
#define BUTTON0_NODE DT_ALIAS(button0)
#define BUTTON1_NODE DT_ALIAS(button1)
#define BUTTON2_NODE DT_ALIAS(button2)
#define BUTTON3_NODE DT_ALIAS(button3)
/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
/*LED Config*/
static const struct gpio_dt_spec heartbeat_led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec buzzer_led = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec ivdrip_led = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec alarm_led = GPIO_DT_SPEC_GET(LED3_NODE, gpios);
static const struct gpio_dt_spec error_led = GPIO_DT_SPEC_GET(LED4_NODE, gpios);
/*BUTTON Config*/
static const struct gpio_dt_spec sleep = GPIO_DT_SPEC_GET(BUTTON0_NODE, gpios);
static const struct gpio_dt_spec freq_up = GPIO_DT_SPEC_GET(BUTTON1_NODE, gpios);
static const struct gpio_dt_spec freq_down = GPIO_DT_SPEC_GET(BUTTON2_NODE, gpios);
static const struct gpio_dt_spec reset = GPIO_DT_SPEC_GET(BUTTON3_NODE, gpios);
/*Call Back Config*/
static struct gpio_callback sleep_cb;
static struct gpio_callback freq_up_cb;
static struct gpio_callback freq_down_cb;
static struct gpio_callback reset_cb;
/*Button Bool Values Here*/
static bool sleep_detected = 0;
static bool freq_up_detected = 0;
static bool freq_down_detected = 0;
static bool reset_detected = 0;
/*Declarations Here*/
void sleep_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
void freq_up_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
void freq_down_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
void reset_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
/* Callback Functions*/
void sleep_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	sleep_detected = 1;
}
void freq_up_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	freq_up_detected = 1;
}
void freq_down_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	freq_down_detected = 1;
}
void reset_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	reset_detected = 1;
}

void main(void)
{
	int ret;
	/*This should check for the entire gpio0 interface*/
	if (!device_is_ready(heartbeat_led.port)) {
		return;
	}
	/*This should check for the entire gpio1 interface*/
	if (!device_is_ready(error_led.port)) {
		return;
	}
	/*Intiliaze LED Pins*/
	ret = gpio_pin_configure_dt(&heartbeat_led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}
	ret = gpio_pin_configure_dt(&buzzer_led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}
	ret = gpio_pin_configure_dt(&ivdrip_led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}
	ret = gpio_pin_configure_dt(&alarm_led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}
	ret = gpio_pin_configure_dt(&error_led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}
	/*Button Config*/
	ret = gpio_pin_configure_dt(&sleep, GPIO_INPUT);
	if (ret < 0){
		return;
	}
	ret = gpio_pin_configure_dt(&freq_up, GPIO_INPUT);
	if (ret < 0){
		return;
	}
	ret = gpio_pin_configure_dt(&freq_down, GPIO_INPUT);
	if (ret < 0){
		return;
	}
	ret = gpio_pin_configure_dt(&reset, GPIO_INPUT);
	if (ret < 0){
		return;
	}
	/*Associate Callback with GPIO Pin*/
	int err;
	err = gpio_pin_interrupt_configure_dt(&sleep, GPIO_INT_EDGE_TO_ACTIVE);
	if (err < 0){
		return;
	}	
	gpio_init_callback(&sleep_cb, sleep_callback, BIT(sleep.pin));
	gpio_add_callback(sleep.port, &sleep_cb);
	err = gpio_pin_interrupt_configure_dt(&freq_up, GPIO_INT_EDGE_TO_ACTIVE);
	if (err < 0){
		return;
	}	
	gpio_init_callback(&freq_up_cb, freq_up_callback, BIT(freq_up.pin));
	gpio_add_callback(freq_up.port, &freq_up_cb);
	err = gpio_pin_interrupt_configure_dt(&freq_down, GPIO_INT_EDGE_TO_ACTIVE);
	if (err < 0){
		return;
	}	
	gpio_init_callback(&freq_down_cb, freq_down_callback, BIT(freq_down.pin));
	gpio_add_callback(freq_down.port, &freq_down_cb);
	err = gpio_pin_interrupt_configure_dt(&reset, GPIO_INT_EDGE_TO_ACTIVE);
	if (err < 0){
		return;
	}	
	gpio_init_callback(&reset_cb, reset_callback, BIT(reset.pin));
	gpio_add_callback(reset.port, &reset_cb);

	while (1) {
		ret = gpio_pin_toggle_dt(&heartbeat_led);
		if (ret < 0) {
			return;
		}
		k_msleep(SLEEP_TIME_MS);
	}
}
