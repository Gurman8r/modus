#ifndef _ML_GUI_APPLICATION_HPP_
#define _ML_GUI_APPLICATION_HPP_

// WIP

#include <modus_core/engine/CoreApplication.hpp>
#include <modus_core/engine/MainWindow.hpp>
#include <modus_core/engine/PlatformAPI.hpp>
#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/window/WindowEvents.hpp>

namespace ml
{
	struct ML_CORE_API gui_application : core_application
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using core_application::allocator_type;

		explicit gui_application(int32 argc, char * argv[], allocator_type alloc = {});

		virtual ~gui_application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_fps() const noexcept -> float32
		{
			return m_fps.value;
		}

		ML_NODISCARD auto get_window() const noexcept -> main_window *
		{
			return const_cast<main_window *>(&m_window);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto open_file_name(cstring filter = "") const -> std::optional<fs::path>
		{
			return platform_api::get_open_file_name(m_window.get_native_handle(), filter);
		}

		ML_NODISCARD auto save_file_name(cstring filter = "") const -> std::optional<fs::path>
		{
			return platform_api::get_save_file_name(m_window.get_native_handle(), filter);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		struct ML_NODISCARD // fps
		{
			float32					value; // 
			float32					accum; // 
			size_t					index; // 
			ds::array<float32, 120>	times; // 
		}
		m_fps;

		main_window m_window; //

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// global gui_application
namespace ml::globals
{
	ML_decl_global(gui_application) get() noexcept;

	ML_decl_global(gui_application) set(gui_application * value) noexcept;
}

#endif // !_ML_GUI_APPLICATION_HPP_