/*
 * app.c
 *
 *  Created on: 19 Apr 2023
 *      Author: a5126135
 */

#include "app.h"
#include "elcl/elcl.h"

/* Hardware event flags & buffer variable*/
volatile hardware_event_t hw_event_flags = NO_EVENT;
/* rotary counter*/
volatile int16_t rotary_count = 0;
/* Display area data*/
ST7735S_display_area_info_t disp_info = {ORIENT_0, 0U, 0U, 0U, 0U};

/* Array of titles for each state*/
const char * titles[MAXIMUM_SYSTEM_STATES] = {
		"1",
		"2",
		"3"
};

int16_t system_output_state = 1;
int16_t prev_system_output_state = 1;

void App_init(void)
{
	/* Start necessary drivers*/
	R_Config_TAU0_0_Start();
	R_Config_TAU0_1_Start();
	R_Config_TAU0_3_Start();
	R_Config_CSI00_Start_app();

	/* Display initialise*/
	St7735s_init(COLOUR_WHITE);
	St7735s_get_display_area_info(&disp_info);
	Text_init(disp_info.xmax, disp_info.ymax);
	Text_set_font(&default_font);
	St7735s_wake_display();
	St7735s_display_on();
	Text_put_line(TEXT_X_POSITION, TEXT_Y_POSITION, "1", COLOUR_BLACK, COLOUR_WHITE);
	St7735s_refresh();
	Hw_enable_backlight();

	/* Initialise the ELCL*/
	Elcl_set_input(&elcl_ctl, ELCL_INPUT_4, ELCL_SRC_PIN1_611_INTC5);
	Elcl_set_input(&elcl_ctl, ELCL_INPUT_6, ELCL_SRC_P51);
	Elcl_link_input(&elcl_ctl, ELCL_BLOCK1, ELCL_LNK_2, ELCL_INPUT_REG_4, ELCL_INVERTED_LOGIC);
	Elcl_link_input(&elcl_ctl, ELCL_BLOCK1, ELCL_LNK_6, ELCL_INPUT_REG_6, ELCL_INVERTED_LOGIC);
	Elcl_link_input(&elcl_ctl, ELCL_BLOCK1, ELCL_LNK_0, ELCL_INPUT_REG_4, ELCL_INVERTED_LOGIC);
	Elcl_link_input(&elcl_ctl, ELCL_BLOCK1, ELCL_LNK_1, ELCL_INPUT_REG_6, ELCL_INVERTED_LOGIC);
	Elcl_set_logic(&elcl_ctl, ELCL_BLOCK1, ELCL_ENABLE_FLIPFLOP0);
	Elcl_set_logic(&elcl_ctl, ELCL_BLOCK1, ELCL_EXOR_CELL_0);
	Elcl_link_to_logic(&elcl_ctl, ELCL_BLOCK1, ELCL_LNK_2, ELCL_FLIP_FLOP0_INPUT);
	Elcl_link_to_logic(&elcl_ctl, ELCL_BLOCK1, ELCL_LNK_6, ELCL_FLIP_FLOP0_CLK);
	Elcl_link_to_logic(&elcl_ctl, ELCL_BLOCK1, ELCL_LNK_0, ELCL_CELL0_INPUT_0);
	Elcl_link_to_logic(&elcl_ctl, ELCL_BLOCK1, ELCL_LNK_1, ELCL_CELL0_INPUT_1);

	Elcl_link_input(&elcl_ctl, ELCL_BLOCK2, ELCL_LNK_0, ELCL_OUTPUT_FLIP_FLOP0_L1, ELCL_INVERTED_LOGIC);
	Elcl_link_input(&elcl_ctl, ELCL_BLOCK2, ELCL_LNK_1, ELCL_OUTPUT_FLIP_FLOP0_L1, ELCL_POSITIVE_LOGIC);
	Elcl_link_input(&elcl_ctl, ELCL_BLOCK2, ELCL_LNK_2, ELCL_OUTPUT_CELL0_L1, ELCL_INVERTED_LOGIC);
	Elcl_link_input(&elcl_ctl, ELCL_BLOCK2, ELCL_LNK_3, ELCL_OUTPUT_CELL0_L1, ELCL_INVERTED_LOGIC);
	Elcl_set_logic(&elcl_ctl, ELCL_BLOCK2, ELCL_AND_CELL_0);
	Elcl_set_logic(&elcl_ctl, ELCL_BLOCK2, ELCL_AND_CELL_1);
	Elcl_link_to_logic(&elcl_ctl, ELCL_BLOCK2, ELCL_LNK_0, ELCL_CELL0_INPUT_0);
	Elcl_link_to_logic(&elcl_ctl, ELCL_BLOCK2, ELCL_LNK_2, ELCL_CELL0_INPUT_1);
	Elcl_link_to_logic(&elcl_ctl, ELCL_BLOCK2, ELCL_LNK_1, ELCL_CELL1_INPUT_0);
	Elcl_link_to_logic(&elcl_ctl, ELCL_BLOCK2, ELCL_LNK_3, ELCL_CELL1_INPUT_1);

	Elcl_set_output(&elcl_ctl, ELCL_OUTPUT_3, ELCL_OUTPUT_L2_CELL0, ELCL_POSITIVE_LOGIC);
	Elcl_set_output(&elcl_ctl, ELCL_OUTPUT_4, ELCL_OUTPUT_L2_CELL1, ELCL_POSITIVE_LOGIC);
	Elcl_set_output_state(&elcl_ctl, ELCL_OUTPUT_3, ELCL_OUTPUT_ENABLED);
	Elcl_set_output_state(&elcl_ctl, ELCL_OUTPUT_4, ELCL_OUTPUT_ENABLED);
}
/* END OF FUNCTION*/

void App_update_display(void)
{
	/* Erase previous text*/
	Text_put_line(TEXT_X_POSITION, TEXT_Y_POSITION, titles[prev_system_output_state-1], COLOUR_WHITE, COLOUR_WHITE);
	/* Write new text*/
	Text_put_line(TEXT_X_POSITION, TEXT_Y_POSITION, titles[system_output_state-1], COLOUR_BLACK, COLOUR_WHITE);
	St7735s_refresh();
}
/* END OF FUNCTION*/

bool App_button_click_event(void)
{
	bool event_occured = false;
	PREPARE_CRITICAL_SECTION();

	ENTER_CRITICAL_SECTION();

	event_occured = HW_EVENT_OCCURRED(hw_event_flags, BUTTON_CLICK);
	HW_CLEAR_EVENT(hw_event_flags, BUTTON_CLICK);

	EXIT_CRITICAL_SECTION();

	return event_occured;
}
/* END OF FUNCTION*/

bool App_rotary_event(void)
{
	bool event_occured = false;
	PREPARE_CRITICAL_SECTION();

	ENTER_CRITICAL_SECTION();

	event_occured = HW_EVENT_OCCURRED(hw_event_flags, ROTARY_COUNT_UPDATED);
	HW_CLEAR_EVENT(hw_event_flags, ROTARY_COUNT_UPDATED);

	EXIT_CRITICAL_SECTION();

	return event_occured;
}
/* END OF FUNCTION*/
