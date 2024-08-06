#include "unity.h"
#include "accion.h" // Necesario para que ceedling compile accion.c
#include "reloj.h"
#include "accion_alarma.h"


enum{TICKS_POR_SEGUNDO = 1};

static AccionAlarma *testigoAlarma;

void setUp(void){
    testigoAlarma = AccionAlarma_new();
    Reloj_init(TICKS_POR_SEGUNDO,AccionAlarma_getAccion(testigoAlarma));
}
void tearDown(void)
{
    AccionAlarma_delete(testigoAlarma);
}
static void avanzaUnSegundo(void)
{
    for (int i=0;i<TICKS_POR_SEGUNDO;++i) Reloj_tick();
}

void test_alInicioHoraNoValida(void)
{
    TEST_ASSERT_FALSE_MESSAGE(Reloj_getTiempoEsValido(),"TiempoValido debe ser false");
}
void test_alInicioHoraCero(void)
{
    TiempoBcd tiempo = {1,1,1,1,1,1};
    Reloj_getTiempo(&tiempo);
    TEST_ASSERT_EQUAL_UINT8_ARRAY((TiempoBcd){},tiempo,TiempoBcd_NUM_DIGITOS);
}
void test_laHoraInvalidaDebeAvanzar(void)
{
    TiempoBcd tiempo;
    avanzaUnSegundo();
    Reloj_getTiempo(&tiempo);
    TEST_ASSERT_EQUAL_UINT8_ARRAY((TiempoBcd){[UNIDAD_SEGUNDO]=1},tiempo,TiempoBcd_NUM_DIGITOS);
}
void test_puedePonerseEnHora(void)
{
    static const TiempoBcd tiempoSet = {1,3,0,0,3,4};
    TiempoBcd tiempo;
    TEST_ASSERT_TRUE(Reloj_setTiempo(&tiempoSet));
    TEST_ASSERT_TRUE_MESSAGE(Reloj_getTiempoEsValido(),"Luego de poner en hora el tiempo debe ser valido");
    Reloj_getTiempo(&tiempo);
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(tiempoSet,tiempo,TiempoBcd_NUM_DIGITOS,"El tiempo establecido debe reflejarse en el reloj");
}


void test_avanzaUnSegundo(void)
{
    static const TiempoBcd tiempoInicial = {1,3,0,0,3,4};
    static const TiempoBcd tiempoFinal = {1,3,0,0,3,5};
    TiempoBcd tiempo;
    Reloj_setTiempo(&tiempoInicial);
    avanzaUnSegundo();
    TEST_ASSERT_TRUE_MESSAGE(Reloj_getTiempoEsValido(),"Al avanzar el tiempo sigue siendo valido");
    Reloj_getTiempo(&tiempo);
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(tiempoFinal,tiempo,TiempoBcd_NUM_DIGITOS,"Debe avanzar");
}

void test_avanzaFronteras(void)
{
    static const struct Caso {
        TiempoBcd inicial; 
        TiempoBcd final;
        const char *mensaje;
    } tablaCasos[]={
        {.inicial = {1,3,0,0,3,9}, .final = {1,3,0,0,4,0}, .mensaje = "A 13:00:39 le sigue 13:00:40"},
        {.inicial = {1,3,0,0,5,9}, .final = {1,3,0,1,0,0}, .mensaje = "A 13:00:59 le sigue 13:01:00"},
        {.inicial = {1,3,0,9,5,9}, .final = {1,3,1,0,0,0}, .mensaje = "A 13:09:59 le sigue 13:10:00"},
        {.inicial = {1,3,5,9,5,9}, .final = {1,4,0,0,0,0}, .mensaje = "A 13:59:59 le sigue 14:00:00"},
        {.inicial = {1,9,5,9,5,9}, .final = {2,0,0,0,0,0}, .mensaje = "A 19:59:59 le sigue 20:00:00"},
        {.inicial = {2,3,5,9,5,9}, .final = {0,0,0,0,0,0}, .mensaje = "A 23:59:59 le sigue 00:00:00"},
    };
    const size_t numCasos = sizeof(tablaCasos)/sizeof(*tablaCasos);
    TiempoBcd tiempo;

    for(size_t i=0;i<numCasos;++i){
        const struct Caso *const caso = tablaCasos + i;
        Reloj_setTiempo(&caso->inicial);
        avanzaUnSegundo();
        Reloj_getTiempo(&tiempo);
        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(caso->final,tiempo,TiempoBcd_NUM_DIGITOS,caso->mensaje);
    }
}

void test_alarmaIniciaDesactivada(void)
{
    TEST_ASSERT_FALSE_MESSAGE(Reloj_getAlarmaActivada(),"Al inicio la alarma debe estar desactivada");
}


void test_activaAlarmaAlEstablecerTiempoDeAlarma(void)
{
    static const TiempoBcd tiempoAlarma = {1,3,0,0,0,1};
    static const TiempoBcd tiempoInicial = {1,3,0,0,0,0};

    Reloj_setTiempo(&tiempoInicial);
    TEST_ASSERT_TRUE_MESSAGE(Reloj_setTiempoAlarma(&tiempoAlarma),"Debe poder establecer un tiempo válido de alarma");
    TEST_ASSERT_TRUE_MESSAGE(Reloj_getAlarmaActivada(),"Al establecer el tiempo de alarma esta debe activarse");
    TEST_ASSERT_EQUAL_UINT_MESSAGE(0,AccionAlarma_vecesDisparada(testigoAlarma),"La alarma no se dispara si no es la hora");
}

