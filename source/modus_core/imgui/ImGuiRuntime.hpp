#ifndef _ML_IMGUI_RUNTIME_HPP_
#define _ML_IMGUI_RUNTIME_HPP_

#include <modus_core/system/Events.hpp>
#include <modus_core/imgui/ImGui.hpp>

namespace ml
{
	struct event_bus;
	struct render_window;

	struct ML_CORE_API imgui_runtime final : trackable, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		imgui_runtime(memory_manager * mem, render_window * win);

		~imgui_runtime();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool configure(json const & j);

		void new_frame();

		void render_frame();

		template <class ... Args
		> void do_frame(Args && ... args) noexcept
		{
			new_frame();
			if constexpr (0 < sizeof...(args)) {
				std::invoke(ML_forward(args)...);
			}
			render_frame();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool load_style(fs::path const & path);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_imgui() const noexcept -> manual<ImGuiContext> const & { return m_ctx; }

		ML_NODISCARD auto get_window() const noexcept -> render_window * { return m_win; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		manual<ImGuiContext>	m_ctx	; // 
		render_window * const	m_win	; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_IMGUI_RUNTIME_HPP_