#ifndef _ML_SCENE_EDITOR_HPP_
#define _ML_SCENE_EDITOR_HPP_

#include <modus_core/gui/ImGui.hpp>
#include <modus_core/scene/Scene.hpp>

namespace ml
{
	struct ML_NODISCARD ML_CORE_API scene_editor final
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		scene_editor(ref<scene> const & context = nullptr);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void draw_hierarchy();

		void draw_inspector();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_context() const noexcept -> ref<scene> const & { return m_context; }

		void set_context(ref<scene> const & value) noexcept { m_context = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	private:
		ref<scene>	m_context	; // 
		ref<scene>	m_selected	; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_SCENE_EDITOR_HPP_