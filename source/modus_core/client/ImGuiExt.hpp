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

// LOCAL STACK
namespace ml::ImGuiExt
{
	struct ML_NODISCARD LocalStack final
	{
		int32_t numcol, numvar;

		LocalStack() noexcept : numcol{}, numvar{} {}

		~LocalStack() noexcept { PopStyleColor(-1); PopStyleVar(-1); };

		int32_t PopStyleColor(int32_t count = 1) noexcept {
			if (count > 0) {
				ImGui::PopStyleColor(count);
				return numcol -= count;
			}
			else {
				return this->PopStyleColor(numcol);
			}
		}

		int32_t PopStyleVar(int32_t count = 1) noexcept {
			if (count > 0) {
				ImGui::PopStyleVar(count);
				return numvar -= count;
			}
			else {
				return this->PopStyleVar(numvar);
			}
		}

		int32_t PushStyleColor(int32_t id, uint32_t value) noexcept {
			ImGui::PushStyleColor(id, value); return numcol++;
		}

		int32_t PushStyleColor(int32_t id, color const & value) noexcept {
			ImGui::PushStyleColor(id, value); return numcol++;
		}

		int32_t PushStyleVar(int32_t id, float_t value) noexcept {
			ImGui::PushStyleVar(id, value); return numvar++;
		}

		int32_t PushStyleVar(int32_t id, vec2 const & value) noexcept {
			ImGui::PushStyleVar(id, value); return numvar++;
		}
	};
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
			using Signature = void(pmr::string &&, pmr::string &&);

			pmr::string					Name		; // name
			std::function<Signature>	Callback	; // callback
			pmr::vector<pmr::string>	Info		; // info

			void operator()(pmr::string && name, pmr::string && line) noexcept {
				std::invoke(ML_check(Callback), std::move(name), std::move(line));
			}

			ML_NODISCARD bool operator==(Command const & other) const noexcept {
				return (this == std::addressof(other)) || (Name == other.Name);
			}

			ML_NODISCARD bool operator<(Command const & other) const noexcept {
				return (this != std::addressof(other)) && (Name < other.Name);
			}
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		struct {
			color
				Delim	{ colors::white },
				User	{ colors::aqua },
				Host	{ colors::magenta },
				Path	{ colors::cyan },
				Mode	{ colors::fuchsia };
		} Colors;

		OutputText					Output		; // output
		ds::array<char, 256>		Input		; // input
		pmr::vector<Command>		Commands	; // commands
		pmr::vector<pmr::string>	History		; // history
		int32_t						HistoryPos	; // history index

		pmr::string User, Host, Path, Mode; // prefix

		CommandLine(allocator_type alloc = {}) noexcept
			: Colors	{}
			, Output	{ alloc }
			, Input		{}
			, Commands	{ alloc }
			, History	{ alloc }
			, HistoryPos{ -1 }
			, User		{ alloc }
			, Host		{ alloc }
			, Path		{ alloc }
			, Mode		{ alloc }
		{
		}

		ML_NODISCARD auto Find(pmr::string const & name) noexcept -> std::optional<pmr::vector<Command>::iterator>
		{
			if (auto const it{ std::find_if(Commands.begin(), Commands.end(),
			[&](auto & elem) noexcept { return elem.Name == name; }) }
			; it != Commands.end()) { return it; }
			else { return std::nullopt; }
		}

