#ifndef _ML_PLUGIN_MANAGER_HPP_
#define _ML_PLUGIN_MANAGER_HPP_

#include <modus_core/detail/BatchVector.hpp>
#include <modus_core/engine/Plugin.hpp>
#include <modus_core/engine/SharedLibrary.hpp>

// instance
namespace ml
{
	// plugin instance
	ML_alias plugin_instance = typename ds::scary<plugin>;
}

// details
namespace ml
{
	// plugin details
	struct ML_NODISCARD plugin_details final : trackable
	{
	public:
		hash_t hash; // hash code

		ds::string name, path, ext; // file info

		plugin_details(plugin_details const &) = default;
		plugin_details(plugin_details &&) noexcept = default;
		plugin_details & operator=(plugin_details const &) = default;
		plugin_details & operator=(plugin_details &&) noexcept = default;

	private:
		friend plugin_manager;

		explicit plugin_details(shared_library & lib) noexcept
			: hash	{ lib.hash() }
			, name	{ lib.path().stem().string() }
			, path	{ lib.path().string() }
			, ext	{ lib.path().extension().string() }
		{
		}
	};
}

// installer
namespace ml
{
	// plugin installer
	struct ML_NODISCARD plugin_installer final : trackable
	{
	public:
		ds::method< plugin * (plugin_manager *, void *) > create; // create plugin

		ds::method< void (plugin_manager *, plugin *) > destroy; // destroy plugin

		plugin_installer(plugin_installer const &) = default;
		plugin_installer(plugin_installer &&) noexcept = default;
		plugin_installer & operator=(plugin_installer const &) = default;
		plugin_installer & operator=(plugin_installer &&) noexcept = default;

	private:
		friend plugin_manager;
		
		explicit plugin_installer(shared_library & lib) noexcept
			: create{ lib.get_proc<plugin *, plugin_manager *, void *>("ml_plugin_install") }
			, destroy{ lib.get_proc<void, plugin_manager *, plugin *>("ml_plugin_uninstall") }
		{
		}
	};
}

// manager
namespace ml
{
	// plugin manager
	struct ML_CORE_API plugin_manager final : non_copyable, trackable, core_object
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		using plugin_storage = typename ds::batch_vector
		<
			plugin_id			,	// id
			plugin_details		,	// details
			plugin_installer	,	// installer
			shared_library		,	// library
			plugin_instance			// plugin
		>;

		static auto make_id(fs::path const & path) noexcept
		{
			return (plugin_id)hashof(shared_library::format_path(path).string());
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		plugin_manager(event_bus * bus, allocator_type alloc = {}) noexcept;

		~plugin_manager() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		plugin_id install(fs::path const & path, void * userptr = nullptr);

		bool uninstall(plugin_id value);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		void uninstall_all() noexcept
		{
			auto & ids{ this->get<plugin_id>() };

			while (!ids.empty()) { uninstall(ids.back()); }
		}

		ML_NODISCARD bool contains(plugin_id id) const noexcept
		{
			return m_storage.contains<plugin_id>(id);
		}

		ML_NODISCARD bool contains(fs::path const & path) const noexcept
		{
			return m_storage.contains<plugin_id>(make_id(path));
		}

		ML_NODISCARD bool contains(shared_library const & value) const noexcept
		{
			return m_storage.contains<shared_library>(value);
		}

		ML_NODISCARD bool contains(plugin const * value) const noexcept
		{
			return m_storage.end<plugin_instance>() != m_storage.find_if<plugin_instance>([&
			](plugin_instance const & e) noexcept
			{
				return value == e.get();
			});
		}

		ML_NODISCARD bool contains(plugin_instance const & value) const noexcept
		{
			return m_storage.contains<plugin_instance>(value);
		}

		ML_NODISCARD auto storage() noexcept -> plugin_storage & { return m_storage; }

		ML_NODISCARD auto storage() const noexcept -> plugin_storage const & { return m_storage; }

		template <class T
		> ML_NODISCARD auto get() & noexcept -> ds::list<T> & { return m_storage.get<T>(); }

		template <class T
		> ML_NODISCARD auto get() const & noexcept -> ds::list<T> const & { return m_storage.get<T>(); }

		template <class T
		> ML_NODISCARD auto get(size_t i) & noexcept -> T & { return m_storage.at<T>(i); }

		template <class T
		> ML_NODISCARD auto get(size_t i) const & noexcept -> T const & { return m_storage.at<T>(i); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		plugin_storage m_storage; // plugin data

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_PLUGIN_MANAGER_HPP_