#include "fsm2.h"
#include "esp_log.h"

#define TAG "FSM"

static bool handle_idle(fsm_context_t *ctx, fsm_event_t event)
{
    if (event == EVENT_START) {
        ESP_LOGI(TAG, "IDLE → RUNNING/PROCESSING");
        ctx->state = STATE_RUNNING;
        ctx->substate = SUB_RUN_PROCESSING;
        return true;
    }
    return false;
}

static bool handle_running(fsm_context_t *ctx, fsm_event_t event)
{
    switch (ctx->substate)
    {
    case SUB_RUN_PROCESSING:
        ESP_LOGI(TAG, "PROCESSING → SENDING");
        ctx->substate = SUB_RUN_SENDING;
        return true;

    case SUB_RUN_SENDING:
        ESP_LOGI(TAG, "SENDING → WAITING_ACK");
        ctx->substate = SUB_RUN_WAITING_ACK;
        return false; // espera eventos externos

    case SUB_RUN_WAITING_ACK:
        if (event == EVENT_ACK) {
            ESP_LOGI(TAG, "ACK → IDLE");
            ctx->state = STATE_IDLE;
            ctx->substate = SUB_IDLE_WAITING;
            return true;
        }
        if (event == EVENT_TICK) {
            ESP_LOGE(TAG, "TIMEOUT → ERROR");
            ctx->state = STATE_ERROR;
            ctx->substate = SUB_ERR_TIMEOUT;
            return true;
        }
        break;
    }
    return false;
}

static bool handle_error(fsm_context_t *ctx, fsm_event_t event)
{
    (void)event;

    switch (ctx->substate)
    {
    case SUB_ERR_TIMEOUT:
        ESP_LOGW(TAG, "ERROR → RECOVERY");
        ctx->substate = SUB_ERR_RECOVERY;
        ctx->retries = 0;
        return true;

    case SUB_ERR_RECOVERY:
        ctx->retries++;
        ESP_LOGW(TAG, "Recovery intento %d", ctx->retries);
        if (ctx->retries >= 3) {
            ESP_LOGI(TAG, "Recovery OK → IDLE");
            ctx->state = STATE_IDLE;
            ctx->substate = SUB_IDLE_WAITING;
            return true;
        }
        break;
    }
    return false;
}

void fsm_init(fsm_context_t *ctx)
{
    ctx->state = STATE_IDLE;
    ctx->substate = SUB_IDLE_WAITING;
    ctx->retries = 0;
}

void fsm_handle_event(fsm_context_t *ctx, fsm_event_t event)
{
    bool progressed;

    do {
        progressed = false;

        switch (ctx->state)
        {
        case STATE_IDLE:
            progressed = handle_idle(ctx, event);
            break;

        case STATE_RUNNING:
            progressed = handle_running(ctx, event);
            break;

        case STATE_ERROR:
            progressed = handle_error(ctx, event);
            break;
        }

        event = EVENT_TICK; // solo transiciones internas
    }
    while (progressed);
}
