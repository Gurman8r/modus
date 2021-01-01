#ifndef _ML_IMGUI_EXT_HPP_
#define _ML_IMGUI_EXT_HPP_

// FIXME: system needs a rework

#include <modus_core/detail/Method.hpp>
#include <modus_core/gui/ImGui.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// TOOLTIPS
namespace ml::ImGuiExt
{
	// TOOLTIP-EX
	template <class Fn, class ... Args
	> void TooltipEx(Fn && fn, Args && ... args) noexcept
	{
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			std::invoke(ML_forward(fn), ML_forward(args)...);
			ImGui::EndTooltip();
		}
	}

	// TOOLTIP
	inline void Tooltip(cstring first, cstring last = NULL) noexcept
	{
		TooltipEx([first, last]() noexcept
		{
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(first, last);
			ImGui::PopTextWrapPos();
		});
	}

	// HELP MARKER
	inline void HelpMarker(cstring first, cstring last = NULL) noexcept
	{
		ImGui::TextDisabled("(?)");
		Tooltip(first, last);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// DOCKSPACE
namespace ml::ImGuiExt
{
	// dockspace
	struct ML_NODISCARD Dockspace
	{
		static constexpr ImGuiWindowFlags DefaultWindowFlags
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

		static constexpr ImGuiDockNodeFlags DefaultDockNodeFlags
		{
			ImGuiDockNodeFlags_AutoHideTabBar
		};

		cstring		Title			{ "##MainDockspace" }	; // title
		bool		IsOpen			{ true }				; // is open
		int32		WindowFlags		{ DefaultWindowFlags }	; // window flags
		int32		DockNodeFlags	{ DefaultDockNodeFlags }; // dock node flags
		float32		Border			{ 0.0f }				; // border
		float32		Rounding		{ 0.0f }				; // rounding
		vec2		Padding			{ 0.0f, 0.0f }			; // padding
		float32		Alpha			{ 0.0f }				; // alpha
		vec2		Size			{ 0.0f, 0.0f }			; // size

		ML_NODISCARD static bool IsDockingEnabled() { return ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable; }

		ML_NODISCARD ImGuiID GetID() const { return ImGui::GetID(Title); }

		ML_NODISCARD ImGuiWindow * GetWindow() const { return ImGui::FindWindowByName(Title); }

		void DockWindow(cstring name, ImGuiID id) noexcept { ImGui::DockBuilderDockWindow(name, id); }

		ML_NODISCARD ImGuiDockNode * GetNode() const noexcept { return ImGui::DockBuilderGetNode(GetID()); }

		ML_NODISCARD ImGuiDockNode * GetNode(ImGuiID id) const noexcept { return ImGui::DockBuilderGetNode(id); }

		ML_NODISCARD ImGuiDockNode * GetCentralNode() const noexcept { return ImGui::DockBuilderGetCentralNode(GetID()); }

		ML_NODISCARD ImGuiDockNode * GetCentralNode(ImGuiID id) const noexcept { return ImGui::DockBuilderGetCentralNode(id); }

		ImGuiID AddNode(ImGuiID id, ImGuiDockNodeFlags flags = ImGuiDockNodeFlags_None) noexcept { return ImGui::DockBuilderAddNode(id, flags); }

		ImGuiID SplitNode(ImGuiID id, ImGuiDir dir, float32 ratio, ImGuiID * out_id_at_dir, ImGuiID * out_id_at_opposite_dir) noexcept
		{
			return ImGui::DockBuilderSplitNode(id, dir, ratio, out_id_at_dir, out_id_at_opposite_dir);
		}

		void RemoveNode(ImGuiID id) noexcept { ImGui::DockBuilderRemoveNode(id); }

		void RemoveNodeDockedWindows(ImGuiID id, bool clear_settings_refs = true) { ImGui::DockBuilderRemoveNodeDockedWindows(id, clear_settings_refs); }

		void SetNodePos(ImGuiID id, vec2 const & pos) const noexcept { ImGui::DockBuilderSetNodePos(id, pos); }

		void SetNodeSize(ImGuiID id, vec2 const & size) const noexcept { ImGui::DockBuilderSetNodeSize(id, size); }

		void Finish(ImGuiID id) noexcept { ImGui::DockBuilderFinish(id); }

		template <class Fn, class ... Args
		> bool Draw(ImGuiViewport * vp, Fn && fn, Args && ... args) noexcept
		{
			if (!IsOpen || !IsDockingEnabled()) { return false; }

			if (!vp) { ML_verify(vp = ImGui::GetMainViewport()); }

			ImGui_Scope(this);
			ImGui::SetNextWindowPos(vp->Pos);
			ImGui::SetNextWindowSize(vp->Size);
			ImGui::SetNextWindowViewport(vp->ID);
			ImGui::SetNextWindowBgAlpha(Alpha);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, Rounding);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, Border);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Padding);
			bool const is_open{ ImGui::Begin(Title, &IsOpen, WindowFlags) };
			ImGui::PopStyleVar(3);
			if (is_open)
			{
				std::invoke(ML_forward(fn), this, ML_forward(args)...);

				ImGui::DockSpace(
					GetID(),
					Size,
					ImGuiDockNodeFlags_PassthruCentralNode | DockNodeFlags,
					nullptr);
			}
			ImGui::End();
			return is_open;
		}
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// TEXT LOG
namespace ml::ImGuiExt
{
	// text log
	struct ML_CORE_API ML_NODISCARD TextLog final : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		using Line = typename ds::string;