		void Draw() noexcept
		{
			// OUTPUT
			auto const input_height{ ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing() };
			Output.Draw("##output area", { 0, -input_height }, false, ImGuiWindowFlags_HorizontalScrollbar);
			ImGui::Separator();

			// INPUT
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4, 4 });
			ImGui::BeginChild("##input area", {}, false, ImGuiWindowFlags_MenuBar);
			ImGui::BeginMenuBar();
			{
				// user@host:path$ /mode
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
				ImGui::PushStyleColor(ImGuiCol_Text, Colors.Delim);
				if (!User.empty()) {
					ImGui::PushStyleColor(ImGuiCol_Text, Colors.User);
					ImGui::Text("%.*s", User.size(), User.c_str());
					ImGui::PopStyleColor(); ImGui::SameLine();
				}
				ImGui::Text("@"); ImGui::SameLine();
				if (!Host.empty()) {
					ImGui::PushStyleColor(ImGuiCol_Text, Colors.Host);
					ImGui::Text("%.*s", Host.size(), Host.c_str());
					ImGui::PopStyleColor(); ImGui::SameLine();
				}
				ImGui::Text(":"); ImGui::SameLine();
				if (!Path.empty()) {
					ImGui::PushStyleColor(ImGuiCol_Text, Colors.Path);
					ImGui::Text("%.*s", Path.size(), Path.c_str());
					ImGui::PopStyleColor(); ImGui::SameLine();
				}
				ImGui::Text("$ "); ImGui::SameLine();
				if (!Mode.empty()) {
					ImGui::Text("/"); ImGui::SameLine();
					ImGui::PushStyleColor(ImGuiCol_Text, Colors.Mode);
					ImGui::Text("%.*s ", Mode.size(), Mode.c_str());
					ImGui::PopStyleColor(); ImGui::SameLine();
				}
				ImGui::PopStyleColor(1);
				ImGui::PopStyleVar(1);

				// INPUT TEXT
				bool reclaim_focus{};
				ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
				if (ImGui::InputText(
					"##input text", Input.data(), Input.size(),
					ImGuiInputTextFlags_EnterReturnsTrue |
					ImGuiInputTextFlags_CallbackCompletion |
					ImGuiInputTextFlags_CallbackHistory,
					[](auto * u) { return ((CommandLine *)u->UserData)->InputTextCallback(u); },
					this
				))
				{
					if (Input) { Execute(Input.data()); }
					std::strcpy(Input.data(), "");
					reclaim_focus = true;
				}
				ImGui::PopItemWidth();
				ImGui::SetItemDefaultFocus(); // focus on window apparition
				if (reclaim_focus) ImGui::SetKeyboardFocusHere(-1); // focus previous widget
			}
			ImGui::EndMenuBar();
			ImGui::EndChild();
			ImGui::PopStyleVar(1);
		}

		void Execute(pmr::string line) noexcept
		{
			// empty, nothing to do
			if (line.empty()) { return; }

			// print line
			Output.Printf("# %s\n", line.c_str());

			// update history
			HistoryPos = -1;
			if (auto const it{ std::find(History.begin(), History.end(), line) }
			; it != History.end()) { History.erase(it); }
			History.push_back(line);

			// process text
			if (!([&]() {
				using namespace util;
				return !line.empty()
					&& !trim(line).empty()
					&& (line.front() == '/' || !Mode.empty())
					&& !trim_front(line, [](char c) { return c == '/' || is_whitespace(c); }).empty()
					;
			})()) { return; }

			// process name
			else if (pmr::string name; auto const it{ this->Find(([&]() noexcept -> pmr::string &
			{
				if (!Mode.empty()) {
					name = Mode;
				} else if (size_t const i{ line.find_first_of(' ') }; i != line.npos) {
					name = line.substr(0, i);
					line = line.substr(i + 1);
				} else {
					name = line;
					line.clear();
				}
				return name;
			})()) })
			{
				std::invoke(**it, std::move(name), std::move(line));
			}
			else
			{
				Output.Printf("[error] unknown command: %s %s\n", name.c_str(), line.c_str());
			}

			Output.ScrollToBottom = true;
		}

	private:
		int32_t InputTextCallback(ImGuiInputTextCallbackData * data) noexcept
		{
			switch (data->EventFlag)
			{
			// COMPLETION
			case ImGuiInputTextFlags_CallbackCompletion: {

				// locate beginning of current word
				cstring last{ data->Buf + data->CursorPos }, first{ last };
				while (first > data->Buf)
				{
					if (char const c{ first[-1] }
					; util::is_whitespace(c) || c == ',' || c == ';' || c == '/')
					{
						break;
					}
					first--;
				}

				// build list of candidates
				pmr::vector<cstring> candidates{};
				for (size_t i = 0; i < Commands.size(); ++i)
				{
					if (!std::strncmp(Commands[i].Name.c_str(), first, (size_t)(last - first)))
					{
						candidates.push_back(Commands[i].Name.c_str());
					}
				}
				// no matches
				if (candidates.empty())
				{
					Output.Printf("# %.*s\n", (size_t)(last - first), first);
				}
				// single match
				else if (candidates.size() == 1)
				{
					// entirely replace beginning of word
					data->DeleteChars((int32_t)(first - data->Buf), (int32_t)(last - first));
					data->InsertChars(data->CursorPos, candidates[0]);
					data->InsertChars(data->CursorPos, " ");
				}
				// multiple matches
				else
				{
					// complete as much as we can
					auto match_len{ (size_t)(last - first) };
					for (;;)
					{
						size_t c{};
						bool all_match{ true };
						for (size_t i = 0; i < candidates.size() && all_match; ++i)
						{
							if (!i) { c = std::toupper(candidates[i][match_len]); }
							else if (c == 0 || c != std::toupper(candidates[i][match_len])) {
								all_match = false;
							}
						}
						if (!all_match) {
							break;
						}
						match_len++;
					}
					if (match_len > 0) {
						data->DeleteChars((int32_t)(first - data->Buf), (int32_t)(last - first));
						data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
					}

					// display suggestions
					Output.Printf("suggestions:\n");
					for (size_t i = 0; i < candidates.size(); ++i)
					{
						Output.Printf("- %s\n", candidates[i]);
					}
				}
			} break;

			// HISTORY
			case ImGuiInputTextFlags_CallbackHistory: {
				int32_t const prev_pos{ HistoryPos };
				if (data->EventKey == ImGuiKey_UpArrow)
				{
					if (HistoryPos == -1)
					{
						HistoryPos = (int32_t)History.size() - 1;
					}
					else if (HistoryPos > 0)
					{
						HistoryPos--;
					}
				}
				else if (data->EventKey == ImGuiKey_DownArrow)
				{
					if (HistoryPos != -1 && (++HistoryPos >= (int32_t)History.size()))
					{
						HistoryPos = -1;
					}
				}

				if (prev_pos != HistoryPos)
				{
					auto history_str{ (HistoryPos >= 0) ? History[HistoryPos].c_str() : "" };
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