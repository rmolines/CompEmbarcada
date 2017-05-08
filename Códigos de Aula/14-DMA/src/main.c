#include "asf.h"

/**
 * LEDs
 */
#define LED_PIO_ID		  ID_PIOC
#define LED_PIO         PIOC
#define LED_PIN		      8
#define LED_PIN_MASK    (1<<LED_PIN)

/**
 * Botão
 */
#define BUT_PIO_ID            ID_PIOA
#define BUT_PIO               PIOA
#define BUT_PIN		            11
#define BUT_PIN_MASK          (1 << BUT_PIN)
#define BUT_DEBOUNCING_VALUE  79

/** 
 *  USART
 */
#define USART_COM     USART1
#define USART_COM_ID  ID_USART1

/************************************************************************/
/* XDMA CONFIG */
/************************************************************************/
/** XDMAC channel used in this example. */
#define XDMAC_TX_CH 0
#define XDMAC_RX_CH 1

/** XDMAC channel HW Interface number for SPI0,refer to datasheet. */
#define USART_XDMAC_TX_CH_NUM											9
#define USART_XDMAC_RX_CH_NUM											10
/** XDMA peripheral interface*/
/** XDMA peripheral interface*/
#define USART_XDMA_DEST_REG		&CONSOLE_UART->US_THR // Mem Two Peripheral
#define USART_XDMA_ORIG_REG		&CONSOLE_UART->US_RHR // Peripheral Two Mem
/** The buffer size for trans fer */
#define BUFFER_SIZE 100

/************************************************************************/
/* VAR globais                                                          */
/************************************************************************/

 /* buffer para recebimento de dados */
 uint8_t bufferRX[100];
 
 /* buffer para transmissão de dados */
 uint8_t bufferTX[100];

/** XDMAC channel configuration. */
static xdmac_channel_config_t g_xdmac_tx_cfg;
static xdmac_channel_config_t g_xdmac_rx_cfg;
/**
* XDMA
*/
uint8_t g_tx_buffer[] = "This is message from USART master transferred by XDMAC test \n";
uint8_t g_rx_buffer[BUFFER_SIZE] = "0";
uint32_t g_buffer_size = sizeof(g_tx_buffer);

/************************************************************************/
/* Flags */
/************************************************************************/
volatile uint8_t flag_led0 = 1;
volatile uint8_t flag_rx = 0;

/************************************************************************/
/* PROTOTYPES                                                           */
/************************************************************************/

void BUT_init(void);
void LED_init(int estado);
void pin_toggle(Pio *pio, uint32_t mask);
uint32_t usart_puts(uint8_t *pstring);
uint32_t usart_gets(uint8_t *pstring);
static void uart_xdmac_configure();
static void uart_xdmac_Tx(uint32_t *dest_address,
uint32_t *orgin_address,
uint32_t buffer_size);
static void uart_xdmac_Rx(uint32_t *peripheral_address,
uint32_t *orgin_address,
uint32_t buffer_size);

/************************************************************************/
/* Handlers                                                             */
/************************************************************************/

/**
 *  Handle Interrupcao botao 1
 */
static void Button1_Handler(uint32_t id, uint32_t mask)
{
  pin_toggle(PIOD, (1<<28));
  pin_toggle(LED_PIO, LED_PIN_MASK);
}

void USART1_Handler(void){
  uint32_t ret = usart_get_status(USART_COM);
  uint8_t  c;
  
  // Verifica por qual motivo entrou na interrupçcao
  if(ret & US_IER_RXRDY){                     // Dado disponível para leitura
    usart_serial_getchar(USART_COM, &c);
    usart_puts(bufferTX);
  } else if(ret & US_IER_TXRDY){              // Transmissão finalizada
    
  }
}

/**
* \brief XDMAC interrupt handler.
*/
void XDMAC_Handler(void)
{
	uint32_t dma_status_tx, dma_status_rx;
	dma_status_tx = xdmac_channel_get_interrupt_status(XDMAC, XDMAC_TX_CH);
	dma_status_rx = xdmac_channel_get_interrupt_status(XDMAC, XDMAC_RX_CH);
	UNUSED(dma_status_tx);
	UNUSED(dma_status_rx);
	// Verificamos se a transferência foi completa
	if(dma_status_rx & (XDMAC_CIS_BIS | XDMAC_CIS_LIS)){
		flag_rx = 1;
	}
}