		using LineBuffer = typename ds::list<Line>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using PrinterSignature = typename void(LineBuffer const &, size_t);

		struct ML_CORE_API ML_NODISCARD Printer final : ds::method< PrinterSignature >
		{
			using ds::method< PrinterSignature >::method;

			static Printer Default;

			Printer() noexcept : Printer{ Default } {}
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ImGuiTextFilter	Filter			; // filter
		LineBuffer		Lines			; // lines
		bool			AutoScroll		; // auto scroll
		bool			ScrollToBottom	; // scroll to bottom

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit TextLog(allocator_type alloc) noexcept : TextLog{ "", true, alloc } {}

		TextLog(cstring default_filter = "", bool auto_scroll = true, allocator_type alloc = {}) noexcept
			: Filter		{ default_filter }
			, Lines			{ alloc }
			, AutoScroll	{ auto_scroll }
			, ScrollToBottom{}
		{
		}

		void Draw(Printer const & print = {}) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto & Write(char value) noexcept
		{
			switch (value)
			{
			default: { // default
				if (Lines.empty()) { Lines.push_back({}); }
				Lines.back().push_back(value);
			} break;

			case '\n': { // new line
				Lines.push_back({});
			} break;
			}
			return (*this);
		}

		auto & Print(cstring value = "\n") noexcept
		{
			std::for_each_n(value, std::strlen(value), [&](char c) noexcept
			{
				this->Write(c);
			});
			return (*this);
		}

		auto & Print(Line const & value = "\n") noexcept
		{
			for (char c : value)
			{
				this->Write(c);
			}
			return (*this);
		}

		template <class ... Args
		> auto & Printf(cstring str, Args && ... args) noexcept
		{
			ds::array<char, 256> buf{};
			std::sprintf(buf, str, ML_forward(args)...);
			return this->Print(buf.data());
		}

		template <class ... Args
		> auto & Printf(Line const & str, Args && ... args) noexcept
		{
			return this->Printf(str.c_str(), ML_forward(args)...);
		}

		auto & operator<<(char value) noexcept { return this->Write(value); }

		auto & operator<<(cstring value) noexcept { return this->Print(value); }

		auto & operator<<(Line const & value) noexcept { return this->Print(value); }

