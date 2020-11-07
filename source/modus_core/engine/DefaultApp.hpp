#ifndef _ML_DEFAULT_APP_
#define _ML_DEFAULT_APP_

#include <modus_core/engine/Application.hpp>
#include <modus_core/imgui/ImGuiExt.hpp>

namespace ml
{
	// default application
	struct ML_CORE_API default_app final : application
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit default_app(engine_context * const ctx) noexcept;

		~default_app() noexcept override = default;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		using application::on_event;

		void on_enter(engine_context * const ctx);

		void on_exit(engine_context * const ctx);

		void on_idle(engine_context * const ctx);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ds::manual<ImGuiContext>	m_imgui		; // imgui
		ImGuiExt::Dockspace			m_docker	; // dockspace

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_DEFAULT_APP_