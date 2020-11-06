#ifndef _ML_RUNTIME_HPP_
#define _ML_RUNTIME_HPP_

#include <modus_core/detail/Database.hpp>
#include <modus_core/detail/Events.hpp>
#include <modus_core/detail/LoopSystem.hpp>
#include <modus_core/graphics/RenderWindow.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// runtime io
	struct ML_NODISCARD runtime_io final
	{
		ds::list<ds::string> args; // command line
		
		json prefs; // preferences

		// paths
		fs::path const
			program_name{ args[0] },
			program_path{ fs::current_path() },
			content_path{ prefs.contains("path") ? prefs["path"].get<fs::path>() : "./" };

		ML_NODISCARD fs::path path2(fs::path const & path) const noexcept
		{
			return content_path.native() + path.native();
		}

		// timers
		timer const				main_timer	{};
		timer					loop_timer	{ false };
		duration				delta_time	{};
		uint64_t				frame_count	{};
		float_t					fps			{};
		float_t					fps_accum	{};
		size_t					fps_index	{};
		ds::array<float_t, 120> fps_times	{};

		// input
		vec2d					cursor		{};
		mouse_state				mouse		{};
		keyboard_state			keyboard	{};
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// runtime context
	struct ML_NODISCARD runtime_context final
	{
		memory_manager	* const memory		; // memory manager
		runtime_io		* const io			; // runtime I/O
		simple_database	* const database	; // database
		event_bus		* const bus			; // event bus
		render_window	* const window		; // render window
		loop_system		* const loopsys		; // loop system
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// runtime object
	template <class Derived
	> struct runtime_object : trackable, non_copyable, event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		explicit runtime_object(runtime_context * const ctx) noexcept
			: event_listener{ ML_check(ctx)->bus }
			, m_context		{ ctx }
		{
			ML_assert_msg(get_bus() == get_context()->bus, "RUNTIME BUS MISMATCH");
		}

		virtual ~runtime_object() noexcept override = default;

		virtual void on_event(event &&) override = 0; // event_listener::on_event

	public:
		using event_listener::get_bus; // event_listener::get_bus

		ML_NODISCARD auto get_context() const noexcept -> runtime_context * { return m_context; }

		ML_NODISCARD auto get_database() const noexcept -> simple_database * { return m_context->database; }
		
		ML_NODISCARD auto get_io() const noexcept -> runtime_io * { return m_context->io; }

		ML_NODISCARD auto get_loopsys() const noexcept -> loop_system * { return m_context->loopsys; }

		ML_NODISCARD auto get_memory() const noexcept -> memory_manager * { return m_context->memory; }

		ML_NODISCARD auto get_window() const noexcept -> render_window * { return m_context->window; }

	private:
		runtime_context * const m_context; // runtime context

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_RUNTIME_HPP_