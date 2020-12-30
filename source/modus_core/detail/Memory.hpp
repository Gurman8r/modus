#ifndef _ML_MEMORY_HPP_
#define _ML_MEMORY_HPP_

#include <modus_core/detail/BatchVector.hpp>
#include <modus_core/detail/Globals.hpp>

// cleanup memory leaks
#ifndef ML_IMPL_CLEANUP
#define ML_IMPL_CLEANUP 0
#endif

// passthrough resource
namespace ml
{
	// proxy for testing an upstream memory resource
	struct passthrough_resource final : public pmr::memory_resource, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using pointer					= typename byte *;
		using const_pointer				= typename byte const *;
		using reference					= typename byte &;
		using const_reference			= typename byte const &;
		using iterator					= typename pointer;
		using const_iterator			= typename const_pointer;
		using reverse_iterator			= typename std::reverse_iterator<iterator>;
		using const_reverse_iterator	= typename std::reverse_iterator<const_iterator>;
		using size_type					= typename size_t;
		using difference_type			= typename ptrdiff_t;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		passthrough_resource(pmr::memory_resource * mres, pointer data, size_t size) noexcept
			: m_resource	{ mres }
			, m_buffer_data	{ data }
			, m_buffer_size	{ size }
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool is_default() const noexcept { return this == pmr::get_default_resource(); }

		ML_NODISCARD auto get_resource() const noexcept -> pmr::memory_resource * const { return m_resource; }

