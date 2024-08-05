#include "unity.h"
#include "reloj.h"

void test_alInicioHoraNoValida(void)
{
    unsigned ticksPorSegundo = 1;
    Reloj_init(ticksPorSegundo);
    TEST_ASSERT_FALSE_MESSAGE(Reloj_getTiempoValido(),"TiempoValido debe ser false");
}