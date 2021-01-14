#ifndef _ML_SCENE_EDITOR_HPP_
#define _ML_SCENE_EDITOR_HPP_

#include <modus_core/gui/ImGui.hpp>
#include <modus_core/scene/Components.hpp>

namespace ml
{
	struct ML_NODISCARD ML_CORE_API scene_editor final : non_copyable, trackable
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		scene_editor(ref<scene_tree> const & context = nullptr);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_context() const noexcept -> ref<scene_tree> const & { return m_context; }

		void set_context(ref<scene_tree> const & value) noexcept { m_context = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void draw_hierarchy(ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None);

		void draw_inspector();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void edit_node(ref<tree_node> const & value, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		ref<scene_tree>	m_context	; // 
		ref<tree_node>	m_selected	; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_SCENE_EDITOR_HPP_