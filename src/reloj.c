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


static void normalizaDigito(uint8_t *const digito,uint8_t *const siguiente,const uint8_t pesoSiguiente)
{
    while(*digito >= pesoSiguiente){
        *digito -= pesoSiguiente;
        if(siguiente) *siguiente += 1;
    }
}
static void normalizaTiempo(TiempoBcd *tiempo)
{
    uint8_t *const base = *tiempo;

    normalizaDigito(base + UNIDAD_SEGUNDO,base + DECENA_SEGUNDO,10);
    normalizaDigito(base + DECENA_SEGUNDO,base + UNIDAD_MINUTO, 6);
    normalizaDigito(base + UNIDAD_MINUTO, base + DECENA_MINUTO, 10);
    normalizaDigito(base + DECENA_MINUTO, base + UNIDAD_HORA,   6);
    
    normalizaDigito(base + DECENA_HORA, NULL,   3);
    normalizaDigito(base + UNIDAD_HORA, base + DECENA_HORA, base[DECENA_HORA] < 2 ? 10:4);
    normalizaDigito(base + DECENA_HORA, NULL,   3);
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
    self->tiempo[UNIDAD_SEGUNDO]++;
    normalizaTiempo(&self->tiempo);

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
    copiaTiempBcd(&self->tiempoAlarmaPospuesta,&self->tiempo);

    self->tiempoAlarmaPospuesta[UNIDAD_MINUTO] += minutos%10;
    self->tiempoAlarmaPospuesta[DECENA_MINUTO] += minutos/10;
    
    normalizaTiempo(&self->tiempoAlarmaPospuesta);
    
    self->alarmaPospuesta = true;

    LOG("Alarma pospuesta para ");
    LOG_TIEMPO(self->tiempoAlarma);
    LOG("\n");
}