#ifndef _ML_RUNTIME_HPP_
#define _ML_RUNTIME_HPP_

#include <modus_core/detail/Database.hpp>
#include <modus_core/detail/Events.hpp>
#include <modus_core/detail/LoopSystem.hpp>
#include <modus_core/graphics/RenderWindow.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// runtime I/O
	struct ML_NODISCARD runtime_io final
	{
		ds::list<ds::string> args; // command line
		
		json prefs; // preferences

		// paths
		fs::path
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
		memory_manager	* const memory	; // memory manager
		runtime_io		* const io		; // runtime I/O
		simple_database	* const db		; // database
		event_bus		* const bus		; // event bus
		render_window	* const window	; // render window
		loop_system		* const loopsys	; // loop system
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// runtime object base
	template <class Derived
	> struct runtime_object
	{
	protected:
		using runtime_base = typename runtime_object<Derived>;

		virtual ~runtime_object() noexcept = default;

		explicit runtime_object(runtime_context * const context) noexcept
			: m_context{ ML_check(context) }
		{
		}

	public:
		ML_NODISCARD auto get_bus() const noexcept { return m_context->bus; }

		ML_NODISCARD auto get_context() const noexcept { return m_context; }

		ML_NODISCARD auto get_db() const noexcept { return m_context->db; }

		ML_NODISCARD auto get_io() const noexcept { return m_context->io; }

		ML_NODISCARD auto get_loopsys() const noexcept { return m_context->loopsys; }

		ML_NODISCARD auto get_memory() const noexcept { return m_context->memory; }

		ML_NODISCARD auto get_window() const noexcept { return m_context->window; }

	private:
		runtime_context * const m_context; // context
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// runtime listener base
	template <class Derived
	> struct runtime_listener : event_listener
	{
	protected:
		using runtime_base = typename runtime_listener<Derived>;

		virtual ~runtime_listener() noexcept override = default;

		explicit runtime_listener(runtime_context * const context) noexcept
			: event_listener{ ML_check(context)->bus }
			, m_context		{ context }
		{
			ML_assert("BUS MISMATCH" && get_bus() == get_context()->bus);
		}

		using event_listener::on_event; // event_listener

	public:
		using event_listener::get_bus; // event_listener

		ML_NODISCARD auto get_context() const noexcept { return m_context; }

		ML_NODISCARD auto get_db() const noexcept { return m_context->db; }
		
		ML_NODISCARD auto get_io() const noexcept { return m_context->io; }

		ML_NODISCARD auto get_loopsys() const noexcept { return m_context->loopsys; }

		ML_NODISCARD auto get_memory() const noexcept { return m_context->memory; }

		ML_NODISCARD auto get_window() const noexcept { return m_context->window; }

	private:
		runtime_context * const m_context; // context
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_RUNTIME_HPP_