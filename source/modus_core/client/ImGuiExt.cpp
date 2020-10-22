#include <modus_core/client/ImGuiExt.hpp>

// OUTPUT LOG
namespace ml::ImGuiExt
{
	OutputLog::Printer OutputLog::Printer::Default{ [](auto const & lines, auto i) noexcept
	{
		color c{ colors::white };
		if (!std::strncmp(lines[i].c_str(), "# ", 2)) {
			c = { 1.0f, 0.8f, 0.6f, 1.0f };
		}
		else if (!std::strncmp(lines[i].c_str(), "[error] ", 8)) {
			c = colors::red;
		}

		ImGui::PushStyleColor(ImGuiCol_Text, c);
		ImGui::TextUnformatted(lines[i].c_str());
		ImGui::PopStyleColor();
	} };

	OutputLog::OutputLog(cstring default_filter, bool auto_scroll, allocator_type alloc) noexcept
		: Filter		{ default_filter }
		, Lines			{ alloc }
		, AutoScroll	{ auto_scroll }
		, ScrollToBottom{}
	{
	}

	void OutputLog::Draw(Printer const & print)
	{
		// draw lines
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
}

// COMMAND LINE
namespace ml::ImGuiExt
{
	CommandLine::CommandLine(allocator_type alloc) noexcept
		: User		{ alloc }
		, Host		{ alloc }
		, Path		{ alloc }
		, Mode		{ alloc }
		, In		{}
		, Out		{ alloc }
		, Commands	{ alloc }
		, History	{ alloc }
		, HistoryPos{ -1 }
		, Colors	{}
	{
	}

	void CommandLine::Draw(Printer const & print, bool prefix)
	{
		// OUTPUT
		ImGuiExt::DrawChild(
			"##output area",
			{ 0, -(ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing()) },
			true,
			ImGuiWindowFlags_HorizontalScrollbar,
			&OutputLog::Draw, &Out, print);
		
		// INPUT
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
		ImGuiExt::DrawChild(
			"##input area",
			{},
			false,
			ImGuiWindowFlags_NoScrollbar,
			&CommandLine::DrawInput, this, "##input text", prefix);
		ImGui::PopStyleVar(1);
	}

	void CommandLine::DrawInput(cstring label, bool prefix)
	{
		ImGui::AlignTextToFramePadding();

		// PREFIX user@host:path$ /mode
		if (prefix) {
			ImGui::BeginGroup();
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
			ImGui::Text("$ "); ImGui::SameLine();
			if (!Mode.empty()) {
				ImGui::Text("/"); ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, Colors.Mode);
				ImGui::Text("%.*s ", Mode.size(), Mode.data());
				ImGui::PopStyleColor(); ImGui::SameLine();
			}
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
			ImGui::EndGroup();
			ImGui::SameLine();
		}

		// INPUT TEXT
		bool reclaim_focus{};
		ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
		if (ImGui::InputText(
			label, In.data(), In.max_size(),
			ImGuiInputTextFlags_EnterReturnsTrue |
			ImGuiInputTextFlags_CallbackCompletion |
			ImGuiInputTextFlags_CallbackHistory,
			[](auto * u) { return ((CommandLine *)u->UserData)->InputTextCallbackStub(u); },
			this
		))
		{
			if (In)
			{
				Out.Printf("# %s\n", In.data());
				
				Execute({ In.data() });
				
				Out.ScrollToBottom = true;
			}
			std::strcpy(In, "");
			reclaim_focus = true;
		}
		ImGui::PopItemWidth();
		ImGui::SetItemDefaultFocus(); // focus on window apparition
		if (reclaim_focus) { ImGui::SetKeyboardFocusHere(-1); } // focus previous widget
	}

	int32_t CommandLine::Execute(Line && line)
	{
		// nothing to do
		if (line.empty()) { return debug::error(); }

		// add to history
		HistoryPos = -1;
		if (auto const it{ std::find(History.begin(), History.end(), line) }
		; it != History.end()) { History.erase(it); }
		History.push_back(line);

		// validate text
		if (!std::invoke([&]() noexcept
		{
			using namespace util;
			return !trim(line).empty()
				&& (line.front() == '/' || !Mode.empty())
				&& !trim_front(line, [
				](char c) { return c == '/' || is_whitespace(c); }).empty();
		}))
		{
			return debug::error("invalid command syntax");
		}
		
		// process command
		if (CommandName name; auto const proc{ GetProc(std::invoke([&]() noexcept -> auto &
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
		})) })
		{
			std::invoke(*proc, std::move(line));
			return debug::ok();
		}
		else
		{
			return debug::error("unknown command: {0} {1}", name, line);
		}
	}

	int32_t CommandLine::InputTextCallbackStub(ImGuiInputTextCallbackData * data)
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
			for (auto const & name : Commands.get<CommandName>())
			{
				if (!std::strncmp(name.c_str(), first, (size_t)(last - first)))
				{
					candidates.push_back(name.c_str());
				}
			}
			// no matches
			if (candidates.empty())
			{
				Out.Printf("# %.*s\n", (size_t)(last - first), first);
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
				Out.Printf("suggestions:\n");
				for (size_t i = 0; i < candidates.size(); ++i)
				{
					Out.Printf("- %s\n", candidates[i]);
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