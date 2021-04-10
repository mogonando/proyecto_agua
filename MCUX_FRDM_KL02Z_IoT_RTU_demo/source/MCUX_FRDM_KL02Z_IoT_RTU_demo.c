/*! @file : MCUX_FRDM_KL02Z_IoT_RTU_demo.c
 * @author  Ernesto Andres Rincon Cruz
 * @version 1.0.0
 * @date    8/01/2021
 * @brief   Funcion principal main
 * @details
 *			v0.1 dato recibido por puerto COM es contestado en forma de ECO
 *			v0.2 dato recibido por puerto COM realiza operaciones especiales
 *					A/a=invierte estado de LED conectado en PTB10
 *					v=apaga LED conectado en PTB7
 *					V=enciende LED conectado en PTB7
 *					r=apaga LED conectado en PTB6
 *			v0.3 nuevo comando por puerto serial para prueba de MMA8451Q
 *					M=detecta acelerometro MM8451Q en bus I2C0
 *
 *
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL02Z4.h"
#include "fsl_debug_console.h"
#include "sdk_hal_adc.h"
#include "sdk_hal_uart0.h"
#include "sdk_hal_gpio.h"

#include "sdk_mdlw_leds.h"
#include "sdk_pph_ec25au.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define HABILITAR_MODEM_EC25 1
#define HABILITAR_ENTRADA_ADC_PTB9	1

/*******************************************************************************
 * Private Prototypes
 ******************************************************************************/

/*******************************************************************************
 * External vars
 ******************************************************************************/

/*******************************************************************************
 * Local vars
 ******************************************************************************/
uint32_t time1,time2,dato,dato1,RESP, i1,i,volumen1,volumen2,adc_base_de_tiempo,adc_dato;
 	float distancia,digital_out,valor_lum;
/*******************************************************************************
 * Private Source Code
 ******************************************************************************/
 	void waitTime(uint32_t timer) {
 	 	for (int32_t x=timer; x >=0; --x){
 	        __asm volatile ("nop");
 	 	};
 	}

 	void waytTime(void) {
	uint32_t tiempo = 0x1FFFF;
	do {
		tiempo--;
	} while (tiempo != 0x0000);
}

/*
 * @brief   Application entry point.
 */
int main(void) {
	uint8_t ec25_mensaje_de_texto[]="Hola desde EC25";
	//uint8_t ec25_mensaje_mqtt[200];
	uint8_t ec25_estado_actual;
	uint8_t ec25_detectado=0;
	//lptmr0Init();	//inicializa timer0 IRQ cada 10us
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();


#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    BOARD_InitDebugConsole();
#endif

    printf("Inicializa UART0:");
    //inicializa puerto UART0 y solo avanza si es exitoso el proceso
    if(uart0Inicializar(115200)!=kStatus_Success){	//115200bps
    	printf("Error");
    	return 0 ;
    };
    printf("OK\r\n");


#if HABILITAR_ENTRADA_ADC_PTB9
    //Inicializa conversor analogo a digital
    //Se debe usar  PinsTools para configurar los pines que van a ser analogicos
    printf("Inicializa ADC:");
    if(adcInit()!=kStatus_Success){
    	printf("Error");
    	return 0 ;
    }
    printf("OK\r\n");
#endif

/*#if HABILITAR_MODEM_EC25
    //Inicializa todas las funciones necesarias para trabajar con el modem EC25
    printf("Inicializa modem EC25\r\n");
    ec25Inicializacion();
    ec25InicializarMQTT();
    //ec25EnviarMensajeMQTT(&ec25_mensaje_de_texto[0], sizeof(ec25_mensaje_de_texto));
    //Configura FSM de modem para enviar mensaje de texto
   //printf("Enviando mensaje de texto por modem EC25\r\n");
    //ec25EnviarMensajeDeTexto(&ec25_mensaje_de_texto[0], sizeof(ec25_mensaje_de_texto));
#endif*/
	//Ciclo infinito encendiendo y apagando led verde
	//inicia SUPERLOOP
    while(1) {




    	waytTime();		//base de tiempo fija aproximadamente 200ms

#if HABILITAR_ENTRADA_ADC_PTB9
    	adc_base_de_tiempo++;//incrementa base de tiempo para tomar una lectura ADC
    	if(adc_base_de_tiempo>10){	// >10 equivale aproximadamente a 2s
    		adc_base_de_tiempo=0;	//reinicia contador de tiempo
    		adcTomarCaptura(PTB9_ADC0_SE10_CH13, &adc_dato);	//inicia lectura por ADC y guarda en variable adc_dato
    		//printf("ADC ->");
    		digital_out = adc_dato*(2.88/65535);
    		valor_lum = (digital_out*100)/2.88;
    		//printf("PTB8:%d ",adc_dato);	//imprime resultado ADC
    		//printf("PTB8 vin:%f ",digital_out);
    		//printf("luminosidad:%f ",valor_lum);
    		printf("%d\r\n",digital_out);	//Imprime cambio de linea
    	}
#endif



/*#if HABILITAR_MODEM_EC25
    	ec25_estado_actual = ec25Polling();	//actualiza maquina de estados encargada de avanzar en el proceso interno del MODEM
											//retorna el estado actual de la FSM

		switch(ec25_estado_actual){			//controla color de los LEDs dependiendo de estado modemEC25
    	case kFSM_RESULTADO_ERROR:
    		toggleLedRojo();
    		apagarLedVerde();
    		apagarLedAzul();
    		break;

    	case kFSM_RESULTADO_EXITOSO:
    		apagarLedRojo();
    		toggleLedVerde();
    		apagarLedAzul();
    		break;

    	case kFSM_RESULTADO_ERROR_RSSI:
    		toggleLedRojo();
    		apagarLedVerde();
    		toggleLedAzul();
    		break;

    	case kFSM_RESULTADO_ERROR_QIACT_1:
    	    toggleLedRojo();
    	    apagarLedVerde();
    	    toggleLedAzul();
    	    break;

    	case kFSM_RESULTADO_ERROR_QMTOPEN:
    	    toggleLedRojo();
    	    apagarLedVerde();
    	    toggleLedAzul();
    	    break;
    	case kFSM_ENVIANDO_MQTT_MSJ_d1_d2:
		sensorultrasonico(volumen1, volumen2);
             break;

    	default:
    		apagarLedRojo();
    		apagarLedVerde();
    		toggleLedAzul();
    		break;

    	}
#endif*/
    }
    return 0 ;
}
