/* *****************************************************************************
 * P.H.2025: TODO
 * implementacion para cumplir el hal_tiempo.h
 */
#include <nrf.h>
#include "hal_tiempo.h" 

#define TICKS_PER_US      1u                 /* 16 MHz / 2^4 = 1 MHz */
#define COUNTER_BITS      32u
#define COUNTER_MAX       (0xFFFFFFFFu)

static volatile uint32_t s_overfloes_t1=0;

/**
 * RSI del timer 1, vector definido en : arm_startup_nrf52840.s
 * This IRQ handler will trigger every DELAY_ms ms
 */
void TIMER1_IRQHandler(void) {
  volatile uint32_t dummy;
	
  if (NRF_TIMER1->EVENTS_COMPARE[0] == 1)
  {
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;

    // Read back event register so ensure we have cleared it before exiting IRQ handler.
    dummy = NRF_TIMER1->EVENTS_COMPARE[0];
    dummy; // to get rid of set but not used warning
  }
	
	s_overfloes_t1++;
}
 
/* Inicializa el contador libre (tick) y devuelve parámetros de conversión 
 * entre ellas, devuelve la constante ticks_per_us,
 * ticks_per_us permite pasar de los ticks del hardware a microsegundos
 * (tip, el driver lo necesitara para trabajar en us y ms de la app y hacer la conversion a ticks del hardware)
 */
void hal_tiempo_iniciar_tick(hal_tiempo_info_t *out_info){
	s_overfloes_t1=0;
	// 32-bit timer
  NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos;

  // 1us timer period
  NRF_TIMER1->PRESCALER = 4 << TIMER_PRESCALER_PRESCALER_Pos;

  // 1000 us compare value, generates EVENTS_COMPARE[0]
  NRF_TIMER1->CC[0] = DELAY_ms * 1000; //1s

  // Enable IRQ on EVENTS_COMPARE[0]
  NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;

  // Clear the timer when COMPARE0 event is triggered
  NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;


  NVIC_EnableIRQ(TIMER1_IRQn);
  NRF_TIMER1->TASKS_START = 1;
	
	out_info->counter_bits=COUNTER_BITS;
	out_info->counter_max=COUNTER_MAX;
	out_info->ticks_per_us=TICKS_PER_US;
	
	return out_info;
}

static uint32_t leer_contador(void){
	NRF_TIMER1->TASK_CAPTURE[1]=1;
	return NRF_TIMER1->CC[1];
}

/* Lectura 64-bit sin carrera: lee TC, luego overflows, re-lee TC si hizo wrap */
uint64_t hal_tiempo_actual_tick64(void) {
    uint32_t hi1, lo1, hi2, lo2;

    lo1 = leer_contador();
    hi1 = s_overflows_t1;
    lo2 = leer_contador();
    if (lo2 < lo1) {
        /* ocurrió wrap entre lecturas: usar hi actualizado */
        hi2 = s_overflows_t1;
        return (((uint64_t)hi2) * ((uint64_t)COUNTER_MAX + 1u)) + lo2;
    }
    return (((uint64_t)hi1) * ((uint64_t)COUNTER_MAX + 1u)) + lo2;
}
	
	