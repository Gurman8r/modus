#ifndef _ML_DEFAULT_LOOP_HPP_
#define _ML_DEFAULT_LOOP_HPP_

#include <modus_core/runtime/LoopSystem.hpp>
#include <modus_core/imgui/ImGuiExt.hpp>

namespace ml
{
	// default loop
	struct ML_CORE_API default_loop final : loop_system
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit default_loop(runtime_api * api) noexcept;

		~default_loop() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		void on_process_enter() override;

		void on_process_exit() override;

		void on_process_idle() override;

		void on_event(event && value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		ds::manual<ImGuiContext>	m_imgui		; // imgui context
		ImGuiExt::Dockspace			m_dockspace	; // imgui dockspace

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_DEFAULT_LOOP_HPP_