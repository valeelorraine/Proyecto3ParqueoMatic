// Valerie Valdez
// Universidad del Valle de Guatemala
//                                               PROYECTO FINAL
//                                        Tivaware e interrupciones

//***********************************************************************************************************************************************
//                                                           L I B R E R Í A S
//***********************************************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "inc/hw_types.h"

#include "utils/uartstdio.h"

//***********************************************************************************************************************************************
//                                                           V A R I A B L E S
//***********************************************************************************************************************************************
uint8_t P1 = 48;                                                                                            // Variable para el parqueo 1 = 1
uint8_t P2 = 48;                                                                                            // Variable para el parqueo 2 = 1
uint8_t P3 = 48;                                                                                            // Variable para el parqueo 3 = 1
uint8_t P4 = 48;                                                                                            // Variable para el parqueo 4 = 1
uint8_t inSerial = 'X';                                                                                           // Número a enviar en el serial
uint8_t flag;
uint8_t flag2;
uint8_t flag3;
uint8_t flag4;
uint32_t ui32Period;

//***********************************************************************************************************************************************
//                                                          P R O T O T I P O S
//***********************************************************************************************************************************************
void delayms(uint32_t);                                                                                     // Función para el delay
void PortDIntHandler(void);                                                                                 // Activar interrupción del puerto D

//***********************************************************************************************************************************************
//                                                       C O N F I G U R A C I Ó N
//***********************************************************************************************************************************************
void main(void){
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);                 // Config. oscilador ext., usando PLL y frec. de 40MHz

//  DIGITAL_INPUT (BOTONES)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);                                                            // Se habilita el reloj para el puerto D
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);               // Push como entrada
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); //Config. WeakPullup

//  DIGITAL_OUTPUT (LEDS)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);                                                            // Se habilita el reloj para el puerto B
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7); // Se establecen como salidas el puerto B

//   U A R T (Mandar datos al display)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinConfigure(GPIO_PC4_U1RX);                                                                        // Para utilizar el RX
    GPIOPinConfigure(GPIO_PC5_U1TX);                                                                        // Para utilizar el TX
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTEnable(UART1_BASE);

    // I N T E R R U P C I O N E S   P O R T   D
    GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_RISING_EDGE);  // Hacer int. en el falling edge (cuando se apaga)
    GPIOIntRegister(GPIO_PORTD_BASE, PortDIntHandler);                                                      // Cuando se hace una interrupcion ir a la función de IntHandler
    GPIOIntEnable(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);                      // Para encender la interrupcion

    IntMasterEnable();                                                                                      // Activar interrupciones generales
//***********************************************************************************************************************************************
//                                                       L O O P   P R I N C I P A L
//***********************************************************************************************************************************************
    while(1){
        if (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_0)){                                              //ANTIREBOTE DEL PUSH1
                   flag = 0;
                       }
               else{
                     flag = 1;
                       }

        if (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_1)){                                              //ANTIREBOTE DEL PUSH1
                   flag2 = 0;
                       }
               else{
                     flag2 = 1;
                       }

        if (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2)){                                              //ANTIREBOTE DEL PUSH1
                   flag3 = 0;
                       }
               else{
                     flag3 = 1;
                       }

        if (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_3)){                                              //ANTIREBOTE DEL PUSH1
                   flag4 = 0;
                       }
               else{
                     flag4 = 1;
                       }
    }
}

//***********************************************************************************************************************************************
//                                                Handler de la interrupción del TIMER 0
//***********************************************************************************************************************************************
void PortDIntHandler(void){                                                                                 // Ver que método hizo la interrupción
    uint32_t PIN = 0;
    PIN = GPIOIntStatus(GPIO_PORTD_BASE, true);                                                             // Guardar que pin hizo la interrupcion

    if((PIN & GPIO_INT_PIN_0) == GPIO_INT_PIN_0){                                                           // numero & la dirección
        if(flag == 1 && GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_0)){
            P1++;
            flag = 0;
            switch(P1){
                case 49:                                                                     // Al hacer el falling edge se cambia de número entonces es como el antirrebote
                    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0X01); // Encender LED 0
                    break;
                case 50:
                   P1 = 48;
                   GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0X02); // Apagar el LED
                   break;
               }
            }
        }

    if((PIN & GPIO_INT_PIN_1) == GPIO_INT_PIN_1){                                                           // numero & la dirección
        if(flag2 == 1 && GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_1)){
            P2++;
            flag2 = 0;
            switch(P2){
                case 49:

                    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5, 0X10); // Encender LED 1
                    break;

                case 50:
                    P2 = 48;
                    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 , 0X20);
                    break;
               }
            }
        }


    if((PIN & GPIO_INT_PIN_2) == GPIO_INT_PIN_2){                                                           // numero & la dirección
        if(flag3 == 1 && GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2)){
            P3++;
            flag3 = 0;
            switch(P3){
            case 49:
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3, 0X04); // Encender LED 4
                break;

            case 50:
                P3 = 48;
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3, 0X08); // Apagar el LED
                break;
               }
            }
        }

    if((PIN & GPIO_INT_PIN_3) == GPIO_INT_PIN_3){                                                           // numero & la dirección
        if(flag4 == 1 && GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_3)){
            P4++;
            flag4 = 0;
            switch(P4){
                case 49:
                    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6 | GPIO_PIN_7, 0X40);  // Encender LED 7
                    break;

                case 50:
                    P4 = 48;
                    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6 | GPIO_PIN_7, 0X80);
                    break;
               }
            }
        }

    UARTCharPut(UART1_BASE, P1);
    UARTCharPut(UART1_BASE, inSerial);
    UARTCharPut(UART1_BASE, P2);
    UARTCharPut(UART1_BASE, inSerial);
    UARTCharPut(UART1_BASE, P3);
    UARTCharPut(UART1_BASE, inSerial);
    UARTCharPut(UART1_BASE, P4);
    UARTCharPut(UART1_BASE, 0X0A);

    GPIOIntClear(GPIO_PORTD_BASE, PIN);
}

void delayms(uint32_t ui32Ms) {
    SysCtlDelay(ui32Ms * (SysCtlClockGet() / 3 / 1000));                                                    // Función para hacer delay de milisegundos
}
