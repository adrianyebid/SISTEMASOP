#include <stdio.h>
#include <stdint.h>  // <-- ¡IMPORTANTE!

int main() {
    int x = 10;
    int *p = &x;

    int result1 = *p * 2;

    uintptr_t result2 = (uintptr_t)p * 2; // Ahora sí lo reconoce bien

    printf("Valor de x: %d\n", x);
    printf("Direccion de x (p): %p\n", (void*)p);
    printf("Resultado 1 (*p * 2): %d\n", result1);
    printf("Resultado 2 (p * 2): %lu (direccion multiplicada)\n", (unsigned long)result2);

    return 0;
}
