#ifndef _ML_JSON_EDITOR_HPP_
#define _ML_JSON_EDITOR_HPP_

#include <modus_core/imgui/ImGui.hpp>

namespace ml
{
	struct json_editor_ex
	{
		json j;
	};

	ML_CORE_API bool show_json_editor_ex(json_editor_ex * ex);
}

#endif // !_ML_JSON_EDITOR_HPP_