#ifndef _ML_GUI_APPLICATION_HPP_
#define _ML_GUI_APPLICATION_HPP_

#include <modus_core/engine/CoreApplication.hpp>
#include <modus_core/engine/MainWindow.hpp>
#include <modus_core/detail/LoopSystem.hpp>

namespace ml
{
	// fps tracker
	template <size_t N = 120
	> struct ML_NODISCARD fps_tracker final : non_copyable, trackable
	{
		float32					value{}; // 
		float32					accum{}; // 
		size_t					index{}; // 
		ds::array<float32, N>	times{}; // 

		fps_tracker() noexcept = default;

		void operator()(float32 dt) noexcept
		{
			accum += dt - times[index];
			times[index] = dt;
			index = (index + 1) % times.size();
			value = (0.f < accum) ? (1.f / (accum / (float32)times.size())) : FLT_MAX;
		}
	};
}

namespace ml
{
	// gui application
	struct ML_CORE_API gui_application : core_application
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using core_application::allocator_type;

		explicit gui_application(int32 argc, char * argv[], allocator_type alloc = {});

		virtual ~gui_application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual int32 exec() override
		{
			ML_check(m_main_loop)->process();

			return core_application::exec();
		}

		virtual void exit(int32 exit_code) override
		{
			ML_check(m_main_loop)->halt();

			core_application::exit(exit_code);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_fps() const noexcept -> fps_tracker<> const *
		{
			return &m_fps_tracker;
		}

		ML_NODISCARD auto get_main_loop() const noexcept -> ds::ref<loop_system> const &
		{
			return m_main_loop;
		}

		ML_NODISCARD auto get_main_window() const noexcept -> main_window *
		{
			return const_cast<main_window *>(&m_main_window);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto set_main_loop(ds::ref<loop_system> const & value) noexcept -> ds::ref<loop_system> &
		{
			ML_assert(!m_main_loop || !m_main_loop->running());

			return m_main_loop = value;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD std::optional<fs::path> open_file_name(ds::string const & filter = "") const;

		ML_NODISCARD std::optional<fs::path> save_file_name(ds::string const & filter = "") const;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		int32					m_exit_code		; // exit code
		event_bus				m_dispatcher	; // dispatcher
		ds::ref<loop_system>	m_main_loop		; // main loop
		main_window				m_main_window	; // main window
		fps_tracker<>			m_fps_tracker	; // fps tracker

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