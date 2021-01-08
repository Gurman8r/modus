#include <modus_core/editor/SceneEditor.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	scene_editor::scene_editor(ref<scene_tree> const & context)
		: m_context{ context }
		, m_selected{ nullptr }
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void scene_editor::draw_hierarchy()
	{
	}

	void scene_editor::draw_inspector()
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}