#ifndef _ML_DEFAULT_LOOP_HPP_
#define _ML_DEFAULT_LOOP_HPP_

#include <modus_core/runtime/MainLoop.hpp>
#include <modus_core/imgui/ImGuiExt.hpp>

namespace ml
{
	// default loop
	struct ML_CORE_API default_loop final : main_loop
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit default_loop(runtime_api * api) noexcept;

		~default_loop() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		void initialize(runtime_api * api);

		void finalize();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_enter() override;

		void on_exit() override;

		void on_idle() override;

		void on_event(event && value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		plugin_manager			m_plugins	; // plugin manager
		manual<ImGuiContext>	m_imgui		; // imgui context
		ImGuiExt::Dockspace		m_dock		; // imgui dockspace

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_DEFAULT_LOOP_HPP_