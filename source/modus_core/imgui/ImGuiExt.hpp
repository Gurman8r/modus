#ifndef _ML_IMGUI_EXT_HPP_
#define _ML_IMGUI_EXT_HPP_

#include <modus_core/detail/Method.hpp>
#include <modus_core/imgui/ImGui.hpp>

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

// WINDOWS
namespace ml::ImGuiExt
{
	// BEGIN/END
	template <class BeginFn, class EndFn, class Fn, class ... Args
	> bool BeginEnd(BeginFn && begin_fn, EndFn && end_fn, Fn && fn, Args && ... args)
	{
		ML_defer(&) { std::invoke(ML_forward(end_fn)); };
		
		bool const is_open{ std::invoke(ML_forward(begin_fn)) };
		
		if (is_open) { std::invoke(ML_forward(fn), ML_forward(args)...); }
		
		return is_open;
	}

	// WINDOW
	template <class Fn, class ... Args
	> bool Window(cstring title, bool * open, int32_t flags, Fn && fn, Args && ... args) noexcept
	{
		return ImGuiExt::BeginEnd(
			std::bind(&ImGui::Begin, title, open, flags),
			&ImGui::End,
			ML_forward(fn), ML_forward(args)...);
	}

	// CHILD WINDOW EX
	template <class Fn, class ... Args
	> bool ChildWindowEx(cstring name, ImGuiID id, vec2 const & size, bool border, int32_t flags, Fn && fn, Args && ... args) noexcept
	{
		return ImGuiExt::BeginEnd(
			std::bind(&ImGui::BeginChildEx, name, id, size, border, flags),
			&ImGui::EndChild,
			ML_forward(fn), ML_forward(args)...);
	}

	// CHILD WINDOW
	template <class Fn, class ... Args
	> bool ChildWindow(cstring id, vec2 const & size, bool border, int32_t flags, Fn && fn, Args && ... args) noexcept
	{
		return ImGuiExt::ChildWindowEx
		(
			id, ImGui::GetID(id), size, border, flags, ML_forward(fn), ML_forward(args)...
		);
	}

