#ifndef RELOJ_H
#define RELOJ_H
#include <stdbool.h>
#include <stdint.h>
#include "accion.h"

/**
 * @addtogroup Reloj
 * @brief Librería de reloj de 24 horas con alarma
 * @{
 */

/**
 * @brief Constantes con los índices, dentro de un arreglo TiempoBcd, de las partes del tiempo
 * 
 */
enum PartesTiempoBcd{
    DECENA_HORA,
    UNIDAD_HORA,
    DECENA_MINUTO,
    UNIDAD_MINUTO,
    DECENA_SEGUNDO,
    UNIDAD_SEGUNDO,
    TiempoBcd_NUM_DIGITOS
};

/**
 * @brief Arreglo con los dígitos BCD correspondientes a hora, minutos y segundos
 * para un tiempo de 24 horas. Ejemplo de literal compuesto: (TiempoBcd){decenaHora, unidadHora, decenaMinuto, unidadMinuto, decenaSegundo, unidadSegundo}
 * @note Las constantes de la forma (DECENA|UNIDAD)_(HORA|MINUTO|SEGUNDO)
 * contienen los índices en el arreglo para las respectivas partes del tiempo.
 */
typedef uint8_t TiempoBcd[TiempoBcd_NUM_DIGITOS];

/**
 * @brief Inicializa el reloj (instancia única)
 * 
 * @param ticksPorSegundo cantidad de ticks por segundo;
 * @param accionAlarma Accion que se ejecutará al disparar la alarma
 */
void Reloj_init(unsigned ticksPorSegundo,Accion *accionAlarma);
/**
 * @brief Consulta si el tiempo del reloj es válido
 * 
 * @retval true El tiempo es válido (el reloj fue puesto en hora)
 * @retval false El tiempo no es válido
 */
bool Reloj_getTiempoEsValido(void);
/**
 * @brief Obtiene el tiempo actual del reloj
 * 
 * @param destino referencia a TiempoBcd donde guardar el tiempo actual
 */
void Reloj_getTiempo(TiempoBcd *destino);

/**
 * @brief Procesa un tick del reloj
 * 
 */
void Reloj_tick(void);

/**
 * @brief Pone el reloj en hora
 * 
 * @param horaActual La hora actual
 * @retval true si puso la hora indicada
 * @retval false si hubo un error al poner la hora
 */
bool Reloj_setTiempo(const TiempoBcd *horaActual);

/**
 * @brief Consulta si la alarma está activada
 * 
 * @retval true Alarma activada
 * @retval false Alarma desactivada
 */
bool Reloj_getAlarmaActivada(void);

/**
 * @brief Establece la hora de alarma. Activa la alarma
 * 
 * @param tiempo Hora de alarma
 * @retval true La alarma fue establecida
 * @retval false No se pudo establecer la alarma
 */
bool Reloj_setTiempoAlarma(const TiempoBcd *tiempo);

/**
 * @brief Desactiva la alarma
 * 
 */
void Reloj_desactivaAlarma(void);

/**
 * @brief Activa la alarma
 * 
 */
void Reloj_activaAlarma(void);

/**
 * @brief Pospone la alarma, establece una alarma de una sola vez
 * luego del tiempo especificado a partir de la hora actual
 * 
 * @param minutos el tiempo 
 */
void Reloj_posponAlarma(unsigned minutos);

/// @}

#endif