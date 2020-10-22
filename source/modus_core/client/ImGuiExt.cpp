#include <modus_core/client/ImGuiExt.hpp>

// OUTPUT TEXT
namespace ml::ImGuiExt
{
	void OutputText::Draw() noexcept
	{
		// draw lines
		LinePrinter printer{ Printer ? Printer : &DefaultPrinter };
		for (pmr::string const & line : Lines)
		{
			if (Filter.PassFilter(line.c_str()))
			{
				printer(line);
			}
		}

		// scroll to bottom
		if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())) {
			ImGui::SetScrollHereY(1.0f);
		} ScrollToBottom = false;
	}

	void OutputText::DefaultPrinter(pmr::string const & line) noexcept
	{
		// color
		color c{};
		bool has_color{};
		if (std::strstr(line.c_str(), "[error]")) {
			c = colors::red; has_color = true;
		}
		else if (!std::strncmp(line.c_str(), "# ", 2)) {
			c = { 1.0f, 0.8f, 0.6f, 1.0f }; has_color = true;
		}
		// text
		if (has_color) { ImGui::PushStyleColor(ImGuiCol_Text, c); }
		ImGui::TextUnformatted(line.c_str());
		if (has_color) { ImGui::PopStyleColor(); }
	}
}

// COMMAND LINE
namespace ml::ImGuiExt
{
	CommandLine::CommandLine(allocator_type alloc) noexcept
		: Colors	{}
		, Commands	{ alloc }
		, Output	{ alloc }
		, Input		{}
		, History	{ alloc }
		, HistoryPos{ -1 }
		, User		{ alloc }
		, Host		{ alloc }
		, Path		{ alloc }
		, Mode		{ alloc }
	{
	}

	void CommandLine::Draw() noexcept
	{
		// OUTPUT AREA
		auto const input_height{
			ImGui::GetStyle().ItemSpacing.y +
			ImGui::GetFrameHeightWithSpacing()
		};
		ImGui::BeginChild("##output area", { 0, -input_height }, false, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4, 1 });
		Output.Draw();
		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::Separator();

		// INPUT AREA
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4, 4 });
		ImGui::BeginChild("##input area", {}, false, ImGuiWindowFlags_MenuBar);
		ImGui::BeginMenuBar();
		{
			// PREFIX user@host:path$ /mode
			ImGui::BeginGroup();
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
			ImGui::EndGroup();

			// INPUT TEXT
			bool reclaim_focus{};
			ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
			if (ImGui::InputText(
				"##input text", Input.data(), Input.size(),
				ImGuiInputTextFlags_EnterReturnsTrue |
				ImGuiInputTextFlags_CallbackCompletion |
				ImGuiInputTextFlags_CallbackHistory,
				[](auto * u) { return ((CommandLine *)u->UserData)->InputTextCallbackStub(u); },
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

	void CommandLine::Execute(pmr::string line) noexcept
	{
		// empty, nothing to do
		if (line.empty()) { return; }

		// append line
		Output.Printf("# %s\n", line.c_str());
		HistoryPos = -1;
		if (auto const it{ std::find(History.begin(), History.end(), line) }
		; it != History.end()) { History.erase(it); }
		History.push_back(line);

		// process line
		if (!std::invoke([&]() {
			using namespace util;
			return !trim(line).empty()
				&& (line.front() == '/' || !Mode.empty())
				&& !trim_front(line, [](char c) { return c == '/' || is_whitespace(c); }).empty()
				;
		})) {
			return;
		}
		// process command
		else if (pmr::string name; auto const clbk{ GetCallback(([&]() noexcept -> pmr::string &
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
			std::invoke(ML_check(*clbk), std::move(line));
		}
		else
		{
			Output.Printf("[error] unknown command: %s %s\n", name.c_str(), line.c_str());
		}

		Output.ScrollToBottom = true;
	}

	int32_t CommandLine::InputTextCallbackStub(ImGuiInputTextCallbackData * data) noexcept
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
			for (auto const & name : Commands.get<pmr::string>())
			{
				if (!std::strncmp(name.c_str(), first, (size_t)(last - first)))
				{
					candidates.push_back(name.c_str());
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
}