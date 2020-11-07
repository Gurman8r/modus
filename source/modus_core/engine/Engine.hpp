#ifndef _ML_ENGINE_HPP_
#define _ML_ENGINE_HPP_

#include <modus_core/detail/Database.hpp>
#include <modus_core/detail/Events.hpp>
#include <modus_core/detail/LoopSystem.hpp>
#include <modus_core/graphics/RenderWindow.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// engine I/O
	struct ML_NODISCARD engine_io final
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

	// engine context
	struct ML_NODISCARD engine_context final
	{
		memory_manager	* const memory		; // memory manager
		engine_io		* const io			; // engine I/O
		simple_database	* const database	; // database
		event_bus		* const bus			; // event bus
		render_window	* const window		; // render window
		loop_system		* const mainloop	; // loop system
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// base engine object
	template <class Derived
	> struct engine_object
	{
	public:
		using engine_base = typename engine_object<Derived>;

	protected:
		virtual ~engine_object() noexcept = default; // ~

		explicit engine_object(engine_context * const context) noexcept
			: m_context{ ML_check(context) }
		{
		}

	public:
		ML_NODISCARD auto get_bus() const noexcept { return m_context->bus; }

		ML_NODISCARD auto get_context() const noexcept { return m_context; }

		ML_NODISCARD auto get_database() const noexcept { return m_context->database; }

		ML_NODISCARD auto get_io() const noexcept { return m_context->io; }

		ML_NODISCARD auto get_main_loop() const noexcept { return m_context->mainloop; }

		ML_NODISCARD auto get_memory() const noexcept { return m_context->memory; }

		ML_NODISCARD auto get_window() const noexcept { return m_context->window; }

	private:
		engine_context * const m_context; // context
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// base engine listener
	template <class Derived
	> struct engine_listener : event_listener
	{
	public:
		using engine_base = typename engine_listener<Derived>;

	protected:
		virtual ~engine_listener() noexcept override = default; // ~

		explicit engine_listener(engine_context * const context) noexcept
			: event_listener{ ML_check(context)->bus }
			, m_context		{ context }
		{
		}

		using event_listener::on_event; // event_listener

	public:
		using event_listener::get_bus; // event_listener

		ML_NODISCARD auto get_context() const noexcept { return m_context; }

		ML_NODISCARD auto get_database() const noexcept { return m_context->database; }
		
		ML_NODISCARD auto get_io() const noexcept { return m_context->io; }

		ML_NODISCARD auto get_main_loop() const noexcept { return m_context->mainloop; }

		ML_NODISCARD auto get_memory() const noexcept { return m_context->memory; }

		ML_NODISCARD auto get_window() const noexcept { return m_context->window; }

	private:
		engine_context * const m_context; // context
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_ENGINE_HPP_