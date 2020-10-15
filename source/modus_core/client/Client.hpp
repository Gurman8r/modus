#ifndef _ML_CLIENT_HPP_
#define _ML_CLIENT_HPP_

#include <modus_core/detail/Matrix.hpp>
#include <modus_core/detail/Timer.hpp>
#include <modus_core/system/Events.hpp>
#include <modus_core/window/Input.hpp>

// CLIENT IO
namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// client io
	struct ML_NODISCARD client_io final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		pmr::vector<pmr::string> args;
		
		json prefs;

		fs::path	program_name{ args[0] },
					program_path{ fs::current_path() },
					content_path{ prefs["path"].get<fs::path>() };

		ML_NODISCARD fs::path path2(fs::path const & path) const noexcept
		{
			return content_path.native() + path.native();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		timer const		main_timer	{};
		timer			loop_timer	{ false };
		duration		delta_time	{};
		uint64_t		frame_count	{};

		float_t			fps			{};
		float_t			fps_accum	{};
		size_t			fps_index	{};
		float_t			fps_times	[120]{};

		vec2d			cursor		{};
		mouse_state		mouse		{};
		keyboard_state	keyboard	{};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

}

// CLIENT CONTEXT
namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct render_window;
	struct content_manager;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// client context
	struct ML_NODISCARD client_context final
	{
		memory_manager	* const mem		; // memory manager
		client_io		* const io		; // client io
		event_bus		* const bus		; // event bus
		render_window	* const win		; // render window
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// CLIENT OBJECT
namespace ml
{
	// client object
	template <class Derived
	> struct client_object : trackable, non_copyable, event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit client_object(client_context * ctx) noexcept
			: event_listener{ ML_check(ctx)->bus }
			, m_ctx			{ ctx }
		{
			ML_assert("BUS MISMATCH" && get_bus() == m_ctx->bus);
		}

		virtual ~client_object() noexcept override = default;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using event_listener::get_bus;

		ML_NODISCARD auto get_context() const noexcept -> client_context * { return m_ctx; }
		
		ML_NODISCARD auto get_io() const noexcept -> client_io * { return m_ctx->io; }

		ML_NODISCARD auto get_memory() const noexcept -> memory_manager * { return m_ctx->mem; }

		ML_NODISCARD auto get_window() const noexcept -> render_window * { return m_ctx->win; }

	protected:
		virtual void on_event(event &&) override = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		client_context * const m_ctx;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_CLIENT_HPP_