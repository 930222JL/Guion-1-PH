/* *****************************************************************************
 * P.H.2025: Driver/Manejador de los temporizadores
 * suministra los servicios independientemente del hardware
 *
 * usa los servicos de hal_tiempo.h: 
 */
 
#include "drv_tiempo.h"
#include "hal_tiempo.h"

#define TODO 0	//pa que no de error de compilacion con el proyecto vacio, modicicar
static volatile uint32_t ticks_us=0;
/**
 * nicializa el reloj y empieza a contar
 */
bool drv_tiempo_iniciar(void){
	hal_tiempo_info_t info;
	hal_tiempo_iniciar_tick(&info);
	ticks_us=info.ticks_per_us;
	return (info.ticks_per_us!=0);
}

/**
 * tiempo desde que se inicio el temporizador en microsegundos
 */
Tiempo_us_t drv_tiempo_actual_us(void){
	return (Tiempo_us_t)(hal_tiempo_actual_tick64()/ticks_us);
}

/**
 * tiempo desde que se inicio el temporizador en milisegundos
 */
Tiempo_ms_t drv_tiempo_actual_ms(void){
	return (Tiempo_ms_t)(hal_tiempo_actual_tick64() /((uint64_t)ticks_us *1000u));
}

/**
 * retardo: esperar un cierto tiempo en milisegundos
 */
void drv_tiempo_esperar_ms(Tiempo_ms_t ms){
	uint64_t fin=hal_tiempo_actual_tick64()+(ms*ticks_us*1000u);
	while(hal_tiempo_actual_tick64()<fin);
}

/**
 * esperar hasta un determinado tiempo (en ms), devuelve el tiempo actual
 */
Tiempo_ms_t drv_tiempo_esperar_hasta_ms(Tiempo_ms_t ms){
	Tiempo_ms_t ahora=drv_tiempo_actual_ms();
	while((int32_t)((uint32_t)ms-(uint32_t)ahora)>0){
		ahora=drv_tiempo_actual_ms();
	}
	return TODO;
}
