#include "asf.h"

/************************************************************************/
/* DEFINES                                                              */
/************************************************************************/


/**
 * LEDs
 */
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
 * Botao
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
/* VAR globais                                                          */
/************************************************************************/
volatile uint8_t flag_led0 = 1;
uint32_t hour, minute, second, year, month, day, week;


/************************************************************************/
/* PROTOTYPES                                                           */
/************************************************************************/

void BUT_init(void);
void LED_init(int estado, int32_t led_pio_id, Pio *led_pio, const uint32_t pin_mask);
void TC_init(Tc *TC, uint32_t ID_TC,  uint32_t channel, uint32_t freq);
void RTC_init(void);
void pin_toggle(Pio *pio, uint32_t mask);
void flag_toggle(volatile uint8_t *flag);

/************************************************************************/
/* Handlers                                                             */
/************************************************************************/

/**
 *  Handle Interrupcao botao 1
 */
static void Button1_Handler(uint32_t id, uint32_t mask)
{

}

/**
 *  Interrupt handler for TC0 interrupt. 
 */
void TC0_Handler(void){
	volatile uint32_t ul_dummy;

    /****************************************************************
	* Devemos indicar ao TC que a interrupção foi satisfeita.
    ******************************************************************/
	ul_dummy = tc_get_status(TC0, 0);

	/* Avoid compiler warning */
	UNUSED(ul_dummy);

	/** Muda o estado do LED */
    if(flag_led0)
        pin_toggle(LED_PIO, LED_PIN_MASK);
}

void TC1_Handler(void){
	volatile uint32_t ul_dummy;

    /****************************************************************
	* Devemos indicar ao TC que a interrupção foi satisfeita.
    ******************************************************************/
	ul_dummy = tc_get_status(TC0, 1);

	/* Avoid compiler warning */
	UNUSED(ul_dummy);

	/** Muda o estado do LED */
    if(flag_led0)
        pin_toggle(L1_PIO, L1_PIN_MASK);
}

void TC2_Handler(void){
	volatile uint32_t ul_dummy;

    /****************************************************************
	* Devemos indicar ao TC que a interrupção foi satisfeita.
    ******************************************************************/
	ul_dummy = tc_get_status(TC0, 2);

	/* Avoid compiler warning */
	UNUSED(ul_dummy);

	/** Muda o estado do LED */
    if(flag_led0)
        pin_toggle(L2_PIO, L2_PIN_MASK);
}

void TC3_Handler(void){
	volatile uint32_t ul_dummy;

    /****************************************************************
	* Devemos indicar ao TC que a interrupção foi satisfeita.
    ******************************************************************/
	ul_dummy = tc_get_status(TC1, 0);

	/* Avoid compiler warning */
	UNUSED(ul_dummy);

	/** Muda o estado do LED */
    if(flag_led0)
        pin_toggle(L3_PIO, L3_PIN_MASK);
}


/**
 * \brief Interrupt handler for the RTC. Refresh the display.
 */
void RTC_Handler(void)
{
	uint32_t ul_status = rtc_get_status(RTC);

	/* Second increment interrupt */
	if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC) {
	
		rtc_clear_status(RTC, RTC_SCCR_SECCLR);

	} else {
		/* Time or date alarm */
		if ((ul_status & RTC_SR_ALARM) == RTC_SR_ALARM) {
			
			/* Inverte a flag */
			flag_toggle(&flag_led0);     
		 			
		 	if (flag_led0){
			 	tc_start(TC0, 0);
			 	tc_start(TC1, 1);
			 	tc_start(TC2, 2);
			 	tc_start(TC3, 0);

			 } else {			 
			 	tc_stop(TC0, 0);
			 	tc_stop(TC1, 1);
			 	tc_stop(TC2, 2);
			 	tc_stop(TC3, 0);
 			}
	 	
			uint32_t hour, minute, second;
			rtc_get_time(RTC, &hour, &minute, &second);		
			
			rtc_set_time_alarm(RTC, 0, hour, 1, minute + 1, 0, second);
	
			
			
		}
	}
}


/************************************************************************/
/* Funcoes                                                              */
/************************************************************************/

/** 
 *  Toggle pin controlado pelo PIO (out)
 */
void pin_toggle(Pio *pio, uint32_t mask){
   if(pio_get_output_data_status(pio, mask))
    pio_clear(pio, mask);
   else
    pio_set(pio,mask);
}

/** 
 *  Toggle flag controlado pelo usuario
 */
void flag_toggle(volatile uint8_t *flag){
	*flag =! 1;
}

/**
 * @Brief Inicializa o pino do BUT
 */
