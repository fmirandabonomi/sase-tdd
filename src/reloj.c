#include "reloj.h"

static struct Reloj{
    TiempoBcd tiempo;
    bool esValido;
    bool alarmaActivada;
}self[1];

void Reloj_init(unsigned ticksPorSegundo)
{
    (void) ticksPorSegundo;
}

bool Reloj_getTiempoEsValido(void)
{
    return self->esValido;
}

void Reloj_getTiempo(TiempoBcd *destino)
{
    for (int i=0;i<TiempoBcd_NUM_DIGITOS;++i) (*destino)[i]=self->tiempo[i];
}

/**
 * @brief Incrementa un dígito, is pasa del límite establecido vuelve a cero
 * 
 * @param digito[in,out] Referencia del dígito a incrementar
 * @param limite[in] El límite
 * @param incremento[in] el valor por el cual incrementar
 * @retval 1 si hubo rebalse, el dígito pasó a cero
 * @retval 0 si no hubo rebase
 */
static uint8_t incrementaDigito(
                        uint8_t *const digito,
                        uint8_t const limite,
                        uint8_t const incremento)
{
    uint8_t rebalse = 0;
    if (*digito < limite) {
        *digito += incremento;
    }else{
        *digito = 0;
        rebalse = 1;
    }
    return rebalse;
}

void Reloj_tick(void)
{
    uint8_t acarreo;
    acarreo = incrementaDigito(self->tiempo+UNIDAD_SEGUNDO,9,1);
    acarreo = incrementaDigito(self->tiempo+DECENA_SEGUNDO,5,acarreo);
    acarreo = incrementaDigito(self->tiempo+UNIDAD_MINUTO ,9,acarreo);
    acarreo = incrementaDigito(self->tiempo+DECENA_MINUTO ,5,acarreo);
    acarreo = incrementaDigito(self->tiempo+UNIDAD_HORA   ,self->tiempo[DECENA_HORA] < 2 ? 9:3,acarreo);
    (void) incrementaDigito(self->tiempo+DECENA_HORA   ,2,acarreo);
}

bool Reloj_setTiempo(const TiempoBcd *horaActual)
{
    for(int i=0;i<TiempoBcd_NUM_DIGITOS;++i)self->tiempo[i]=(*horaActual)[i];
    self->esValido = true;
    return true;
}

bool Reloj_getAlarmaActivada(void)
{
    return self->alarmaActivada;
}

bool Reloj_setTiempoAlarma(const TiempoBcd *tiempo)
{
    (void)tiempo;
    self->alarmaActivada = true;
    return true;
}