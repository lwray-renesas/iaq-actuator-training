#ifndef MAIN_H_
#define MAIN_H_

#include "r_smc_entry.h"
#include "st7735s.h"
#include "text.h"
#include "elcl.h"

#define MAXIMUM_SYSTEM_STATES	(3)

/** @brief sets a hardware event flag*/
#define HW_SET_EVENT(flags,event)	{flags |= event;}
/** @brief Checks if hardware event flag is set*/
#define HW_EVENT_OCCURRED(flags,event)	( ((flags) & (event)) == event )


/** @brief macro used to prepare for disabling interrupts*/
#define PREPARE_CRITICAL_SECTION() uint8_t l_int_status = Interrupts_enabled()
/** @brief macro used to disable interrupts*/
#define ENTER_CRITICAL_SECTION()	DI()
/** @brief macro used to enable interrupts*/
#define EXIT_CRITICAL_SECTION()	if(l_int_status == 1U) { EI(); }


/** @brief Configuration macro for state text X location*/
#define TEXT_X_POSITION		(65U)
/** @brief Configuration macro for state text Y location*/
#define TEXT_Y_POSITION		(20U)


/** @brief Enumerated type for hardware event flags*/
typedef enum
{
	NO_EVENT				=	0x0000U,
	BUTTON_CLICK 			=	0x0001U,
	ROTARY_COUNT_UPDATED 	=	0x0002U,
	ALL_HARDWARE_EVENTS		=	0x0003U,
}hardware_event_t;

/** @brief Enumerated type for backlight level*/
typedef enum
{
	BACKLIGHT_OFF			=	0x0000U,
	BACKLIGHT_DIM			=	0x0001U,
	BACKLIGHT_ON			=	0x0002U,
}backlight_level_t;

/** Colours*/
static const uint8_t COLOUR_BLACK[] = {0x00, 0x00};
static const uint8_t COLOUR_WHITE[] = {0xFF, 0xFF};

/** rotary counter*/
extern volatile int16_t rotary_count;
/** hardware event flags*/
extern volatile hardware_event_t hw_event_flags;

#endif
