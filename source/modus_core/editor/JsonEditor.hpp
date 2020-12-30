#ifndef _ML_JSON_EDITOR_HPP_
#define _ML_JSON_EDITOR_HPP_

#include <modus_core/imgui/ImGui.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	enum json_editor_flags_ : int32
	{
		json_editor_flags_none,
	};

	enum json_object_flags_ : int32
	{
		json_object_flags_none,
	};

	enum json_array_flags_ : int32
	{
		json_array_flags_none,
	};

	enum json_float_flags_ : int32
	{
		json_float_flags_none,
	};

	enum json_integer_flags_ : int32
	{
		json_integer_flags_none,
	};

	enum json_unsigned_flags_ : int32
	{
		json_unsigned_flags_none,
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct ML_NODISCARD json_editor
	{
		cstring	label			{ "json" };
		json *	context			{};
		int32	object_flags	{ json_object_flags_none };
		int32	array_flags		{ json_array_flags_none };
		int32	editor_flags	{ json_editor_flags_none };
		int32	float_flags		{ json_float_flags_none };
		int32	integer_flags	{ json_integer_flags_none };
		int32	unsigned_flags	{ json_unsigned_flags_none };
	};

	ML_CORE_API bool show_json_editor(json_editor * p_editor);

	ML_CORE_API bool show_json_editor(
		cstring				title,
		json_editor *		p_editor,
		bool *				p_open = NULL,
		ImGuiWindowFlags	window_flags = ImGuiWindowFlags_None);

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_JSON_EDITOR_HPP_