/************************************************************************/
/* Funcoes                                                              */
/************************************************************************/



static void uart_xdmac_configure()
{
	uint32_t xdmaint;
	/* Initialize and enable DMA controller */
	pmc_enable_periph_clk(ID_XDMAC);
	xdmaint = ( XDMAC_CIE_BIE |
	XDMAC_CIE_DIE |
	XDMAC_CIE_FIE |
	XDMAC_CIE_RBIE |
	XDMAC_CIE_WBIE |
	XDMAC_CIE_ROIE);
	xdmac_channel_enable_interrupt(XDMAC, XDMAC_TX_CH, xdmaint);
	xdmac_enable_interrupt(XDMAC, XDMAC_TX_CH);
	xdmac_channel_enable_interrupt(XDMAC, XDMAC_RX_CH, xdmaint);
	xdmac_enable_interrupt(XDMAC, XDMAC_RX_CH);
	/*Enable XDMAC interrupt */
	NVIC_ClearPendingIRQ(XDMAC_IRQn);
	NVIC_SetPriority( XDMAC_IRQn ,1);
	NVIC_EnableIRQ(XDMAC_IRQn);
}

/** 
 *  Toggle pin controlado pelo PIO (out)
 */
void pin_toggle(Pio *pio, uint32_t mask){
   if(pio_get_output_data_status(pio, mask))
    pio_clear(pio, mask);
   else
    pio_set(pio,mask);
}

/*
45.6.10.12 / pg. 1185
The DMA uses the trigger flags, TXRDY and RXRDY, to write or read into the USART. The DMA the Transmit Holding register (US_THR) and it always reads in the Receive Holding register of the data written or read by the DMA in the USART is always a byte
*/
static void uart_xdmac_Tx(uint32_t *peripheral_address,
uint32_t *orgin_address,
uint32_t buffer_size)
{
	/* Initialize channel config for transmitter */
	g_xdmac_tx_cfg.mbr_ubc = buffer_size;
	g_xdmac_tx_cfg.mbr_sa = (uint32_t)orgin_address;
	g_xdmac_tx_cfg.mbr_da = (uint32_t)peripheral_address;
	g_xdmac_tx_cfg.mbr_cfg = XDMAC_CC_TYPE_PER_TRAN |
	XDMAC_CC_MBSIZE_SINGLE |
	XDMAC_CC_DSYNC_MEM2PER |
	XDMAC_CC_CSIZE_CHK_1 |
	XDMAC_CC_DWIDTH_BYTE |
	XDMAC_CC_SIF_AHB_IF0 |
	XDMAC_CC_DIF_AHB_IF1 |
	XDMAC_CC_SAM_INCREMENTED_AM |
	XDMAC_CC_DAM_FIXED_AM |
	XDMAC_CC_PERID(USART_XDMAC_TX_CH_NUM);
	g_xdmac_tx_cfg.mbr_bc = 0;
	g_xdmac_tx_cfg.mbr_ds = 0;
	g_xdmac_tx_cfg.mbr_sus = 0;
	g_xdmac_tx_cfg.mbr_dus = 0;
	xdmac_configure_transfer(XDMAC, XDMAC_TX_CH, &g_xdmac_tx_cfg);
	xdmac_channel_set_descriptor_control(XDMAC, XDMAC_TX_CH, 0);
	xdmac_channel_enable(XDMAC, XDMAC_TX_CH);
}



