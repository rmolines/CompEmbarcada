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

#define LED_PIO_ID		ID_PIOC
#define LED_PIO         PIOC
#define LED_PIN		    8
#define LED_PIN_MASK    (1<<LED_PIN)

#define L1_PIO_ID		ID_PIOA
#define L1_PIO			PIOA
#define L1_PIN		    0
#define L1_PIN_MASK    (1<<L1_PIN)

#define L2_PIO_ID		ID_PIOC
#define L2_PIO			PIOC
#define L2_PIN		    30
#define L2_PIN_MASK    (1<<L2_PIN)

#define L3_PIO_ID		ID_PIOB
#define L3_PIO			PIOB
#define L3_PIN		    2
#define L3_PIN_MASK    (1<<L3_PIN)



/**
 * Bot?o
 */
#define BUT_PIO_ID      ID_PIOA
#define BUT_PIO         PIOA
#define BUT_PIN		    11
#define BUT_PIN_MASK    (1 << BUT_PIN)
#define BUT_DEBOUNCING_VALUE  79

#define B1_PIO_ID		ID_PIOD
#define B1_PIO			PIOD
#define B1_PIN		    28
#define B1_PIN_MASK		(1 << B1_PIN)

#define B2_PIO_ID		ID_PIOC
#define B2_PIO			PIOC
#define B2_PIN		    31
#define B2_PIN_MASK		(1 << B2_PIN)

#define B3_PIO_ID		ID_PIOA
#define B3_PIO			PIOA
#define B3_PIN		    19
#define B3_PIN_MASK		(1 << B3_PIN)

/************************************************************************/
/* prototype                                                             */
/************************************************************************/
void led1_init(int estado, int32_t led_pio_id, Pio *led_pio, const uint32_t pin_mask);
void but_init(int32_t pio_id, Pio *but_pio, const uint32_t pin_mask, void(*handler)(uint32_t, uint32_t));

void but_Handler();
void but1_Handler();
void but2_Handler();
void but3_Handler();

/************************************************************************/
/* Interrupçcões                                                        */
/************************************************************************/

int flag_handler = 0;


void but_Handler(){
    /*
     *  prepara a flag para piscar o LED
     */
	flag_handler = 1;

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

void but1_Handler(){
    /*
     *  prepara a flag para piscar o LED
     */
	flag_handler = 1;
	
    /*
     *  limpa interrupcao do PIO
     */
    uint32_t pioIntStatus;
    pioIntStatus =  pio_get_interrupt_status(B1_PIO);
    
   /**
    *  Toggle status led
    */
   if(pio_get_output_data_status(L1_PIO, L1_PIN_MASK))
    pio_clear(L1_PIO, L1_PIN_MASK);
   else
    pio_set(L1_PIO,L1_PIN_MASK);
    
}

void but2_Handler(){
    /*
     *  prepara a flag para piscar o LED
     */
	flag_handler = 1;
	
    /*
     *  limpa interrupcao do PIO
     */
    uint32_t pioIntStatus;
    pioIntStatus =  pio_get_interrupt_status(B2_PIO);
    
   /**
    *  Toggle status led
    */
   if(pio_get_output_data_status(L2_PIO, L2_PIN_MASK))
    pio_clear(L2_PIO, L2_PIN_MASK);
   else
    pio_set(L2_PIO,L2_PIN_MASK);
    
}

void but3_Handler(){
    /*
     *  prepara a flag para piscar o LED
     */
	flag_handler = 1;
	
    /*
     *  limpa interrupcao do PIO
     */
    uint32_t pioIntStatus;
    pioIntStatus =  pio_get_interrupt_status(B3_PIO);
    
   /**
    *  Toggle status led
    */
   if(pio_get_output_data_status(L3_PIO, L3_PIN_MASK))
    pio_clear(L3_PIO, L3_PIN_MASK);
   else
    pio_set(L3_PIO,L3_PIN_MASK);
    
}

/************************************************************************/
/* Funções	                                                            */
/************************************************************************/

/**
 * @Brief Inicializa o pino do LED
 */
void led_init(int estado, int32_t led_pio_id, Pio *led_pio, const uint32_t pin_mask){
	pmc_enable_periph_clk(led_pio_id);
	pio_set_output(led_pio, pin_mask, estado, 0, 0 );
};

/**
 * @Brief Inicializa o pino do BUT
 *  config. botao em modo entrada enquanto 
 *  ativa e configura sua interrupcao.
 */
void but_init(int32_t pio_id, Pio *pio, const uint32_t pin_mask, void (*handler)(uint32_t, uint32_t)){
    /* config. pino botao em modo de entrada */
    pmc_enable_periph_clk(pio_id);
    pio_set_input(pio, pin_mask, PIO_PULLUP | PIO_DEBOUNCE);
    
    /* config. interrupcao em borda de descida no botao do kit */
    /* indica funcao (but_Handler) a ser chamada quando houver uma interrupção */
    pio_enable_interrupt(pio, pin_mask);
    pio_handler_set(pio, pio_id, pin_mask, PIO_IT_FALL_EDGE, (*handler));
    
    /* habilita interrupçcão do PIO que controla o botao */
    /* e configura sua prioridade                        */
    NVIC_EnableIRQ(pio_id);
    NVIC_SetPriority(pio_id, 1);
};

void led_blink_3s (){
	int tc = 0;
	for (int i=0;i < 3; i++){
		pio_set(LED_PIO, LED_PIN_MASK);
		while(tc<3000000){
			tc++;
		}
		tc = 0;
		pio_clear(LED_PIO, LED_PIN_MASK);
		while(tc<3000000){
			tc++;
		}
		tc = 0;
	}
}

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
	led_init(1, LED_PIO_ID, LED_PIO, LED_PIN_MASK);
	led_init(1, L1_PIO_ID, L1_PIO, L1_PIN_MASK);
	led_init(1, L2_PIO_ID, L2_PIO, L2_PIN_MASK);
	led_init(1, L3_PIO_ID, L3_PIO, L3_PIN_MASK);

	but_init(BUT_PIO_ID, BUT_PIO, BUT_PIN_MASK, but_Handler);
	but_init(B1_PIO_ID, B1_PIO, B1_PIN_MASK, but1_Handler);
	but_init(B2_PIO_ID, B2_PIO, B2_PIN_MASK, but2_Handler);
    but_init(B3_PIO_ID, B3_PIO, B3_PIN_MASK, but3_Handler);
	/************************************************************************/
	/* Super loop                                                           */
	/************************************************************************/
	while(1){
       /* entra em modo sleep */
       pmc_sleep(SLEEPMGR_SLEEP_WFI);
	   
	   
	   if(flag_handler == 1){
		   led_blink_3s();
		   flag_handler = 0;
	   }
	};
}


