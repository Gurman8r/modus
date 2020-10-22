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

// PANELS
namespace ml::ImGuiExt
{
	// DRAW PANEL
	template <class Fn, class ... Args
	> bool DrawPanel(cstring title, bool * p_open, int32_t flags, Fn && fn, Args && ... args)
	{
		ML_defer(&) { ImGui::End(); };
		
		bool const is_open{ ImGui::Begin(title, p_open, flags) };
		
		if (is_open) { std::invoke(ML_forward(fn), ML_forward(args)...); }
		
		return is_open;
	}
	
	// PANEL
	struct ML_NODISCARD Panel final
	{
		cstring title	{ "Panel" };
		bool	open	{ false };
		int32_t flags	{ ImGuiWindowFlags_None };

		template <class Fn, class ... Args
		> bool operator()(Fn && fn, Args && ... args) noexcept
		{
			ML_ImGui_ScopeID(this);
			return open && ImGuiExt::DrawPanel
			(
				title, &open, flags, ML_forward(fn), ML_forward(args)...
			);
		}
	};

	// PANEL MENU ITEM
	static bool MenuItem(Panel & p, cstring shortcut = {}, bool enabled = true)
	{
		return ImGui::MenuItem(p.title, shortcut, &p.open, enabled);
	}

	// PANEL SELECTABLE
	static bool Selectable(Panel & p, int32_t flags = ImGuiSelectableFlags_None, vec2 const & size = {})
	{
		return ImGui::Selectable(p.title, &p.open, flags, size);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// OUTPUT TEXT
namespace ml::ImGuiExt
{
	struct ML_CORE_API ML_NODISCARD OutputText
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		using LineBuffer = typename pmr::vector<pmr::string>;

		using LinePrinter = typename ds::method< void(pmr::string const &) >;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		LineBuffer		Lines			; // line buffer
		ImGuiTextFilter	Filter			; // line filter
		LinePrinter		Printer			; // line printer
		bool			ScrollToBottom	; // scroll to bottom
		bool			AutoScroll		; // auto-scroll

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		OutputText(allocator_type alloc = {}) noexcept
			: Lines			{ alloc }
			, Filter		{}
			, Printer		{ &DefaultPrinter }
			, ScrollToBottom{}
			, AutoScroll	{ true }
		{
		}

		void Draw() noexcept;

		static void DefaultPrinter(pmr::string const & line) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		OutputText & Write(char const c) noexcept
		{
			switch (c)
			{
			default: { // default
				if (Lines.empty()) { Lines.push_back({}); }
				Lines.back().push_back(c);
			} break;

			case '\n': { // new line
				Lines.push_back({});
			} break;
			}
			return (*this);
		}

		OutputText & Print(cstring value = "\n") noexcept
		{
			for (size_t i = 0; i < std::strlen(value); ++i) {
				this->Write(value[i]);
			}
			return (*this);
		}

		OutputText & Print(pmr::string const & value = "\n") noexcept
		{
			for (char c : value) {
				this->Write(c);
			}
			return (*this);
		}

		template <class ... Args
		> OutputText & Printf(cstring str, Args && ... args) noexcept
		{
			ds::array<char, 512> buf{};
			std::sprintf(buf, str, ML_forward(args)...);
			return this->Print(buf.data());
		}

		template <class ... Args
		> OutputText & Printf(pmr::string const & str, Args && ... args) noexcept
		{
			return this->Printf(str.c_str(), ML_forward(args)...);
		}

		template <
			class Ch = char,
			class Tr = std::char_traits<Ch>,
			class Al = pmr::polymorphic_allocator<Ch>
		> OutputText & Dump(std::basic_stringstream<Ch, Tr, Al> & ss) noexcept
		{
			for (Ch const c : ss.str()) {
				this->Write(static_cast<char>(c));
			}
			ss.str({});
			return (*this);
		}

		template <class T
		> OutputText & operator<<(T && value) noexcept
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
	struct ML_CORE_API ML_NODISCARD CommandLine
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		using InputTextCallback = ImGuiInputTextCallback;

		using InputTextCallbackData = ImGuiInputTextCallbackData;

		using Callback = ds::method<void(pmr::string &&)>;

		using CommandInfo = pmr::vector<pmr::string>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// colors
		struct {
			color
				Delim	{ colors::white },
				User	{ colors::aqua },
				Host	{ colors::magenta },
				Path	{ colors::cyan },
				Mode	{ colors::fuchsia };
		} Colors;

		// commands
		ds::batch_vector<
			pmr::string,	// name
			CommandInfo,	// info
			Callback		// callback
		> Commands;

		OutputText					Output		; // output
		ds::array<char, 256>		Input		; // input
		pmr::vector<pmr::string>	History		; // history
		int32_t						HistoryPos	; // history index

		pmr::string User, Host, Path, Mode; // prefix

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		CommandLine(allocator_type alloc = {}) noexcept;

		void Draw() noexcept;

		void Execute(pmr::string line) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool AddCommand(pmr::string const & name, CommandInfo const & info, Callback const & clbk) noexcept
		{
			if (Commands.contains<pmr::string>(name)) { return false; }
			else { Commands.push_back(name, info, clbk); return true; }
		}

		bool DelCommand(pmr::string const & name) noexcept
		{
			if (auto const i{ Commands.lookup<pmr::string>(name) }
			; i == Commands.npos) { return false; }
			else { Commands.erase(i); return true; }
		}

		ML_NODISCARD Callback * GetCallback(pmr::string const & name) noexcept
		{
			if (auto const i{ Commands.lookup<pmr::string>(name) }
			; i == Commands.npos) { return nullptr; }
			else { return std::addressof(Commands.at<Callback>(i)); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		int32_t InputTextCallbackStub(ImGuiInputTextCallbackData * data) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_IMGUI_EXT_HPP_