#ifndef _ML_BUILTIN_RUNTIME_HPP_
#define _ML_BUILTIN_RUNTIME_HPP_

#include <modus_core/runtime/RuntimeContext.hpp>
#include <modus_core/imgui/ImGuiExt.hpp>

namespace ml
{
	// default backend
	struct ML_CORE_API builtin_runtime final : runtime_context
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit builtin_runtime(runtime_api * api) noexcept;

		~builtin_runtime() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		void initialize(runtime_api * api);

		void finalize();

		void on_enter() override;

		void on_exit() override;

		void on_idle() override;

		void on_event(event && value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		manual<ImGuiContext>	m_imgui		; // imgui context
		ImGuiExt::Dockspace		m_docker	; // imgui dockspace

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_BUILTIN_RUNTIME_HPP_