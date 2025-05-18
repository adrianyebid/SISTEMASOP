#include <stdio.h>

int main() {
    
   int x = 10;
   int *p = &x;    // p guarda la dirección de x
   printf("%d", *p); // Muestra 10 (valor en esa dirección)


    return 0;
}