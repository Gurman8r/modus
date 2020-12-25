#ifndef _ML_LIBRARY_MANAGER_HPP_
#define _ML_LIBRARY_MANAGER_HPP_

#include <modus_core/detail/BatchVector.hpp>
#include <modus_core/runtime/SharedLibrary.hpp>

namespace ml
{
	ML_decl_handle(library_id);

	struct library_manager final : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		using storage_type = typename ds::batch_vector<
			library_id				,	// id
			fs::path				,	// path
			ds::ref<shared_library>		// library
		>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		library_manager(allocator_type alloc = {}) noexcept
			: m_alloc	{ alloc }
			, m_data	{ alloc }
		{
		}

		~library_manager() noexcept final
		{
			this->free_all_libraries();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_allocator() const noexcept -> allocator_type { return m_alloc; }

		ML_NODISCARD auto get_data() const noexcept -> storage_type const & { return m_data; }

		template <class T
		> ML_NODISCARD auto get_data() const noexcept -> ds::list<T> const & { return m_data.get<T>(); }

		template <class T
		> ML_NODISCARD auto get_data(size_t i) const noexcept -> T const & { return m_data.at<T>(i); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool contains(library_id id) const { return m_data.contains<library_id>(id); }

		ML_NODISCARD bool contains(fs::path const & path) const noexcept { return this->contains((library_id)hashof(path.string())); }

		ML_NODISCARD bool contains(ds::ref<shared_library> const & value) const noexcept { return value && this->contains((library_id)value->hash_code()); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto load_library(fs::path const & path) -> ds::ref<shared_library>
		{
			if (ds::ref<shared_library> lib{ this->find(path) })
			{
				return lib;
			}
			else
			{
				lib = make_ref<shared_library>(path);

				m_data.push_back((library_id)lib->hash_code(), lib->path(), lib);

				return lib;
			}
		}

		bool free_library(library_id id) noexcept
		{
			if (size_t const i{ m_data.lookup<library_id>(id) }; i == m_data.npos) { return false; }
			else { m_data.erase(i); return true; }
		}

		bool free_library(fs::path const & path) noexcept
		{
			return this->free_library((library_id)hashof(path.string()));
		}

		void free_all_libraries()
		{
			auto & ids{ m_data.get<library_id>() };

			while (!ids.empty()) { m_data.pop_back(); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto find(library_id id) -> ds::ref<shared_library>
		{
			if (size_t const i{ m_data.lookup<library_id>(id) }
			; i == m_data.npos) { return nullptr; }
			else { return m_data.at<ds::ref<shared_library>>(i); }
		}

		ML_NODISCARD auto find(library_id id) const -> ds::ref<shared_library>
		{
			if (size_t const i{ m_data.lookup<library_id>(id) }
			; i == m_data.npos) { return nullptr; }
			else { return m_data.at<ds::ref<shared_library>>(i); }
		}

		ML_NODISCARD auto find(fs::path const & path) noexcept -> ds::ref<shared_library>
		{
			return this->find((library_id)hashof(path.string()));
		}

		ML_NODISCARD auto find(fs::path const & path) const noexcept -> ds::ref<shared_library>
		{
			return this->find((library_id)hashof(path.string()));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class ID
		> ML_NODISCARD auto proc(library_id id, ID && method_name) noexcept -> void *
		{
			if (auto const lib{ this->find(id) })
			{
				return lib->proc(ML_forward(method_name));
			}
			else
			{
				return nullptr;
			}
		}

		template <class ID
		> ML_NODISCARD auto proc(fs::path const & path, ID && method_name) noexcept -> void *
		{
			return this->proc((library_id)hashof(path.string()), ML_forward(method_name));
		}

		template <class Ret, class ... Args, class ID
		> ML_NODISCARD auto proc(library_id id, ID && method_name) noexcept -> Ret(*)(Args...)
		{
			if (auto const lib{ this->find(id) })
			{
				return lib->proc<Ret, Args...>(ML_forward(method_name));
			}
			else
			{
				return nullptr;
			}
		}

		template <class Ret, class ... Args, class ID
		> ML_NODISCARD auto proc(fs::path const & path, ID && method_name) noexcept -> Ret(*)(Args...)
		{
			return this->proc<Ret, Args...>((library_id)hashof(path.string()), ML_forward(method_name));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Ret, class ... Args, class ID
		> auto eval(library_id id, ID && method_name, Args && ... args) noexcept -> shared_library::result_type<Ret>
		{
			if (auto const lib{ this->find(id) })
			{
				return lib->eval<Ret, Args...>(ML_forward(method_name), ML_forward(args)...);
			}
			else if constexpr (!std::is_same_v<Ret, void>)
			{
				return std::nullopt;
			}
		}

		template <class Ret, class ... Args, class ID
		> auto eval(fs::path const & path, ID && method_name, Args && ... args) noexcept -> shared_library::result_type<Ret>
		{
			return this->eval<Ret, Args...>((library_id)hashof(path.string()), ML_forward(method_name), ML_forward(args)...);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		allocator_type		m_alloc	; // allocator
		storage_type		m_data	; // storage

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_LIBRARY_MANAGER_HPP_