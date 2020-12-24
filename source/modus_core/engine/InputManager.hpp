#ifndef _ML_INPUT_MANAGER_HPP_
#define _ML_INPUT_MANAGER_HPP_

// WIP

#include <modus_core/detail/Memory.hpp>
#include <modus_core/window/Input.hpp>

namespace ml
{
	struct ML_NODISCARD input_manager final : trackable
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		char
			last_char{};
		float32
			mouse_wheel{};
		vec2
			mouse_pos{},
			mouse_delta{};
		bool
			mouse[mouse_button_MAX]{},
			keys[keycode_MAX]{};
		timer
			mouse_timers[mouse_button_MAX]{},
			key_timers[keycode_MAX]{};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		ML_NODISCARD bool is_shift() const noexcept { return keys[keycode_left_shift] || keys[keycode_right_shift]; }
		
		ML_NODISCARD bool is_ctrl() const noexcept { return keys[keycode_left_ctrl] || keys[keycode_right_ctrl]; }
		
		ML_NODISCARD bool is_alt() const noexcept { return keys[keycode_left_alt] || keys[keycode_right_alt]; }
		
		ML_NODISCARD bool is_super() const noexcept { return keys[keycode_left_super] || keys[keycode_right_super]; }
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		input_manager() noexcept = default;
		input_manager(input_manager const &) = default;
		input_manager(input_manager &&) noexcept = default;
		input_manager & operator=(input_manager const &) = default;
		input_manager & operator=(input_manager &&) noexcept = default;
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_INPUT_MANAGER_HPP_