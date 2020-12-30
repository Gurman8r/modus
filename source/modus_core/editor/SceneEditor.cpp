#include <modus_core/editor/SceneEditor.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool show_scene_editor(scene_editor * p_editor)
	{
		if (!p_editor) { return false; }

		ImGui_Scope(p_editor);

		ML_defer(&) { ImGui::End(); };

		bool const is_open{ ImGui::Begin(
			p_editor->title.data(),
			&p_editor->open,
			p_editor->window_flags
		) };

		if (is_open)
		{
			if (!p_editor->context.expired())
			{
				auto sp{ p_editor->context.lock() };
			}
			else
			{
			}
		}

		return is_open;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}