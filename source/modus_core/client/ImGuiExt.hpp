#ifndef _ML_IMGUI_EXT_HPP_
#define _ML_IMGUI_EXT_HPP_

#include <modus_core/detail/Method.hpp>
#include <modus_core/client/ImGui.hpp>

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
	// BEGIN / END
	template <class BeginFn, class EndFn, class Fn, class ... Args
	> bool BeginEnd(BeginFn && begin_fn, EndFn && end_fn, Fn && fn, Args && ... args)
	{
		ML_defer(&) { std::invoke(ML_forward(end_fn)); };
		
		bool const is_open{ std::invoke(ML_forward(begin_fn)) };
		
		if (is_open) { std::invoke(ML_forward(fn), ML_forward(args)...); }
		
		return is_open;
	}

	// DRAW WINDOW
	template <class Fn, class ... Args
	> bool DrawWindow(cstring title, bool * p_open, int32_t flags, Fn && fn, Args && ... args) noexcept
	{
		return ImGuiExt::BeginEnd(
			std::bind(&ImGui::Begin, title, p_open, flags),
			&ImGui::End,
			ML_forward(fn), ML_forward(args)...);
	}

	// DRAW CHILD EX
	template <class Fn, class ... Args
	> bool DrawChildEx(cstring name, ImGuiID id, vec2 const & size, bool border, int32_t flags, Fn && fn, Args && ... args) noexcept
	{
		return ImGuiExt::BeginEnd(
			std::bind(&ImGui::BeginChildEx, name, id, size, border, flags),
			&ImGui::EndChild,
			ML_forward(fn), ML_forward(args)...);
	}

	// DRAW CHILD
	template <class Fn, class ... Args
	> bool DrawChild(cstring id, vec2 const & size, bool border, int32_t flags, Fn && fn, Args && ... args) noexcept
	{
		return ImGuiExt::DrawChildEx
		(
			id, ImGui::GetID(id), size, border, flags, ML_forward(fn), ML_forward(args)...
		);
	}

	// DRAW CHILD
	template <class Fn, class ... Args
	> bool DrawChild(ImGuiID id, vec2 const & size, bool border, int32_t flags, Fn && fn, Args && ... args) noexcept
	{
		return ImGuiExt::DrawChildEx
		(
			nullptr, id, size, border, flags, ML_forward(fn), ML_forward(args)...
		);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// PANELS
namespace ml::ImGuiExt
{
	// PANEL
	struct ML_NODISCARD Panel final
	{
		cstring Title	{ "Panel" };
		bool	IsOpen	{ false };
		int32_t Flags	{ ImGuiWindowFlags_None };

		template <class Fn, class ... Args
		> bool operator()(Fn && fn, Args && ... args) noexcept
		{
			ML_ImGui_ScopeID(this);
			return IsOpen && ImGuiExt::DrawWindow
			(
				Title, &IsOpen, Flags, ML_forward(fn), ML_forward(args)...
			);
		}
	};

	// PANEL MENU ITEM
	static bool MenuItem(Panel & p, cstring shortcut = {}, bool enabled = true)
	{
		return ImGui::MenuItem(p.Title, shortcut, &p.IsOpen, enabled);
	}

	// PANEL SELECTABLE
	static bool Selectable(Panel & p, int32_t flags = ImGuiSelectableFlags_None, vec2 const & size = {})
	{
		return ImGui::Selectable(p.Title, &p.IsOpen, flags, size);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// OUTPUT LOG
namespace ml::ImGuiExt
{
	struct ML_CORE_API ML_NODISCARD OutputLog final : trackable, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		using Line = typename pmr::string;

		using LineBuffer = typename pmr::vector<Line>;

		struct ML_CORE_API Printer final : ds::method< void(LineBuffer const &, size_t) >
		{
			using ds::method< void(LineBuffer const &, size_t) >::method;

			static Printer Default;

			Printer() noexcept : Printer{ Default } {}
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ImGuiTextFilter	Filter			; // line filter
		LineBuffer		Lines			; // line buffer
		bool			AutoScroll		; // auto-scroll
		bool			ScrollToBottom	; // scroll to bottom

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit OutputLog(allocator_type alloc) noexcept : OutputLog{ NULL, true, alloc } {}

		OutputLog(cstring default_filter = "", bool auto_scroll = true, allocator_type alloc = {}) noexcept;

		void Draw(Printer const & print = {});

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
			for (size_t i = 0; i < std::strlen(value); ++i)
				this->Write(value[i]);
			return (*this);
		}

		auto & Print(Line const & value = "\n") noexcept
		{
			for (char c : value)
				this->Write(c);
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

		auto & Dump(pmr::stringstream & ss) noexcept
		{
			for (char c : ss.str())
				this->Write(c);
			ss.str({});
			return (*this);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto & operator<<(char value) noexcept { return this->Write(value); }

		auto & operator<<(cstring value) noexcept { return this->Print(value); }

		auto & operator<<(Line const & value) noexcept { return this->Print(value); }

		template <class T> auto & operator<<(T && value) noexcept
		{
			pmr::stringstream ss{};
			ss << value;
			return this->Dump(ss);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// COMMAND LINE
namespace ml::ImGuiExt
{
	struct ML_CORE_API ML_NODISCARD CommandLine final : trackable, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		using InputBuffer		= typename ds::array<char, 256>;
		using Line				= typename OutputLog::Line;
		using Printer			= typename OutputLog::Printer;
		using CommandName		= typename pmr::string;
		using CommandProc			= typename ds::method< void(Line &&) >;
		using CommandInfo		= typename pmr::vector<pmr::string>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		pmr::string User, Host, Path, Mode; // session

		InputBuffer In; // input

		OutputLog Out; // output

		ds::batch_vector<CommandName, CommandInfo, CommandProc> Commands; // commands

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

		CommandLine(allocator_type alloc = {}) noexcept;

		void Draw(Printer const & print = {}, bool prefix = true);

		void DrawInput(cstring label, bool prefix = true);

		int32_t Execute(Line && line);

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
			return Commands.contains<CommandName>(ML_forward(name));
		}

		template <class Name = CommandName
		> ML_NODISCARD std::optional<size_t> GetIndex(Name && name) const noexcept
		{
			if (auto const i{ Commands.lookup<CommandName>(ML_forward(name)) }
			; i == Commands.npos) { return std::nullopt; }
			else { return i; }
		}

		template <class Name = CommandName
		> ML_NODISCARD CommandProc * GetProc(Name && name) noexcept
		{
			if (auto const i{ this->GetIndex(ML_forward(name)) }; !i) { return nullptr; }
			else {
				return std::addressof(Commands.at<CommandProc>(*i));
			}
		}

		template <class Name = CommandName
		> ML_NODISCARD CommandInfo * GetInfo(Name && name) noexcept
		{
			if (auto const i{ this->GetIndex(ML_forward(name)) }; !i) { return nullptr; }
			else {
				return std::addressof(Commands.at<CommandInfo>(*i));
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