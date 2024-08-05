#include "unity.h"
#include "reloj.h"

void test_alInicioHoraNoValida(void)
{
    unsigned ticksPorSegundo = 1;
    Reloj_init(ticksPorSegundo);
    TEST_ASSERT_FALSE_MESSAGE(Reloj_getTiempoEsValido(),"TiempoValido debe ser false");
}
void test_alInicioHoraCero(void)
{
    TiempoBcd tiempo = {1,1,1,1,1,1};
    unsigned ticksPorSegundo = 1;
    Reloj_init(ticksPorSegundo);
    Reloj_getTiempo(&tiempo);
    TEST_ASSERT_EQUAL_UINT8_ARRAY((TiempoBcd){},tiempo,TiempoBcd_NUM_DIGITOS);
}
void test_laHoraInvalidaDebeAvanzar(void)
{
    TiempoBcd tiempo;
    unsigned ticksPorSegundo = 1;
    Reloj_init(ticksPorSegundo);
    Reloj_tick();
    Reloj_getTiempo(&tiempo);
    TEST_ASSERT_EQUAL_UINT8_ARRAY((TiempoBcd){[UNIDAD_SEGUNDO]=1},tiempo,TiempoBcd_NUM_DIGITOS);
}