#ifndef _ML_IMGUI_RUNTIME_HPP_
#define _ML_IMGUI_RUNTIME_HPP_

#include <modus_core/Export.hpp>
#include <modus_core/system/Events.hpp>
#include <modus_core/imgui/ImGui.hpp>

namespace ml
{
	struct event_bus;
	struct render_window;

	struct ML_CORE_API imgui_runtime final : trackable, non_copyable, event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		imgui_runtime(event_bus * bus, render_window * win, allocator_type alloc = {});

		~imgui_runtime();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_event(event && value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool startup(json const & j);

		bool shutdown();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void new_frame();

		void render_frame();

		template <class ... Args
		> void do_frame(Args && ... args) noexcept
		{
			new_frame();
			if constexpr (0 < sizeof...(args))
			{
				std::invoke(ML_forward(args)...);
			}
			render_frame();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// menubar
		struct ML_CORE_API menubar final : trackable, non_copyable
		{
			bool enabled{ true };

			explicit menubar(allocator_type alloc) noexcept {}

			void configure(json const & j);
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

			void configure(json const & j);

			ML_NODISCARD auto & operator[](size_t i) noexcept { return nodes[i]; }

			ML_NODISCARD auto const & operator[](size_t i) const noexcept { return nodes[i]; }

			uint32_t begin_builder();

			uint32_t end_builder(uint32_t root);

			uint32_t dock(cstring name, uint32_t id);

			uint32_t split(uint32_t i, uint32_t id, int32_t dir, float_t ratio, uint32_t * value);

			uint32_t split(uint32_t id, int32_t dir, float_t ratio, uint32_t * value);

			uint32_t split(uint32_t id, int32_t dir, float_t ratio, uint32_t * out, uint32_t * value);
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool load_style(fs::path const & path);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_dockspace() noexcept -> dockspace & { return m_dockspace; }

		ML_NODISCARD auto get_menubar() noexcept -> menubar & { return m_menubar; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		render_window * const	m_win			; // 
		ImGuiContext *			m_ctx			; // 
		menubar					m_menubar		; // 
		dockspace				m_dockspace		; // 
		bool					m_running		; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_IMGUI_RUNTIME_HPP_