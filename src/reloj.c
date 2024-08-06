#include "reloj.h"

static struct Reloj{
    TiempoBcd tiempo;
    TiempoBcd tiempoAlarma;
    TiempoBcd tiempoAlarmaPospuesta;
    bool esValido;
    bool alarmaActivada;
    bool alarmaPospuesta;
    Accion *accionAlarma;
}self[1];

void Reloj_init(unsigned ticksPorSegundo,Accion *accionAlarma)
{
    (void) ticksPorSegundo;
    self->accionAlarma = accionAlarma;
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
 * @brief Incrementa un dígito, is pasa del límite establecido se produce
 * acarreo
 * 
 * @param digito[in,out] Referencia del dígito a incrementar
 * @param incremento[in] el incremento
 * @param limite[in] El límite
 * @retval true si hubo acarreo, en este caso se restó limite del digito resultante
 * @retval false si no hubo rebase
 */
static bool sumaDigitos(uint8_t *const digito,
                        uint8_t const incremento,
                        uint8_t const limite)
{
    const int valorFinal = *digito + incremento;
    bool acarreo = valorFinal > limite;

    *digito = (uint8_t) (acarreo ? valorFinal - (limite+1) : valorFinal);

    return acarreo;
}

static int TiempoBcd_compara(const TiempoBcd *a,const TiempoBcd *b)
{
    for(int i=0;i<TiempoBcd_NUM_DIGITOS;++i){
        const uint8_t ai = (*a)[i];
        const uint8_t bi = (*b)[i];
        if (ai > bi) return 1;
        if (ai < bi) return -1;
    }
    return 0;
}

static bool Reloj_coincideAlarma(void)
{
    return !TiempoBcd_compara(&self->tiempo,&self->tiempoAlarma);
}
static bool Reloj_coincideAlarmaPospuesta(void)
{
    return !TiempoBcd_compara(&self->tiempo,&self->tiempoAlarmaPospuesta);
}
static void TiempoBcd_suma(TiempoBcd *self,const TiempoBcd *otro)
{
    bool acarreo;
    acarreo = sumaDigitos(*self+UNIDAD_SEGUNDO, (*otro)[UNIDAD_SEGUNDO]        ,9);
    acarreo = sumaDigitos(*self+DECENA_SEGUNDO, (*otro)[DECENA_SEGUNDO]+acarreo,5);
    acarreo = sumaDigitos(*self+UNIDAD_MINUTO , (*otro)[UNIDAD_MINUTO ]+acarreo,9);
    acarreo = sumaDigitos(*self+DECENA_MINUTO , (*otro)[DECENA_MINUTO ]+acarreo,5);
    (void)    sumaDigitos(*self+DECENA_HORA   , (*otro)[DECENA_HORA   ]        ,2);
    acarreo = sumaDigitos(*self+UNIDAD_HORA   , (*otro)[UNIDAD_HORA   ]+acarreo,((*self)[DECENA_HORA] == 2) ? 3:9);
    (void)    sumaDigitos(*self+DECENA_HORA   , acarreo,2);
}
static void TiempoBcd_copia(TiempoBcd *destino,const TiempoBcd *origen)
{
    for(int i=0;i<TiempoBcd_NUM_DIGITOS;++i) (*destino)[i]=(*origen)[i];
}

void Reloj_tick(void)
{
    static const TiempoBcd uno = {0,0,0,0,0,1}; 
    TiempoBcd_suma(&self->tiempo,&uno);
    if(!self->accionAlarma) return;
    if (self->alarmaActivada && Reloj_coincideAlarma())
    {
        Accion_ejecuta(self->accionAlarma);
    }else if (self->alarmaPospuesta && Reloj_coincideAlarmaPospuesta())
    {
        self->alarmaPospuesta = false;
        Accion_ejecuta(self->accionAlarma);
    }
}

bool Reloj_setTiempo(const TiempoBcd *horaActual)
{
    TiempoBcd_copia(&self->tiempo,horaActual);
    self->esValido = true;
    return true;
}

bool Reloj_getAlarmaActivada(void)
{
    return self->alarmaActivada;
}

bool Reloj_setTiempoAlarma(const TiempoBcd *tiempo)
{
    TiempoBcd_copia(&self->tiempoAlarma,tiempo);
    self->alarmaActivada = true;
    return true;
}

void Reloj_desactivaAlarma(void)
{
    self->alarmaActivada = false;
}

void Reloj_activaAlarma(void)
{
    self->alarmaActivada = true;
}

void Reloj_posponerAlarma(const TiempoBcd *plazo)
{
    TiempoBcd_copia(&self->tiempoAlarmaPospuesta,&self->tiempo);
    TiempoBcd_suma(&self->tiempoAlarmaPospuesta,plazo);
    self->alarmaPospuesta = true;
}