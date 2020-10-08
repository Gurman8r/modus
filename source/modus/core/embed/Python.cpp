#include <core/embed/Python.hpp>
#include <core/graphics/RenderWindow.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	python_context::python_context(pmr::memory_resource * mres, fs::path const & name, fs::path const & home)
	{
		if (Py_IsInitialized()) { return; }

		PyObject_SetArenaAllocator(std::invoke([&mres]() noexcept
		{
			static PyObjectArenaAllocator temp
			{
				mres,
				[](auto mres, size_t size) noexcept
				{
					return ((pmr::memory_resource *)mres)->allocate(size);
				},
				[](auto mres, void * addr, size_t size) noexcept
				{
					return ((pmr::memory_resource *)mres)->deallocate(addr, size);
				}
			};
			return &temp;
		}));

		Py_SetProgramName(name.c_str());

		Py_SetPythonHome(home.c_str());
		
		Py_InitializeEx(1);
		
		ML_assert(Py_IsInitialized());
	}

	python_context::~python_context() noexcept
	{
		if (!Py_IsInitialized()) { return; }

		ML_assert(Py_FinalizeEx() == EXIT_SUCCESS);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

PYBIND11_EMBEDDED_MODULE(modus, m)
{
	using namespace ml;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	m.def("exit", [](py::args) // exit
	{
		//plugin_manager::get()->get_window()->set_should_close(true);
	});
	py::module::import("builtins").attr("exit") = m.attr("exit");
	py::module::import("sys").attr("exit") = m.attr("exit");

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	py::class_<non_copyable, manual<non_copyable>>(m, "non_copyable");

	py::class_<trackable, manual<trackable>>(m, "trackable");

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct ml_info {};
	py::class_<ml_info>(m, "info")
		.def_property_readonly_static("author"		, [](py::object) { return ML__auth; })
		.def_property_readonly_static("date"		, [](py::object) { return ML__date; })
		.def_property_readonly_static("name"		, [](py::object) { return ML__name; })
		.def_property_readonly_static("time"		, [](py::object) { return ML__time; })
		.def_property_readonly_static("url"			, [](py::object) { return ML__url; })
		.def_property_readonly_static("ver"			, [](py::object) { return ML__ver; })
		.def_property_readonly_static("arch"		, [](py::object) { return ML_arch; })
		.def_property_readonly_static("cc_name"		, [](py::object) { return ML_cc_name; })
		.def_property_readonly_static("cc_version"	, [](py::object) { return ML_cc_version; })
		.def_property_readonly_static("is_debug"	, [](py::object) { return ML_is_debug; })
		.def_property_readonly_static("config"		, [](py::object) { return ML_configuration; })
		.def_property_readonly_static("lang"		, [](py::object) { return ML_lang; })
		.def_property_readonly_static("platform"	, [](py::object) { return ML_platform; })
		.def_property_readonly_static("os_name"		, [](py::object) { return ML_os_name; })
		;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct stdio
	{
		struct output
		{
			std::reference_wrapper<std::ostream> m_os;

			output(std::ostream & os = std::cout) noexcept : m_os{ os } {}

			int32_t fileno() const noexcept
			{
				if (auto const addr{ std::addressof(m_os.get()) }; !addr) { return -2; }
				else if (addr == std::addressof(std::cout)) { return 1; }
				else if (addr == std::addressof(std::cerr)) { return 2; }
				else { return -1; }
			}

			void flush() noexcept { m_os.get().flush(); }

			void write(py::object o) noexcept { m_os << o; }

			void writelines(py::list l) noexcept { for (auto const & e : l) { m_os << e; } }
		};

		static auto & cerr(py::object) noexcept { static output temp{ std::cerr }; return temp; }

		static auto & cout(py::object) noexcept { static output temp{ std::cout }; return temp; }
	};
	py::class_<stdio::output>(m, "output")
		.def(py::init<>())
		.def("closed"		, []() { return false; })
		.def("isatty"		, []() { return false; })
		.def("readable"		, []() { return false; })
		.def("seekable"		, []() { return false; })
		.def("writable"		, []() { return true; })
		.def("fileno"		, &stdio::output::fileno)
		.def("flush"		, &stdio::output::flush)
		.def("write"		, &stdio::output::write)
		.def("writelines"	, &stdio::output::writelines)
		;
	py::class_<stdio>(m, "stdio")
		.def(py::init<>())
		.def_property_readonly_static("cerr", &stdio::cerr)
		.def_property_readonly_static("cout", &stdio::cout)
		;
	{
		auto sys{ py::module::import("sys") };
		sys.attr("stdout") = m.attr("stdio").attr("cout");
		sys.attr("stderr") = m.attr("stdio").attr("cout");
		sys.attr("stdin") = py::none{};
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	py::class_<json>(m, "json")
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

	using pmr_allocator = pmr::polymorphic_allocator<byte_t>;

	py::class_<pmr_allocator>(py_mem, "allocator")
		.def(py::init<>())
		.def(py::init([](intptr_t p) { return pmr_allocator{ (pmr::memory_resource *)p }; }))
		.def(py::init<pmr_allocator const &>())
		.def("allocate", [](pmr_allocator & o, size_t s) { return (intptr_t)o.allocate(s); })
		.def("deallocate", [](pmr_allocator & o, intptr_t p, size_t s) { o.deallocate((byte_t *)p, s); })
		;

	py::class_<memory_record>(py_mem, "record")
		.def(py::init<>())
		.def(py::init<memory_record const &>())
		.def(py::init([&rec = memory::get_records()](intptr_t p)
		{
			if (auto const i{ rec.lookup<memory::id_addr>((byte_t *)p) }; i != rec.npos) {
				return memory::get_record(i);
			} else {
				return memory_record{};
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
		case 1	: return *(uint8_t *)p;
		case 2	: return *(uint16_t *)p;
		case 3	: return *(uint32_t *)p;
		case 4	: return *(uint64_t *)p;
		}
	};

	py_mem // memory
		.def("get_default_resource", []() { return (intptr_t)pmr::get_default_resource(); })
		.def("set_default_resource", [](intptr_t p) { return (intptr_t)pmr::set_default_resource((pmr::memory_resource *)p); })

		// test resource
		.def("arena_base"	, []() { return memory::get_resource()->base(); })
		.def("arena_count"	, []() { return memory::get_resource()->count(); })
		.def("arena_free"	, []() { return memory::get_resource()->free(); })
		.def("arena_size"	, []() { return memory::get_resource()->capacity(); })
		.def("arena_used"	, []() { return memory::get_resource()->used(); })

		// allocation
		.def("malloc"	, [](size_t s) { return (intptr_t)memory::allocate(s); })
		.def("calloc"	, [](size_t c, size_t s) { return (intptr_t)memory::allocate(c, s); })
		.def("free"		, [](intptr_t p) { memory::deallocate((void *)p); })
		.def("realloc"	, [](intptr_t p, size_t s) { return (intptr_t)memory::reallocate((void *)p, s); })
		.def("realloc"	, [](intptr_t p, size_t o, size_t n) { return (intptr_t)memory::reallocate((void *)p, o, n); })

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
		.def("memchr"	, [](intptr_t p, int32_t v, size_t n) { return (intptr_t)std::memchr((void *)p, v, n); })
		// memcmp
		.def("memcmp"	, [](intptr_t l, intptr_t r, size_t n) { return std::memcmp((void *)l, (void *)r, n); })
		// memcpy
		.def("memcpy"	, [](intptr_t d, intptr_t s, size_t n) { return (intptr_t)std::memcpy((void *)d, (void *)s, n); })
		// memmove
		.def("memmove"	, [](intptr_t d, intptr_t s, size_t n) { return (intptr_t)std::memmove((void *)d, (void *)s, n); })
		// memset
		.def("memset"	, [](intptr_t d, int32_t v, size_t n) { return (intptr_t)std::memset((void *)d, v, n); })
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
	//	void on_event(event && ev) override
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

	// CONTEXT CLIENT
	py::class_<context_api_>(m, "context_client")
		.def_property_readonly_static("unknown"	, [](py::object) { return (int32_t)context_api_unknown; })
		.def_property_readonly_static("opengl"	, [](py::object) { return (int32_t)context_api_opengl; })
		.def_property_readonly_static("vulkan"	, [](py::object) { return (int32_t)context_api_vulkan; })
		.def_property_readonly_static("directx"	, [](py::object) { return (int32_t)context_api_directx; })
		;

	// CONTEXT PROFILE
	py::class_<context_profile_>(m, "context_profile")
		.def_property_readonly_static("any"			, [](py::object) { return (int32_t)context_profile_any; })
		.def_property_readonly_static("core"		, [](py::object) { return (int32_t)context_profile_core; })
		.def_property_readonly_static("compat"		, [](py::object) { return (int32_t)context_profile_compat; })
		.def_property_readonly_static("debug"		, [](py::object) { return (int32_t)context_profile_debug; })
		;

	// CURSOR MODE
	py::class_<cursor_mode_>(m, "cursor_mode")
		.def_property_readonly_static("normal"		, [](py::object) { return (int32_t)cursor_mode_normal; })
		.def_property_readonly_static("hidden"		, [](py::object) { return (int32_t)cursor_mode_hidden; })
		.def_property_readonly_static("disabled"	, [](py::object) { return (int32_t)cursor_mode_disabled; })
		;

	// CURSOR SHAPE
	py::class_<cursor_shape_>(m, "cursor_shape")
		.def_property_readonly_static("arrow"		, [](py::object) { return (int32_t)cursor_shape_arrow; })
		.def_property_readonly_static("ibeam"		, [](py::object) { return (int32_t)cursor_shape_ibeam; })
		.def_property_readonly_static("crosshair"	, [](py::object) { return (int32_t)cursor_shape_crosshair; })
		.def_property_readonly_static("hand"		, [](py::object) { return (int32_t)cursor_shape_hand; })
		.def_property_readonly_static("ew"			, [](py::object) { return (int32_t)cursor_shape_ew; })
		.def_property_readonly_static("ns"			, [](py::object) { return (int32_t)cursor_shape_ns; })
		.def_property_readonly_static("nesw"		, [](py::object) { return (int32_t)cursor_shape_nesw; })
		.def_property_readonly_static("nwse"		, [](py::object) { return (int32_t)cursor_shape_nwse; })
		;

	// KEY CODE
	py::class_<key_code_>(m, "key_code")
		.def_property_readonly_static("space"			, [](py::object) { return (int32_t)key_code_space; })
		.def_property_readonly_static("apostrophe"		, [](py::object) { return (int32_t)key_code_apostrophe; })
		.def_property_readonly_static("comma"			, [](py::object) { return (int32_t)key_code_comma; })
		.def_property_readonly_static("minus"			, [](py::object) { return (int32_t)key_code_minus; })
		.def_property_readonly_static("period"			, [](py::object) { return (int32_t)key_code_period; })
		.def_property_readonly_static("slash"			, [](py::object) { return (int32_t)key_code_slash; })
		.def_property_readonly_static("num_0"			, [](py::object) { return (int32_t)key_code_num_0; })
		.def_property_readonly_static("num_1"			, [](py::object) { return (int32_t)key_code_num_1; })
		.def_property_readonly_static("num_2"			, [](py::object) { return (int32_t)key_code_num_2; })
		.def_property_readonly_static("num_3"			, [](py::object) { return (int32_t)key_code_num_3; })
		.def_property_readonly_static("num_4"			, [](py::object) { return (int32_t)key_code_num_4; })
		.def_property_readonly_static("num_5"			, [](py::object) { return (int32_t)key_code_num_5; })
		.def_property_readonly_static("num_6"			, [](py::object) { return (int32_t)key_code_num_6; })
		.def_property_readonly_static("num_7"			, [](py::object) { return (int32_t)key_code_num_7; })
		.def_property_readonly_static("num_8"			, [](py::object) { return (int32_t)key_code_num_8; })
		.def_property_readonly_static("num_9"			, [](py::object) { return (int32_t)key_code_num_9; })
		.def_property_readonly_static("semicolon"		, [](py::object) { return (int32_t)key_code_semicolon; })
		.def_property_readonly_static("equal"			, [](py::object) { return (int32_t)key_code_equal; })
		.def_property_readonly_static("a"				, [](py::object) { return (int32_t)key_code_a; })
		.def_property_readonly_static("b"				, [](py::object) { return (int32_t)key_code_b; })
		.def_property_readonly_static("c"				, [](py::object) { return (int32_t)key_code_c; })
		.def_property_readonly_static("d"				, [](py::object) { return (int32_t)key_code_d; })
		.def_property_readonly_static("e"				, [](py::object) { return (int32_t)key_code_e; })
		.def_property_readonly_static("f"				, [](py::object) { return (int32_t)key_code_f; })
		.def_property_readonly_static("g"				, [](py::object) { return (int32_t)key_code_g; })
		.def_property_readonly_static("h"				, [](py::object) { return (int32_t)key_code_h; })
		.def_property_readonly_static("i"				, [](py::object) { return (int32_t)key_code_i; })
		.def_property_readonly_static("j"				, [](py::object) { return (int32_t)key_code_j; })
		.def_property_readonly_static("k"				, [](py::object) { return (int32_t)key_code_k; })
		.def_property_readonly_static("l"				, [](py::object) { return (int32_t)key_code_l; })
		.def_property_readonly_static("m"				, [](py::object) { return (int32_t)key_code_m; })
		.def_property_readonly_static("n"				, [](py::object) { return (int32_t)key_code_n; })
		.def_property_readonly_static("o"				, [](py::object) { return (int32_t)key_code_o; })
		.def_property_readonly_static("p"				, [](py::object) { return (int32_t)key_code_p; })
		.def_property_readonly_static("q"				, [](py::object) { return (int32_t)key_code_q; })
		.def_property_readonly_static("r"				, [](py::object) { return (int32_t)key_code_r; })
		.def_property_readonly_static("s"				, [](py::object) { return (int32_t)key_code_s; })
		.def_property_readonly_static("t"				, [](py::object) { return (int32_t)key_code_t; })
		.def_property_readonly_static("u"				, [](py::object) { return (int32_t)key_code_u; })
		.def_property_readonly_static("v"				, [](py::object) { return (int32_t)key_code_v; })
		.def_property_readonly_static("w"				, [](py::object) { return (int32_t)key_code_w; })
		.def_property_readonly_static("x"				, [](py::object) { return (int32_t)key_code_x; })
		.def_property_readonly_static("y"				, [](py::object) { return (int32_t)key_code_y; })
		.def_property_readonly_static("z"				, [](py::object) { return (int32_t)key_code_z; })
		.def_property_readonly_static("left_bracket"	, [](py::object) { return (int32_t)key_code_left_bracket; })
		.def_property_readonly_static("backslash"		, [](py::object) { return (int32_t)key_code_backslash; })
		.def_property_readonly_static("right_bracket"	, [](py::object) { return (int32_t)key_code_right_bracket; })
		.def_property_readonly_static("grave_accent"	, [](py::object) { return (int32_t)key_code_grave_accent; })
		.def_property_readonly_static("world_1"			, [](py::object) { return (int32_t)key_code_world_1; })
		.def_property_readonly_static("world_2"			, [](py::object) { return (int32_t)key_code_world_2; })
		.def_property_readonly_static("escape"			, [](py::object) { return (int32_t)key_code_escape; })
		.def_property_readonly_static("enter"			, [](py::object) { return (int32_t)key_code_enter; })
		.def_property_readonly_static("tab"				, [](py::object) { return (int32_t)key_code_tab; })
		.def_property_readonly_static("backspace"		, [](py::object) { return (int32_t)key_code_backspace; })
		.def_property_readonly_static("insert"			, [](py::object) { return (int32_t)key_code_insert; })
		.def_property_readonly_static("del"				, [](py::object) { return (int32_t)key_code_del; })
		.def_property_readonly_static("right"			, [](py::object) { return (int32_t)key_code_right; })
		.def_property_readonly_static("left"			, [](py::object) { return (int32_t)key_code_left; })
		.def_property_readonly_static("down"			, [](py::object) { return (int32_t)key_code_down; })
		.def_property_readonly_static("up"				, [](py::object) { return (int32_t)key_code_up; })
		.def_property_readonly_static("page_up"			, [](py::object) { return (int32_t)key_code_page_up; })
		.def_property_readonly_static("page_down"		, [](py::object) { return (int32_t)key_code_page_down; })
		.def_property_readonly_static("home"			, [](py::object) { return (int32_t)key_code_home; })
		.def_property_readonly_static("end"				, [](py::object) { return (int32_t)key_code_end; })
		.def_property_readonly_static("caps_lock"		, [](py::object) { return (int32_t)key_code_caps_lock; })
		.def_property_readonly_static("scroll_lock"		, [](py::object) { return (int32_t)key_code_scroll_lock; })
		.def_property_readonly_static("num_lock"		, [](py::object) { return (int32_t)key_code_num_lock; })
		.def_property_readonly_static("print_screen"	, [](py::object) { return (int32_t)key_code_print_screen; })
		.def_property_readonly_static("pause"			, [](py::object) { return (int32_t)key_code_pause; })
		.def_property_readonly_static("fn_1"			, [](py::object) { return (int32_t)key_code_fn_1; })
		.def_property_readonly_static("fn_2"			, [](py::object) { return (int32_t)key_code_fn_2; })
		.def_property_readonly_static("fn_3"			, [](py::object) { return (int32_t)key_code_fn_3; })
		.def_property_readonly_static("fn_4"			, [](py::object) { return (int32_t)key_code_fn_4; })
		.def_property_readonly_static("fn_5"			, [](py::object) { return (int32_t)key_code_fn_5; })
		.def_property_readonly_static("fn_6"			, [](py::object) { return (int32_t)key_code_fn_6; })
		.def_property_readonly_static("fn_7"			, [](py::object) { return (int32_t)key_code_fn_7; })
		.def_property_readonly_static("fn_8"			, [](py::object) { return (int32_t)key_code_fn_8; })
		.def_property_readonly_static("fn_9"			, [](py::object) { return (int32_t)key_code_fn_9; })
		.def_property_readonly_static("fn_10"			, [](py::object) { return (int32_t)key_code_fn_10; })
		.def_property_readonly_static("fn_11"			, [](py::object) { return (int32_t)key_code_fn_11; })
		.def_property_readonly_static("fn_12"			, [](py::object) { return (int32_t)key_code_fn_12; })
		.def_property_readonly_static("fn_13"			, [](py::object) { return (int32_t)key_code_fn_13; })
		.def_property_readonly_static("fn_14"			, [](py::object) { return (int32_t)key_code_fn_14; })
		.def_property_readonly_static("fn_15"			, [](py::object) { return (int32_t)key_code_fn_15; })
		.def_property_readonly_static("fn_16"			, [](py::object) { return (int32_t)key_code_fn_16; })
		.def_property_readonly_static("fn_17"			, [](py::object) { return (int32_t)key_code_fn_17; })
		.def_property_readonly_static("fn_18"			, [](py::object) { return (int32_t)key_code_fn_18; })
		.def_property_readonly_static("fn_19"			, [](py::object) { return (int32_t)key_code_fn_19; })
		.def_property_readonly_static("fn_20"			, [](py::object) { return (int32_t)key_code_fn_20; })
		.def_property_readonly_static("fn_21"			, [](py::object) { return (int32_t)key_code_fn_21; })
		.def_property_readonly_static("fn_22"			, [](py::object) { return (int32_t)key_code_fn_22; })
		.def_property_readonly_static("fn_23"			, [](py::object) { return (int32_t)key_code_fn_23; })
		.def_property_readonly_static("fn_24"			, [](py::object) { return (int32_t)key_code_fn_24; })
		.def_property_readonly_static("fn_25"			, [](py::object) { return (int32_t)key_code_fn_25; })
		.def_property_readonly_static("kp_0"			, [](py::object) { return (int32_t)key_code_kp_0; })
		.def_property_readonly_static("kp_1"			, [](py::object) { return (int32_t)key_code_kp_1; })
		.def_property_readonly_static("kp_2"			, [](py::object) { return (int32_t)key_code_kp_2; })
		.def_property_readonly_static("kp_3"			, [](py::object) { return (int32_t)key_code_kp_3; })
		.def_property_readonly_static("kp_4"			, [](py::object) { return (int32_t)key_code_kp_4; })
		.def_property_readonly_static("kp_5"			, [](py::object) { return (int32_t)key_code_kp_5; })
		.def_property_readonly_static("kp_6"			, [](py::object) { return (int32_t)key_code_kp_6; })
		.def_property_readonly_static("kp_7"			, [](py::object) { return (int32_t)key_code_kp_7; })
		.def_property_readonly_static("kp_8"			, [](py::object) { return (int32_t)key_code_kp_8; })
		.def_property_readonly_static("kp_9"			, [](py::object) { return (int32_t)key_code_kp_9; })
		.def_property_readonly_static("kp_decimal"		, [](py::object) { return (int32_t)key_code_kp_decimal; })
		.def_property_readonly_static("kp_divide"		, [](py::object) { return (int32_t)key_code_kp_divide; })
		.def_property_readonly_static("kp_multiply"		, [](py::object) { return (int32_t)key_code_kp_multiply; })
		.def_property_readonly_static("kp_subtract"		, [](py::object) { return (int32_t)key_code_kp_subtract; })
		.def_property_readonly_static("kp_add"			, [](py::object) { return (int32_t)key_code_kp_add; })
		.def_property_readonly_static("kp_enter"		, [](py::object) { return (int32_t)key_code_kp_enter; })
		.def_property_readonly_static("kp_equal"		, [](py::object) { return (int32_t)key_code_kp_equal; })
		.def_property_readonly_static("left_shift"		, [](py::object) { return (int32_t)key_code_left_shift; })
		.def_property_readonly_static("left_control"	, [](py::object) { return (int32_t)key_code_left_control; })
		.def_property_readonly_static("left_alt"		, [](py::object) { return (int32_t)key_code_left_alt; })
		.def_property_readonly_static("left_super"		, [](py::object) { return (int32_t)key_code_left_super; })
		.def_property_readonly_static("right_shift"		, [](py::object) { return (int32_t)key_code_right_shift; })
		.def_property_readonly_static("right_control"	, [](py::object) { return (int32_t)key_code_right_control; })
		.def_property_readonly_static("right_alt"		, [](py::object) { return (int32_t)key_code_right_alt; })
		.def_property_readonly_static("right_super"		, [](py::object) { return (int32_t)key_code_right_super; })
		.def_property_readonly_static("menu"			, [](py::object) { return (int32_t)key_code_menu; })
		;

	// KEY MODS
	py::class_<key_mods_>(m, "key_mods")
		.def_property_readonly_static("none", [](py::object) { return (int32_t)key_mods_none; })
		.def_property_readonly_static("shift", [](py::object) { return (int32_t)key_mods_shift; })
		.def_property_readonly_static("ctrl", [](py::object) { return (int32_t)key_mods_ctrl; })
		.def_property_readonly_static("alt", [](py::object) { return (int32_t)key_mods_alt; })
		.def_property_readonly_static("super", [](py::object) { return (int32_t)key_mods_super; })
		.def_property_readonly_static("caps_lock", [](py::object) { return (int32_t)key_mods_caps_lock; })
		.def_property_readonly_static("num_lock", [](py::object) { return (int32_t)key_mods_num_lock; })
		;

	// KEY STATE
	py::class_<input_state_>(m, "input_state")
		.def_property_readonly_static("release", [](py::object) { return (int32_t)input_state_release; })
		.def_property_readonly_static("press", [](py::object) { return (int32_t)input_state_press; })
		.def_property_readonly_static("repeat", [](py::object) { return (int32_t)input_state_repeat; })
		;

	// WINDOW HINTS
	py::class_<window_hints_>(m, "window_hints")
		.def_property_readonly_static("none"			, [](py::object) { return (int32_t)window_hints_none; })
		.def_property_readonly_static("resizable"		, [](py::object) { return (int32_t)window_hints_resizable; })
		.def_property_readonly_static("visible"			, [](py::object) { return (int32_t)window_hints_visible; })
		.def_property_readonly_static("decorated"		, [](py::object) { return (int32_t)window_hints_decorated; })
		.def_property_readonly_static("focused"			, [](py::object) { return (int32_t)window_hints_focused; })
		.def_property_readonly_static("is_auto_iconify"	, [](py::object) { return (int32_t)window_hints_auto_iconify; })
		.def_property_readonly_static("floating"		, [](py::object) { return (int32_t)window_hints_floating; })
		.def_property_readonly_static("maximized"		, [](py::object) { return (int32_t)window_hints_maximized; })
		.def_property_readonly_static("doublebuffer"	, [](py::object) { return (int32_t)window_hints_doublebuffer; })
		.def_property_readonly_static("center_cursor"	, [](py::object) { return (int32_t)window_hints_center_cursor; })
		.def_property_readonly_static("focus_on_show"	, [](py::object) { return (int32_t)window_hints_focus_on_show; })
		.def_property_readonly_static("default"			, [](py::object) { return (int32_t)window_hints_default; })
		.def_property_readonly_static("default_max"		, [](py::object) { return (int32_t)window_hints_default_max; })
		;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// CONTEXT SETTINGS
	py::class_<context_settings>(m, "context_settings")
		.def(py::init<>())
		.def(py::init<int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, bool, bool>())
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
		.def(py::init<vec2i const &, vec4b const &, int32_t>())
		.def_readwrite("resolution"			, &video_mode::resolution)
		.def_readwrite("bits_per_pixel"		, &video_mode::bits_per_pixel)
		.def_readwrite("refresh_rate"		, &video_mode::refresh_rate)
		.def_static("get_desktop_mode"		, &video_mode::get_desktop_mode)
		.def_static("get_fullscreen_modes"	, &video_mode::get_fullscreen_modes)
		;

	// WINDOW SETTINGS
	py::class_<window_settings>(m, "window_settings")
		.def(py::init<>())
		.def(py::init<pmr::string const &, video_mode const &, context_settings const &, int32_t>())
		.def_readwrite("title"	, &window_settings::title)
		.def_readwrite("video"	, &window_settings::video)
		.def_readwrite("context", &window_settings::context)
		.def_readwrite("hints"	, &window_settings::hints)
		;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}