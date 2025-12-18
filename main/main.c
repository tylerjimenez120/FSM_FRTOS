#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "esp_random.h"

#include "fsm2.h"
#include "events.h"

#define TAG "MAIN"

fsm_context_t fsm;
QueueHandle_t event_queue;
TimerHandle_t tick_timer;

static void tick_timer_cb(TimerHandle_t t)
{
    fsm_event_t e = EVENT_TICK;
    xQueueSendFromISR(event_queue, &e, NULL);
}

void fsm_task(void *pv)
{
    fsm_event_t e;
    while (1) {
        if (xQueueReceive(event_queue, &e, portMAX_DELAY)) {
            fsm_handle_event(&fsm, e);
        }
    }
}

void simulator_task(void *pv)
{
    while (1) {
        uint32_t r = esp_random() % 100;

        if (fsm.state == STATE_IDLE && r < 20) {
            ESP_LOGI(TAG, "SIM → START");
            fsm_event_t e = EVENT_START;
            xQueueSend(event_queue, &e, 0);
        }

        if (fsm.state == STATE_RUNNING &&
            fsm.substate == SUB_RUN_WAITING_ACK &&
            r < 40) {
            ESP_LOGI(TAG, "SIM → ACK");
            fsm_event_t e = EVENT_ACK;
            xQueueSend(event_queue, &e, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

void app_main(void)
{
    event_queue = xQueueCreate(8, sizeof(fsm_event_t));

    fsm_init(&fsm);

    tick_timer = xTimerCreate(
        "fsm_tick",
        pdMS_TO_TICKS(500),
        pdTRUE,
        NULL,
        tick_timer_cb
    );
    xTimerStart(tick_timer, 0);

    xTaskCreate(fsm_task, "fsm_task", 4096, NULL, 5, NULL);
    xTaskCreate(simulator_task, "sim_task", 4096, NULL, 4, NULL);
}
