#ifndef _ML_SANDBOX_HPP_
#define _ML_SANDBOX_HPP_

#include <modus_core/detail/FileUtility.hpp>
#include <modus_core/detail/StreamSniper.hpp>
#include <modus_core/graphics/Material.hpp>
#include <modus_core/graphics/Mesh.hpp>
#include <modus_core/imgui/ImGuiExt.hpp>
#include <modus_core/runtime/Application.hpp>
#include <modus_core/scene/Components.hpp>

#include <modus_core/events/RuntimeEvents.hpp>
#include <modus_core/events/ImGuiEvents.hpp>
#include <modus_core/events/InputEvents.hpp>
#include <modus_core/events/SceneEvents.hpp>
#include <modus_core/events/WindowEvents.hpp>

namespace ml::Widgets
{
	template <class ID = ImTextureID
	> void Image(ID const & tex, ImRect const & bb, ImVec2 const & uv0 = { 0, 1 }, ImVec2 const & uv1 = { 1, 0 }, uint32 tint = 0xffffffff)
	{
		if constexpr (std::is_scalar_v<std::decay_t<decltype(tex)>>)
		{
			ImGuiContext * const ctx{ ImGui::GetCurrentContext() };
			ImGuiWindow * const window{ ctx->CurrentWindow };
			if (!window || window->SkipItems) { return; }
			ImGui::ItemSize(bb);
			if (!ImGui::ItemAdd(bb, 0)) { return; }
			window->DrawList->AddImage(tex, bb.Min, bb.Max, uv0, uv1, tint);
		}
		else
		{
			Widgets::Image((ImTextureID)(tex ? tex->get_handle() : NULL), bb, uv0, uv1, tint);
		}
	}
}

#endif // !_ML_SANDBOX_HPP_