#ifndef _ML_IMGUI_EXT_HPP_
#define _ML_IMGUI_EXT_HPP_

#include <modus_core/detail/Method.hpp>
#include <modus_core/imgui/ImGui.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// SCOPE ID
namespace ml::ImGuiExt::impl
{
	// scoped imgui id
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
	// BASIC PANEL
	template <class Derived> struct ML_NODISCARD BasicPanel
	{
		cstring		Title		; // title
		bool		IsOpen		; // is open
		int32		WindowFlags	; // window flags

		BasicPanel(cstring title, bool open = false, int32 winflags = ImGuiWindowFlags_None) noexcept
			: Title			{ ML_check(title) }
			, IsOpen		{ open }
			, WindowFlags	{ winflags }
		{
		}

		BasicPanel(BasicPanel const & other) noexcept
			: BasicPanel{ other.Title, other.IsOpen, other.WindowFlags }
		{
		}

		ML_NODISCARD bool GetWindowFlag(ImGuiWindowFlags_ index) const noexcept {
			return ML_flag_read(WindowFlags, (int32)index);
		}

		bool SetWindowFlag(ImGuiWindowFlags_ index, bool value) noexcept {
			return ML_flag_write(WindowFlags, (int32)index, value);
		}

		ML_NODISCARD auto GetID() const noexcept -> ImGuiID {
			return ImGui::GetID(Title);
		}

		ML_NODISCARD auto Get() const noexcept -> ImGuiWindow * {
			return ImGui::FindWindowByName(Title);
		}
	};

	// PANEL
	struct ML_NODISCARD Panel : BasicPanel<Panel>
	{
		using BasicPanel<Panel>::BasicPanel;

		Panel(cstring title, bool open = false, int32 winflags = ImGuiWindowFlags_None) noexcept
			: BasicPanel{ title, open, winflags }
		{
		}

		template <class Fn, class ... Args
		> bool Draw(Fn && fn, Args && ... args) noexcept
		{
			if (!IsOpen) { return false; }
			ImGuiExt_ScopeID(this);
			return ImGuiExt::BeginEnd
			(
				std::bind(&ImGui::Begin, Title, &IsOpen, WindowFlags),
				&ImGui::End,
				ML_forward(fn), this, ML_forward(args)...
			);
		}
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// OVERLAY
namespace ml::ImGuiExt
{
	template <class Fn
	> bool DrawSimpleOverlay(ImGuiViewport * vp, cstring title, bool * open = 0, int32 corner = 0, vec2 const & offset = { 10.f, 10.f }, float32 alpha = 0.35f, Fn fn = ([]() noexcept {}))
	{
		ImGuiWindowFlags window_flags{
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav
		};
		if (corner != -1)
		{
			window_flags |= ImGuiWindowFlags_NoMove;
			float_rect const bounds{
				vp->GetWorkPos(),
				vp->GetWorkSize()
			};
			vec2 const pos{
				((corner & 1) ? (bounds[0] + bounds[2] - offset[0]) : (bounds[0] + offset[0])),
				((corner & 2) ? (bounds[1] + bounds[3] - offset[1]) : (bounds[1] + offset[1]))
			};
			vec2 const piv{
				((corner & 1) ? 1.f : 0.f),
				((corner & 2) ? 1.f : 0.f)
			};
			ImGui::SetNextWindowPos(pos, ImGuiCond_Always, piv);
			ImGui::SetNextWindowViewport(vp->ID);
		}
		ImGui::SetNextWindowBgAlpha(alpha);
		return ImGuiExt::Window(title, open, window_flags, fn);
	}

	struct ML_NODISCARD Overlay : BasicPanel<Overlay>
	{
		int32	Corner	{ 0 };
		vec2	Offset	{ 10.f, 10.f };
		float32	Alpha	{ .35f };

		static constexpr auto DefaultWindowFlags
		{
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav
		};

		using BasicPanel<Overlay>::BasicPanel;

		Overlay(
			cstring			title,
			bool			open = false,
			int32			corner = 0,
			vec2 const &	offset = { 10.f, 10.f },
			float32			alpha = .35f) noexcept
			: BasicPanel	{ title, open, DefaultWindowFlags }
			, Corner		{ corner }
			, Offset		{ offset }
			, Alpha			{ alpha }
		{
		}

		template <class Fn> bool Draw(ImGuiViewport * vp, Fn && fn) noexcept
		{
			if (!IsOpen) { return false; }
			ImGuiExt_ScopeID(this);
			return ImGuiExt::DrawSimpleOverlay
			(
				vp,
				Title,
				&IsOpen,
				Corner,
				Offset,
				Alpha,
				std::bind(ML_forward(fn), this)
			);
		}

		template <class Fn> bool Draw(Fn && fn) noexcept
		{
			return this->Draw
			(
				ImGui::GetMainViewport(),
				ML_forward(fn)
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

		ML_NODISCARD auto GetID() const noexcept -> ImGuiID {
			return ImGui::GetID(Title);
		}

		ML_NODISCARD auto Get() const noexcept -> ImGuiWindow * {
			return ImGui::FindWindowByName(Title);
		}

		template <class Fn, class ... Args
		> bool Draw(Fn && fn, Args && ... args) noexcept
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
		> bool Draw(ImGuiViewport * vp, Fn && fn, Args && ... args) noexcept
		{
			if (!IsOpen || !IsDockingEnabled()) { return false; }

			if (!vp) { ML_verify(vp = ImGui::GetMainViewport()); }

			ImGuiExt_ScopeID(this);
			ImGui::SetNextWindowPos(vp->Pos);
			ImGui::SetNextWindowSize(vp->Size);
			ImGui::SetNextWindowViewport(vp->ID);
			ImGui::SetNextWindowBgAlpha(Alpha);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, Rounding);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, Border);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Padding);
			
			bool const is_open{ ImGui::Begin(Title, &IsOpen, WindowFlags) };
			ML_defer(&) { ImGui::End(); };
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
		ImGuiExt_ScopeID(label);
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
			ImGuiExt_ScopeID(this);
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
			ImGuiExt_ScopeID(this);
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
			ImGuiExt_ScopeID(this);
			bool const is_scale{ Operation == ImGuizmo::SCALE };
			ImGui::BeginGroup();
			if (ImGui::RadioButton("local", !is_scale && Mode == ImGuizmo::LOCAL)) { Mode = ImGuizmo::LOCAL; }
			ImGui::SameLine();
			if (ImGui::RadioButton("world", !is_scale && Mode == ImGuizmo::WORLD)) { Mode = ImGuizmo::WORLD; }
			ImGui::EndGroup();
		}

		void ShowSnapControls(float32 speed = 0.01f)
		{
			ImGuiExt_ScopeID(this);
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
			ImGuiExt_ScopeID(this);
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