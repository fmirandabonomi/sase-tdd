#ifndef RELOJ_H
#define RELOJ_H
#include <stdbool.h>
#include <stdint.h>
#include "accion.h"

enum PartesTiempoBcd{
    DECENA_HORA,
    UNIDAD_HORA,
    DECENA_MINUTO,
    UNIDAD_MINUTO,
    DECENA_SEGUNDO,
    UNIDAD_SEGUNDO,
    TiempoBcd_NUM_DIGITOS
};

typedef uint8_t TiempoBcd[TiempoBcd_NUM_DIGITOS];

/**
 * @brief Inicializa el reloj (instancia única)
 * 
 * @param ticksPorSegundo cantidad de ticks por segundo;
 */
void Reloj_init(unsigned ticksPorSegundo,Accion *accionAlarma);
/**
 * @brief Consulta si el tiempo del reloj es válido
 * 
 * @retval true El tiempo es válido (el reloj fue puesto en hora)
 * @retval false El tiempo no es válido
 */
bool Reloj_getTiempoEsValido();
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
 * @return true La alarma fue establecida
 * @return false No se pudo establecer la alarma
 */
bool Reloj_setTiempoAlarma(const TiempoBcd *tiempo);

void Reloj_desactivaAlarma(void);

void Reloj_activaAlarma(void);

/**
 * @brief Pospone una alarma activada, que volverá a sonar luego de un plazo
 * dado a partir de la hora actual
 * 
 * @param plazo Plazo que se postpone la alarma
 */
void Reloj_posponerAlarma(const TiempoBcd *plazo);
#endif // RELOJ_H
