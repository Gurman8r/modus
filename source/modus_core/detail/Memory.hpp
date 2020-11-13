#ifndef _ML_MEMORY_HPP_
#define _ML_MEMORY_HPP_

#include <modus_core/detail/BatchVector.hpp>
#include <modus_core/detail/Debug.hpp>

// passthrough resource
namespace ml
{
	// proxy for testing an upstream resource
	struct passthrough_resource final : public pmr::memory_resource, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using pointer			= typename byte_t *;
		using const_pointer		= typename byte_t const *;
		using reference			= typename byte_t &;
		using const_reference	= typename byte_t const &;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		explicit passthrough_resource(pmr::memory_resource * u, pointer const b, size_t c) noexcept
			: m_upstream{ u }, m_buffer{ b }, m_capacity{ c }
		{
			ML_assert("invalid passthrough_resource parameters" && u && b && c);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto base() const noexcept -> size_t { return (size_t)m_buffer; }

		ML_NODISCARD auto capacity() const noexcept -> size_t { return m_capacity; }

		ML_NODISCARD auto count() const noexcept -> size_t { return m_count; }
		
		ML_NODISCARD auto data() const noexcept -> pointer const { return m_buffer; }

		ML_NODISCARD auto free() const noexcept -> size_t { return m_capacity - m_used; }

		ML_NODISCARD auto resource() const noexcept -> pmr::memory_resource * const { return m_upstream; }

		ML_NODISCARD auto used() const noexcept -> size_t { return m_used; }
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto fraction() const noexcept -> float_t { return (float_t)m_used / (float_t)m_capacity; }

		ML_NODISCARD auto percentage() const noexcept -> float_t { return fraction() * 100.f; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto front() & noexcept -> reference { return m_buffer[0]; }

		ML_NODISCARD auto front() const & noexcept -> const_reference { return m_buffer[0]; }

		ML_NODISCARD auto back() & noexcept -> reference { return m_buffer[m_capacity - 1]; }

		ML_NODISCARD auto back() const & noexcept -> const_reference { return m_buffer[m_capacity - 1]; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto begin() noexcept -> pointer { return m_buffer; }

		ML_NODISCARD auto begin() const noexcept -> const_pointer { return m_buffer; }

		ML_NODISCARD auto cbegin() const noexcept -> const_pointer { return begin(); }

		ML_NODISCARD auto end() noexcept -> pointer { return m_buffer + m_capacity; }

		ML_NODISCARD auto end() const noexcept -> const_pointer { return m_buffer + m_capacity; }

		ML_NODISCARD auto cend() const noexcept -> const_pointer { return end(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		void * do_allocate(size_t bytes, size_t align) override
		{
			++m_count;
			m_used += bytes;
			return m_upstream->allocate(bytes, align);
		}

		void do_deallocate(void * ptr, size_t bytes, size_t align) override
		{
			--m_count;
			m_used -= bytes;
			return m_upstream->deallocate(ptr, bytes, align);
		}

		bool do_is_equal(pmr::memory_resource const & value) const noexcept override
		{
			return m_upstream->is_equal(value);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		pmr::memory_resource * m_upstream;
		pointer const m_buffer;
		size_t const m_capacity;

		size_t m_count{};
		size_t m_used{};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// deleters
namespace ml
{
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
}

// pointers
namespace ml
{
	// smart pointers
	namespace ds
	{
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
	}

	template <class T, class Alloc = pmr::polymorphic_allocator<byte_t>, class ... Args
	> ML_NODISCARD ds::ref<T> alloc_ref(Alloc alloc, Args && ... args)
	{
		return std::allocate_shared<T>(alloc, ML_forward(args)...);
	}

	template <class T, class ... Args
	> ML_NODISCARD ds::ref<T> make_ref(Args && ... args)
	{
		return std::make_shared<T>(ML_forward(args)...);
	}

	template <class T, class ... Args
	> ML_NODISCARD ds::scope<T> make_scope(Args && ... args)
	{
		auto const g{ ML_check(get_global<memory_manager>()) };

		return { g->new_object<T>(ML_forward(args)...), default_delete<T>{} };
	}

	template <class T, class ... Args
	> ML_NODISCARD ds::scary<T> make_scary(Args && ... args)
	{
		auto const g{ ML_check(get_global<memory_manager>()) };

		return { g->new_object<T>(ML_forward(args)...), no_delete{} };
	}
}

// memory record
namespace ml
{
	// memory record
	struct ML_NODISCARD memory_record final
	{
		byte_t * addr; size_t index; size_t count; size_t size;

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

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		enum : size_t { id_addr, id_index, id_count, id_size };

		using record_storage = typename ds::batch_vector
		<
			byte_t *	,	// address
			size_t		,	// index
			size_t		,	// count
			size_t			// size
		>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit memory_manager(passthrough_resource & mres);

		memory_manager(pmr::memory_resource * mres = pmr::get_default_resource()) noexcept(false)
			: memory_manager{ *reinterpret_cast<passthrough_resource *>(mres) }
		{
		}

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
			return util::construct(this->allocate_object<T>(), ML_forward(args)...);
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
		ML_NODISCARD auto get_record_addr(size_t i) const noexcept -> byte_t *
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
			auto const addr{ m_alloc.allocate(count * size) };

			return std::get<id_addr>(m_records.push_back
			(
				addr, ++m_counter, count, size
			));
		}

		void do_deallocate(void * addr) noexcept
		{
			if (auto const i{ m_records.lookup<id_addr>(addr) }; i != m_records.npos)
			{
				m_alloc.deallocate(
					(byte_t *)addr,
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
	ML_decl_global(memory_manager) get() noexcept;

	ML_decl_global(memory_manager) set(memory_manager * value) noexcept;
}

// c-like interface
namespace ml
{
	// malloc
	inline void * ml_malloc(size_t size) noexcept
	{
		return ML_check(get_global<memory_manager>())->allocate(size);
	}

	// calloc
	inline void * ml_calloc(size_t count, size_t size) noexcept
	{
		return ML_check(get_global<memory_manager>())->allocate(count, size);
	}

	// realloc
	inline void * ml_realloc(void * addr, size_t size) noexcept
	{
		return ML_check(get_global<memory_manager>())->reallocate(addr, size);
	}

	// realloc (sized)
	inline void * ml_realloc(void * addr, size_t oldsz, size_t newsz) noexcept
	{
		return ML_check(get_global<memory_manager>())->reallocate(addr, oldsz, newsz);
	}

	// free
	inline void ml_free(void * addr) noexcept
	{
		ML_check(get_global<memory_manager>())->deallocate(addr);
	}
}

// trackable
namespace ml
{
	// trackable base
	struct ML_CORE_API trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual ~trackable() noexcept = default;

		ML_NODISCARD void * operator new(size_t size) noexcept { return ml_malloc(size); }

		ML_NODISCARD void * operator new[](size_t size) noexcept { return ml_malloc(size); }

		void operator delete(void * addr) noexcept { ml_free(addr); }

		void operator delete[](void * addr) noexcept { ml_free(addr); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_MEMORY_HPP_