#ifndef _ML_IMGUI_CONTEXT_HPP_
#define _ML_IMGUI_CONTEXT_HPP_

#include <core/Export.hpp>
#include <core/imgui/ImGui.hpp>

namespace ml
{
	struct event_bus;
	struct render_window;

	struct ML_CORE_API imgui_context final : trackable, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		imgui_context(event_bus * bus, render_window * win, allocator_type alloc);

		~imgui_context();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool startup(bool install_callbacks = true);

		void shutdown();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void new_frame();

		void render_frame();

		template <class ... Args
		> void do_frame(Args && ... args)
		{
			new_frame();
			if constexpr (0 < sizeof...(args))
			{
				std::invoke(ML_forward(args));
			}
			render_frame();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static void imgui_demo(bool * p_open = {});

		static void imgui_metrics(bool * p_open = {});

		static void imgui_about(bool * p_open = {});

		static void imgui_style_editor(ImGuiStyle * ref = {});

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// main menu bar
		struct ML_CORE_API menubar final : trackable, non_copyable
		{
			bool enabled{ true };

			explicit menubar(allocator_type alloc) noexcept {}

			void configure(json const & j)
			{
				j["enabled"].get_to(enabled);
			}
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// dockspace
		struct ML_CORE_API dockspace final : trackable, non_copyable
		{
			bool					enabled		{ true };
			pmr::string				title		{ "dockspace##modus" };
			float_t					border		{};
			float_t					rounding	{};
			float_t					alpha		{};
			vec2					padding		{};
			vec2					size		{};
			int32_t					flags		{ ImGuiDockNodeFlags_AutoHideTabBar };
			pmr::vector<uint32_t>	nodes		{};

			explicit dockspace(allocator_type alloc) noexcept : nodes{ alloc } {}

			void configure(json const & j)
			{
				j["enabled"	].get_to(enabled);
				j["title"	].get_to(title);
				j["border"	].get_to(border);
				j["rounding"].get_to(rounding);
				j["alpha"	].get_to(alpha);
				j["padding"	].get_to(padding);
				j["size"	].get_to(size);
				j["nodes"	].get_to(nodes);
			}

			ML_NODISCARD auto & operator[](size_t i) noexcept { return nodes[i]; }

			ML_NODISCARD auto const & operator[](size_t i) const noexcept { return nodes[i]; }

			uint32_t begin_builder();

			uint32_t end_builder(uint32_t root);

			uint32_t dock(cstring name, uint32_t id);

			uint32_t dock(gui_form const & w, uint32_t id);

			uint32_t split(uint32_t i, uint32_t id, int32_t dir, float_t ratio, uint32_t * value);

			uint32_t split(uint32_t id, int32_t dir, float_t ratio, uint32_t * value);

			uint32_t split(uint32_t id, int32_t dir, float_t ratio, uint32_t * out, uint32_t * value);
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool load_style(fs::path const & path);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_dockspace() noexcept -> dockspace & { return m_dockspace; }

		ML_NODISCARD auto get_main_menu_bar() noexcept -> menubar & { return m_menubar; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		render_window * const	m_win			; // 
		event_bus * const		m_bus			; // 
		ImGuiContext *			m_ctx			; // 
		menubar					m_menubar		; // 
		dockspace				m_dockspace		; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_IMGUI_CONTEXT_HPP_