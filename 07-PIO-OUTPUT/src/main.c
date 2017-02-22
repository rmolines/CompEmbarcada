/**************************************************************************
* Rafael Corsi   - Insper 
* rafael.corsi@insper.edu.br        
*
* Computa��o Embarcada
*
* 07-PIO-SAIDA
*
* Objetivo : Acionar um LED via controle do pino pelo PIO
*
* Explicativo : 
*
* 1.     
*    configura o clock interno do perif�rico
* 
*  Cristal  |  _ _ _                        Aberto    _ _ _   
*   |   |   | |     | --> Clk principal       /      |     |
*   | X | --> | PMC | --> Clk perif�ricos - -   ---->|PIOC |
*   |   |   | |_ _ _| --> ....                       |_ _ _|
*           |                             
*             uC
*
* 2.
*    desativa o watchdog para que o uc n�o seja reinicializado
*    sem necessitarmos.
*
* 3. 
*    Ativamos o clock do perif�rico a ser utilizado
*
*  Cristal  |  _ _ _      Fechado  _ _ _  
*   |   |   | |     | -->         |     |
*   | X | --> | PMC | ----------->|PIOC | 
*   |   |   | |_ _ _| -->         |_ _ _|
*           | 
*             uC
*
* 4.
*   Agora come�amos a configurar no PIOC o pino respons�vel por controlar
*   o LED:
*
*     _ _ _          
*    |     |out       
* -->|PIOC |---------- LED 
*    |_ _ _|PIOC6
*
*
*   Para isso � preciso saber que o PIOC possui diversos registradores
*   que configuram seu funcionamento interno, cada PIO pode tratar at�
*   32 pinos diferentes, sendo que cada pino pode ter uma configura��o
*   pr�pria, isso � poss�vel por cada registrador � composto de 32 bits
*   e cada bit � respons�vel por um pino espec�fico do PIO:
*
*   | bit 31 | bit 30 | ... ... ... | bit 2 | bit 1 | bit 0 |  
*   ---------------------------------------------------------
*   | PIOC31 | PIOC30 |             | PIOC2 | PIOC1 | PIOC0 |
*   ---------------------------------------------------------
*
*   Exemplo: desejamos configurar o pino PIOC 8 como sendo sa�da.
*   
*   Para isso escrevemos no registrador OER (output enable register) no
*   bit 8
*
*   PIOC->PIO_OER = (1 << 8); 
*
*   O comando anterior pode ser lido como :
*   O registrador OER do perif�rico PIOC recebe 1 no bit 8.
*
*   # Enable/Disable/Status
*   
*   Cada registrador de configura��o desse microcontrolador possui
*   geralmente 3 registradores : Enable/Disable/Status
*
*    Exemplo para o Output Enable:
*    - ENABLE  : Output Enable Register   : OER : (write only)
*    - Disable : Output Disable Register  : ODR : (write only)
*    - Status  : Output Status Register   : OSR : (read  only)
*    
*   O valor efetivo do registrador (por exemplo do Output Enable) est�
*   salve no OSR. Por�m para alterarmos seu valor � necess�rio fazer o
*   seu acesso por um dos dois registradores: Enable/Disable.
*
*   - Enable : 
*     Torna 1 os bits no registrador de STATUS
*
*   - Disable :
*     Torna 0 os bits no registrador de STATUS
*  
*   Exemplo, OSR inicializado em 0 e desejamos colocar em 1 o bit 8 do OSR, devemos :
*    
*    1.
*    n[0] (instante 0)
*    ---------------------   ---------------------   ---------------------                            
*    |OER | 000000000000 |   |ODR | 000000000000 |   |OSR | 000000000000 |                        
*    ---------------------   ---------------------   ---------------------                         

*
*    2. 
*    PIO->PIO_OER = (1 << 8) | (1 << 2);
*    
*    n[1]
*    ---------------------   ---------------------   ---------------------                            
*    |OER | 000100000100 |   |ODR | 000000000000 |   |OSR | 000000000000 |                        
*    ---------------------   ---------------------   ---------------------                         
*
*    n[2]
*    ---------------------   ---------------------   ---------------------                            
*    |OER | 000000000000 |   |ODR | 000000000000 |   |OSR | 000100000100 |                        
*    ---------------------   ---------------------   ---------------------       
*    
*
*    Esse comando faz com que o bit 8 no OSR v� de 0 para 1 e automaticamente
*    o bit rec�m programando no OER vai para 0.
*
*    3. Mas como n�o podemos acessar o OSR, para zerarmos algum de seus bits
*    basta escrevermos no ODR (Disable)
*    
*    PIO->PIO_ODR = (1<<2); 
*
*    n[3]
*    ---------------------   ---------------------   ---------------------                            
*    |OER | 000000000000 |   |ODR | 000000000100 |   |OSR | 000000000000 |                        
*    ---------------------   ---------------------   ---------------------                         
*
*    n[4]
*    ---------------------   ---------------------   ---------------------                            
*    |OER | 000000000000 |   |ODR | 000000000000 |   |OSR | 000100000000 |                        
*    ---------------------   ---------------------   ---------------------       
*
************************************************************************/

