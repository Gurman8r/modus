#ifndef _ML_DOCKSPACE_HPP_
#define _ML_DOCKSPACE_HPP_

#include <modus_core/gui/ImGui.hpp>

namespace ml::ImGuiExt
{
	// dockspace callback
	using DockspaceCallback = method<void(struct Dockspace *)>;

	// dockspace
	struct ML_NODISCARD Dockspace
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr ImGuiWindowFlags DefaultWindowFlags // default window flags
		{
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoBackground
		};

		static constexpr ImGuiDockNodeFlags DefaultDockNodeFlags // default dock node flags
		{
			ImGuiDockNodeFlags_AutoHideTabBar |
			ImGuiDockNodeFlags_PassthruCentralNode
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ImGuiID				ID				; // 
		cstring				Title			; // 
		bool				Open			; // 
		ImGuiWindowFlags	WindowFlags		; // 
		ImGuiDockNodeFlags	DockNodeFlags	; // 
		ImGuiWindowClass *	WindowClass		; // 
		float32				Border			; // 
		float32				Rounding		; // 
		ImVec2				Padding			; // 
		float32				Alpha			; // 
		ImVec2				Size			; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		Dockspace(cstring title, bool open = true, ImGuiDockNodeFlags dock_node_flags = DefaultDockNodeFlags, ImGuiWindowClass * window_class = nullptr)
			: ID			{ ImHashStr(title) }
			, Title			{ title }
			, Open			{ open }
			, WindowFlags	{ DefaultWindowFlags }
			, DockNodeFlags	{ dock_node_flags }
			, WindowClass	{ window_class }
			, Border		{ 0.f }
			, Rounding		{ 0.f }
			, Padding		{ 0.f, 0.f }
			, Alpha			{ 0.f }
			, Size			{ 0.f, 0.f }
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static bool IsDockingEnabled() noexcept { return ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable; }

		ML_NODISCARD static bool BeginBuilder(ImGuiID id, ImGuiDockNodeFlags flags = ImGuiDockNodeFlags_None) noexcept
		{
			if (GetNode(id)) { return false; }
			else {
				RemoveNode(id);
				AddNode(id, flags);
				return true;
			}
		}

		static void Finish(ImGuiID id) noexcept { ImGui::DockBuilderFinish(id); }

		static void DockWindow(cstring name, ImGuiID id) noexcept { ImGui::DockBuilderDockWindow(name, id); }

		ML_NODISCARD static ImGuiDockNode * GetNode(ImGuiID id) noexcept { return ImGui::DockBuilderGetNode(id); }

		ML_NODISCARD static ImGuiDockNode * GetCentralNode(ImGuiID id) noexcept { return ImGui::DockBuilderGetCentralNode(id); }

		static ImGuiID AddNode(ImGuiID id, ImGuiDockNodeFlags flags = ImGuiDockNodeFlags_None) noexcept { return ImGui::DockBuilderAddNode(id, flags); }

		static ImGuiID SplitNode(ImGuiID id, ImGuiDir dir, float32 size_ratio_for_node_at_dir, ImGuiID * out_id_at_dir, ImGuiID * out_id_at_opposite_dir)
		{
			return ImGui::DockBuilderSplitNode(id, dir, size_ratio_for_node_at_dir, out_id_at_dir, out_id_at_opposite_dir);
		}

		static void RemoveNode(ImGuiID id) noexcept { ImGui::DockBuilderRemoveNode(id); }

		static void RemoveNodeDockedWindows(ImGuiID id, bool clear_settings_refs = true) noexcept { ImGui::DockBuilderRemoveNodeDockedWindows(id, clear_settings_refs); }

		static void SetNodePos(ImGuiID id, ImVec2 const & pos) noexcept { ImGui::DockBuilderSetNodePos(id, pos); }

		static void SetNodeSize(ImGuiID id, ImVec2 const & size) noexcept { ImGui::DockBuilderSetNodeSize(id, size); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD ImGuiWindow * GetWindow() const noexcept { return ImGui::FindWindowByName(Title); }

		ML_NODISCARD bool BeginBuilder() noexcept { return BeginBuilder(ID, DockNodeFlags); }

		void Finish() noexcept { Finish(ID); }

		ML_NODISCARD ImGuiDockNode * GetNode() const noexcept { return ImGui::DockBuilderGetNode(ID); }

		ML_NODISCARD ImGuiDockNode * GetCentralNode() const noexcept { return ImGui::DockBuilderGetCentralNode(ID); }

		bool Draw(ImGuiViewport * vp, DockspaceCallback const & callback = {})
		{
			ML_assert(vp);
			if (!Open || !IsDockingEnabled()) { return false; }
			ImGui::SetNextWindowPos(vp->Pos);
			ImGui::SetNextWindowSize(vp->Size);
			ImGui::SetNextWindowViewport(vp->ID);
			ImGui::SetNextWindowBgAlpha(Alpha);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, Rounding);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, Border);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Padding);
			bool const is_open{ ImGui::Begin(Title, &Open, WindowFlags) };
			ImGui::PopStyleVar(3);
			if (is_open)
			{
				if (callback) { callback(this); }

				ImGui::DockSpace(ID, Size, DockNodeFlags, WindowClass);
			}
			ImGui::End();
			return is_open;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_DOCKSPACE_HPP_