#include <modus_core/embed/Python.hpp>
#include <modus_core/runtime/CoreApplication.hpp>
#include <modus_core/window/WindowAPI.hpp>

PYBIND11_EMBEDDED_MODULE(modus, m)
{
	using namespace ml;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// exit
	m.def("exit", [](py::args) { ML_get_global(core_application)->quit(); });
	py::module::import("builtins").attr("exit") = m.attr("exit");
	py::module::import("sys").attr("exit") = m.attr("exit");

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	py::class_<non_copyable, scary<non_copyable>>(m, "non_copyable");

	py::class_<trackable, scary<trackable>>(m, "trackable");

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct ml_info {};
	py::class_<ml_info>(m, "info")
		.def_property_readonly_static("lib_auth"		, [](py::object) { return ML_lib_author; })
		.def_property_readonly_static("lib_name"		, [](py::object) { return ML_lib_name; })
		.def_property_readonly_static("lib_url"			, [](py::object) { return ML_lib_url; })
		.def_property_readonly_static("lib_ver"			, [](py::object) { return ML_lib_ver; })
		.def_property_readonly_static("is_debug"		, [](py::object) { return ML_is_debug; })
		.def_property_readonly_static("configuration"	, [](py::object) { return ML_is_debug ? "debug" : "release"; })
		.def_property_readonly_static("cc_lang"			, [](py::object) { return ML_cc_lang; })
		.def_property_readonly_static("cc_name"			, [](py::object) { return ML_cc_name; })
		.def_property_readonly_static("cc_ver"			, [](py::object) { return ML_cc_ver; })
		.def_property_readonly_static("os"				, [](py::object) { return ML_os_name; })
		.def_property_readonly_static("arch"			, [](py::object) { return ML_arch; })
		.def_property_readonly_static("cpu"				, [](py::object) { return ML_cpu; })
		;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct py_stdio final
	{
		struct output final
		{
			std::reference_wrapper<std::ostream> m_os{ std::cout };

			int32 fileno() const noexcept {
				if (auto const addr{ std::addressof(m_os.get()) }; !addr) { return -2; }
				else if (addr == std::addressof(std::cout)) { return 1; }
				else if (addr == std::addressof(std::cerr)) { return 2; }
				else { return -1; }
			}

			void flush() noexcept { m_os.get().flush(); }

			void write(py::object o) noexcept { m_os << o; }

			void writelines(py::list l) noexcept { for (auto const & e : l) { m_os << e; } }
		};

		static output cerr(py::object) noexcept { return { std::cerr }; }

		static output cout(py::object) noexcept { return { std::cout }; }
	};
	py::class_<py_stdio::output>(m, "output")
		.def(py::init<>())
		.def("closed"		, []() { return false; })
		.def("isatty"		, []() { return false; })
		.def("readable"		, []() { return false; })
		.def("seekable"		, []() { return false; })
		.def("writable"		, []() { return true; })
		.def("fileno"		, &py_stdio::output::fileno)
		.def("flush"		, &py_stdio::output::flush)
		.def("write"		, &py_stdio::output::write)
		.def("writelines"	, &py_stdio::output::writelines)
		;
	py::class_<py_stdio>(m, "stdio")
		.def(py::init<>())
		.def_property_readonly_static("cerr", &py_stdio::cerr)
		.def_property_readonly_static("cout", &py_stdio::cout)
		;
	{
		auto sys{ py::module::import("sys") };
		sys.attr("stdout") = m.attr("stdio").attr("cout");
		sys.attr("stderr") = m.attr("stdio").attr("cout");
		sys.attr("stdin") = py::none{};
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	py::class_<json>(m, "json") // FIXME
		.def(py::init<>())
		.def(py::init<json const &>())
		.def(py::init<py::handle>())

		.def_static("parse", [](cstring s) { return json::parse(s); })

		.def("__setitem__", [](json & j, size_t k, json const & v)	{ j[k] = v; }, py::is_operator())
		.def("__setitem__", [](json & j, cstring k, json const & v) { j[k] = v; }, py::is_operator())
		.def("__setitem__", [](json & j, size_t k, py::handle v)	{ j[k] = v; }, py::is_operator())
		.def("__setitem__", [](json & j, cstring k, py::handle v)	{ j[k] = v; }, py::is_operator())

		.def("__getitem__", [](json & j, size_t k) -> json & { return j[k]; }, py::is_operator())
		.def("__getitem__", [](json & j, cstring k) -> json & { return j[k]; }, py::is_operator())

		.def("__bool__"	, [](json const & j) { return j.get<py::bool_>(); })
		.def("__int__"	, [](json const & j) { return j.get<py::int_>(); })
		.def("__float__", [](json const & j) { return j.get<py::float_>(); })
		.def("__str__"	, [](json const & j) { return j.dump(); })
		.def("__repr__"	, [](json const & j) { return j.dump(); })
		;
	m	// json
		.def("to_json"	, [](py::handle h) { return json{ h }.dump(); })
		.def("from_json", [](cstring s) { return json{ s }.get<py::handle>(); })
		;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// MEMORY

	auto py_mem{ m.def_submodule("memory") };

	using pmr_allocator = pmr::polymorphic_allocator<byte>;

	py::class_<pmr_allocator>(py_mem, "allocator")
		.def(py::init<>())
		.def(py::init([](intptr_t p) { return pmr_allocator{ (pmr::memory_resource *)p }; }))
		.def(py::init<pmr_allocator const &>())
		.def("allocate", [](pmr_allocator & o, size_t s) { return (intptr_t)o.allocate(s); })
		.def("deallocate", [](pmr_allocator & o, intptr_t p, size_t s) { o.deallocate((byte *)p, s); })
		;

	py::class_<memory_record>(py_mem, "record")
		.def(py::init<>())
		.def(py::init<memory_record const &>())
		.def(py::init([g = ML_memory_manager()](intptr_t p) -> memory_record
		{
			if (auto const i{ g->get_storage().lookup<memory_manager::id_addr>((byte *)p) }
			; i != g->get_storage().npos) {
				return g->get_record(i);
			} else {
				return {};
			}
		}))
		.def_property_readonly("addr", [](memory_record const & o) { return (intptr_t)o.addr; })
		.def_readonly("index", &memory_record::index)
		.def_readonly("count", &memory_record::count)
		.def_readonly("size", &memory_record::size)
		.def("__nonzero__", &memory_record::operator bool)
		.def("__repr__", [](memory_record const & o) { return json{ o }.dump(); })
		.def("__str__", [](memory_record const & o) { return json{ o }.dump(); })
		;

	auto memget = [](intptr_t p, size_t s) -> py::int_
	{
		switch (s)
		{
		default	: return 0;
		case 1	: return *(uint8 *)p;
		case 2	: return *(uint16 *)p;
		case 3	: return *(uint32 *)p;
		case 4	: return *(uint64 *)p;
		}
	};

	py_mem // memory
		// passthrough resource
		.def("num_allocations", []() { return ML_memory_manager()->get_resource()->num_allocations(); })
		.def("buffer_base", []() { return ML_memory_manager()->get_resource()->buffer_base(); })
		.def("buffer_free", []() { return ML_memory_manager()->get_resource()->buffer_free(); })
		.def("buffer_size", []() { return ML_memory_manager()->get_resource()->buffer_size(); })
		.def("buffer_used", []() { return ML_memory_manager()->get_resource()->buffer_used(); })

		// allocation
		.def("malloc"	, [](size_t s) { return (intptr_t)ML_memory_manager()->allocate(s); })
		.def("calloc"	, [](size_t c, size_t s) { return (intptr_t)ML_memory_manager()->allocate(c, s); })
		.def("free"		, [](intptr_t p) { ML_memory_manager()->deallocate((void *)p); })
		.def("realloc"	, [](intptr_t p, size_t s) { return (intptr_t)ML_memory_manager()->reallocate((void *)p, s); })
		.def("realloc"	, [](intptr_t p, size_t o, size_t n) { return (intptr_t)ML_memory_manager()->reallocate((void *)p, o, n); })

		// getters
		.def("memget"	, [&memget](intptr_t p) { return memget(p, 1); })
		.def("memget"	, [&memget](intptr_t p, size_t s) { return memget(p, s); })
		.def("memget"	, [&memget](intptr_t p, size_t s, size_t n)
		{
			py::list temp{};
			for (intptr_t first = p, last = p + (ptrdiff_t)(s * n); first != last; ++first)
			{
				temp.append(memget(first, s));
			}
			return temp;
		})
		.def("strget"	, [](intptr_t p) { return (cstring)p; })
		.def("strget"	, [](intptr_t l, intptr_t r) { return std::string{ (cstring)l, (cstring)r }; })

		// memchr
		.def("memchr"	, [](intptr_t p, int32 v, size_t n) { return (intptr_t)std::memchr((void *)p, v, n); })
		// memcmp
		.def("memcmp"	, [](intptr_t l, intptr_t r, size_t n) { return std::memcmp((void *)l, (void *)r, n); })
		// memcpy
		.def("memcpy"	, [](intptr_t d, intptr_t s, size_t n) { return (intptr_t)std::memcpy((void *)d, (void *)s, n); })
		// memmove
		.def("memmove"	, [](intptr_t d, intptr_t s, size_t n) { return (intptr_t)std::memmove((void *)d, (void *)s, n); })
		// memset
		.def("memset"	, [](intptr_t d, int32 v, size_t n) { return (intptr_t)std::memset((void *)d, v, n); })
		// strcpy
		.def("strcpy"	, [](intptr_t d, intptr_t s) { return (intptr_t)std::strcpy((char *)d, (char *)s); })
		.def("strcpy"	, [](intptr_t d, cstring s) { return (intptr_t)std::strcpy((char *)d, s); })
		// strncpy
		.def("strncpy"	, [](intptr_t d, intptr_t s, size_t n) { return (intptr_t)std::strncpy((char *)d, (char *)s, n); })
		.def("strncpy"	, [](intptr_t d, cstring s, size_t n) { return (intptr_t)std::strncpy((char *)d, s, n); })
		// strcat
		.def("strcat"	, [](intptr_t d, intptr_t s) { return (intptr_t)std::strcat((char *)d, (char *)s); })
		.def("strcat"	, [](intptr_t d, cstring s) { return (intptr_t)std::strcat((char *)d, s); })
		// strncat
		.def("strncat"	, [](intptr_t d, intptr_t s, size_t n) { return (intptr_t)std::strncat((char *)d, (char *)s, n); })
		.def("strncat"	, [](intptr_t d, cstring s, size_t n) { return (intptr_t)std::strncat((char *)d, s, n); })
		// strlen
		.def("strlen"	, [](intptr_t p) { return std::strlen((cstring)p); })
		;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	//py::class_<event, non_copyable>(m, "event")
	//	.def("__int__", &event::operator hash_t)
	//	;
	//
	//struct py_event_listener : event_listener
	//{
	//	void on_event(event const & ev) override
	//	{
	//		PYBIND11_OVERLOAD_PURE(void, event_listener, on_event, ev);
	//	}
	//};
	//
	//py::class_<event_listener, py_event_listener>(m, "event_listener")
	//	.def(py::init<event_bus *>())
	//	.def("on_event", &event_listener::on_event)
	//	;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// CONTEXT API
	py::class_<context_api_>(m, "context_api")
		.def_property_readonly_static("unknown"	, [](py::object) { return (int32)context_api_unknown; })
		.def_property_readonly_static("opengl"	, [](py::object) { return (int32)context_api_opengl; })
		.def_property_readonly_static("vulkan"	, [](py::object) { return (int32)context_api_vulkan; })
		.def_property_readonly_static("directx"	, [](py::object) { return (int32)context_api_directx; })
		;

	// CONTEXT PROFILE
	py::class_<context_profile_>(m, "context_profile")
		.def_property_readonly_static("any"			, [](py::object) { return (int32)context_profile_any; })
		.def_property_readonly_static("core"		, [](py::object) { return (int32)context_profile_core; })
		.def_property_readonly_static("compat"		, [](py::object) { return (int32)context_profile_compat; })
		.def_property_readonly_static("debug"		, [](py::object) { return (int32)context_profile_debug; })
		;

	// CURSOR MODE
	py::class_<cursor_mode_>(m, "cursor_mode")
		.def_property_readonly_static("normal"		, [](py::object) { return (int32)cursor_mode_normal; })
		.def_property_readonly_static("hidden"		, [](py::object) { return (int32)cursor_mode_hidden; })
		.def_property_readonly_static("disabled"	, [](py::object) { return (int32)cursor_mode_disabled; })
		;

	// CURSOR SHAPE
	py::class_<cursor_shape_>(m, "cursor_shape")
		.def_property_readonly_static("arrow"		, [](py::object) { return (int32)cursor_shape_arrow; })
		.def_property_readonly_static("ibeam"		, [](py::object) { return (int32)cursor_shape_ibeam; })
		.def_property_readonly_static("crosshair"	, [](py::object) { return (int32)cursor_shape_crosshair; })
		.def_property_readonly_static("hand"		, [](py::object) { return (int32)cursor_shape_hand; })
		.def_property_readonly_static("ew"			, [](py::object) { return (int32)cursor_shape_ew; })
		.def_property_readonly_static("ns"			, [](py::object) { return (int32)cursor_shape_ns; })
		.def_property_readonly_static("nesw"		, [](py::object) { return (int32)cursor_shape_nesw; })
		.def_property_readonly_static("nwse"		, [](py::object) { return (int32)cursor_shape_nwse; })
		;

	// KEY CODE
	py::class_<keycode_>(m, "keycode")
		.def_property_readonly_static("space"			, [](py::object) { return (int32)keycode_space; })
		.def_property_readonly_static("apostrophe"		, [](py::object) { return (int32)keycode_apostrophe; })
		.def_property_readonly_static("comma"			, [](py::object) { return (int32)keycode_comma; })
		.def_property_readonly_static("minus"			, [](py::object) { return (int32)keycode_minus; })
		.def_property_readonly_static("period"			, [](py::object) { return (int32)keycode_period; })
		.def_property_readonly_static("slash"			, [](py::object) { return (int32)keycode_slash; })
		.def_property_readonly_static("num_0"			, [](py::object) { return (int32)keycode_num_0; })
		.def_property_readonly_static("num_1"			, [](py::object) { return (int32)keycode_num_1; })
		.def_property_readonly_static("num_2"			, [](py::object) { return (int32)keycode_num_2; })
		.def_property_readonly_static("num_3"			, [](py::object) { return (int32)keycode_num_3; })
		.def_property_readonly_static("num_4"			, [](py::object) { return (int32)keycode_num_4; })
		.def_property_readonly_static("num_5"			, [](py::object) { return (int32)keycode_num_5; })
		.def_property_readonly_static("num_6"			, [](py::object) { return (int32)keycode_num_6; })
		.def_property_readonly_static("num_7"			, [](py::object) { return (int32)keycode_num_7; })
		.def_property_readonly_static("num_8"			, [](py::object) { return (int32)keycode_num_8; })
		.def_property_readonly_static("num_9"			, [](py::object) { return (int32)keycode_num_9; })
		.def_property_readonly_static("semicolon"		, [](py::object) { return (int32)keycode_semicolon; })
		.def_property_readonly_static("equal"			, [](py::object) { return (int32)keycode_equal; })
		.def_property_readonly_static("a"				, [](py::object) { return (int32)keycode_a; })
		.def_property_readonly_static("b"				, [](py::object) { return (int32)keycode_b; })
		.def_property_readonly_static("c"				, [](py::object) { return (int32)keycode_c; })
		.def_property_readonly_static("d"				, [](py::object) { return (int32)keycode_d; })
		.def_property_readonly_static("e"				, [](py::object) { return (int32)keycode_e; })
		.def_property_readonly_static("f"				, [](py::object) { return (int32)keycode_f; })
		.def_property_readonly_static("g"				, [](py::object) { return (int32)keycode_g; })
		.def_property_readonly_static("h"				, [](py::object) { return (int32)keycode_h; })
		.def_property_readonly_static("i"				, [](py::object) { return (int32)keycode_i; })
		.def_property_readonly_static("j"				, [](py::object) { return (int32)keycode_j; })
		.def_property_readonly_static("k"				, [](py::object) { return (int32)keycode_k; })
		.def_property_readonly_static("l"				, [](py::object) { return (int32)keycode_l; })
		.def_property_readonly_static("m"				, [](py::object) { return (int32)keycode_m; })
		.def_property_readonly_static("n"				, [](py::object) { return (int32)keycode_n; })
		.def_property_readonly_static("o"				, [](py::object) { return (int32)keycode_o; })
		.def_property_readonly_static("p"				, [](py::object) { return (int32)keycode_p; })
		.def_property_readonly_static("q"				, [](py::object) { return (int32)keycode_q; })
		.def_property_readonly_static("r"				, [](py::object) { return (int32)keycode_r; })
		.def_property_readonly_static("s"				, [](py::object) { return (int32)keycode_s; })
		.def_property_readonly_static("t"				, [](py::object) { return (int32)keycode_t; })
		.def_property_readonly_static("u"				, [](py::object) { return (int32)keycode_u; })
		.def_property_readonly_static("v"				, [](py::object) { return (int32)keycode_v; })
		.def_property_readonly_static("w"				, [](py::object) { return (int32)keycode_w; })
		.def_property_readonly_static("x"				, [](py::object) { return (int32)keycode_x; })
		.def_property_readonly_static("y"				, [](py::object) { return (int32)keycode_y; })
		.def_property_readonly_static("z"				, [](py::object) { return (int32)keycode_z; })
		.def_property_readonly_static("left_bracket"	, [](py::object) { return (int32)keycode_left_bracket; })
		.def_property_readonly_static("backslash"		, [](py::object) { return (int32)keycode_backslash; })
		.def_property_readonly_static("right_bracket"	, [](py::object) { return (int32)keycode_right_bracket; })
		.def_property_readonly_static("grave_accent"	, [](py::object) { return (int32)keycode_grave_accent; })
		.def_property_readonly_static("world_1"			, [](py::object) { return (int32)keycode_world_1; })
		.def_property_readonly_static("world_2"			, [](py::object) { return (int32)keycode_world_2; })
		.def_property_readonly_static("escape"			, [](py::object) { return (int32)keycode_escape; })
		.def_property_readonly_static("enter"			, [](py::object) { return (int32)keycode_enter; })
		.def_property_readonly_static("tab"				, [](py::object) { return (int32)keycode_tab; })
		.def_property_readonly_static("backspace"		, [](py::object) { return (int32)keycode_backspace; })
		.def_property_readonly_static("insert"			, [](py::object) { return (int32)keycode_insert; })
		.def_property_readonly_static("del"				, [](py::object) { return (int32)keycode_del; })
		.def_property_readonly_static("right"			, [](py::object) { return (int32)keycode_right; })
		.def_property_readonly_static("left"			, [](py::object) { return (int32)keycode_left; })
		.def_property_readonly_static("down"			, [](py::object) { return (int32)keycode_down; })
		.def_property_readonly_static("up"				, [](py::object) { return (int32)keycode_up; })
		.def_property_readonly_static("page_up"			, [](py::object) { return (int32)keycode_page_up; })
		.def_property_readonly_static("page_down"		, [](py::object) { return (int32)keycode_page_down; })
		.def_property_readonly_static("home"			, [](py::object) { return (int32)keycode_home; })
		.def_property_readonly_static("end"				, [](py::object) { return (int32)keycode_end; })
		.def_property_readonly_static("caps_lock"		, [](py::object) { return (int32)keycode_caps_lock; })
		.def_property_readonly_static("scroll_lock"		, [](py::object) { return (int32)keycode_scroll_lock; })
		.def_property_readonly_static("num_lock"		, [](py::object) { return (int32)keycode_num_lock; })
		.def_property_readonly_static("print_screen"	, [](py::object) { return (int32)keycode_print_screen; })
		.def_property_readonly_static("pause"			, [](py::object) { return (int32)keycode_pause; })
		.def_property_readonly_static("fn_1"			, [](py::object) { return (int32)keycode_fn_1; })
		.def_property_readonly_static("fn_2"			, [](py::object) { return (int32)keycode_fn_2; })
		.def_property_readonly_static("fn_3"			, [](py::object) { return (int32)keycode_fn_3; })
		.def_property_readonly_static("fn_4"			, [](py::object) { return (int32)keycode_fn_4; })
		.def_property_readonly_static("fn_5"			, [](py::object) { return (int32)keycode_fn_5; })
		.def_property_readonly_static("fn_6"			, [](py::object) { return (int32)keycode_fn_6; })
		.def_property_readonly_static("fn_7"			, [](py::object) { return (int32)keycode_fn_7; })
		.def_property_readonly_static("fn_8"			, [](py::object) { return (int32)keycode_fn_8; })
		.def_property_readonly_static("fn_9"			, [](py::object) { return (int32)keycode_fn_9; })
		.def_property_readonly_static("fn_10"			, [](py::object) { return (int32)keycode_fn_10; })
		.def_property_readonly_static("fn_11"			, [](py::object) { return (int32)keycode_fn_11; })
		.def_property_readonly_static("fn_12"			, [](py::object) { return (int32)keycode_fn_12; })
		.def_property_readonly_static("fn_13"			, [](py::object) { return (int32)keycode_fn_13; })
		.def_property_readonly_static("fn_14"			, [](py::object) { return (int32)keycode_fn_14; })
		.def_property_readonly_static("fn_15"			, [](py::object) { return (int32)keycode_fn_15; })
		.def_property_readonly_static("fn_16"			, [](py::object) { return (int32)keycode_fn_16; })
		.def_property_readonly_static("fn_17"			, [](py::object) { return (int32)keycode_fn_17; })
		.def_property_readonly_static("fn_18"			, [](py::object) { return (int32)keycode_fn_18; })
		.def_property_readonly_static("fn_19"			, [](py::object) { return (int32)keycode_fn_19; })
		.def_property_readonly_static("fn_20"			, [](py::object) { return (int32)keycode_fn_20; })
		.def_property_readonly_static("fn_21"			, [](py::object) { return (int32)keycode_fn_21; })
		.def_property_readonly_static("fn_22"			, [](py::object) { return (int32)keycode_fn_22; })
		.def_property_readonly_static("fn_23"			, [](py::object) { return (int32)keycode_fn_23; })
		.def_property_readonly_static("fn_24"			, [](py::object) { return (int32)keycode_fn_24; })
		.def_property_readonly_static("fn_25"			, [](py::object) { return (int32)keycode_fn_25; })
		.def_property_readonly_static("kp_0"			, [](py::object) { return (int32)keycode_kp_0; })
		.def_property_readonly_static("kp_1"			, [](py::object) { return (int32)keycode_kp_1; })
		.def_property_readonly_static("kp_2"			, [](py::object) { return (int32)keycode_kp_2; })
		.def_property_readonly_static("kp_3"			, [](py::object) { return (int32)keycode_kp_3; })
		.def_property_readonly_static("kp_4"			, [](py::object) { return (int32)keycode_kp_4; })
		.def_property_readonly_static("kp_5"			, [](py::object) { return (int32)keycode_kp_5; })
		.def_property_readonly_static("kp_6"			, [](py::object) { return (int32)keycode_kp_6; })
		.def_property_readonly_static("kp_7"			, [](py::object) { return (int32)keycode_kp_7; })
		.def_property_readonly_static("kp_8"			, [](py::object) { return (int32)keycode_kp_8; })
		.def_property_readonly_static("kp_9"			, [](py::object) { return (int32)keycode_kp_9; })
		.def_property_readonly_static("kp_decimal"		, [](py::object) { return (int32)keycode_kp_decimal; })
		.def_property_readonly_static("kp_divide"		, [](py::object) { return (int32)keycode_kp_divide; })
		.def_property_readonly_static("kp_multiply"		, [](py::object) { return (int32)keycode_kp_multiply; })
		.def_property_readonly_static("kp_subtract"		, [](py::object) { return (int32)keycode_kp_subtract; })
		.def_property_readonly_static("kp_add"			, [](py::object) { return (int32)keycode_kp_add; })
		.def_property_readonly_static("kp_enter"		, [](py::object) { return (int32)keycode_kp_enter; })
		.def_property_readonly_static("kp_equal"		, [](py::object) { return (int32)keycode_kp_equal; })
		.def_property_readonly_static("left_shift"		, [](py::object) { return (int32)keycode_left_shift; })
		.def_property_readonly_static("left_control"	, [](py::object) { return (int32)keycode_left_ctrl; })
		.def_property_readonly_static("left_alt"		, [](py::object) { return (int32)keycode_left_alt; })
		.def_property_readonly_static("left_super"		, [](py::object) { return (int32)keycode_left_super; })
		.def_property_readonly_static("right_shift"		, [](py::object) { return (int32)keycode_right_shift; })
		.def_property_readonly_static("right_control"	, [](py::object) { return (int32)keycode_right_ctrl; })
		.def_property_readonly_static("right_alt"		, [](py::object) { return (int32)keycode_right_alt; })
		.def_property_readonly_static("right_super"		, [](py::object) { return (int32)keycode_right_super; })
		.def_property_readonly_static("menu"			, [](py::object) { return (int32)keycode_menu; })
		;

	// KEY MODS
	py::class_<keymods_>(m, "keymods")
		.def_property_readonly_static("none", [](py::object) { return (int32)keymods_none; })
		.def_property_readonly_static("shift", [](py::object) { return (int32)keymods_shift; })
		.def_property_readonly_static("ctrl", [](py::object) { return (int32)keymods_ctrl; })
		.def_property_readonly_static("alt", [](py::object) { return (int32)keymods_alt; })
		.def_property_readonly_static("super", [](py::object) { return (int32)keymods_super; })
		.def_property_readonly_static("caps", [](py::object) { return (int32)keymods_caps; })
		.def_property_readonly_static("numlk", [](py::object) { return (int32)keymods_numlk; })
		;

	// WINDOW HINTS
	py::class_<window_hints_>(m, "window_hints")
		.def_property_readonly_static("none"			, [](py::object) { return (int32)window_hints_none; })
		.def_property_readonly_static("resizable"		, [](py::object) { return (int32)window_hints_resizable; })
		.def_property_readonly_static("visible"			, [](py::object) { return (int32)window_hints_visible; })
		.def_property_readonly_static("decorated"		, [](py::object) { return (int32)window_hints_decorated; })
		.def_property_readonly_static("focused"			, [](py::object) { return (int32)window_hints_focused; })
		.def_property_readonly_static("is_auto_iconify"	, [](py::object) { return (int32)window_hints_auto_iconify; })
		.def_property_readonly_static("floating"		, [](py::object) { return (int32)window_hints_floating; })
		.def_property_readonly_static("maximized"		, [](py::object) { return (int32)window_hints_maximized; })
		.def_property_readonly_static("doublebuffer"	, [](py::object) { return (int32)window_hints_doublebuffer; })
		.def_property_readonly_static("center_cursor"	, [](py::object) { return (int32)window_hints_center_cursor; })
		.def_property_readonly_static("focus_on_show"	, [](py::object) { return (int32)window_hints_focus_on_show; })
		.def_property_readonly_static("default"			, [](py::object) { return (int32)window_hints_default; })
		.def_property_readonly_static("default_max"		, [](py::object) { return (int32)window_hints_default_max; })
		;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// CONTEXT SETTINGS
	py::class_<context_settings>(m, "context_settings")
		.def(py::init<>())
		.def(py::init<int32, int32, int32, int32, int32, int32, bool, bool>())
		.def_readwrite("api"			, &context_settings::api)
		.def_readwrite("major"			, &context_settings::major)
		.def_readwrite("minor"			, &context_settings::minor)
		.def_readwrite("profile"		, &context_settings::profile)
		.def_readwrite("depth_bits"		, &context_settings::depth_bits)
		.def_readwrite("stencil_bits"	, &context_settings::stencil_bits)
		.def_readwrite("multisample"	, &context_settings::multisample)
		.def_readwrite("srgb_capable"	, &context_settings::srgb_capable)
		;

	// VIDEO MODE
	py::class_<video_mode>(m, "video_mode")
		.def(py::init<>())
		.def(py::init<vec2i const &>())
		.def(py::init<vec2i const &, vec4b const &>())
		.def(py::init<vec2i const &, vec4b const &, int32>())
		.def_readwrite("resolution", &video_mode::resolution)
		.def_readwrite("bits_per_pixel", &video_mode::bits_per_pixel)
		.def_readwrite("refresh_rate", &video_mode::refresh_rate)
		.def_property_readonly_static("desktop_mode", [](py::object) { return video_mode::desktop_mode; })
		.def_property_readonly_static("fullscreen_modes", [](py::object) { return video_mode::fullscreen_modes; })
		;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}