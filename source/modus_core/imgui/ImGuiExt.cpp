#include <modus_core/imgui/ImGuiExt.hpp>

// TEXT LOG
namespace ml::ImGuiExt
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	TextLog::TextLog(cstring default_filter, bool auto_scroll, allocator_type alloc) noexcept
		: Filter		{ default_filter }
		, Lines			{ alloc }
		, AutoScroll	{ auto_scroll }
		, ScrollToBottom{}
	{
	}

	void TextLog::Draw(Printer const & print) noexcept
	{
		// print lines
		ImGui::BeginGroup(); {
			for (size_t i = 0, imax = Lines.size(); i < imax; ++i)
			{
				if (Filter.PassFilter(Lines[i].c_str()))
				{
					print(Lines, i);
				}
			}
		} ImGui::EndGroup();

		// scroll to bottom
		if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())) {
			ImGui::SetScrollHereY(1.0f);
		}
		ScrollToBottom = false;
	}

	TextLog::Printer TextLog::Printer::Default{ [
	](ds::list<pmr::string> const & lines, size_t i) noexcept
	{
		auto check_prefix = [&lines, i](auto str) noexcept -> bool
		{
			return !std::strncmp(lines[i].c_str(), str, std::strlen(str));
		};

		color c;
		if (check_prefix( "# ")) {
			c = { 1.0f, 0.8f, 0.6f, 1.0f };
		}
		else if (check_prefix(ML_DEBUG_MSG_SUCCESS)) {
			c = colors::lime;
		}
		else if (check_prefix(ML_DEBUG_MSG_FAILURE)) {
			c = colors::red;
		}
		else if (check_prefix(ML_DEBUG_MSG_WARNING)) {
			c = colors::yellow;
		}
		else {
			c = colors::white;
		}

		ImGui::PushStyleColor(ImGuiCol_Text, c);
		ImGui::TextUnformatted(lines[i].c_str());
		ImGui::PopStyleColor();
	} };

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// TERMINAL
namespace ml::ImGuiExt
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	Terminal::Terminal(allocator_type alloc) noexcept
		: User		{ alloc }
		, Host		{ alloc }
		, Path		{ alloc }
		, Mode		{ alloc }
		, Input		{}
		, Output	{ alloc }
		, Commands	{ alloc }
		, History	{ alloc }
		, HistoryPos{ -1 }
		, Colors	{}
	{
	}

	void Terminal::DrawPrefixOptions()
	{
		// USER
		char username[32]{}; std::strcpy(username, this->User.c_str());
		ImGui::TextDisabled("user"); ImGui::SameLine();
		if (ImGui::InputText("##username", username, ML_arraysize(username), ImGuiInputTextFlags_EnterReturnsTrue)) {
			this->User = username;
		} ImGui::SameLine();
		ImGui::ColorEdit4("##usercolor", this->Colors.User, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

		// HOST
		char hostname[32]{}; std::strcpy(hostname, this->Host.c_str());
		ImGui::TextDisabled("host"); ImGui::SameLine();
		if (ImGui::InputText("##hostname", hostname, ML_arraysize(hostname), ImGuiInputTextFlags_EnterReturnsTrue)) {
			this->Host = hostname;
		} ImGui::SameLine();
		ImGui::ColorEdit4("##hostcolor", this->Colors.Host, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

		// PATH
		char pathname[32]{}; std::strcpy(pathname, this->Path.c_str());
		ImGui::TextDisabled("path"); ImGui::SameLine();
		if (ImGui::InputText("##pathname", pathname, ML_arraysize(pathname), ImGuiInputTextFlags_EnterReturnsTrue)) {
			this->Path = pathname;
		} ImGui::SameLine();
		ImGui::ColorEdit4("##pathcolor", this->Colors.Path, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

		// MODE
		char modename[32]{}; std::strcpy(modename, this->Mode.c_str());
		ImGui::TextDisabled("mode"); ImGui::SameLine();
		if (ImGui::InputText("##modename", modename, ML_arraysize(modename), ImGuiInputTextFlags_EnterReturnsTrue)) {
			this->Mode = modename;
		} ImGui::SameLine();
		ImGui::ColorEdit4("##modecolor", this->Colors.Mode, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	}

	void Terminal::DrawPrefix()
	{
		ImGui::BeginGroup(); ML_defer(&) { ImGui::EndGroup(); };
		ImGui::AlignTextToFramePadding();

		// PREFIX user@host:path$ /mode
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_Text, Colors.Delim);
		if (!User.empty()) {
			ImGui::PushStyleColor(ImGuiCol_Text, Colors.User);
			ImGui::Text("%.*s", User.size(), User.data());
			ImGui::PopStyleColor(); ImGui::SameLine();
		}
		ImGui::Text("@"); ImGui::SameLine();
		if (!Host.empty()) {
			ImGui::PushStyleColor(ImGuiCol_Text, Colors.Host);
			ImGui::Text("%.*s", Host.size(), Host.data());
			ImGui::PopStyleColor(); ImGui::SameLine();
		}
		ImGui::Text(":"); ImGui::SameLine();
		if (!Path.empty()) {
			ImGui::PushStyleColor(ImGuiCol_Text, Colors.Path);
			ImGui::Text("%.*s", Path.size(), Path.data());
			ImGui::PopStyleColor(); ImGui::SameLine();
		}
		ImGui::Text("$"); ImGui::SameLine();
		if (!Mode.empty()) {
			ImGui::Text(" /"); ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, Colors.Mode);
			ImGui::Text("%.*s", Mode.size(), Mode.data());
			ImGui::PopStyleColor(); ImGui::SameLine();
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}

	void Terminal::DrawInput()
	{
		ImGui::BeginGroup(); ML_defer(&) { ImGui::EndGroup(); };
		ImGui::AlignTextToFramePadding();

		bool reclaim_focus{};
		ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
		if (ImGui::InputText(
			"##input_text", Input.data(), Input.max_size(),
			ImGuiInputTextFlags_EnterReturnsTrue |
			ImGuiInputTextFlags_CallbackCompletion |
			ImGuiInputTextFlags_CallbackHistory,
			[](auto * u) { return ((Terminal *)u->UserData)->InputTextCallbackStub(u); },
			this
		))
		{
			if (Input) {
				Execute(Input.data());
				Output.ScrollToBottom = true;
			}
			std::strcpy(Input, "");
			reclaim_focus = true;
		}
		ImGui::PopItemWidth();
		ImGui::SetItemDefaultFocus(); // focus on window apparition
		if (reclaim_focus) { ImGui::SetKeyboardFocusHere(-1); } // focus previous widget
	}

	int32 Terminal::Execute(Line line)
	{
		// empty check
		if (util::trim(line).empty()) { return debug::failure(); }
		
		// append line
		Output.Printf("# %s\n", line.c_str());
		HistoryPos = -1;
		if (auto const it{ std::find(History.begin(), History.end(), line) }
		; it != History.end()) {
			History.erase(it);
		} History.push_back(line);

		// validate line
		if ((line.front() != '/' && Mode.empty()) || util::trim_front(line, [
		](char c) { return c == '/' || util::is_whitespace(c); }).empty()) {
			return debug::failure();
		}

		// process command
		if (ds::string name; auto const proc{ GetProc(std::invoke([&]() noexcept -> auto &
		{
			if (!Mode.empty()) {
				name = Mode;
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
		})) })
		{
			std::invoke(*proc, std::move(line));

			return debug::success();
		}
		else
		{
			return debug::failure("unknown command: {0} {1}", name, line);
		}
	}

	int32 Terminal::InputTextCallbackStub(ImGuiInputTextCallbackData * data)
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
			ds::list<cstring> candidates{};
			for (auto const & name : Commands.get<ds::string>())
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
				data->DeleteChars((int32)(first - data->Buf), (int32)(last - first));
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
					data->DeleteChars((int32)(first - data->Buf), (int32)(last - first));
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
			int32 const prev_pos{ HistoryPos };
			if (data->EventKey == ImGuiKey_UpArrow)
			{
				if (HistoryPos == -1)
				{
					HistoryPos = (int32)History.size() - 1;
				}
				else if (HistoryPos > 0)
				{
					HistoryPos--;
				}
			}
			else if (data->EventKey == ImGuiKey_DownArrow)
			{
				if (HistoryPos != -1 && (++HistoryPos >= (int32)History.size()))
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
}