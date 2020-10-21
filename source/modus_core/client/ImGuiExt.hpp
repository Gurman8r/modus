#ifndef _ML_IMGUI_EXT_HPP_
#define _ML_IMGUI_EXT_HPP_

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
	// DRAW WINDOW
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
	struct ML_NODISCARD OutputText
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		pmr::vector<pmr::string>	Lines			; // lines
		ImGuiTextFilter				Filter			; // text filter
		bool						AutoScroll		; // auto-scroll
		bool						ScrollToBottom	; // scroll-to-bottom

		OutputText(allocator_type alloc = {}) noexcept
			: Lines			{ alloc }
			, Filter		{}
			, AutoScroll	{ true }
			, ScrollToBottom{}
		{
		}

		void Clear() noexcept
		{
			Lines.clear();
		}

		void Write(char const value) noexcept
		{
			switch (value)
			{
			default: {
				if (Lines.empty()) { Lines.push_back({}); }
				Lines.back().push_back(value);
			} break;

			case '\n': {
				Lines.push_back({});
			} break;
			}
		}

		void Dump(pmr::stringstream & value) noexcept
		{
			for (char c : value.str()) { Write(c); }
			value.str({});
		}

		void Print(pmr::string const & value = "\n") noexcept
		{
			for (char c : value) { Write(c); }
		}

		template <class ... Args
		> void Printf(pmr::string const & value, Args && ... args) noexcept
		{
			ds::array<char, 512> buf{};
			std::sprintf(buf, value.c_str(), ML_forward(args)...);
			Print(buf.data());
		}

		template <class ID
		> void Draw(ID && id, vec2 const & size = {}, bool border = {}, int32_t flags = ImGuiWindowFlags_HorizontalScrollbar) noexcept
		{
			// LINES
			ImGui::BeginChild(ML_forward(id), size, border, flags);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4, 1 });
			for (pmr::string const & line : Lines)
			{
				// filter
				if (!Filter.PassFilter(line.c_str())) { continue; }

				// color
				color c{}; bool has_color{};
				if (strstr(line.c_str(), "[error]")) {
					c = colors::red; has_color = true;
				}
				else if (!std::strncmp(line.c_str(), "# ", 2)) {
					c = { 1.0f, 0.8f, 0.6f, 1.0f }; has_color = true;
				}

				if (has_color) { ImGui::PushStyleColor(ImGuiCol_Text, c); }
				ImGui::TextUnformatted(line.c_str());
				if (has_color) { ImGui::PopStyleColor(); }
			}
			if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())) {
				ImGui::SetScrollHereY(1.0f);
			}
			ScrollToBottom = false;
			ImGui::PopStyleVar();
			ImGui::EndChild();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// COMMAND LINE
