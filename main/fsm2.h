#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "events.h"

typedef enum {
    STATE_IDLE = 0,
    STATE_RUNNING,
    STATE_ERROR
} fsm_state_t;

typedef enum {
    SUB_IDLE_WAITING = 0,
} fsm_idle_sub_t;

typedef enum {
    SUB_RUN_PROCESSING = 0,
    SUB_RUN_SENDING,
    SUB_RUN_WAITING_ACK
} fsm_run_sub_t;

typedef enum {
    SUB_ERR_TIMEOUT = 0,
    SUB_ERR_RECOVERY
} fsm_err_sub_t;

typedef struct {
    fsm_state_t state;
    int substate;
    int retries;
} fsm_context_t;

void fsm_init(fsm_context_t *ctx);
void fsm_handle_event(fsm_context_t *ctx, fsm_event_t event);
