#pragma once

typedef enum {
    EVENT_START = 0,
    EVENT_ACK,
    EVENT_TICK        // evento de progreso / timeout / recovery
} fsm_event_t;
