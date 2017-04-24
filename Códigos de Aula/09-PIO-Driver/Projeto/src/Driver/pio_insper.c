#include "pio_insper.h"

void _pio_set_output(	Pio *p_pio,
                    	const uint32_t ul_mask,
		       	const uint32_t ul_default_level,
		       	const uint32_t ul_pull_up_enable){
              p_pio->PIO_PER  = ul_mask;           // Ativa controle do pino no PIO    (PIO ENABLE register)
            	p_pio->PIO_OER  = ul_mask;           // Ativa sa�da                      (Output ENABLE register)

              if(!ul_default_level)                                 // Checa pela inicializa��o desejada
          	    p_pio->PIO_CODR = ul_mask;       // Coloca 0 na sa�da                (CLEAR Output Data register)
              else
                p_pio->PIO_SODR = ul_mask;       // Coloca 1 na sa�da                (SET Output Data register)

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
            	   	const uint32_t ul_attribute)
{
	p_pio->PIO_ODR	 = ul_mask;        // Desativa sa�da                   (Output DISABLE register)
	p_pio->PIO_PER	 = ul_mask;        // Ativa controle do pino no PIO    (PIO ENABLE register)
	p_pio->PIO_IFER	 = ul_mask;        // Ativa debouncing
	p_pio->PIO_IFSCER  = ul_mask;      // Ativa clock periferico
	p_pio->PIO_SCDR	 = ul_mask;		   // Configura a frequencia do debouncing
}

/**
 * \brief Configure PIO internal pull-up.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 * \param ul_pull_up_enable Indicates if the pin(s) internal pull-up shall be
 * configured.
 */
void _pio_pull_up(	Pio *p_pio,
               const uint32_t ul_mask,
		            const uint32_t ul_pull_up_enable)
{
	/* Enable the pull-up(s) if necessary */
	if (ul_pull_up_enable) {
		p_pio->PIO_PUER = ul_mask;
		} else {
		p_pio->PIO_PUDR = ul_mask;
	}
}

/**
 * \brief Configure PIO pin internal pull-down.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 * \param ul_pull_down_enable Indicates if the pin(s) internal pull-down shall
 * be configured.
 */
void _pio_pull_down(Pio *p_pio, const uint32_t ul_mask,
		const uint32_t ul_pull_down_enable)
{
	/* Enable the pull-down if necessary */
	if (ul_pull_down_enable) {
		p_pio->PIO_PPDER = ul_mask;
	} else {
		p_pio->PIO_PPDDR = ul_mask;
	}
}

/**
 * \brief Set a high output level on all the PIOs defined in ul_mask.
 * This has no immediate effects on PIOs that are not output, but the PIO
 * controller will save the value if they are changed to outputs.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 */
void _pio_set(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio->PIO_SODR = ul_mask;
}

/**
 * \brief Set a low output level on all the PIOs defined in ul_mask.
 * This has no immediate effects on PIOs that are not output, but the PIO
 * controller will save the value if they are changed to outputs.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 */
void _pio_clear(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio->PIO_CODR = ul_mask;
}

/**
 * \brief Return 1 if one or more PIOs of the given Pin are configured to
 * output a high level (even if they are not output).
 * To get the actual value of the pin, use PIO_Get() instead.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s).
 *
 * \retval 1 At least one PIO is configured to output a high level.
 * \retval 0 All PIOs are configured to output a low level.
 */
uint32_t _pio_get_output_data_status(const Pio *p_pio,
		const uint32_t ul_mask)
{
	if ((p_pio->PIO_ODSR & ul_mask) == 0) {
		return 0;
	} else {
		return 1;
	}
}