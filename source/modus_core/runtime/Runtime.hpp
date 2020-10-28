#ifndef _ML_RUNTIME_HPP_
#define _ML_RUNTIME_HPP_

#include <modus_core/detail/Matrix.hpp>
#include <modus_core/detail/Timer.hpp>
#include <modus_core/detail/Database.hpp>
#include <modus_core/runtime/Layers.hpp>
#include <modus_core/window/Input.hpp>

namespace ml { struct render_window; }

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// runtime io
	struct ML_NODISCARD runtime_io final
	{
		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		runtime_io(int32_t argc, char ** argv, json const & j, allocator_type alloc = {})
			: argc			{ argc }
			, argv			{ argv }
			, prefs			{ json{ j } }
			, program_name	{ argv[0] }
			, program_path	{ fs::current_path() }
			, content_path	{ j.contains("path") ? j["path"].get<fs::path>() : "./" }
		{
		}

		// config
		int32_t const	argc;
		char ** const	argv;
		json			prefs;
		fs::path const	program_name, program_path, content_path;

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

	// runtime api
	struct ML_NODISCARD runtime_api final
	{
		memory_manager	* const mem		; // memory
		runtime_io		* const io		; // io
		event_bus		* const bus		; // bus
		layer_stack		* const layers	; // layers
		render_window	* const win		; // window
		simple_database * const db		; // database
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// runtime object
	template <class Derived
	> struct runtime_object : trackable, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit runtime_object(runtime_api * api) noexcept : m_api{ ML_check(api) }
		{
		}

		virtual ~runtime_object() noexcept override = default;

		ML_NODISCARD auto get_api() const noexcept -> runtime_api * { return m_api; }

		ML_NODISCARD auto get_bus() const noexcept -> event_bus * { return m_api->bus; }

		ML_NODISCARD auto get_db() const noexcept -> simple_database * { return m_api->db; }

		ML_NODISCARD auto get_io() const noexcept -> runtime_io * { return m_api->io; }
		
		ML_NODISCARD auto get_layers() const noexcept -> layer_stack * { return m_api->layers; }

		ML_NODISCARD auto get_memory() const noexcept -> memory_manager * { return m_api->mem; }

		ML_NODISCARD auto get_window() const noexcept -> render_window * { return m_api->win; }

	private:
		runtime_api * const m_api;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// runtime listener
	template <class Derived
	> struct runtime_listener : trackable, non_copyable, event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit runtime_listener(runtime_api * api) noexcept
			: event_listener{ ML_check(api)->bus }
			, m_api			{ api }
		{
			ML_assert("BUS MISMATCH" && get_bus() == m_api->bus);
		}

		virtual ~runtime_listener() noexcept override = default;

		ML_NODISCARD auto get_api() const noexcept -> runtime_api * { return m_api; }

		using event_listener::get_bus;

		ML_NODISCARD auto get_db() const noexcept -> simple_database * { return m_api->db; }
		
		ML_NODISCARD auto get_io() const noexcept -> runtime_io * { return m_api->io; }

		ML_NODISCARD auto get_layers() const noexcept -> layer_stack * { return m_api->layers; }

		ML_NODISCARD auto get_memory() const noexcept -> memory_manager * { return m_api->mem; }

		ML_NODISCARD auto get_window() const noexcept -> render_window * { return m_api->win; }

	protected:
		virtual void on_event(event &&) override = 0;

	private:
		runtime_api * const m_api;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_RUNTIME_HPP_