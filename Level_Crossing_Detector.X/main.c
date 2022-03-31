/**
* \file main.c
*
* \brief Main source file.
*
(c) 2022 Microchip Technology Inc. and its subsidiaries.
    Subject to your compliance with these terms, you may use this software and
    any derivatives exclusively with Microchip products. It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*
*/

#include <avr/io.h>
#include <avr/interrupt.h>

/* Default fuses configuration:
- BOD disabled
- Oscillator in High-Frequency Mode
- UPDI pin active(WARNING: DO NOT CHANGE!)
- RESET pin used as GPIO
- CRC disabled
- MVIO enabled for dual supply
- Watchdog Timer disabled
*/
FUSES =
{
    .BODCFG = ACTIVE_DISABLE_gc | LVL_BODLEVEL0_gc | SAMPFREQ_128Hz_gc | SLEEP_DISABLE_gc,
    .BOOTSIZE = 0x0,
    .CODESIZE = 0x0,
    .OSCCFG = CLKSEL_OSCHF_gc,
    .SYSCFG0 = CRCSEL_CRC16_gc | CRCSRC_NOCRC_gc | RSTPINCFG_GPIO_gc | UPDIPINCFG_UPDI_gc,
    .SYSCFG1 = MVSYSCFG_DUAL_gc | SUT_0MS_gc,
    .WDTCFG = PERIOD_OFF_gc | WINDOW_OFF_gc,
};

/* set DACREF to 0.8 Volts for Vref = 2.048 Volts */
#define DACREF_VALUE (0.8 * 256 / 2.048)


static void PORT_init (void);
static void AC0_init(void);
            
ISR(AC0_AC_vect)
{
    /* Insert AC interrupt handling code here */
    /* The interrupt flag has to be cleared manually */
    AC0.STATUS = AC_CMPIF_bm;
}

int main(void)
{
    PORT_init();
    AC0_init();
    sei();            /* Global interrupts enabled */

    while (1) 
    {
        ;    
    }
}

static void PORT_init (void)
{
	/* Positive Input PD2 - Disable digital input buffer */
	PORTD.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
	/* Enable output buffer on PA7 */
	PORTA.DIRSET = PIN7_bm;
}

static void AC0_init(void)
{
    /* Negative input uses internal reference - voltage reference should be enabled */
    VREF.ACREF = VREF_REFSEL_2V048_gc       /* Voltage reference at 2.048V */
               | VREF_ALWAYSON_bm;          /* AC VREF reference enable: enabled */

    AC0.DACREF = DACREF_VALUE;              /* Set DAC voltage reference */

    /*Select proper inputs for comparator*/
    AC0.MUXCTRL =  AC_MUXPOS_AINP0_gc       /* Positive Input - Analog Positive Pin 0 */
                 | AC_MUXNEG_DACREF_gc;     /* Negative Input - DAC Voltage Reference */
    
    AC0.CTRLA = AC_ENABLE_bm                /* Enable analog comparator */
              | AC_OUTEN_bm;                /* Output Buffer Enable: enabled */
    
    AC0.INTCTRL = AC_CMP_bm;                /* Analog Comparator 0 Interrupt enabled */
}