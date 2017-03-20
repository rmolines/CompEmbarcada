#include "pio_insper.h"

void _pio_set_output(	Pio *p_pio,
                    	const uint32_t ul_mask,
		       	const uint32_t ul_default_level,
		       	const uint32_t ul_pull_up_enable){
              p_pio->PIO_PER  = ul_mask;           // Ativa controle do pino no PIO    (PIO ENABLE register)
            	p_pio->PIO_OER  = ul_mask;           // Ativa saída                      (Output ENABLE register)

              if(!ul_default_level)                                 // Checa pela inicialização desejada
          	    p_pio->PIO_CODR = ul_mask;       // Coloca 0 na saída                (CLEAR Output Data register)
              else
                p_pio->PIO_SODR = ul_mask;       // Coloca 1 na saída                (SET Output Data register)

              if (ul_pull_up_enable){
                p_pio->PIO_PUER	 = ul_mask;        // Ativa pull-up no PIO             (PullUp ENABLE register)
              }else{
                p_pio->PIO_PUDR = ul_mask;
              }
            }


/**
 * \brief Configure one or more pin(s) or a PIO controller as inputs.
 * Optionally, the corresponding internal pull-up(s) and glitch filter(s) can
 * be enabled.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask indicating which pin(s) to configure as input(s).
 * \param ul_attribute PIO attribute(s).
 */
void _pio_set_input( 	Pio *p_pio,
                    	const uint32_t ul_mask,
            	   	const uint32_t ul_attribute){

                  p_pio->PIO_PER  = ul_mask;           // Ativa controle do pino no PIO    (PIO ENABLE register)
                  p_pio->PIO_OER  = ul_mask;           // Ativa saída                      (Output ENABLE register)

                  if(!ul_default_level)                                 // Checa pela inicialização desejada
                    p_pio->PIO_CODR = ul_mask;       // Coloca 0 na saída                (CLEAR Output Data register)
                  else
                    p_pio->PIO_SODR = ul_mask;       // Coloca 1 na saída                (SET Output Data register)


                  }