static void uart_xdmac_Rx(uint32_t *peripheral_address,
uint32_t *orgin_address,
uint32_t buffer_size)
{
	/* Initialize channel config for receiver */
	g_xdmac_rx_cfg.mbr_ubc = buffer_size;
	g_xdmac_rx_cfg.mbr_da = (uint32_t)orgin_address;
	g_xdmac_rx_cfg.mbr_sa = (uint32_t)peripheral_address;
	g_xdmac_rx_cfg.mbr_cfg = XDMAC_CC_TYPE_PER_TRAN |
	XDMAC_CC_MBSIZE_SINGLE |
	XDMAC_CC_DSYNC_PER2MEM |
	XDMAC_CC_CSIZE_CHK_1 |
	XDMAC_CC_DWIDTH_BYTE|
	XDMAC_CC_SIF_AHB_IF1 |
	XDMAC_CC_DIF_AHB_IF0 |
	XDMAC_CC_SAM_FIXED_AM |
	XDMAC_CC_DAM_INCREMENTED_AM |
	XDMAC_CC_PERID(USART_XDMAC_RX_CH_NUM);
	g_xdmac_rx_cfg.mbr_bc = 0;
	g_xdmac_tx_cfg.mbr_ds = 0;
	g_xdmac_rx_cfg.mbr_sus = 0;
	g_xdmac_rx_cfg.mbr_dus =0;
	xdmac_configure_transfer(XDMAC, XDMAC_RX_CH, &g_xdmac_rx_cfg);
	xdmac_channel_set_descriptor_control(XDMAC, XDMAC_RX_CH, 0);
	xdmac_channel_enable(XDMAC, XDMAC_RX_CH);
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
void LED_init(int estado){
    pmc_enable_periph_clk(LED_PIO_ID);
    pio_set_output(LED_PIO, LED_PIN_MASK, estado, 0, 0 );
};

/**
 * \brief Configure UART console.
 */
static void USART1_init(void){
  
  /* Configura USART1 Pinos */
 sysclk_enable_peripheral_clock(ID_PIOB);
 sysclk_enable_peripheral_clock(ID_PIOA);
 pio_set_peripheral(PIOB, PIO_PERIPH_D, PIO_PB4);  // RX
 pio_set_peripheral(PIOA, PIO_PERIPH_A, PIO_PA21); // TX
 MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;
  
  /* Configura opcoes USART */
  const sam_usart_opt_t usart_settings = {
    .baudrate     = 115200,
    .char_length  = US_MR_CHRL_8_BIT,
    .parity_type  = US_MR_PAR_NO,
    .stop_bits    = US_MR_NBSTOP_1_BIT    ,
    .channel_mode = US_MR_CHMODE_NORMAL
  };

  /* Ativa Clock periferico USART0 */
  sysclk_enable_peripheral_clock(USART_COM_ID);
  
  /* Configura USART para operar em modo RS232 */
  usart_init_rs232(USART_COM, &usart_settings, sysclk_get_peripheral_hz());
  
  /* Enable the receiver and transmitter. */
	usart_enable_tx(USART_COM);
	usart_enable_rx(USART_COM);
 }

/**
 * Envia para o UART uma string
 * envia todos os dados do vetor até
 * encontrar o \NULL (0x00)
 *
 * Retorna a quantidade de char escritos
 */
uint32_t usart_puts(uint8_t *pstring){
     
  return 0;
}

/*
 * Usart get string
 * monta um buffer com valores recebidos da USART até 
 * encontrar o char '\n'
 *
 * Retorna a quantidade de char lidos
 */
uint32_t usart_gets(uint8_t *pstring){

  return 0;  
}

void usart_gets_dma(uint8_t *pstring) {
		
	uart_xdmac_Rx(USART_XDMA_ORIG_REG, (uint32_t) pstring, BUFFER_SIZE);
		
	return;
}

void usart_puts_dma(uint8_t *pstring, int nChars) {
	
	uart_xdmac_Tx(USART_XDMA_DEST_REG, (uint32_t) pstring, nChars);
	
	return;
}
/************************************************************************/
/* Main Code	                                                        */
/************************************************************************/
int main(void){


  /* Initialize the SAM system */
  sysclk_init();
   
  /* Disable the watchdog */
  WDT->WDT_MR = WDT_MR_WDDIS;

  /* Configura Leds */
  LED_init(1);
  
  /* Configura os botões */
  BUT_init();  
  
  /* Inicializa com serial com PC*/
  USART1_init();
 
  /* Inicializa funcao de delay */
  delay_init( sysclk_get_cpu_hz());
	
	/*Configura XDMA*/
	uart_xdmac_configure();
	
	//uart_xdmac_Tx(USART_XDMA_DEST_REG, (uint32_t) g_tx_buffer, g_buffer_size);
	
	int size = sprintf(bufferTX, "%s \n", "Ola Voce");
	usart_puts_dma(bufferTX, size);
	usart_gets_dma(bufferRX);
	usart_puts_dma(bufferRX, size);
        
	while (1) {
		
		
    delay_s(1);
	}
}
