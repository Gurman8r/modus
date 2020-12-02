#ifndef _ML_IMGUI_EXT_HPP_
#define _ML_IMGUI_EXT_HPP_

#include <modus_core/detail/Method.hpp>
#include <modus_core/imgui/ImGui.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// SCOPE ID
namespace ml::ImGuiExt::impl
{
	// scoped imgui ID
	struct ML_NODISCARD ImplScopeID final
	{
		template <class ... Args
		> ImplScopeID(Args && ... args) noexcept { ImGui::PushID(ML_forward(args)...); }

		~ImplScopeID() noexcept { ImGui::PopID(); }
	};
}

// Scope ID
#define ImGuiExt_ScopeID(...) \
	auto ML_anon = _ML ImGuiExt::impl::ImplScopeID{ ##__VA_ARGS__ }

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// TOOLTIPS
namespace ml::ImGuiExt
{
	// TOOLTIP-EX
	template <class Fn, class ... Args
	> void TooltipEx(Fn && fn, Args && ... args) noexcept
	{
		if (!ImGui::IsItemHovered()) { return; }
		ImGui::BeginTooltip();
		std::invoke(ML_forward(fn), ML_forward(args)...);
		ImGui::EndTooltip();
	}

	// TOOLTIP
	inline void Tooltip(cstring first, cstring last = {}) noexcept
	{
		TooltipEx([first, last]() noexcept
		{
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(first, last);
			ImGui::PopTextWrapPos();
		});
	}

	// HELP MARKER
	inline void HelpMarker(cstring first, cstring last = nullptr) noexcept
	{
		ImGui::TextDisabled("(?)");
		Tooltip(first, last);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// HELPERS
namespace ml::ImGuiExt
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// BEGIN/END
	template <class BeginFn, class EndFn, class Fn, class ... Args
	> bool BeginEnd(BeginFn && begin_fn, EndFn && end_fn, Fn && fn, Args && ... args) noexcept
	{
		bool const is_open{ std::invoke(ML_forward(begin_fn)) };
		
		ML_defer(&) { std::invoke(ML_forward(end_fn)); };
		
		if (is_open) { std::invoke(ML_forward(fn), ML_forward(args)...); }
		
		return is_open;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// WINDOW
	template <class Fn, class ... Args
	> bool Window(cstring title, bool * open, int32 flags, Fn && fn, Args && ... args) noexcept
	{
		return ImGuiExt::BeginEnd(
			std::bind(&ImGui::Begin, title, open, flags),
			&ImGui::End,
			ML_forward(fn), ML_forward(args)...);
	}

	// CHILD WINDOW EX
	template <class Fn, class ... Args
	> bool ChildWindowEx(cstring name, ImGuiID id, vec2 const & size, bool border, int32 flags, Fn && fn, Args && ... args) noexcept
	{
		return ImGuiExt::BeginEnd(
			std::bind(&ImGui::BeginChildEx, name, id, size, border, flags),
			&ImGui::EndChild,
			ML_forward(fn), ML_forward(args)...);
	}

	// CHILD WINDOW
	template <class Fn, class ... Args
	> bool ChildWindow(cstring id, vec2 const & size, bool border, int32 flags, Fn && fn, Args && ... args) noexcept
	{
		return ImGuiExt::ChildWindowEx
		(
			id, ImGui::GetID(id), size, border, flags, ML_forward(fn), ML_forward(args)...
		);
	}

	// CHILD WINDOW
	template <class Fn, class ... Args
	> bool ChildWindow(ImGuiID id, vec2 const & size, bool border, int32 flags, Fn && fn, Args && ... args) noexcept
	{
		return ImGuiExt::ChildWindowEx
		(
			nullptr, id, size, border, flags, ML_forward(fn), ML_forward(args)...
		);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class T
	> ML_NODISCARD cstring GetName(ds::basic_string<T> const & value) noexcept
	{
		return value.c_str();
	}

	template <class T
	> ML_NODISCARD constexpr cstring GetName(T const * value) noexcept
	{
		if constexpr (util::is_char_v<T>)
		{
			return value;
		}
		else
		{
			return value->Title;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_NODISCARD constexpr ImGuiID GetID(ImGuiID value) noexcept
	{
		return value;
	}

	template <class T
	> ML_NODISCARD ImGuiID GetID(ds::basic_string<T> const & value) noexcept
	{
		return ImGui::GetID(value.c_str());
	}

	template <class T
	> ML_NODISCARD ImGuiID GetID(T const * value) noexcept
	{
		if constexpr (util::is_char_v<T>)
		{
			return ImGui::GetID(value);
		}
		else
		{
			return ImGui::GetID(value->Title);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class T
	> ML_NODISCARD ImGuiWindow * FindWindowByName(T const * p)
	{
		return ImGui::FindWindowByName(p->Title);
	}

	template <class T
	> bool MenuItem(T * p, cstring shortcut = {}, bool enabled = true)
	{
		return ImGui::MenuItem(p->Title, shortcut, &p->IsOpen, enabled);
	}

	template <class T
	> bool Selectable(T * p, int32 flags = ImGuiSelectableFlags_None, vec2 const & size = {})
	{
		return ImGui::Selectable(p->Title, &p->IsOpen, flags, size);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// PANELS
namespace ml::ImGuiExt
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Derived> struct ML_NODISCARD BasicPanel
	{
		cstring		Title		; // title
		bool		IsOpen		; // is open
		int32		WindowFlags	; // window flags

		BasicPanel(cstring title, bool open = false, int32 winflags = ImGuiWindowFlags_None) noexcept
			: Title			{ title }
			, IsOpen		{ open }
			, WindowFlags	{ winflags }
		{
		}

		BasicPanel(BasicPanel const & other) noexcept
			: BasicPanel{ other.Title, other.IsOpen, other.WindowFlags }
		{
		}

		bool Begin() noexcept {
			return IsOpen && ImGui::Begin(Title, &IsOpen, WindowFlags);
		}

		void End() noexcept {
			ImGui::End();
		}

		ML_NODISCARD bool GetWindowFlag(ImGuiWindowFlags_ index) const noexcept {
			return ML_flag_read(WindowFlags, (int32)index);
		}

		auto SetWindowFlag(ImGuiWindowFlags_ index, bool value) noexcept {
			return ML_flag_write(WindowFlags, (int32)index, value);
		}

		ML_NODISCARD ImGuiID GetID() const noexcept {
			return ImGui::GetID(Title);
		}

		ML_NODISCARD ImGuiWindow * FindByName() const noexcept {
			return ImGui::FindWindowByName(Title);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// PANEL
	struct ML_NODISCARD Panel : BasicPanel<Panel>
	{
		Panel(cstring title, bool open = false, int32 winflags = ImGuiWindowFlags_None) noexcept
			: BasicPanel{ title, open, winflags }
		{
		}

		Panel(Panel const & other) noexcept : BasicPanel{ other } {}

		template <class Fn, class ... Args
		> bool operator()(Fn && fn, Args && ... args) noexcept
		{
			if (!IsOpen) { return false; }
			ImGuiExt_ScopeID(this);
			return ImGuiExt::BeginEnd
			(
				std::bind(&Panel::Begin, this),
				std::bind(&Panel::End, this),
				ML_forward(fn), this, ML_forward(args)...
			);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// MAIN MENU BAR
namespace ml::ImGuiExt
{
	// main menu bar (WIP)
	struct ML_NODISCARD MainMenuBar final
	{
		static constexpr cstring Title{ "##MainMenuBar" };

		bool IsOpen{ true };

		void Configure(json const & j)
		{
		}

		ML_NODISCARD ImGuiID GetID() const noexcept {
			return ImGui::GetID(Title);
		}

		ML_NODISCARD ImGuiWindow * FindByName() const noexcept {
			return ImGui::FindWindowByName(Title);
		}

		template <class Fn, class ... Args
		> bool operator()(Fn && fn, Args && ... args) noexcept
		{
			return IsOpen && ImGuiExt::BeginEnd
			(
				&ImGui::BeginMainMenuBar,
				&ImGui::EndMainMenuBar,
				ML_forward(fn), this, ML_forward(args)...
			);
		}
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// DOCKSPACE
namespace ml::ImGuiExt
{
	// dockspace
	struct ML_NODISCARD Dockspace : BasicPanel<Dockspace>
	{
		float32		Border			; // 
		float32		Rounding		; // 
		vec2		Padding			; // 
		float32		Alpha			; // 
		vec2		Size			; // 
		int32		DockNodeFlags	; // 

		static constexpr auto DefaultWindowFlags
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

		Dockspace(
			cstring			title		= "##MainDockspace",
			bool			open		= true,
			float32			border		= {},
			float32			rounding	= {},
			vec2 const &	padding		= {},
			float32			alpha		= {},
			vec2 const &	docksize	= {},
			int32			winflags	= DefaultWindowFlags,
			int32			dockflags	= ImGuiDockNodeFlags_AutoHideTabBar)
			: BasicPanel	{ title, open, winflags }
			, Border		{ border }
			, Rounding		{ rounding }
			, Padding		{ padding }
			, Alpha			{ alpha }
			, Size			{ docksize }
			, DockNodeFlags	{ dockflags }
		{}

		Dockspace(Dockspace const & other) noexcept : Dockspace{
			other.Title,
			other.IsOpen,
			other.Border,
			other.Rounding,
			other.Padding,
			other.Alpha,
			other.Size,
			other.WindowFlags,
			other.DockNodeFlags
		}
		{}

		void Configure(json const & j)
		{
			util::get_from(j, "alpha"	, Alpha);
			util::get_from(j, "border"	, Border);
			util::get_from(j, "padding"	, Padding);
			util::get_from(j, "rounding", Rounding);
			util::get_from(j, "size"	, Size);
		}

		ML_NODISCARD static bool IsDockingEnabled(ImGuiIO & io = ImGui::GetIO()) noexcept {
			return io.ConfigFlags & ImGuiConfigFlags_DockingEnable;
		}

		ML_NODISCARD bool GetDockNodeFlag(ImGuiDockNodeFlags_ index) const noexcept {
			return ML_flag_read(DockNodeFlags, (int32)index);
		}

		auto SetDockNodeFlag(ImGuiDockNodeFlags_ index, bool value) noexcept {
			return (ImGuiDockNodeFlags_)ML_flag_write(DockNodeFlags, (int32)index, value);
		}

		template <class T, class ID
		> void DockWindow(T && name, ID && id) noexcept
		{
			ImGui::DockBuilderDockWindow(
				ImGuiExt::GetName(ML_forward(name)),
				ImGuiExt::GetID(ML_forward(id)));
		}

		ML_NODISCARD ImGuiDockNode * GetNode() const noexcept {
			return ImGui::DockBuilderGetNode(GetID());
		}

		template <class ID
		> ML_NODISCARD ImGuiDockNode * GetNode(ID && id) const noexcept {
			return ImGui::DockBuilderGetNode(ImGuiExt::GetID(ML_forward(id)));
		}

		ML_NODISCARD ImGuiDockNode * GetCentralNode() const noexcept {
			return ImGui::DockBuilderGetCentralNode(GetID());
		}

		template <class ID
		> ML_NODISCARD ImGuiDockNode * GetCentralNode(ID && id) const noexcept {
			return ImGui::DockBuilderGetCentralNode(ImGuiExt::GetID(ML_forward(id)));
		}

		template <class ID
		> ImGuiID AddNode(ID && id, ImGuiDockNodeFlags flags = 0) noexcept {
			return ImGui::DockBuilderAddNode(ImGuiExt::GetID(ML_forward(id)), flags);
		}

		template <class ID
		> void RemoveNode(ID && id) noexcept {
			ImGui::DockBuilderRemoveNode(ImGuiExt::GetID(ML_forward(id)));
		}

		template <class ID
		> void SetNodePos(ID && id, vec2 const & pos) const noexcept {
			return ImGui::DockBuilderSetNodePos(ImGuiExt::GetID(ML_forward(id)), pos);
		}

		template <class ID
		> void SetNodeSize(ID && id, vec2 const & size) const noexcept {
			return ImGui::DockBuilderSetNodeSize(ImGuiExt::GetID(ML_forward(id)), size);
		}

		template <class ID
		> ImGuiID SplitNode(ID && id, ImGuiDir split_dir, float32 size_ratio_for_node_at_dir, ImGuiID * out_id_at_dir, ImGuiID * out_id_at_opposite_dir) noexcept {
			ImGui::DockBuilderSplitNode(ImGuiExt::GetID(ML_forward(id)), split_dir, out_id_at_dir, out_id_at_opposite_dir);
		}

		template <class ID
		> void Finish(ID && id) noexcept
		{
			ImGui::DockBuilderFinish(ImGuiExt::GetID(ML_forward(id)));
		}

		void Finish() noexcept
		{
			ImGui::DockBuilderFinish(GetID());
		}

		template <class Fn, class ... Args
		> bool operator()(ImGuiViewport const * vp, Fn && fn, Args && ... args) noexcept
		{
			if (!IsOpen || !vp || !IsDockingEnabled()) { return false; }

			ImGuiExt_ScopeID(this);
			ImGui::SetNextWindowPos(vp->Pos);
			ImGui::SetNextWindowSize(vp->Size);
			ImGui::SetNextWindowViewport(vp->ID);
			ImGui::SetNextWindowBgAlpha(Alpha);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, Rounding);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, Border);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Padding);
			
			bool const is_open{ Begin() }; ML_defer(&) { End(); };
			ImGui::PopStyleVar(3);
			if (is_open)
			{
				ImGuiID const id{ GetID() };

				if (!GetNode(id))
				{
					RemoveNode(id);
					
					AddNode(id, DockNodeFlags);

					std::invoke(ML_forward(fn), this, ML_forward(args)...);
					
					Finish(id);
				}

				ImGui::DockSpace(
					id,
					Size,
					ImGuiDockNodeFlags_PassthruCentralNode | DockNodeFlags,
					nullptr);
			}
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

		explicit TextLog(allocator_type alloc) noexcept : TextLog{ NULL, true, alloc } {}

		TextLog(cstring default_filter = "", bool auto_scroll = true, allocator_type alloc = {}) noexcept;

		void Draw(Printer const & print = {}) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto & Write(char const value) noexcept
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
			std::for_each_n(value, std::strlen(value), [&](char c)
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

		auto & Dump(ds::stringstream & ss) noexcept
		{
			this->Print(ss.str());
			ss.str({});
			return (*this);
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

		using CommandName = typename ds::string;
		
		using CommandProc = typename ds::method< void(Line &&) >;
		
		using CommandInfo = typename ds::list<ds::string>;

		using CommandData = typename ds::batch_vector
		<
			CommandName, CommandInfo, CommandProc
		>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ds::string User, Host, Path, Mode; // session

		InputBuffer Input; // input

		TextLog Output; // output

		CommandData Commands; // commands

		ds::list<Line> History; // history

		int32 HistoryPos; // history pos

		// colors
		struct ML_NODISCARD {
			color
				Delim	{ colors::white },
				User	{ colors::aqua },
				Host	{ colors::magenta },
				Path	{ colors::cyan },
				Mode	{ colors::fuchsia };
		} Colors;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		Terminal(allocator_type alloc = {}) noexcept;

		void DrawPrefixOptions();

		void DrawPrefix();
		
		void DrawInput();

		int32 Execute(Line line);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Name = CommandName
		> bool AddCommand(Name && name, CommandInfo const & info, CommandProc const & clbk) noexcept
		{
			if (this->HasCommand(ML_forward(name))) { return false; }
			else { Commands.push_back(ML_forward(name), info, clbk); return true; }
		}

		template <class Name = CommandName
		> bool DelCommand(Name && name) noexcept
		{
			if (auto const i{ this->GetIndex(ML_forward(name)) }; !i) { return false; }
			else { Commands.erase(*i); return true; }
		}

		template <class Name = CommandName
		> ML_NODISCARD bool HasCommand(Name && name) const noexcept
		{
			return Commands.contains<ds::string>(ML_forward(name));
		}

		template <class Name = CommandName
		> ML_NODISCARD std::optional<size_t> GetIndex(Name && name) const noexcept
		{
			if (auto const i{ Commands.lookup<ds::string>(ML_forward(name)) }
			; i == Commands.npos) { return std::nullopt; }
			else { return i; }
		}

		template <class Name = CommandName
		> ML_NODISCARD CommandInfo * GetInfo(Name && name) noexcept
		{
			if (auto const i{ this->GetIndex(ML_forward(name)) }; !i) { return nullptr; }
			else { return std::addressof(Commands.at<CommandInfo>(*i)); }
		}

		template <class Name = CommandName
		> ML_NODISCARD CommandProc * GetProc(Name && name) noexcept
		{
			if (auto const i{ this->GetIndex(ML_forward(name)) }; !i) { return nullptr; }
			else { return std::addressof(Commands.at<CommandProc>(*i)); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		int32 InputTextCallbackStub(ImGuiInputTextCallbackData * data);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// EDIT TRANSFORM
namespace ml::ImGuiExt
{
	struct ML_NODISCARD TransformDecompositionEditor final : non_copyable, trackable
	{
		ImGuizmo::OPERATION	CurrentGizmoOperation;
		ImGuizmo::MODE		CurrentGizmoMode;
		bool				UseSnap;
		vec3				Snap;
		float32				Bounds[6];
		float32				BoundsSnap[3];
		bool				BoundSizing;
		bool				BoundSizingSnap;

		TransformDecompositionEditor() noexcept
			: CurrentGizmoOperation	{ ImGuizmo::TRANSLATE }
			, CurrentGizmoMode		{ ImGuizmo::LOCAL }
			, UseSnap				{ false }
			, Snap					{ 1.f, 1.f, 1.f }
			, Bounds				{ -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f }
			, BoundsSnap			{ 0.1f, 0.1f, 0.1f }
			, BoundSizing			{ false }
			, BoundSizingSnap		{ false }
		{
		}

		void operator()(mat4 & value)
		{
			if (ImGui::IsKeyPressed(keycode_z)) CurrentGizmoOperation = ImGuizmo::TRANSLATE;
			if (ImGui::IsKeyPressed(keycode_e)) CurrentGizmoOperation = ImGuizmo::ROTATE;
			if (ImGui::IsKeyPressed(keycode_r))  CurrentGizmoOperation = ImGuizmo::SCALE;

			if (ImGui::RadioButton("Translate", CurrentGizmoOperation == ImGuizmo::TRANSLATE)) {
				CurrentGizmoOperation = ImGuizmo::TRANSLATE;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Rotate", CurrentGizmoOperation == ImGuizmo::ROTATE)) {
				CurrentGizmoOperation = ImGuizmo::ROTATE;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Scale", CurrentGizmoOperation == ImGuizmo::SCALE)) {
				CurrentGizmoOperation = ImGuizmo::SCALE;
			}

			vec3 matrixTranslation, matrixRotation, matrixScale;
			ImGuizmo::DecomposeMatrixToComponents(value, matrixTranslation, matrixRotation, matrixScale);
			ImGui::InputFloat3("Tr", matrixTranslation);
			ImGui::InputFloat3("Rt", matrixRotation);
			ImGui::InputFloat3("Sc", matrixScale);
			ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, value);

			if (CurrentGizmoOperation != ImGuizmo::SCALE) {
				if (ImGui::RadioButton("Local", CurrentGizmoMode == ImGuizmo::LOCAL)) {
					CurrentGizmoMode = ImGuizmo::LOCAL;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("World", CurrentGizmoMode == ImGuizmo::WORLD)) {
					CurrentGizmoMode = ImGuizmo::WORLD;
				}
			}

			if (ImGui::IsKeyPressed(keycode_s)) {
				UseSnap = !UseSnap;
			}
			ImGui::Checkbox("", &UseSnap);
			ImGui::SameLine();

			switch (CurrentGizmoOperation)
			{
			case ImGuizmo::TRANSLATE: ImGui::InputFloat3("Snap", &Snap[0]); break;
			case ImGuizmo::ROTATE: ImGui::InputFloat("Angle Snap", &Snap[0]); break;
			case ImGuizmo::SCALE: ImGui::InputFloat("Scale Snap", &Snap[0]); break;
			}
			ImGui::Checkbox("Bound Sizing", &BoundSizing);
			if (BoundSizing)
			{
				ImGui::PushID(3);
				ImGui::Checkbox("", &BoundSizingSnap);
				ImGui::SameLine();
				ImGui::InputFloat3("Snap", BoundsSnap);
				ImGui::PopID();
			}
		}
	};

	inline void EditTransform(
		float32 const * view,
		float32 const * proj,
		mat4 & value,
		float_rect const & bounds,
		bool editTransformDecomposition
	)
	{
		static TransformDecompositionEditor t{};
		if (editTransformDecomposition) { t(value); }
		
		ImGuizmo::SetRect(
			bounds[0],
			bounds[1],
			bounds[2],
			bounds[3]);

		ImGuizmo::Manipulate(
			view,
			proj,
			t.CurrentGizmoOperation,
			t.CurrentGizmoMode,
			value,
			NULL,
			t.UseSnap ? &t.Snap[0] : NULL,
			t.BoundSizing ? t.Bounds : NULL,
			t.BoundSizingSnap ? t.BoundsSnap : NULL);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_IMGUI_EXT_HPP_