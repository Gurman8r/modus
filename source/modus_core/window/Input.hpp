#ifndef _ML_INPUT_HPP_
#define _ML_INPUT_HPP_

#include <modus_core/detail/Array.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// key code
	enum keycode_ : int32_t
	{
		keycode_space			= 32,	// space
		keycode_apostrophe		= 39,	// apostrophe
		keycode_comma			= 44,	// comma
		keycode_minus			= 45,	// minus
		keycode_period			= 46,	// period
		keycode_slash			= 47,	// slash
		keycode_num_0			= 48,	// 0
		keycode_num_1			= 49,	// 1
		keycode_num_2			= 50,	// 2
		keycode_num_3			= 51,	// 3
		keycode_num_4			= 52,	// 4
		keycode_num_5			= 53,	// 5
		keycode_num_6			= 54,	// 6
		keycode_num_7			= 55,	// 7
		keycode_num_8			= 56,	// 8
		keycode_num_9			= 57,	// 9
		keycode_semicolon		= 59,	// semicolon
		keycode_equal			= 61,	// equal
		keycode_a				= 65,	// a
		keycode_b				= 66,	// b
		keycode_c				= 67,	// c
		keycode_d				= 68,	// d
		keycode_e				= 69,	// e
		keycode_f				= 70,	// f
		keycode_g				= 71,	// g
		keycode_h				= 72,	// h
		keycode_i				= 73,	// i
		keycode_j				= 74,	// j
		keycode_k				= 75,	// k
		keycode_l				= 76,	// l
		keycode_m				= 77,	// m
		keycode_n				= 78,	// n
		keycode_o				= 79,	// o
		keycode_p				= 80,	// p
		keycode_q				= 81,	// q
		keycode_r				= 82,	// r
		keycode_s				= 83,	// s
		keycode_t				= 84,	// t
		keycode_u				= 85,	// u
		keycode_v				= 86,	// v
		keycode_w				= 87,	// w
		keycode_x				= 88,	// x
		keycode_y				= 89,	// y
		keycode_z				= 90,	// z
		keycode_left_bracket	= 91,	// left bracket
		keycode_backslash		= 92,	// backslash
		keycode_right_bracket	= 93,	// right bracket
		keycode_grave_accent	= 96,	// grave accent
		keycode_world_1			= 161,	// world 1
		keycode_world_2			= 162,	// world 2
		keycode_escape			= 256,	// escape
		keycode_enter			= 257,	// enter
		keycode_tab				= 258,	// tab
		keycode_backspace		= 259,	// backspace
		keycode_insert			= 260,	// insert
		keycode_del				= 261,	// del
		keycode_right			= 262,	// right
		keycode_left			= 263,	// left
		keycode_down			= 264,	// down
		keycode_up				= 265,	// up
		keycode_page_up			= 266,	// page up
		keycode_page_down		= 267,	// page down
		keycode_home			= 268,	// home
		keycode_end				= 269,	// end
		keycode_caps_lock		= 280,	// caps lock
		keycode_scroll_lock		= 281,	// scroll lock
		keycode_num_lock		= 282,	// num lock
		keycode_print_screen	= 283,	// print screen
		keycode_pause			= 284,	// pause
		keycode_fn_1			= 290,	// f1
		keycode_fn_2			= 291,	// f2
		keycode_fn_3			= 292,	// f3
		keycode_fn_4			= 293,	// f4
		keycode_fn_5			= 294,	// f5
		keycode_fn_6			= 295,	// f6
		keycode_fn_7			= 296,	// f7
		keycode_fn_8			= 297,	// f8
		keycode_fn_9			= 298,	// f9
		keycode_fn_10			= 299,	// f10
		keycode_fn_11			= 300,	// f11
		keycode_fn_12			= 301,	// f12
		keycode_fn_13			= 302,	// f13
		keycode_fn_14			= 303,	// f14
		keycode_fn_15			= 304,	// f15
		keycode_fn_16			= 305,	// f16
		keycode_fn_17			= 306,	// f17
		keycode_fn_18			= 307,	// f18
		keycode_fn_19			= 308,	// f19
		keycode_fn_20			= 309,	// f20
		keycode_fn_21			= 310,	// f21
		keycode_fn_22			= 311,	// f22
		keycode_fn_23			= 312,	// f23
		keycode_fn_24			= 313,	// f24
		keycode_fn_25			= 314,	// f25
		keycode_kp_0			= 320,	// keypad 0
		keycode_kp_1			= 321,	// keypad 1
		keycode_kp_2			= 322,	// keypad 2
		keycode_kp_3			= 323,	// keypad 3
		keycode_kp_4			= 324,	// keypad 4
		keycode_kp_5			= 325,	// keypad 5
		keycode_kp_6			= 326,	// keypad 6
		keycode_kp_7			= 327,	// keypad 7
		keycode_kp_8			= 328,	// keypad 8
		keycode_kp_9			= 329,	// keypad 9
		keycode_kp_decimal		= 330,	// keypad decimal
		keycode_kp_divide		= 331,	// keypad divide
		keycode_kp_multiply		= 332,	// keypad multiply
		keycode_kp_subtract		= 333,	// keypad subtract
		keycode_kp_add			= 334,	// keypad add
		keycode_kp_enter		= 335,	// keypad enter
		keycode_kp_equal		= 336,	// keypad equal
		keycode_left_shift		= 340,	// left shift
		keycode_left_control	= 341,	// left control
		keycode_left_alt		= 342,	// left alt
		keycode_left_super		= 343,	// left super
		keycode_right_shift		= 344,	// right shift
		keycode_right_control	= 345,	// right control
		keycode_right_alt		= 346,	// right alt
		keycode_right_super		= 347,	// right super
		keycode_menu			= 348,	// menu

		keycode_MAX
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// key modifiers
	enum keymods_ : int32_t
	{
		keymods_none		= 0 << 0,	// none
		keymods_shift		= 0 << 1,	// shift
		keymods_ctrl		= 1 << 1,	// ctrl
		keymods_alt		= 1 << 2,	// alt
		keymods_super		= 1 << 3,	// super
		keymods_caps_lock	= 1 << 4,	// caps
		keymods_num_lock	= 1 << 5	// numlock
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

	// states

	ML_alias keyboard_state = typename ds::array<int32_t, keycode_MAX>;

	ML_alias mouse_state = typename ds::array<int32_t, mouse_button_MAX>;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_INPUT_HPP_