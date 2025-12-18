# FSM_FRTOS
codigo que ejemplifica el patron de diseño ST - FSM




El sistema implementa una FSM jerárquica y completamente event-driven sobre FreeRTOS.
 La FSM no se ejecuta por sí sola ni en bucles internos: solo avanza cuando recibe eventos a través de una cola.
Un timer periódico (FreeRTOS software timer) actúa como heartbeat, garantizando que la FSM siga evaluando timeouts y lógica interna aunque no lleguen eventos externos.