		template <class T> auto & operator<<(T && value) noexcept
		{
			ds::stringstream ss{};
			ss << ML_forward(value);
			return this->Dump(ss);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// TERMINAL
namespace ml::ImGuiExt
{
	struct ML_CORE_API ML_NODISCARD Terminal final : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		using InputBuffer = typename ds::array<char, 256>;
		
		using Line = typename TextLog::Line;
		
		using Printer = typename TextLog::Printer;

		using CommandProc = typename ds::method< void(Line &&) >;
		
		using CommandInfo = typename ds::list<ds::string>;

		using CommandStorage = typename ds::batch_vector
		<
			ds::string, CommandInfo, CommandProc
		>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ds::string
			UserName,
			HostName,
			PathName,
			ModeName;

		InputBuffer Input; // input

		TextLog Output; // output

		CommandStorage CommandData; // commands

		ds::list<Line> History; // history

		int32 HistoryPos; // history pos

		// colors
		struct ML_NODISCARD {
			color
				Delimeter	{ colors::white },
				UserName	{ colors::aqua },
				HostName	{ colors::magenta },
				PathName	{ colors::cyan },
				ModeName	{ colors::fuchsia };
		} Colors;

		Terminal(allocator_type alloc = {}) noexcept
			: UserName		{ alloc }
			, HostName		{ alloc }
			, PathName		{ alloc }
			, ModeName		{ alloc }
			, Input			{}
			, Output		{ alloc }
			, CommandData	{ alloc }
			, History		{ alloc }
			, HistoryPos	{ -1 }
			, Colors		{}
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void DrawPrefixOptions();

		void DrawPrefix();
		
		void DrawInput();

		int32 Execute(Line line);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Name = ds::string
		> bool AddCommand(Name && name, CommandInfo const & info, CommandProc const & clbk) noexcept
		{
			if (this->HasCommand(ML_forward(name))) { return false; }
			else { CommandData.push_back(ML_forward(name), info, clbk); return true; }
		}

		template <class Name = ds::string
		> bool DelCommand(Name && name) noexcept
		{
			if (auto const i{ this->GetIndex(ML_forward(name)) }; !i) { return false; }
			else { CommandData.erase(*i); return true; }
		}

		template <class Name = ds::string
		> ML_NODISCARD bool HasCommand(Name && name) const noexcept
		{
			return CommandData.contains<ds::string>(ML_forward(name));
		}

		template <class Name = ds::string
		> ML_NODISCARD std::optional<size_t> GetIndex(Name && name) const noexcept
		{
			if (auto const i{ CommandData.lookup<ds::string>(ML_forward(name)) }
			; i == CommandData.npos) { return std::nullopt; }
			else { return i; }
		}

		template <class Name = ds::string
		> ML_NODISCARD CommandInfo * GetInfo(Name && name) noexcept
		{
			if (auto const i{ this->GetIndex(ML_forward(name)) }; !i) { return nullptr; }
			else { return std::addressof(CommandData.at<CommandInfo>(*i)); }
		}

		template <class Name = ds::string
		> ML_NODISCARD CommandProc * GetProc(Name && name) noexcept
		{
			if (auto const i{ this->GetIndex(ML_forward(name)) }; !i) { return nullptr; }
			else { return std::addressof(CommandData.at<CommandProc>(*i)); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		int32 InputTextCallbackStub(ImGuiInputTextCallbackData * data);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// TRANSFORM
namespace ml::ImGuiExt
{
	inline bool EditVec3(cstring label, float32 * value, float32 spd = 0.001f, float32 min = 0.f, float32 max = 0.f, cstring fmt = "%.3f", float32 reset_value = 0.f, float32 column_width = 100.f)
	{
		ImGui_Scope(label);
		bool dirty{};
		ImGuiIO & io{ ImGui::GetIO() };

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, column_width);
		ImGui::Text(label);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

		auto line_height{ ImGui::GetFontSize() + ImGui::GetStyle().FramePadding[1] * 2.0f };
		vec2 button_size{ line_height + 3.0f, line_height };

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, .1f, 0.15f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.9f, 0.2f, 0.2f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f, .1f, 0.15f, 1.f });
		if (ImGui::Button("X", button_size)) { value[0] = reset_value; }
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		dirty |= ImGui::DragFloat("##X", &value[0], spd, min, max, fmt);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.7f, 0.2f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.3f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.2f, 0.7f, 0.2f, 1.f });
		if (ImGui::Button("Y", button_size)) { value[1] = reset_value; }
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		dirty |= ImGui::DragFloat("##Y", &value[1], spd, min, max, fmt);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, { .1f, 0.25f, 0.8f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.35f, 0.9f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { .1f, 0.25f, 0.8f, 1.f });
		if (ImGui::Button("Z", button_size)) { value[2] = reset_value; }
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		dirty |= ImGui::DragFloat("##Z", &value[2], spd, min, max, fmt);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		return dirty;
	}