namespace ml::ImGuiExt
{
	struct ML_NODISCARD CommandLine
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		struct ML_NODISCARD Command final
		{
			using Information = typename pmr::vector<pmr::string>;
			
			using Callback = typename std::function< void(pmr::string &&) >;

			pmr::string	name; // name
			Information	info; // info
			Callback	clbk; // callback

			void operator()(pmr::string && value) noexcept {
				std::invoke(ML_check(clbk), std::move(value));
			}

			ML_NODISCARD bool operator==(Command const & other) const noexcept {
				return (this == std::addressof(other)) || (name == other.name);
			}

			ML_NODISCARD bool operator<(Command const & other) const noexcept {
				return (this != std::addressof(other)) && (name < other.name);
			}
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;
		
		pmr::string
			User	, // user
			Domain	, // domain
			Path	; // path

		pmr::vector<Command>		Commands	; // commands
		pmr::vector<pmr::string>	History		; // history
		int32_t						HistoryPos	; // history index
		ds::array<char, 256>		Input		; // input
		OutputText					Output		; // output

		CommandLine(allocator_type alloc = {}) noexcept
			: User		{ alloc }
			, Domain	{ alloc }
			, Path		{ alloc }
			, Commands	{ alloc }
			, History	{ alloc }
			, HistoryPos{ -1 }
			, Input		{}
			, Output	{ alloc }
		{
		}

		ML_NODISCARD auto Find(pmr::string const & name) noexcept
			-> std::optional<pmr::vector<Command>::iterator>
		{
			if (auto const it{ std::find_if(Commands.begin(), Commands.end(),
			[&](auto & elem) noexcept { return elem.name == name; }) }
			; it != Commands.end()) { return it; }
			else { return std::nullopt; }
		}

		void Execute(pmr::string line) noexcept
		{
			// print line
			if (line.empty()) { return; }
			Output.Printf("# %s\n", line.c_str());

			// update history
			HistoryPos = -1;
			if (auto const it{ std::find(History.begin(), History.end(), line) }
			; it != History.end()) { History.erase(it); }
			History.push_back(line);

			// process command
			if (pmr::string name; auto const cmd{ this->Find(([&]() noexcept -> pmr::string &
			{
				if (!Domain.empty()) {
					name = Domain;
				}
				else if (size_t const i{ line.find_first_of(' ') }; i != line.npos) {
					name = line.substr(0, i);
					line = line.substr(i + 1);
				}
				else {
					name = line;
					line.clear();
				}
				return name;
			})()) })
			{
				std::invoke(**cmd, std::move(line));
			}
			else
			{
				Output.Printf("unknown command: %s%s\n", name.c_str(), line.c_str());
			}

			Output.ScrollToBottom = true;
		}

		void Draw() noexcept
		{
			// OPTIONS
			Output.Filter.Draw("filter", 180); ImGui::SameLine();
			ImGui::Checkbox("auto-scroll", &Output.AutoScroll); ImGui::SameLine();
			if (ImGui::Button("clear")) { Output.Clear(); }
			ImGui::Separator();

			// OUTPUT
			auto const footer{ ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing() };
			Output.Draw("output area", { 0, -footer }, false, ImGuiWindowFlags_HorizontalScrollbar);
			ImGui::Separator();

			// INFO HEADER
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 }); {
				ImGui::PushStyleColor(ImGuiCol_Text, colors::green); ImGui::Text( // user
					User.empty() ? "root" : User.c_str()
				); ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, colors::white); ImGui::Text( // @
					"@"
				); ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, colors::green); ImGui::Text( // domain
					Domain.empty() ? "modus" : Domain.c_str()
				); ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, colors::white); ImGui::Text( // :
					":"
				); ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, colors::cyan); ImGui::Text( // path
					Path.empty() ? "~" : Path.c_str()
				); ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, colors::white); ImGui::Text( // $
					"$"
				);
				ImGui::PopStyleColor(6);
			} ImGui::PopStyleVar();
			ImGui::SameLine();

			// INPUT TEXT
			bool reclaim_focus{};
			ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
			if (ImGui::InputText(
				"##input", Input.data(), Input.size(),
				ImGuiInputTextFlags_EnterReturnsTrue |
				ImGuiInputTextFlags_CallbackCompletion |
				ImGuiInputTextFlags_CallbackHistory,
				[](auto * u) { return ((CommandLine *)u->UserData)->InputTextCallback(u); },
				this
			))
			{
				if (Input) { this->Execute(Input.data()); }
				std::strcpy(Input.data(), "");
				reclaim_focus = true;
			}
			ImGui::PopItemWidth();

			// Auto-focus on window apparition
			ImGui::SetItemDefaultFocus();
			// Auto focus previous widget
			if (reclaim_focus) { ImGui::SetKeyboardFocusHere(-1); }
		}

	private:
		int32_t InputTextCallback(ImGuiInputTextCallbackData * data) noexcept
		{
			switch (data->EventFlag)
			{
			// COMPLETION
			case ImGuiInputTextFlags_CallbackCompletion: {

				// Locate beginning of current word
				cstring word_end = data->Buf + data->CursorPos;
				cstring word_start = word_end;
				while (word_start > data->Buf)
				{
					char const c = word_start[-1];
					if (c == ' ' || c == '\t' || c == ',' || c == ';')
						break;
					word_start--;
				}

				// Build a list of candidates
				pmr::vector<cstring> candidates;
				for (size_t i = 0; i < Commands.size(); i++)
					if (std::strncmp(Commands[i].name.c_str(), word_start, (size_t)(word_end - word_start)) == 0)
						candidates.push_back(Commands[i].name.c_str());

				if (candidates.size() == 0)
				{
					// No match
					Output.Printf("No match for \'%.*s\'!\n", (size_t)(word_end - word_start), word_start);
				}
				else if (candidates.size() == 1)
				{
					// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
					data->DeleteChars((int32_t)(word_start - data->Buf), (int32_t)(word_end - word_start));
					data->InsertChars(data->CursorPos, candidates[0]);
					data->InsertChars(data->CursorPos, " ");
				}
				else
				{
					// Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
					size_t match_len = (size_t)(word_end - word_start);
					for (;;)
					{
						size_t c = 0;
						bool all_candidates_matches = true;
						for (size_t i = 0; i < candidates.size() && all_candidates_matches; i++)
							if (i == 0)
								c = toupper(candidates[i][match_len]);
							else if (c == 0 || c != toupper(candidates[i][match_len]))
								all_candidates_matches = false;
						if (!all_candidates_matches)
							break;
						match_len++;
					}

					if (match_len > 0)
					{
						data->DeleteChars((int32_t)(word_start - data->Buf), (int32_t)(word_end - word_start));
						data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
					}

					// List matches
					Output.Printf("Possible matches:\n");
					for (size_t i = 0; i < candidates.size(); i++) {
						Output.Printf("- %s\n", candidates[i]);
					}
				}
			} break;

			// HISTORY
			case ImGuiInputTextFlags_CallbackHistory: {
				const size_t prev_history_pos = HistoryPos;
				if (data->EventKey == ImGuiKey_UpArrow)
				{
					if (HistoryPos == -1)
						HistoryPos = (int32_t)History.size() - 1;
					else if (HistoryPos > 0)
						HistoryPos--;
				}
				else if (data->EventKey == ImGuiKey_DownArrow)
				{
					if (HistoryPos != -1)
						if (++HistoryPos >= (int32_t)History.size())
							HistoryPos = -1;
				}

				if (prev_history_pos != HistoryPos)
				{
					cstring history_str = (HistoryPos >= 0) ? History[HistoryPos].c_str() : "";
					data->DeleteChars(0, data->BufTextLen);
					data->InsertChars(0, history_str);
				}
			} break;
			}
			return 0;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_IMGUI_EXT_HPP_