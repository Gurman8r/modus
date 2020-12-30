#ifndef _ML_SCENE_EDITOR_HPP_
#define _ML_SCENE_EDITOR_HPP_

#include <modus_core/imgui/ImGui.hpp>
#include <modus_core/scene/Scene.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	enum scene_editor_flags_
	{
		scene_editor_flags_none,
	};

	struct ML_NODISCARD scene_editor
	{
		bool				open			{ true };
		std::string_view	title			{ "scene editor" };
		ImGuiWindowFlags_	window_flags	{ ImGuiWindowFlags_None };
		scene_editor_flags_	editor_flags	{ scene_editor_flags_none };
		unown<scene>		context			{};
	};

	ML_CORE_API bool show_scene_editor(scene_editor * p_editor);

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_SCENE_EDITOR_HPP_