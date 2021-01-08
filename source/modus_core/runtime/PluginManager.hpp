#ifndef _ML_PLUGIN_MANAGER_HPP_
#define _ML_PLUGIN_MANAGER_HPP_

#include <modus_core/embed/NativeLibrary.hpp>
#include <modus_core/runtime/Plugin.hpp>

namespace ml
{
	// plugin id
	ML_decl_handle(plugin_id);

	// plugin context
	struct ML_NODISCARD plugin_context final
	{
		plugin_id hash_code;
		string file_name, file_path, extension;
		method<plugin * (plugin_manager *, void *)> allocate;
		method<void(plugin_manager *, plugin *)> deallocate;

		plugin_context() noexcept = default;
		plugin_context(plugin_context const &) = default;
		plugin_context(plugin_context &&) noexcept = default;
		plugin_context & operator=(plugin_context const &) = default;
		plugin_context & operator=(plugin_context &&) noexcept = default;

	private:
		friend plugin_manager;

		explicit plugin_context(ref<native_library> const & lib) : plugin_context{}
		{
			ML_verify(lib && *lib);
			hash_code	= (plugin_id)lib->hash_code();
			file_name	= lib->path().stem().string();
			file_path	= lib->path().string();
			extension	= lib->path().extension().string();
			allocate	= lib->proc<plugin *, plugin_manager *, void *>("ml_plugin_create");
			deallocate	= lib->proc<void, plugin_manager *, plugin *>("ml_plugin_destroy");
		}
	};

	// plugin manager
	struct plugin_manager final : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		using storage_type = typename batch_vector
		<
			plugin_id,
			plugin_context,
			unown<native_library>,
			scary<plugin>
		>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		plugin_manager(event_bus * bus, allocator_type alloc = {}) noexcept
			: m_bus		{ bus }
			, m_alloc	{ alloc }
			, m_data	{ alloc }
			, m_userptr	{}
		{
		}

		~plugin_manager() noexcept final
		{
			this->free_all_plugins();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_allocator() const noexcept -> allocator_type { return m_alloc; }

		ML_NODISCARD auto get_bus() const noexcept -> event_bus * { return m_bus; }

		ML_NODISCARD auto get_data() const noexcept -> storage_type const & { return m_data; }

		template <class T
		> ML_NODISCARD auto get_data() const noexcept -> list<T> const & { return m_data.get<T>(); }

		template <class T
		> ML_NODISCARD auto get_data(size_t i) const noexcept -> T const & { return m_data.at<T>(i); }

		ML_NODISCARD auto get_user_pointer() const noexcept -> void * { return m_userptr; }

		void set_user_pointer(void * value) noexcept { m_userptr = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool contains(plugin_id id) const noexcept { return m_data.contains<plugin_id>(id); }

		ML_NODISCARD bool contains(fs::path const & path) const noexcept { return this->contains((plugin_id)hashof(path.string())); }

		ML_NODISCARD bool contains(ref<native_library> const & value) const noexcept { return value && this->contains((plugin_id)value->hash_code()); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Derived, class ... Args
		> ML_NODISCARD auto allocate_plugin(Args && ... args) -> Derived *
		{
			static_assert(std::is_base_of_v<plugin, Derived>);
			auto ptr{ (Derived *)m_alloc.allocate(sizeof(Derived)) };
			util::construct(ptr, this, ML_forward(args)...);
			return ptr;
		}

		template <class Derived
		> void deallocate_plugin(plugin * value)
		{
			static_assert(std::is_base_of_v<plugin, Derived>);
			util::destruct((Derived *)value);
			m_alloc.deallocate((byte *)value, sizeof(Derived));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		plugin_id load_plugin(ref<native_library> const & lib, void * userptr = nullptr)
		{
			if (!lib || !*lib) { return nullptr; }
			
			if (size_t const i{ m_data.lookup_if<unown<native_library>>([&
			](auto const & e) { return !e.expired() && e.lock()->hash_code() == lib->hash_code(); }) }
			; i != m_data.npos)
			{
				return m_data.at<plugin_id>(i);
			}
			else
			{
				m_data.push_back((plugin_id)lib->hash_code(), plugin_context{ lib }, lib, nullptr);

				if (plugin * p{ m_data.back<plugin_context>().allocate(this, userptr) })
				{
					m_data.back<scary<plugin>>().reset(p);

					return m_data.back<plugin_id>();
				}
				else
				{
					return nullptr;
				}
			}
		}

		bool free_plugin(plugin_id value)
		{
			if (!value) { return false; }
			else if (size_t const i{ m_data.lookup<plugin_id>(value) }
			; i == m_data.npos) { return false; }
			else
			{
				plugin * const p{ m_data.at<scary<plugin>>(i).release() };
				m_data.at<plugin_context>(i).deallocate(this, p);
				m_data.erase(i);
				return true;
			}
		}

		bool free_plugin(fs::path const & path) noexcept
		{
			return this->free_plugin((plugin_id)hashof(path.string()));
		}

		void free_all_plugins()
		{
			auto & ids{ m_data.get<plugin_id>() };

			while (!ids.empty()) { this->free_plugin(ids.back()); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		event_bus * const	m_bus		; // event bus
		allocator_type		m_alloc		; // allocator
		storage_type		m_data		; // plugin data
		void *				m_userptr	; // user pointer

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_PLUGIN_MANAGER_HPP_