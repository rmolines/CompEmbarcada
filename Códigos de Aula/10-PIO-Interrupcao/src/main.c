/************************************************************************
* Rafael Corsi   - Insper
* rafael.corsi@insper.edu.br
*
* Computação Embarcada
*
* 10-PIO-INTERRUPCAO
*
* [ref] http://www.atmel.com/Images/Atmel-42142-SAM-AT03258-Using-Low-Power-Mode-in-SAM4E-Microcontroller_Application-Note.pdf
* [ref] https://www.eecs.umich.edu/courses/eecs373/labs/refs/M3%20Guide.pdf
************************************************************************/


#include "asf.h"
#include "conf_clock.h"
#include "Driver/pio_insper.h"
#include "Driver/pmc_insper.h"


/************************************************************************/
/* Defines                                                              */
/************************************************************************/

/**
 * LEDs
 */
#define LED_PIO_ID		ID_PIOC
#define LED_PIO         PIOC
#define LED_PIN		    7
#define LED_PIN_MASK    (1<<LED_PIN)

#define L1_PIO_ID		ID_PIOA
#define L1_PIO			PIOA
#define L1_PIN		    9
#define L1_PIN_MASK    (1<<LED_PIN)

#define L2_PIO_ID		ID_PIOC
#define L2_PIO			PIOC
#define L2_PIN		    8
#define L2_PIN_MASK    (1<<LED_PIN)

#define L3_PIO_ID		ID_PIOB
#define L3_PIO			PIOB
#define L3_PIN		    6
#define L3_PIN_MASK    (1<<LED_PIN)



/**
 * Botão
 */
#define BUT_PIO_ID      ID_PIOA
#define BUT_PIO         PIOA
#define BUT_PIN		    11
#define BUT_PIN_MASK    (1 << BUT_PIN)
#define BUT_DEBOUNCING_VALUE  79

#define B1_PIO_ID		ID_PIOD
#define B1_PIO			PIOD
#define B1_PIN		    28
#define B1_PIN_MASK		(1 << BUT_PIN)

#define B2_PIO_ID		ID_PIOC
#define B2_PIO			PIOC
#define B2_PIN		    31
#define B2_PIN_MASK		(1 << BUT_PIN)

#define B3_PIO_ID		ID_PIOA
#define B3_PIO			PIOA
#define B3_PIN		    19
#define B3_PIN_MASK		(1 << BUT_PIN)

/************************************************************************/
/* prototype                                                             */
/************************************************************************/
void led_init(int estado);
void but_init(uint32_t pio_id, Pio *but_pio, const uint32_t pin_mask);
void but_Handler();

/************************************************************************/
/* Interrupçcões                                                        */
/************************************************************************/

void but_Handler(){
    /*
     *  limpa interrupcao do PIO
     */
    uint32_t pioIntStatus;
    pioIntStatus =  pio_get_interrupt_status(B1_PIO);
    
   /**
    *  Toggle status led
    */
   if(pio_get_output_data_status(LED_PIO, LED_PIN_MASK))
    pio_clear(LED_PIO, LED_PIN_MASK);
   else
    pio_set(LED_PIO,LED_PIN_MASK);
    
}

/************************************************************************/
/* Funções	                                                            */
/************************************************************************/

/**
 * @Brief Inicializa o pino do LED
 */
void led_init(int estado){
    pmc_enable_periph_clk(LED_PIO_ID);
    pio_set_output(LED_PIO, LED_PIN_MASK, 1, 0, 0 );
};

/**
 * @Brief Inicializa o pino do BUT
 *  config. botao em modo entrada enquanto 
 *  ativa e configura sua interrupcao.
 */
void but_init(uint32_t pio_id, Pio *but_pio, const uint32_t pin_mask){
    /* config. pino botao em modo de entrada */
    pmc_enable_periph_clk(pio_id);
    pio_set_input(but_pio, pin_mask, PIO_PULLUP | PIO_DEBOUNCE);
    
    /* config. interrupcao em borda de descida no botao do kit */
    /* indica funcao (but_Handler) a ser chamada quando houver uma interrupção */
    pio_enable_interrupt(but_pio, pin_mask);
    pio_handler_set(B1_PIO, B1_PIO_ID, B1_PIN_MASK, PIO_IT_FALL_EDGE, but_Handler);
    
    /* habilita interrupçcão do PIO que controla o botao */
    /* e configura sua prioridade                        */
    NVIC_EnableIRQ(pio_id);
    NVIC_SetPriority(pio_id, 1);
};

/************************************************************************/
/* Main                                                                 */
/************************************************************************/
int main(void)
{
	/************************************************************************/
	/* Inicialização básica do uC                                           */
	/************************************************************************/
	sysclk_init();
	WDT->WDT_MR = WDT_MR_WDDIS;

	/************************************************************************/
	/* Inicializao I/OS                                                     */
	/************************************************************************/
	led_init(1);
    but_init(B1_PIO_ID, B1_PIO, B1_PIN_MASK);

	/************************************************************************/
	/* Super loop                                                           */
	/************************************************************************/
	while(1){
       /* entra em modo sleep */
       //pmc_sleep(SLEEPMGR_SLEEP_WFI);
	};
}


