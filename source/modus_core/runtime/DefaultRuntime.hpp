#ifndef _ML_DEFAULT_RUNTIME_HPP_
#define _ML_DEFAULT_RUNTIME_HPP_

#include <modus_core/runtime/RuntimeContext.hpp>
#include <modus_core/imgui/ImGuiExt.hpp>

namespace ml
{
	// default runtime
	struct ML_CORE_API default_runtime final : runtime_context
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit default_runtime(runtime_api * api) noexcept;

		~default_runtime() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		void initialize(runtime_api * api);

		void on_enter() noexcept override;

		void on_exit() noexcept override;

		void on_idle() override;

		void on_event(event && value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		manual<ImGuiContext>	m_imgui	; // imgui context
		ImGuiExt::Dockspace		m_dock	; // imgui dockspace

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_DEFAULT_RUNTIME_HPP_