	inline bool EditTransformMatrix(float32 * value, cstring labels = "tr\0rt\0sc", float32 spd = 0.001f, float32 min = 0.f, float32 max = 0.f, cstring fmt = "%.3f", float32 reset_value = 0.f, float32 column_width = 100.f)
	{
		if (!labels || !*labels) { labels = "tr\0rt\0sc"; } // default labels

		bool dirty{};
		vec3 t, r, s;
		ImGuizmo::DecomposeMatrixToComponents(value, t, r, s);
		dirty |= ImGuiExt::EditVec3(util::single_str(labels, 0), t, spd, min, max, fmt, reset_value, column_width);
		dirty |= ImGuiExt::EditVec3(util::single_str(labels, 1), r, spd, min, max, fmt, reset_value, column_width);
		dirty |= ImGuiExt::EditVec3(util::single_str(labels, 2), s, spd, min, max, fmt, reset_value, column_width);
		ImGuizmo::RecomposeMatrixFromComponents(t, r, s, value);
		return dirty;
	}

	struct ML_NODISCARD TransformEditor final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ImGuizmo::OPERATION	Operation		{ ImGuizmo::TRANSLATE };
		ImGuizmo::MODE		Mode			{ ImGuizmo::LOCAL };
		bool				UseSnap			{ false };
		vec3				Snap			{ 1.f, 1.f, 1.f };
		vec3				Bounds[2]		{ { -.5f, -.5f, -.5f }, { .5f, .5f, .5f } };
		vec3				BoundsSnap		{ .1f, .1f, .1f };
		bool				BoundSizing		{ false };
		bool				BoundSizingSnap	{ false };

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool Manipulate(float32 const * view, float32 const * proj, float32 * value, float32 * delta = {})
		{
			ImGui_Scope(this);
			return ImGuizmo::Manipulate
			(
				view,
				proj,
				Operation,
				Mode,
				value,
				delta,
				UseSnap ? (float32 *)Snap : nullptr,
				BoundSizing ? (float32 *)Bounds : nullptr,
				BoundSizingSnap ? (float32 *)BoundsSnap : nullptr
			);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void ShowOperationControls()
		{
			ImGui_Scope(this);
			ImGui::BeginGroup();
			if (ImGui::RadioButton("translate", Operation == ImGuizmo::TRANSLATE)) { Operation = ImGuizmo::TRANSLATE; }
			ImGui::SameLine();
			if (ImGui::RadioButton("rotate", Operation == ImGuizmo::ROTATE)) { Operation = ImGuizmo::ROTATE; }
			ImGui::SameLine();
			if (ImGui::RadioButton("scale", Operation == ImGuizmo::SCALE)) { Operation = ImGuizmo::SCALE; }
			ImGui::EndGroup();
		}

		void ShowModeControls()
		{
			ImGui_Scope(this);
			bool const is_scale{ Operation == ImGuizmo::SCALE };
			ImGui::BeginGroup();
			if (ImGui::RadioButton("local", !is_scale && Mode == ImGuizmo::LOCAL)) { Mode = ImGuizmo::LOCAL; }
			ImGui::SameLine();
			if (ImGui::RadioButton("world", !is_scale && Mode == ImGuizmo::WORLD)) { Mode = ImGuizmo::WORLD; }
			ImGui::EndGroup();
		}

		void ShowSnapControls(float32 speed = 0.01f)
		{
			ImGui_Scope(this);
			ImGui::BeginGroup();
			ImGui::Checkbox("##usesnap", &UseSnap); ImGui::SameLine();
			switch (Operation)
			{
			case ImGuizmo::TRANSLATE: ImGui::DragFloat3("snap##translate", &Snap[0], speed); break;
			case ImGuizmo::ROTATE: ImGui::DragFloat("snap##rotate", &Snap[0], speed); break;
			case ImGuizmo::SCALE: ImGui::DragFloat("snap##scale", &Snap[0], speed); break;
			}
			ImGui::EndGroup();
		}

		void ShowBoundsControls(float32 speed = 0.01f)
		{
			ImGui_Scope(this);
			ImGui::BeginGroup();
			if (ImGui::Checkbox("bound sizing", &BoundSizing); BoundSizing)
			{
				ImGui::Checkbox("##boundsizingsnap", &BoundSizingSnap); ImGui::SameLine();
				ImGui::DragFloat3("snap##boundsizing", BoundsSnap, speed, 0.f, 0.f, "%.2f");
			}
			ImGui::EndGroup();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_IMGUI_EXT_HPP_