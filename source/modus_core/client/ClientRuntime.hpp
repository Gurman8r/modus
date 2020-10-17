#ifndef _ML_CLIENT_RUNTIME_HPP_
#define _ML_CLIENT_RUNTIME_HPP_

#include <modus_core/client/ImGui.hpp>
#include <modus_core/client/PluginManager.hpp>

namespace ml
{
	// loop condition
	ML_alias loop_condition_fn = typename std::function<int32_t()>;

	// client runtime
	struct ML_CORE_API client_runtime final : client_object<client_runtime>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		explicit client_runtime(client_context * context);

		~client_runtime() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// LOOP

		ML_NODISCARD int32_t idle();

		ML_NODISCARD int32_t check_loop_condition() const noexcept { return m_loopcond && m_loopcond(); }

		ML_NODISCARD loop_condition_fn const & get_loop_condition() const noexcept { return m_loopcond; }

		void set_loop_condition(loop_condition_fn const & value) noexcept { m_loopcond = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// GUI

		ML_NODISCARD auto get_imgui() const noexcept -> manual<ImGuiContext> const & { return m_imgui; }

		ML_NODISCARD bool get_menu_enabled() const noexcept { return m_menu_enabled; }
		
		ML_NODISCARD bool get_dock_enabled() const noexcept { return m_dock_enabled; }

		ML_NODISCARD auto get_dock_title() const noexcept -> pmr::string const & { return m_dock_title; }

		ML_NODISCARD auto get_dock_border() const noexcept -> float_t { return m_dock_border; }

		ML_NODISCARD auto get_dock_rounding() const noexcept -> float_t { return m_dock_rounding; }

		ML_NODISCARD auto get_dock_alpha() const noexcept -> float_t { return m_dock_alpha; }

		ML_NODISCARD auto get_dock_padding() const noexcept  -> vec2 const & { return m_dock_padding; }

		ML_NODISCARD auto get_dock_size() const noexcept -> vec2 const & { return m_dock_size; }

		ML_NODISCARD auto get_dock_flags() const noexcept -> int32_t { return m_dock_flags; }

		ML_NODISCARD void set_menu_enabled(bool value) noexcept { m_menu_enabled = value; }

		ML_NODISCARD void set_dock_enabled(bool value) noexcept { m_dock_enabled = value; }

		ML_NODISCARD void set_dock_title(pmr::string const & value) noexcept { m_dock_title = value; }

		ML_NODISCARD void set_dock_border(float_t value) noexcept { m_dock_border = value; }

		ML_NODISCARD void set_dock_rounding(float_t value) noexcept { m_dock_rounding = value; }

		ML_NODISCARD void set_dock_alpha(float_t value) noexcept { m_dock_alpha = value; }

		ML_NODISCARD void set_dock_padding(vec2 const & value) noexcept { m_dock_padding = value; }

		ML_NODISCARD void set_dock_size(vec2 const & value) noexcept { m_dock_size = value; }

		ML_NODISCARD void set_dock_flags(int32_t value) noexcept { m_dock_flags = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// PLUGINS

		ML_NODISCARD auto get_plugins() noexcept -> plugin_manager & { return m_plugins; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		void do_startup(client_context * ctx);

		void do_shutdown();

		void do_idle();

		void do_gui();

		void on_event(event && value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		bool					m_idling	; // run lock
		loop_condition_fn		m_loopcond	; // loop condition
		manual<ImGuiContext>	m_imgui		; // imgui context
		plugin_manager			m_plugins	; // plugin manager
		
		bool		m_menu_enabled	, // menu enabled
					m_dock_enabled	; // dock enabled
		pmr::string	m_dock_title	; // dock title
		float_t		m_dock_border	; // dock border
		float_t		m_dock_rounding	; // dock rounding
		float_t		m_dock_alpha	; // dock alpha
		vec2		m_dock_padding	; // dock padding
		vec2		m_dock_size		; // dock size
		int32_t		m_dock_flags	; // dock flags

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_CLIENT_RUNTIME_HPP_