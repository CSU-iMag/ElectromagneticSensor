/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

/*! @name PORTA4 (number 54), NMI
  @{ */
#define BOARD_NMI_GPIO GPIOA /*!<@brief GPIO device name: GPIOA */
#define BOARD_NMI_PORT PORTA /*!<@brief PORT device name: PORTA */
#define BOARD_NMI_PIN 4U     /*!<@brief PORTA pin index: 4 */
                             /* @} */

/*! @name PORTC17 (number 124), MRST
  @{ */
#define BOARD_MRST_PORT PORTC /*!<@brief PORT device name: PORTC */
#define BOARD_MRST_PIN 17U    /*!<@brief PORTC pin index: 17 */
                              /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void);

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/