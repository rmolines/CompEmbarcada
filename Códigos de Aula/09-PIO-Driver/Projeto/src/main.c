/************************************************************************
* Rafael Corsi   - Insper
* rafael.corsi@insper.edu.br
*
* Computa��o Embarcada
*
* 09-PIO-DRIVER
************************************************************************/


#include "asf.h"
#include "conf_clock.h"
#include "Driver/pmc_insper.h"
#include "Driver/pio_insper.h"

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

/**
 * LEDs
 */
#define LED_PIO_ID		ID_PIOC
#define LED_PIO         PIOC
#define LED_PIN		    8
#define LED_PIN_MASK    (1<<LED_PIN)

/**
 * Bot�o
 */
#define BUT_PIO_ID      ID_PIOA
#define BUT_PIO         PIOA
#define BUT_PIN		    11
#define BUT_PIN_MASK    (1 << BUT_PIN)
#define BUT_DEBOUNCING_VALUE  79

/************************************************************************/
/* prototype                                                             */
/************************************************************************/
void led_init(int estado);
void but_init(void);

/************************************************************************/
/* Fun��es	                                                            */
/************************************************************************/

/**
 * @Brief Inicializa o pino do LED
 */
void led_init(int estado){
	_pmc_enable_periph_clock(LED_PIO_ID);        // Ativa clock do perif�rico no PMC
	_pio_set_output(LED_PIO, LED_PIN_MASK, estado, 0); 
};

/**
 * @Brief Inicializa o pino do BUT
 */
void but_init(void){
	_pmc_enable_periph_clock(BUT_PIO_ID);    // Ativa clock do perif�rico no PMC
	_pio_set_input(BUT_PIO,BUT_PIN_MASK, PIO_DEBOUNCE | PIO_PULLUP);
	_pio_pull_up(BUT_PIO, BUT_PIN_MASK, 1);
	BUT_PIO->PIO_SCDR	 = BUT_DEBOUNCING_VALUE;// Configura a frequencia do debouncing
};

static void configure_console(void)
{
	
	const usart_serial_options_t uart_serial_options = {
		.baudrate   = CONF_UART_BAUDRATE,
		.charlength = CONF_UART_CHAR_LENGTH,
		.paritytype = CONF_UART_PARITY,
		.stopbits   = CONF_UART_STOP_BITS,
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}



/************************************************************************/
/* Main                                                                 */
/************************************************************************/
int main(void)
{
	/************************************************************************/
	/* Inicializa��o b�sica do uC                                           */
	/************************************************************************/

	sysclk_init();
	board_init();
	configure_console();
	WDT->WDT_MR = WDT_MR_WDDIS;

	/************************************************************************/
	/* Inicializao I/OS                                                     */
	/************************************************************************/

		led_init(1);
    but_init();

	/************************************************************************/
	/* Super loop                                                           */
	/************************************************************************/

	while(1){
		/**
		* @Brief Verifica constantemente o status do bot�o
		* 1 : n�o apertado
		* 0 : apertado
		*/
		if(_pio_get_output_data_status(BUT_PIO, BUT_PIN_MASK)){
			printf("apertou");
			_pio_set(LED_PIO, LED_PIN_MASK);
		} else {
			_pio_clear(LED_PIO, LED_PIN_MASK);
		}
	};
}


