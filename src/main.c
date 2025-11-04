#include <stdlib.h>
#include <time.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <lvgl.h>
#include "ui.h"
#include "led_strip.h"

// void btn_event_cb(lv_event_t * e) {
//     lv_obj_t * btn = lv_event_get_target(e);
//     lv_obj_t * label = lv_obj_get_child(btn, 0);

//     lv_label_set_text(label, "Clicked!");
//     printk("Button pressed!\n");
// }

#define SERSOR_UPDATE_STACK_SIZE 1024
#define SERSOR_UPDATE_PRIORITY 5

static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios);

K_THREAD_STACK_DEFINE(sensor_update_stack, SERSOR_UPDATE_STACK_SIZE);

static struct k_thread sensor_update_thread;

static void sensor_update_thread_fn(void *arg1, void *arg2, void *arg3)
{
    while (1)
    {
        // giả lập dữ liệu cảm biến
        int temperature = rand() % 40;
        int humidity = rand() % 100;   
        char temp_str[16];
        char hum_str[16];
        snprintf(temp_str, sizeof(temp_str), "%d C", temperature);
        snprintf(hum_str, sizeof(hum_str), "%d %%", humidity);
        lv_label_set_text(ui_temperature1, temp_str);
        lv_label_set_text(ui_humidity, hum_str);
        k_sleep(K_MSEC(1000));
    }
}

void ui_event_Button3(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *sw = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED)
    {
        if (lv_obj_has_state(sw, LV_STATE_CHECKED))
        {
            printf("Switch ON\n");
            gpio_pin_set_dt(&led1, 1);
        }
        else
        {
            printf("Switch OFF\n");
            gpio_pin_set_dt(&led1, 0);
        }
    }
}

void ui_event_Button4(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *sw = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED)
    {
        if (lv_obj_has_state(sw, LV_STATE_CHECKED))
        {
            printf("Switch ON\n");
            gpio_pin_set_dt(&led2, 1);
        }
        else
        {
            printf("Switch OFF\n");
            gpio_pin_set_dt(&led2, 0);
        }
    }
}

void ui_event_SliderChange(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        lv_obj_t *slider = lv_event_get_target(e);
        int value = lv_slider_get_value(slider);
        // slider điều khiển R, G, B
        static int r = 0, g = 0, b = 0;
        if (slider == ui_Slider3)
        {
            r = value;
        }
        else if (slider == ui_Slider1)
        {
            g = value;
        }
        else if (slider == ui_Slider2)
        {
            b = value;
        }
        led_strip_update(r, g, b);
    }
}

void main(void)
{
    k_tid_t sensor_update_tid;
    const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display_dev))
    {
        printk("Display device not ready!\n");
        return;
    }
    if (!gpio_is_ready_dt(&led1) || !gpio_is_ready_dt(&led2) || !gpio_is_ready_dt(&led3))
    {
        printk("Led not ready!\n");
        return;
    }
    int ret;
    ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE); // Or GPIO_OUTPUT_ACTIVE_LOW
    if (ret < 0)
    {
        printk("Led1 not configured!\n");
        return;
    }
    ret = gpio_pin_configure_dt(&led2, GPIO_OUTPUT_INACTIVE); // Or GPIO_OUTPUT_ACTIVE_LOW
    if (ret < 0)
    {
        printk("Led2 not configured!\n");
        return;
    }
    ret = gpio_pin_configure_dt(&led3, GPIO_OUTPUT_INACTIVE); // Or GPIO_OUTPUT_ACTIVE_LOW
    if (ret < 0)
    {
        printk("Led3 not configured!\n");
        return;
    }
    led_strip_init();

    ui_init(ui_event_Button3, ui_event_Button4, ui_event_SliderChange);

    sensor_update_tid = k_thread_create(&sensor_update_thread,
                                        sensor_update_stack,
                                        K_THREAD_STACK_SIZEOF(sensor_update_stack),
                                        sensor_update_thread_fn,
                                        NULL, NULL, NULL,
                                        SERSOR_UPDATE_PRIORITY, 0, K_NO_WAIT);

    // /* Show screen */
    lv_task_handler();
    display_blanking_off(display_dev);

    // /* Main loop */
    while (1)
    {
        lv_task_handler();
        k_sleep(K_MSEC(10));
    }
}