#ifndef _ML_CLIENT_DATABASE_HPP_
#define _ML_CLIENT_DATABASE_HPP_

// WIP

#include <modus_core/system/Memory.hpp>

namespace ml
{
	struct client_database final : non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		using category_type = typename pmr::unordered_map<pmr::string, std::any>;

		using storage_type = typename pmr::unordered_map<hash_t, category_type>;

		client_database(allocator_type alloc = {}) noexcept : m_data{ alloc }
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// get all categories
		ML_NODISCARD storage_type & all() noexcept
		{
			return m_data;
		}

		// get category
		template <class Type
		> ML_NODISCARD category_type & category() noexcept
		{
			return m_data[hashof_v<Type>];
		}

		// get element
		template <class Type, class Key = pmr::string
		> ML_NODISCARD std::any & element(Key && key) noexcept
		{
			category_type & cat{ this->category<Type>() };
			if (auto const it{ cat.find(ML_forward(key)) }
			; it != cat.end()) { return it->second; }
			else { return cat[ML_forward(key)] = std::make_any<Type>(); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Type, class Key = pmr::string
		> ML_NODISCARD Type & get(Key && key) noexcept
		{
			return *std::any_cast<Type>(&this->element<Type>(ML_forward(key)));
		}

		template <class Type, class Key = pmr::string, class ... Args
		> Type & emplace(Key && key, Args && ... args) noexcept
		{
			return this->element<Type>(ML_forward(key)).emplace<Type>(ML_forward(args)...);
		}

		template <class Type, class Key = pmr::string
		> category_type::iterator erase(Key && key) noexcept
		{
			category_type & cat{ this->category<Type>() };
			if (auto const it{ cat.find(ML_forward(key)) }
			; it != cat.end()) { return cat.erase(it); }
			else { return it; }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto begin() noexcept -> storage_type::iterator { return m_data.begin(); }
		
		ML_NODISCARD auto begin() const noexcept -> storage_type::const_iterator { return m_data.begin(); }
		
		ML_NODISCARD auto cbegin() const noexcept -> storage_type::const_iterator { return m_data.cbegin(); }
		
		ML_NODISCARD auto end() noexcept -> storage_type::iterator { return m_data.end(); }
		
		ML_NODISCARD auto end() const noexcept -> storage_type::const_iterator { return m_data.end(); }
		
		ML_NODISCARD auto cend() const noexcept -> storage_type::const_iterator { return m_data.cend(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		storage_type m_data;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_CLIENT_DATABASE_HPP_
