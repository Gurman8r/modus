#ifndef _ML_CLIENT_RUNTIME_HPP_
#define _ML_CLIENT_RUNTIME_HPP_

#include <modus_core/client/PluginManager.hpp>
#include <modus_core/client/ImGui.hpp>

// CLIENT DOCKSPACE
namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// client dockspace
	struct ML_CORE_API client_dockspace final : client_object<client_dockspace>
	{
		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		bool					enabled;
		pmr::string				title;
		float_t					border;
		float_t					rounding;
		float_t					alpha;
		vec2					padding;
		vec2					size;
		int32_t					flags;
		pmr::vector<uint32_t>	nodes;

		client_dockspace(client_context * ctx) noexcept;

		~client_dockspace() noexcept override = default;

		void configure(json const & j);

		uint32_t begin_builder();

		uint32_t end_builder(uint32_t root);

		uint32_t dock(cstring name, uint32_t id);

		uint32_t split(uint32_t i, uint32_t id, int32_t dir, float_t ratio, uint32_t * value);

		uint32_t split(uint32_t id, int32_t dir, float_t ratio, uint32_t * value);

		uint32_t split(uint32_t id, int32_t dir, float_t ratio, uint32_t * out, uint32_t * value);

		ML_NODISCARD auto & operator[](size_t i) noexcept { return nodes[i]; }

		ML_NODISCARD auto const & operator[](size_t i) const noexcept { return nodes[i]; }

	private:
		void on_event(event && value) override;
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// CLIENT MENUBAR
namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// client menubar
	struct ML_CORE_API client_menubar final : client_object<client_menubar>
	{
		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		bool		enabled;
		pmr::string	title;

		client_menubar(client_context * ctx) noexcept;

		~client_menubar() noexcept override = default;

		void configure(json const & j);

	private:
		void on_event(event && value) override;
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// CLIENT RUNTIME
namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// client runtime
	struct ML_CORE_API client_runtime final : client_object<client_runtime>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		explicit client_runtime(client_context * context);

		~client_runtime() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD int32_t idle();

		ML_NODISCARD int32_t operator()() noexcept { return idle(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_gui() const noexcept -> manual<ImGuiContext> const & { return m_gui; }

		ML_NODISCARD auto get_dock() const noexcept -> unique<client_dockspace> const & { return m_dock; }

		ML_NODISCARD auto get_menubar() const noexcept -> unique<client_menubar> const & { return m_menu; }

		ML_NODISCARD auto get_plugins() const noexcept -> unique<plugin_manager> const & { return m_plugins; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		void do_startup(json const & j);

		void do_shutdown();

		void do_idle();

		void do_gui();

		void on_event(event && value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		bool						m_running	; // 
		manual<	ImGuiContext	>	m_gui		; // 
		unique<	client_dockspace>	m_dock		; // 
		unique<	client_menubar	>	m_menu		; // 
		unique<	plugin_manager	>	m_plugins	; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_CLIENT_RUNTIME_HPP_