	// CHILD WINDOW
	template <class Fn, class ... Args
	> bool ChildWindow(ImGuiID id, vec2 const & size, bool border, int32_t flags, Fn && fn, Args && ... args) noexcept
	{
		return ImGuiExt::ChildWindowEx
		(
			nullptr, id, size, border, flags, ML_forward(fn), ML_forward(args)...
		);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// PANELS
namespace ml::ImGuiExt
{

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// BASIC PANEL
	template <class Derived
	> struct ML_NODISCARD BasicPanel
	{
		cstring		Title;
		bool		IsOpen;
		int32_t		WinFlags;

		BasicPanel(cstring title, bool open = false, int32_t winflags = ImGuiWindowFlags_None) noexcept
			: Title		{ title }
			, IsOpen	{ open }
			, WinFlags	{ winflags }
		{
		}

		BasicPanel(BasicPanel const & other) noexcept
			: BasicPanel{ other.Title, other.IsOpen, other.WinFlags }
		{
		}

		bool Begin() noexcept {
			return IsOpen && ImGui::Begin(Title, &IsOpen, WinFlags);
		}

		void End() noexcept {
			ImGui::End();
		}

		ML_NODISCARD ImGuiID GetID() const noexcept {
			return ImGuiExt::GetID(this);
		}
	};

	// BASIC PANEL ID
	template <class Derived
	> ImGuiID GetID(BasicPanel<Derived> const * p) noexcept
	{
		return ImGui::GetID(p->Title);
	}

	// BASIC PANEL MENU ITEM
	template <class Derived
	> bool MenuItem(BasicPanel<Derived> * p, cstring shortcut = {}, bool enabled = true)
	{
		return ImGui::MenuItem(p->Title, shortcut, &p->IsOpen, enabled);
	}

	// BASIC PANEL SELECTABLE
	template <class Derived
	> bool Selectable(BasicPanel<Derived> * p, int32_t flags = ImGuiSelectableFlags_None, vec2 const & size = {})
	{
		return ImGui::Selectable(p->Title, &p->IsOpen, flags, size);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// PANEL
	struct ML_NODISCARD Panel : BasicPanel<Panel>
	{
		Panel(cstring title, bool open = false, int32_t winflags = ImGuiWindowFlags_None) noexcept
			: BasicPanel{ title, open, winflags }
		{
		}

		Panel(Panel const & other) noexcept : BasicPanel{ other }
		{
		}

		template <class Fn, class ... Args
		> bool operator()(Fn && fn, Args && ... args) noexcept
		{
			ML_ImGui_ScopeID(this);
			return IsOpen && ImGuiExt::Window
			(
				Title, &IsOpen, WinFlags, ML_forward(fn), ML_forward(args)...
			);
		}
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// DOCKSPACE
namespace ml::ImGuiExt
{
	struct ML_NODISCARD Dockspace : BasicPanel<Dockspace>
	{
		float_t		Border		; // 
		float_t		Rounding	; // 
		vec2		Padding		; // 
		float_t		Alpha		; // 
		vec2		Size		; // 
		int32_t		DockFlags	; // 

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

		static bool IsDockingEnabled() noexcept {
			return ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable;
		}

		Dockspace(
			cstring			title		= "Dockspace",
			bool			open		= true,
			float_t			border		= {},
			float_t			rounding	= {},
			vec2 const &	padding		= {},
			float_t			alpha		= {},
			vec2 const &	docksize	= {},
			int32_t			winflags	= DefaultWindowFlags,
			int32_t			dockflags	= ImGuiDockNodeFlags_AutoHideTabBar)
			: BasicPanel	{ title, open, winflags }
			, Border		{ border }
			, Rounding		{ rounding }
			, Padding		{ padding }
			, Alpha			{ alpha }
			, Size			{ docksize }
			, DockFlags		{ dockflags }
		{}

		Dockspace(Dockspace const & other) noexcept : Dockspace{
			other.Title,
			other.IsOpen,
			other.Border,
			other.Rounding,
			other.Padding,
			other.Alpha,
			other.Size,
			other.WinFlags,
			other.DockFlags
		}
		{}

		template <class Fn, class ... Args
		> bool operator()(ImGuiViewport const * vp, Fn && fn, Args && ... args) noexcept
		{
			if (!vp || !IsOpen || !IsDockingEnabled()) { return false; }

			ML_ImGui_ScopeID(this);
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
				std::invoke(ML_forward(fn), ML_forward(args)...);

				ImGui::DockSpace(
					GetID(),
					Size,
					ImGuiDockNodeFlags_PassthruCentralNode | DockFlags,
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
	struct ML_CORE_API ML_NODISCARD TextLog final : trackable, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		using Line = typename ds::string;

		using LineBuffer = typename pmr::vector<Line>;

		using PrinterSignature = typename void(LineBuffer const &, size_t);

		struct ML_CORE_API Printer final : ds::method< PrinterSignature >
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
	struct ML_CORE_API ML_NODISCARD Terminal final : trackable, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		using InputBuffer = typename ds::array<char, 256>;
		
		using Line = typename TextLog::Line;
		
		using Printer = typename TextLog::Printer;
		
		using CommandProc = typename ds::method< void(Line &&) >;
		
		using CommandInfo = typename pmr::vector<ds::string>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ds::string User, Host, Path, Mode; // session

		InputBuffer Input; // input

		TextLog Output; // output

		ds::batch_vector<ds::string, CommandInfo, CommandProc> Commands; // commands

		pmr::vector<Line> History; // history

		int32_t HistoryPos; // history pos

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

		void DrawPrefix();
		
		void DrawInput();

		int32_t Execute(Line && line);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Name = ds::string
		> bool AddCommand(Name && name, CommandInfo const & info, CommandProc const & clbk) noexcept
		{
			if (this->HasCommand(ML_forward(name))) { return false; }
			else { Commands.push_back(ML_forward(name), info, clbk); return true; }
		}

		template <class Name = ds::string
		> bool DelCommand(Name && name) noexcept
		{
			if (auto const i{ this->GetIndex(ML_forward(name)) }; !i) { return false; }
			else { Commands.erase(*i); return true; }
		}

		template <class Name = ds::string
		> ML_NODISCARD bool HasCommand(Name && name) const noexcept
		{
			return Commands.contains<ds::string>(ML_forward(name));
		}

		template <class Name = ds::string
		> ML_NODISCARD std::optional<size_t> GetIndex(Name && name) const noexcept
		{
			if (auto const i{ Commands.lookup<ds::string>(ML_forward(name)) }
			; i == Commands.npos) { return std::nullopt; }
			else { return i; }
		}

		template <class Name = ds::string
		> ML_NODISCARD CommandInfo * GetInfo(Name && name) noexcept
		{
			if (auto const i{ this->GetIndex(ML_forward(name)) }; !i) { return nullptr; }
			else {
				return std::addressof(Commands.at<CommandInfo>(*i));
			}
		}

		template <class Name = ds::string
		> ML_NODISCARD CommandProc * GetProc(Name && name) noexcept
		{
			if (auto const i{ this->GetIndex(ML_forward(name)) }; !i) { return nullptr; }
			else {
				return std::addressof(Commands.at<CommandProc>(*i));
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		int32_t InputTextCallbackStub(ImGuiInputTextCallbackData * data);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_IMGUI_EXT_HPP_