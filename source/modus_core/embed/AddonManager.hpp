#ifndef _ML_ADDON_MANAGER_HPP_
#define _ML_ADDON_MANAGER_HPP_

#include <modus_core/embed/NativeLibrary.hpp>
#include <modus_core/embed/Addon.hpp>

namespace ml
{
	ML_alias create_addon_fn = method<addon * (addon_manager *, void *)>;

	ML_alias destroy_addon_fn = method<void(addon_manager *, addon *)>;

	// addon manager
	struct addon_manager final : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		using storage_type = typename batch_vector
		<
			hash_t,
			file_info_struct,
			create_addon_fn,
			destroy_addon_fn,
			unown<native_library>,
			scary<addon>
		>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		addon_manager(event_bus * bus, allocator_type alloc = {}) noexcept
			: m_bus		{ bus }
			, m_alloc	{ alloc }
			, m_data	{ alloc }
			, m_userptr	{}
		{
		}

		~addon_manager() noexcept final
		{
			this->uninstall_all_addons();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_allocator() const noexcept -> allocator_type { return m_alloc; }

		ML_NODISCARD auto get_bus() const noexcept -> event_bus * { return m_bus; }

		ML_NODISCARD auto get_user_pointer() const noexcept -> void * { return m_userptr; }

		void set_user_pointer(void * value) noexcept { m_userptr = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool contains(hash_t id) const noexcept { return m_data.contains<hash_t>(id); }

		ML_NODISCARD bool contains(fs::path const & path) const noexcept { return this->contains(hashof(path.filename().string())); }

		ML_NODISCARD bool contains(ref<native_library> const & value) const noexcept { return value && this->contains(value->get_hash_code()); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Derived, class ... Args
		> ML_NODISCARD auto allocate(Args && ... args) -> Derived *
		{
			static_assert(std::is_base_of_v<addon, Derived>);
			auto ptr{ (Derived *)m_alloc.allocate(sizeof(Derived)) };
			util::construct(ptr, this, ML_forward(args)...);
			return ptr;
		}

		template <class Derived
		> void deallocate(addon * value)
		{
			static_assert(std::is_base_of_v<addon, Derived>);
			util::destruct((Derived *)value);
			m_alloc.deallocate((byte *)value, sizeof(Derived));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		hash_t install_addon(ref<native_library> const & lib, void * userptr = nullptr)
		{
			if (!lib || !*lib) { return 0; }
			
			if (size_t const i{ m_data.lookup_if<unown<native_library>>([&
			](auto const & e) { return !e.expired() && e.lock()->get_hash_code() == lib->get_hash_code(); }) }
			; i != m_data.npos)
			{
				return m_data.get<hash_t>(i);
			}
			else
			{
				m_data.push_back(
					lib->get_hash_code(),
					lib->get_file_info(),
					lib->target<addon *, addon_manager *, void *>("ml_addon_create"),
					lib->target<void, addon_manager *, addon *>("ml_addon_destroy"),
					lib,
					nullptr);

				auto fn{ m_data.back<create_addon_fn>() };

				if (addon * p{ m_data.back<create_addon_fn>()(this, userptr) }; !p) { return 0; }
				else
				{
					m_data.back<scary<addon>>().reset(p);

					return m_data.back<hash_t>();
				}
			}
		}

		bool uninstall_addon(hash_t value)
		{
			if (!value)
			{
				return false;
			}
			else if (size_t const i{ m_data.lookup<hash_t>(value) }; i == m_data.npos)
			{
				return false;
			}
			else
			{
				addon * const p{ m_data.get<scary<addon>>(i).release() };
				m_data.get<destroy_addon_fn>(i)(this, p);
				m_data.erase(i);
				return true;
			}
		}

		bool uninstall_addon(fs::path const & path) noexcept
		{
			return this->uninstall_addon(hashof(path.filename().string()));
		}

		void uninstall_all_addons()
		{
			auto & ids{ m_data.get<hash_t>() };

			while (!ids.empty()) { this->uninstall_addon(ids.back()); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		event_bus * const	m_bus		; // event bus
		allocator_type		m_alloc		; // allocator
		storage_type		m_data		; // addon data
		void *				m_userptr	; // user pointer

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_ADDON_MANAGER_HPP_