/** 
 * @Brief Defines
 * Aqui inclu�mos as bibliotecas (head files) necess�rias 
 * para o complementar o nosso arquivo principal main.c
 *
 * O arquivo asf.h � um head file que cont�m includes para outros arquivos, 
 * iremos ver como isso funciona futuramente.
 */
#include "asf.h"
#include "conf_clock.h"

/**
 * @Brief LED
 * Declaramos alguns #defines que ir� facilitar o desenvolvimento desse projeto.
 *	LED_PIO_ID	 - 12 
 *	LED_PIO		 - Ponteiro para estrutura que cont�m os registradores do PIOC
 *  LED_PIN		 - PIO C 8, � o pino que o LED est� conectado
 *  LED_PIN_MASK - M�scara para configurarmos o LED
 *
 *
 * LED_PIO � uma estrutura que cont�m os registradores (endere�o de mem�ria) do PIO em quest�o, 
 * podemos acessar cada registrador desse perif�rico da seguinte maneira :
 *	PIOC->PIO_OER = (1 << 8);
 * nesse caso, o registrador Output Enable register do PIOC ir� configurar o bit
 * 8 como 1, ou seja, ir� configurar o pino PIOC8 como sendo sa�da.
 * 
 */
#define LED_PIO_ID	            // id do perif�rico C	
#define LED_PIO			PIOC
#define LED_PIN			8
#define LED_PIN_MASK	(1<<LED_PIN) 


/************************************************************************/
/* Main                                                                 */
/************************************************************************/
int main(void)
{

	/************************************************************************/
	/* 1. Inicializa��o do clock interno                                    */
	/************************************************************************/
	
	/**
	* Perif�rico : PMC
	* @Brief Inicializa clock do microcontrolador em 300Mhz
	* A configura��o do clock dessa placa est� no arquivo:
	* conf_clock.h
	*/
	sysclk_init();

    /************************************************************************/
	/* 2. Desativa o watchdog                                               */
	/************************************************************************/
	
    /**
	* Perif�rico : Watchdog
	* @Brief Desabilita o watchdog
	* Watchdog � uma fun��o do microcontrolador respons�vel
	* por verificar poss�veis travamentos.
	*/ 
	WDT->WDT_MR = WDT_MR_WDDIS;

	/************************************************************************/
	/* 3. Ativa clock no PIO Configura��o do PMC	                        */
	/************************************************************************/
	
	/**
	* Perif�rico : PMC 
	* @Brief Peripheral Clock Enable Register
	* O PMC � o perif�rico respons�vel pelo controle de energia dos
	* demais perif�ricos.
	* Inicializamos aqui o clock do perif�rico PIO C.
	*/ 
	PMC->PMC_PCER0 = (1<<LED_PIO_ID);

	/************************************************************************/
	/* PIO				                                                    */
	/************************************************************************/

	/**
	* Perif�rico : PIO C
	* @Brief Output Enable Register
	* Configuramos o pino como sa�da
	*/ 
	PIOC->PIO_OER = (1 << 8);
	
	/**
	* Perif�rico : PIO C
	* @Brief Peripheral Enable Register
	* Fazemos com que o controle do pino seja realizado pelo PIO
	* e n�o por outro perif�rico
	*/ 
	PIOC->PIO_PER = (1 << 8);

	/**
	* Perif�rico : PIO C
	* @Brief Clear/Set Output Data Register
	* Aqui controlamos o valor da sa�da desse pino
	* no caso especifico colocamos 0 (acende o LED)
	* O registrador :
	*   - PIO_SODR : coloca 1 no pino 
	*	- PIO_CODR : coloca 0 no pino 
    */
	PIOC->PIO_CODR =  (1 << 8);
	
	/************************************************************************/
	/* Super loop                                                           */
	/************************************************************************/
	
	/**
	* @Brief Diferente de um c�digo comum que executa em um pc, no uC deve estar
	* sempre executando um c�digo, por isso utilizamos esse loop infinito.
	*/
	while(1){
		
	};
}


