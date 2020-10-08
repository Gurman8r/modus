#ifndef _ML_INPUT_HPP_
#define _ML_INPUT_HPP_

#include <core/detail/Array.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// key code
	enum key_code_ : int32_t
	{
		key_code_space			= 32,	// space
		key_code_apostrophe		= 39,	// apostrophe
		key_code_comma			= 44,	// comma
		key_code_minus			= 45,	// minus
		key_code_period			= 46,	// period
		key_code_slash			= 47,	// slash
		key_code_num_0			= 48,	// 0
		key_code_num_1			= 49,	// 1
		key_code_num_2			= 50,	// 2
		key_code_num_3			= 51,	// 3
		key_code_num_4			= 52,	// 4
		key_code_num_5			= 53,	// 5
		key_code_num_6			= 54,	// 6
		key_code_num_7			= 55,	// 7
		key_code_num_8			= 56,	// 8
		key_code_num_9			= 57,	// 9
		key_code_semicolon		= 59,	// semicolon
		key_code_equal			= 61,	// equal
		key_code_a				= 65,	// a
		key_code_b				= 66,	// b
		key_code_c				= 67,	// c
		key_code_d				= 68,	// d
		key_code_e				= 69,	// e
		key_code_f				= 70,	// f
		key_code_g				= 71,	// g
		key_code_h				= 72,	// h
		key_code_i				= 73,	// i
		key_code_j				= 74,	// j
		key_code_k				= 75,	// k
		key_code_l				= 76,	// l
		key_code_m				= 77,	// m
		key_code_n				= 78,	// n
		key_code_o				= 79,	// o
		key_code_p				= 80,	// p
		key_code_q				= 81,	// q
		key_code_r				= 82,	// r
		key_code_s				= 83,	// s
		key_code_t				= 84,	// t
		key_code_u				= 85,	// u
		key_code_v				= 86,	// v
		key_code_w				= 87,	// w
		key_code_x				= 88,	// x
		key_code_y				= 89,	// y
		key_code_z				= 90,	// z
		key_code_left_bracket	= 91,	// left bracket
		key_code_backslash		= 92,	// backslash
		key_code_right_bracket	= 93,	// right bracket
		key_code_grave_accent	= 96,	// grave accent
		key_code_world_1		= 161,	// world 1
		key_code_world_2		= 162,	// world 2
		key_code_escape			= 256,	// escape
		key_code_enter			= 257,	// enter
		key_code_tab			= 258,	// tab
		key_code_backspace		= 259,	// backspace
		key_code_insert			= 260,	// insert
		key_code_del			= 261,	// del
		key_code_right			= 262,	// right
		key_code_left			= 263,	// left
		key_code_down			= 264,	// down
		key_code_up				= 265,	// up
		key_code_page_up		= 266,	// page up
		key_code_page_down		= 267,	// page down
		key_code_home			= 268,	// home
		key_code_end			= 269,	// end
		key_code_caps_lock		= 280,	// caps lock
		key_code_scroll_lock	= 281,	// scroll lock
		key_code_num_lock		= 282,	// num lock
		key_code_print_screen	= 283,	// print screen
		key_code_pause			= 284,	// pause
		key_code_fn_1			= 290,	// f1
		key_code_fn_2			= 291,	// f2
		key_code_fn_3			= 292,	// f3
		key_code_fn_4			= 293,	// f4
		key_code_fn_5			= 294,	// f5
		key_code_fn_6			= 295,	// f6
		key_code_fn_7			= 296,	// f7
		key_code_fn_8			= 297,	// f8
		key_code_fn_9			= 298,	// f9
		key_code_fn_10			= 299,	// f10
		key_code_fn_11			= 300,	// f11
		key_code_fn_12			= 301,	// f12
		key_code_fn_13			= 302,	// f13
		key_code_fn_14			= 303,	// f14
		key_code_fn_15			= 304,	// f15
		key_code_fn_16			= 305,	// f16
		key_code_fn_17			= 306,	// f17
		key_code_fn_18			= 307,	// f18
		key_code_fn_19			= 308,	// f19
		key_code_fn_20			= 309,	// f20
		key_code_fn_21			= 310,	// f21
		key_code_fn_22			= 311,	// f22
		key_code_fn_23			= 312,	// f23
		key_code_fn_24			= 313,	// f24
		key_code_fn_25			= 314,	// f25
		key_code_kp_0			= 320,	// keypad 0
		key_code_kp_1			= 321,	// keypad 1
		key_code_kp_2			= 322,	// keypad 2
		key_code_kp_3			= 323,	// keypad 3
		key_code_kp_4			= 324,	// keypad 4
		key_code_kp_5			= 325,	// keypad 5
		key_code_kp_6			= 326,	// keypad 6
		key_code_kp_7			= 327,	// keypad 7
		key_code_kp_8			= 328,	// keypad 8
		key_code_kp_9			= 329,	// keypad 9
		key_code_kp_decimal		= 330,	// keypad decimal
		key_code_kp_divide		= 331,	// keypad divide
		key_code_kp_multiply	= 332,	// keypad multiply
		key_code_kp_subtract	= 333,	// keypad subtract
		key_code_kp_add			= 334,	// keypad add
		key_code_kp_enter		= 335,	// keypad enter
		key_code_kp_equal		= 336,	// keypad equal
		key_code_left_shift		= 340,	// left shift
		key_code_left_control	= 341,	// left control
		key_code_left_alt		= 342,	// left alt
		key_code_left_super		= 343,	// left super
		key_code_right_shift	= 344,	// right shift
		key_code_right_control	= 345,	// right control
		key_code_right_alt		= 346,	// right alt
		key_code_right_super	= 347,	// right super
		key_code_menu			= 348,	// menu

		key_code_MAX
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// key modifiers
	enum key_mods_ : int32_t
	{
		key_mods_none		= 0 << 0,	// none
		key_mods_shift		= 0 << 1,	// shift
		key_mods_ctrl		= 1 << 1,	// ctrl
		key_mods_alt		= 1 << 2,	// alt
		key_mods_super		= 1 << 3,	// super
		key_mods_caps_lock	= 1 << 4,	// caps
		key_mods_num_lock	= 1 << 5	// numlock
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// mouse button
	enum mouse_button_ : int32_t
	{
		mouse_button_0, // mouse button 0
		mouse_button_1, // mouse button 1
		mouse_button_2, // mouse button 2
		mouse_button_3, // mouse button 3
		mouse_button_4, // mouse button 4
		mouse_button_5, // mouse button 5
		mouse_button_6, // mouse button 6
		mouse_button_7, // mouse button 7

		mouse_button_MAX
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// input state
	enum input_state_ : int32_t
	{
		input_state_release	, // release | hi -> lo
		input_state_press	, // press   | lo -> hi
		input_state_repeat	, // repeat  | hi -> hi

		input_state_MAX
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_alias keyboard_state = typename ds::array<int32_t, key_code_MAX>;

	ML_alias mouse_state = typename ds::array<int32_t, mouse_button_MAX>;

	struct input_axis final
	{
		// WIP
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_INPUT_HPP_