void BUT_init(void){
    /* config. pino botao em modo de entrada */
    pmc_enable_periph_clk(BUT_PIO_ID);
    pio_set_input(BUT_PIO, BUT_PIN_MASK, PIO_PULLUP | PIO_DEBOUNCE);
    
    /* config. interrupcao em borda de descida no botao do kit */
    /* indica funcao (but_Handler) a ser chamada quando houver uma interrupção */
    pio_enable_interrupt(BUT_PIO, BUT_PIN_MASK);
    pio_handler_set(BUT_PIO, BUT_PIO_ID, BUT_PIN_MASK, PIO_IT_FALL_EDGE, Button1_Handler);
    
    /* habilita interrupçcão do PIO que controla o botao */
    /* e configura sua prioridade                        */
    NVIC_EnableIRQ(BUT_PIO_ID);
    NVIC_SetPriority(BUT_PIO_ID, 1);
};

/**
 * @Brief Inicializa o pino do LED
 */
void LED_init(int estado, int32_t led_pio_id, Pio *led_pio, const uint32_t pin_mask){
	pmc_enable_periph_clk(led_pio_id);
	pio_set_output(led_pio, pin_mask, estado, 0, 0 );
};

/**
 * Configura TimerCounter (TC) para gerar uma interrupcao no canal channel-(ID_TC) 
 * a cada 1/freq (freq)
 */
void TC_init(Tc *TC, uint32_t ID_TC,  uint32_t channel, uint32_t freq){   
    uint32_t ul_div;
    uint32_t ul_tcclks;
    uint32_t ul_sysclk = sysclk_get_cpu_hz();
        
    /* Configura o PMC */
    pmc_enable_periph_clk(ID_TC);    

    /** Configura o TC para operar em  freqMHz e interrupçcão no RC compare */
    tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
    tc_init(TC, channel, ul_tcclks | TC_CMR_CPCTRG);
    tc_write_rc(TC, channel, (ul_sysclk / ul_div) / freq);

    /* Configura e ativa interrupçcão no TC canal channel */
    NVIC_EnableIRQ((IRQn_Type) ID_TC);
    tc_enable_interrupt(TC, channel, TC_IER_CPCS);

    /* Inicializa o canal channel do TC */
    tc_start(TC, channel);
}

/**
 * Configura o RTC para funcionar com interrupcao de alarme
 */
void RTC_init(){
    /* Configura o PMC */
    pmc_enable_periph_clk(ID_RTC);
        
    /* Default RTC configuration, 24-hour mode */
    rtc_set_hour_mode(RTC, 0);

    /* Configura data e hora manualmente */
    rtc_set_date(RTC, year, month, day, week);
    rtc_set_time(RTC, hour, minute, second);

    /* Configure RTC interrupts */
    NVIC_DisableIRQ(RTC_IRQn);
    NVIC_ClearPendingIRQ(RTC_IRQn);
    NVIC_SetPriority(RTC_IRQn, 0);
    NVIC_EnableIRQ(RTC_IRQn);
    
    /* Ativa interrupcao via alarme */
    rtc_enable_interrupt(RTC,  RTC_IER_ALREN);   
    
}

/************************************************************************/
/* Main Code	                                                        */
/************************************************************************/
int main(void){
	/* Initialize the SAM system */
	sysclk_init();
	
	
	rtc_get_time(RTC, &hour, &minute, &second);
	rtc_get_date(RTC, &year, &month, &day, &week);

	/* Disable the watchdog */
	WDT->WDT_MR = WDT_MR_WDDIS;

    /* Configura Leds */
    LED_init(0, LED_PIO_ID, LED_PIO, LED_PIN_MASK);
    LED_init(0, L1_PIO_ID, L1_PIO, L1_PIN_MASK);
    LED_init(0, L2_PIO_ID, L2_PIO, L2_PIN_MASK);
    LED_init(0, L3_PIO_ID, L3_PIO, L3_PIN_MASK);
	
	/* Configura os botões */
	BUT_init();    
    
    /** Configura timer 0 */
    TC_init(TC0, ID_TC0, 0, 4);
    TC_init(TC0, ID_TC1, 1, 5);
    TC_init(TC0, ID_TC2, 2, 6);
    TC_init(TC1, ID_TC3, 0, 7);
    
    /** Configura RTC */
    RTC_init();

    /* configura alarme do RTC */
    rtc_set_date_alarm(RTC, 1, month, 1, day);
    rtc_set_time_alarm(RTC, 1, hour, 1, minute, 1, second+4);

          
	while (1) {
		/* Entra em modo sleep */
		
	}
}
