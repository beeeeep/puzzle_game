#ifndef  ROTARY_H
#define ROTARY_H

#define  BUTTON_DEBOUNCE_TIME_MS 200

typedef enum
{
	BUTTON_RELEASED,
	BUTTON_HOLDING_DOWN
}button_state_t;

typedef struct rotary
{
	int direction;
	unsigned char button;
	unsigned int button_hold_time_in_ms;
}rotary_t;

/*! 
 * \defgroup Rotary Rotary encoder library
 *
 *	\brief This is the library for the rotary encoder, it uses two external interrupt ISRs, one for the button 
 *	and one for the A and B pins. The external interrupts need to be set  to both "rising" and "falling" edge.	
 *  \{
 */

typedef char (*Get_rotary_pin_state_t)(void);
typedef void (*Rotary_interrupt_callback_t) (void);

/** Initialization function. Passes the pin handling function pointers to the library 
 *
 *  \param[in] get_A_pin_state   Pointer to function that reads the A pin's state  
 *  \param[in] get_B_pin_state   Pointer to function that reads the B pin's state   
 *  \param[in] get_button_pin_state   Pointer to function that reads the Button pin's state  
 *  \param[in] time_in_millis  Pointer to function that reads millis timers value
 *
 */
void Rotary_init(Get_rotary_pin_state_t get_A_pin_state, Get_rotary_pin_state_t get_B_pin_state, Get_rotary_pin_state_t get_button_pin_state,uint32_t *time_in_millis);

/** Main function. Passes the values of the rotary encoder to the struct
 *
 *  \param[in] status   The rotary struct
 *
 */
void Rotary_get_status(rotary_t * status);

/** External interrupt ISR function for the A and B pins
 *
 *
 */
void Rotary_AB_pin_callback_fuction(void);

/** External interrupt ISR function for button pin
 *
 *
 */
void Rotary_button_pin_callback_fuction(void);


#endif // ! ROTARY_H