void test_laAlarmaSeEjecutaAlLlegarAlTiempoDeAlarma(void)
{
    static const TiempoBcd tiempoAlarma = {1,3,0,0,0,1};
    static const TiempoBcd tiempoInicial = {1,3,0,0,0,0};

    Reloj_setTiempo(&tiempoInicial);
    Reloj_setTiempoAlarma(&tiempoAlarma);
    avanzaUnSegundo();
    TEST_ASSERT_EQUAL_UINT_MESSAGE(1,AccionAlarma_vecesDisparada(testigoAlarma),"La alarma se dispara al llegar la hora establecida");

}

void test_laAlarmaDesactiadaNoSeEjecutaAlLlegarAlTiempoDeAlarma(void)
{
    static const TiempoBcd tiempoAlarma = {1,3,0,0,0,1};
    static const TiempoBcd tiempoInicial = {1,3,0,0,0,0};

    Reloj_setTiempo(&tiempoInicial);
    Reloj_setTiempoAlarma(&tiempoAlarma);
    Reloj_desactivaAlarma();
    avanzaUnSegundo();
    TEST_ASSERT_EQUAL_UINT_MESSAGE(0,AccionAlarma_vecesDisparada(testigoAlarma),"La alarma desactivada no se dispara al llegar la hora establecida");

}

void test_puedeActivarseLaAlarma(void)
{
    static const TiempoBcd tiempoAlarma = {1,3,0,0,0,1};
    static const TiempoBcd tiempoInicial = {1,3,0,0,0,0};

    Reloj_setTiempo(&tiempoInicial);
    Reloj_setTiempoAlarma(&tiempoAlarma);
    Reloj_desactivaAlarma();
    Reloj_activaAlarma();
    avanzaUnSegundo();
    TEST_ASSERT_EQUAL_UINT_MESSAGE(1,AccionAlarma_vecesDisparada(testigoAlarma),"Se puede activar la alarma inactiva");
}

void test_puedePosponerLaAlarma(void)
{
    static const TiempoBcd tiempoInicial = {1,3,4,4,0,0};
    static const TiempoBcd tiempoAlarma = {1,3,4,4,0,1};
    static const uint8_t minutosEspera = 16;
    static const TiempoBcd tiempo2 = {1,4,0,0,0,0};

    Reloj_setTiempo(&tiempoInicial);
    Reloj_setTiempoAlarma(&tiempoAlarma);
    avanzaUnSegundo();
    TEST_ASSERT_EQUAL_UINT(1,AccionAlarma_vecesDisparada(testigoAlarma));

    Reloj_posponAlarma(minutosEspera);

    Reloj_setTiempo(&tiempo2);
    avanzaUnSegundo();    
    TEST_ASSERT_EQUAL_UINT_MESSAGE(2,AccionAlarma_vecesDisparada(testigoAlarma),"Debe disparar la alarma pospuesta");

    Reloj_setTiempo(&tiempoInicial);
    avanzaUnSegundo();
    TEST_ASSERT_EQUAL_UINT_MESSAGE(3,AccionAlarma_vecesDisparada(testigoAlarma),"Alarma original debe permanecer");
    
    Reloj_setTiempo(&tiempo2);
    avanzaUnSegundo();
    TEST_ASSERT_EQUAL_UINT_MESSAGE(3,AccionAlarma_vecesDisparada(testigoAlarma),"La alarma pospuesta vale una sola vez");

}

void test_puedePosponerAlSiguienteDia(void)
{
    static const TiempoBcd tiempoInicial = {2,3,4,3,5,9};
    static const TiempoBcd tiempoAlarma = {2,3,4,4,0,1};
    static const unsigned minutosEspera = 10*60;
    static const TiempoBcd tiempo2 = {0,9,4,3,5,9};

    Reloj_setTiempo(&tiempoInicial);
    Reloj_setTiempoAlarma(&tiempoAlarma);
    avanzaUnSegundo();
    avanzaUnSegundo();

    Reloj_posponAlarma(minutosEspera);

    Reloj_setTiempo(&tiempo2);
    avanzaUnSegundo();
    TEST_ASSERT_EQUAL_UINT(1,AccionAlarma_vecesDisparada(testigoAlarma));
    avanzaUnSegundo();
    TEST_ASSERT_EQUAL_UINT_MESSAGE(2,AccionAlarma_vecesDisparada(testigoAlarma),"Debe disparar la alarma pospuesta");
}

void test_aceptaTicksPorSegundoDistintosDeUno(void)
{
    enum{TICKS_POR_SEGUNDO = 19};
    static const TiempoBcd tiempoInicial = {1,3,0,0,0,0};
    static const TiempoBcd tiempoFinal = {1,3,0,0,0,1};
    TiempoBcd tiempo;
    Reloj_init(TICKS_POR_SEGUNDO,NULL);
    Reloj_setTiempo(&tiempoInicial);
    for(int i=0;i<(TICKS_POR_SEGUNDO-1);++i) Reloj_tick();
    Reloj_getTiempo(&tiempo);
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(tiempoInicial,tiempo,TiempoBcd_NUM_DIGITOS,"Solo debe avanzar luego de transcurridos los ticks prescritos");
    Reloj_tick();
    Reloj_getTiempo(&tiempo);
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(tiempoFinal,tiempo,TiempoBcd_NUM_DIGITOS,"Luego de transcurridos los ticks prescritos debe avanzar");
}

