#include "reloj.h"
#include <string.h>
//#define HACER_LOG
#ifdef HACER_LOG
#include <stdio.h>
#define LOG(s,...) printf(s __VA_OPT__(,) __VA_ARGS__)
#define LOG_TIEMPO(t)     do{\
        uint8_t *const base = (t);\
        LOG("%02d:%02d:%02d",\
        base[DECENA_HORA]*10+base[UNIDAD_HORA],\
        base[DECENA_MINUTO]*10+base[UNIDAD_MINUTO],\
        base[DECENA_SEGUNDO]*10+base[UNIDAD_SEGUNDO]);\
}while(0)
#else
#define LOG(s,...)
#define LOG_TIEMPO(t)
#endif

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
    uint8_t const suma = *digito + incremento;
    uint8_t const pesoSiguiente = limite + 1;
    uint8_t rebalse = suma >= pesoSiguiente;

    *digito = rebalse ? suma - pesoSiguiente : suma;

    return rebalse;
}

static int comparaTiempos(TiempoBcd *a,TiempoBcd *b)
{
    return memcmp(*a,*b,TiempoBcd_NUM_DIGITOS);
}
static bool Reloj_coincideAlarma(void)
{
    return !comparaTiempos(&self->tiempo,&self->tiempoAlarma);
}
static bool Reloj_coincideAlarmaPospuesta(void)
{
    return !comparaTiempos(&self->tiempo,&self->tiempoAlarmaPospuesta);
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

    if(!self->accionAlarma) return;

    if (self->alarmaActivada && Reloj_coincideAlarma()){
        Accion_ejecuta(self->accionAlarma);
    }else if(self->alarmaPospuesta && Reloj_coincideAlarmaPospuesta()){
        self->alarmaPospuesta = false;
        Accion_ejecuta(self->accionAlarma);
    }
}

static void copiaTiempBcd(TiempoBcd *destino,const TiempoBcd *horaActual)
{
    for(int i=0;i<TiempoBcd_NUM_DIGITOS;++i)(*destino)[i]=(*horaActual)[i];
}

bool Reloj_setTiempo(const TiempoBcd *horaActual)
{
    copiaTiempBcd(&self->tiempo,horaActual);
    self->esValido = true;
    return true;
}

bool Reloj_getAlarmaActivada(void)
{
    return self->alarmaActivada;
}

bool Reloj_setTiempoAlarma(const TiempoBcd *tiempoAlarma)
{
    copiaTiempBcd(&self->tiempoAlarma,tiempoAlarma);
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


void Reloj_posponAlarma(uint8_t minutos)
{
    uint8_t acarreo;
    copiaTiempBcd(&self->tiempoAlarmaPospuesta,&self->tiempo);

    acarreo = incrementaDigito(self->tiempoAlarmaPospuesta + UNIDAD_MINUTO,9,minutos%10);
    acarreo = incrementaDigito(self->tiempoAlarmaPospuesta + DECENA_MINUTO,5,minutos/10+acarreo);
    acarreo = incrementaDigito(self->tiempoAlarmaPospuesta + UNIDAD_HORA,self->tiempoAlarmaPospuesta[DECENA_HORA] < 2 ? 9:3,acarreo);
    incrementaDigito(self->tiempoAlarmaPospuesta + DECENA_HORA,2,acarreo);
    self->alarmaPospuesta = true;

    LOG("Alarma pospuesta para ");
    LOG_TIEMPO(self->tiempoAlarma);
    LOG("\n");
}