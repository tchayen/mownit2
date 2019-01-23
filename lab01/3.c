#include <stdio.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_ieee_utils.h>

int main (void)
{
    float a = 1.1;
    float b = 2.0;
    for(int i = 1; i < 150; i++) {
        a = a / b;
        printf ("a = "); 
        gsl_ieee_printf_float(&a);
        printf ("\n");
    }
    return 0;
}