		ML_NODISCARD auto num_allocations() const noexcept -> size_t { return m_num_allocations; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto buffer_data() const noexcept -> pointer { return m_buffer_data; }

		ML_NODISCARD auto buffer_base() const noexcept -> intptr_t { return (intptr_t)m_buffer_data; }

		ML_NODISCARD auto buffer_size() const noexcept -> size_t { return m_buffer_size; }

		ML_NODISCARD auto buffer_used() const noexcept -> size_t { return m_buffer_used; }

		ML_NODISCARD auto buffer_free() const noexcept -> size_t { return m_buffer_size - m_buffer_used; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto front() & noexcept -> reference { return m_buffer_data[0]; }

		ML_NODISCARD auto front() const & noexcept -> const_reference { return m_buffer_data[0]; }

		ML_NODISCARD auto back() & noexcept -> reference { return m_buffer_data[m_buffer_size - 1]; }

		ML_NODISCARD auto back() const & noexcept -> const_reference { return m_buffer_data[m_buffer_size - 1]; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto begin() noexcept -> iterator { return m_buffer_data; }

		ML_NODISCARD auto begin() const noexcept -> const_iterator { return m_buffer_data; }

		ML_NODISCARD auto cbegin() const noexcept -> const_iterator { return begin(); }

		ML_NODISCARD auto end() noexcept -> iterator { return m_buffer_data + m_buffer_size; }

		ML_NODISCARD auto end() const noexcept -> const_iterator { return m_buffer_data + m_buffer_size; }

		ML_NODISCARD auto cend() const noexcept -> const_iterator { return end(); }

		ML_NODISCARD auto rbegin() noexcept -> reverse_iterator { return std::make_reverse_iterator(end()); }

		ML_NODISCARD auto rbegin() const noexcept -> const_reverse_iterator { return std::make_reverse_iterator(end()); }

		ML_NODISCARD auto crbegin() const noexcept -> const_reverse_iterator { return rbegin(); }

		ML_NODISCARD auto rend() noexcept -> reverse_iterator { return std::make_reverse_iterator(begin()); }

		ML_NODISCARD auto rend() const noexcept -> const_reverse_iterator { return std::make_reverse_iterator(begin()); }

		ML_NODISCARD auto crend() const noexcept -> const_reverse_iterator { return crend(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		void * do_allocate(size_t bytes, size_t align) override
		{
			++m_num_allocations;
			m_buffer_used += bytes;
			return m_resource->allocate(bytes, align);
		}

		void do_deallocate(void * ptr, size_t bytes, size_t align) override
		{
			--m_num_allocations;
			m_buffer_used -= bytes;
			return m_resource->deallocate(ptr, bytes, align);
		}

		bool do_is_equal(pmr::memory_resource const & value) const noexcept override
		{
			return m_resource->is_equal(value);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		pmr::memory_resource * m_resource;
		pointer const m_buffer_data;
		size_t const m_buffer_size;

		size_t m_num_allocations{};
		size_t m_buffer_used{};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// smart pointers
namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// no delete
	struct no_delete final
	{
		constexpr no_delete() noexcept = default;

		template <class U> void operator()(U *) const noexcept {}
	};

	// default delete
	template <class T> struct default_delete final
	{
		constexpr default_delete() noexcept = default;

		template <class U> void operator()(U * value) const noexcept
		{
			delete value;
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// shared pointer
	template <class T
	> ML_alias ref = typename std::shared_ptr<T>;

	// weak pointer
	template <class T
	> ML_alias unown = typename std::weak_ptr<T>;

	// unique pointer
	template <class T
	> ML_alias scope = typename std::unique_ptr<T, default_delete<T>>;

	// non-deleting pointer
	template <class T
	> ML_alias scary = typename std::unique_ptr<T, no_delete>;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class T, class Alloc = pmr::polymorphic_allocator<byte>, class ... Args
	> ML_NODISCARD ref<T> alloc_ref(Alloc alloc, Args && ... args)
	{
		return std::allocate_shared<T>(alloc, ML_forward(args)...);
	}

	template <class T, class ... Args
	> ML_NODISCARD ref<T> make_ref(Args && ... args)
	{
		return std::make_shared<T>(ML_forward(args)...);
	}

	template <class T, class ... Args
	> ML_NODISCARD scope<T> make_scope(Args && ... args)
	{
		static auto const g{ ML_get_global(memory_manager) };

		return { g->new_object<T>(ML_forward(args)...), default_delete<T>{} };
	}

	template <class T, class ... Args
	> ML_NODISCARD scary<T> make_scary(Args && ... args)
	{
		static auto const g{ ML_get_global(memory_manager) };

		return { g->new_object<T>(ML_forward(args)...), no_delete{} };
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// memory record
namespace ml
{
	// memory record
	struct ML_NODISCARD memory_record final
	{
		byte * addr; size_t index; size_t count; size_t size;

		constexpr operator bool() const noexcept
		{
			return addr && index && count && size;
		}
	};

	static void to_json(json & j, memory_record const & v)
	{
		j["index"	] = v.index;
		j["addr"	] = (intptr_t)v.addr;
		j["count"	] = v.count;
		j["size"	] = v.size;
	}

	static void from_json(json const & j, memory_record & v)
	{
		j["index"	].get_to(v.index);
		j["addr"	].get_to(*(intptr_t *)v.addr);
		j["count"	].get_to(v.count);
		j["size"	].get_to(v.size);
	}
}

// memory manager
namespace ml
{
	// memory manager
	struct ML_CORE_API memory_manager final : non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		enum : size_t { id_addr, id_index, id_count, id_size };

		using record_storage = typename ds::batch_vector
		<
			byte *	,	// address
			size_t	,	// index
			size_t	,	// count
			size_t		// size
		>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		memory_manager(pmr::memory_resource * mres = pmr::get_default_resource())
			: memory_manager{ ML_check(reinterpret_cast<passthrough_resource *>(mres)) }
		{
		}

		explicit memory_manager(passthrough_resource * mres);

		~memory_manager() noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// malloc
		ML_NODISCARD void * allocate(size_t size) noexcept
		{
			return this->do_allocate(1, size);
		}

		// calloc
		ML_NODISCARD void * allocate(size_t count, size_t size) noexcept
		{
			return this->do_allocate(count, size);
		}

		// free
		void deallocate(void * addr) noexcept
		{
			this->do_deallocate(addr);
		}

		// realloc
		void * reallocate(void * addr, size_t size) noexcept
		{
			return this->reallocate(addr, size, size);
		}

		// realloc (sized)
		void * reallocate(void * addr, size_t oldsz, size_t newsz) noexcept
		{
			if (newsz == 0)				{ this->deallocate(addr); return nullptr; }
			else if (addr == nullptr)	{ return this->allocate(newsz); }
			else if (newsz <= oldsz)	{ return addr; }
			else
			{
				auto const temp{ this->allocate(newsz) };
				if (temp)
				{
					std::memcpy(temp, addr, oldsz);
					this->deallocate(addr);
				}
				return temp;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// allocate object
		template <class T
		> ML_NODISCARD T * allocate_object(size_t count = 1) noexcept
		{
			return (T *)this->do_allocate(count, sizeof(T));
		}

		// deallocate object
		template <class T
		> void deallocate_object(T * addr) noexcept
		{
			this->do_deallocate(addr);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// new object
		template <class T, class ... Args
		> ML_NODISCARD T * new_object(Args && ... args) noexcept
		{
			auto ptr{ this->allocate_object<T>() };
			util::construct(ptr, ML_forward(args)...);
			return ptr;
		}

		// delete object
		template <class T
		> void delete_object(T * addr) noexcept
		{
			util::destruct(addr);
			this->deallocate_object(addr);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// get allocator
		ML_NODISCARD auto get_allocator() const noexcept -> allocator_type { return m_alloc; }

		// get counter
		ML_NODISCARD auto get_counter() const noexcept -> size_t { return m_counter; }

		// get resource
		ML_NODISCARD auto get_resource() const noexcept -> passthrough_resource * { return m_resource; }

		// get storage
		ML_NODISCARD auto get_storage() const noexcept -> record_storage const & { return m_records; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// get record
		ML_NODISCARD auto get_record(size_t i) const noexcept -> memory_record
		{
			return {
				m_records.at<id_addr>(i),
				m_records.at<id_index>(i),
				m_records.at<id_index>(i),
				m_records.at<id_index>(i)
			};
		}

		// get record address
		ML_NODISCARD auto get_record_addr(size_t i) const noexcept -> byte *
		{
			return m_records.at<id_addr>(i);
		}

		// get record index
		ML_NODISCARD auto get_record_index(size_t i) const noexcept -> size_t
		{
			return m_records.at<id_index>(i);
		}

		// get record count
		ML_NODISCARD auto get_record_count(size_t i) const noexcept -> size_t
		{
			return m_records.at<id_count>(i);
		}

		// get record size
		ML_NODISCARD auto get_record_size(size_t i) const noexcept -> size_t
		{
			return m_records.at<id_size>(i);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		void * do_allocate(size_t count, size_t size) noexcept
		{
			byte * const addr{ m_alloc.allocate(count * size) };

			m_records.push_back(addr, ++m_counter, count, size);

			return addr;
		}

		void do_deallocate(void * addr) noexcept
		{
			if (size_t const i{ m_records.lookup<id_addr>(addr) }; i != m_records.npos)
			{
				m_alloc.deallocate(
					(byte *)addr,
					m_records.at<id_count>(i) *
					m_records.at<id_size>(i));

				m_records.erase(i);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		passthrough_resource * const	m_resource	; // resource
		allocator_type					m_alloc		; // allocator
		record_storage					m_records	; // records
		size_t							m_counter	; // counter

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// global memory manager
namespace ml::globals
{
	ML_decl_global(memory_manager) get_global() noexcept;

	ML_decl_global(memory_manager) set_global(memory_manager * value) noexcept;
}

// c-like memory interface
namespace ml
{
	// malloc
	inline void * ml_malloc(size_t size) noexcept
	{
		return ML_get_global(memory_manager)->allocate(size);
	}

	// calloc
	inline void * ml_calloc(size_t count, size_t size) noexcept
	{
		return ML_get_global(memory_manager)->allocate(count, size);
	}

	// realloc
	inline void * ml_realloc(void * addr, size_t size) noexcept
	{
		return ML_get_global(memory_manager)->reallocate(addr, size);
	}

	// realloc (sized)
	inline void * ml_realloc(void * addr, size_t oldsz, size_t newsz) noexcept
	{
		return ML_get_global(memory_manager)->reallocate(addr, oldsz, newsz);
	}

	// free
	inline void ml_free(void * addr) noexcept
	{
		ML_get_global(memory_manager)->deallocate(addr);
	}
}

#define ML_free(addr)							(ML_get_global(_ML memory_manager)->deallocate(addr))
#define ML_malloc(size)							(ML_get_global(_ML memory_manager)->allocate(size))
#define ML_calloc(count, size)					(ML_get_global(_ML memory_manager)->allocate(count, size))
#define ML_realloc(addr, size)					(ML_get_global(_ML memory_manager)->reallocate(addr, size))
#define ML_realloc_sized(addr, oldsz, newsz)	(ML_get_global(_ML memory_manager)->reallocate(addr, oldsz, newsz))

// trackable
namespace ml
{
	// trackable base
	struct ML_CORE_API trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual ~trackable() noexcept = default;

		ML_NODISCARD void * operator new(size_t size) noexcept { return ML_malloc(size); }

		ML_NODISCARD void * operator new[](size_t size) noexcept { return ML_malloc(size); }

		void operator delete(void * addr) noexcept { ML_free(addr); }

		void operator delete[](void * addr) noexcept { ML_free(addr); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_MEMORY_HPP_