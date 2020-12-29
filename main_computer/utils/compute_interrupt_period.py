 # -*- coding: utf-8 -*-


"""
How to use calibration default values to speed up the MSP?
http://www.argenox.com/library/msp430/msp430-clock-system-chapter-6/
"""

# Posibles fuentes de reloj configurables en el registro TASSEL:
# TACLK: no se usa porque necesita un oscilador externo
TASSEL_0 = 0 
# ACLK typicall is 12kHz
TASSEL_1 = 12000 
# SMCLK: el reloj principal que se puede configurar
TASSEL_2 = 16000000
# INCLK no sé qué es
TASSEL_3 = 0

# Posibles divisores de frecuencia
ID_0 = 1.0
ID_1 = 2.0
ID_2 = 4.0
ID_3 = 8.0


# ***** Page 370 ***** 
# To compute the rate of each interrupt:
# Interrupt(s) = TACCR0 x (ID_3 / TASSEL)
# Important registers:
# TASSEL_2 = SMCLK
# MC_1     = Contar "de ida" hasta TACCR0. El script sólo funciona para este caso
# ID_3     = aumentar 1 cada 8 ciclos de reloj

# Jugar con los siguientes 3 valores hasta encontrar la frecuencia deseada
# TACCR0 es un registro de 16 bits, por lo que su valor máximo es 2^16 - 1 = 32767
TACCR0 = 32767
# Divisor de frecuencia
id = ID_3
# Frecuencia del reloj que alimenta al timer. SIEMPRE va a ser TASSEL_1
tassel = TASSEL_2

"""
Para obtener los 21.8 segundos que escogí para el modo hibernación:
TACCR0 = 32767
id = ID_3
tassel = TASSEL_1
"""

"""
Para obtener los 1 milisegundos que escogí para el modo de operación normal:
TACCR0 = 3
id = ID_2
tassel = TASSEL_1
"""

interrupt_s = TACCR0 * id / tassel
print( "Interrupt will trigger every %f seconds" % interrupt_s )



