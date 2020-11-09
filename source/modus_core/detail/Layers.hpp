#ifndef _ML_LAYERS_HPP_
#define _ML_LAYERS_HPP_

#include <modus_core/detail/Events.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct layer;
	struct layer_stack;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct layer : non_copyable, event_listener
	{
		explicit layer(event_bus * bus) noexcept : event_listener{ bus } {}

		virtual ~layer() noexcept override = default;

		virtual void on_attach() = 0;

		virtual void on_detach() = 0;

		virtual void on_event(event const &) override = 0;
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct layer_stack : non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using storage_type		= typename ds::list<layer *>;
		using iterator			= typename storage_type::iterator;
		using const_iterator	= typename storage_type::const_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		layer_stack(event_bus * bus) noexcept : m_bus{ bus }
		{
		}

		~layer_stack() noexcept
		{
			for (auto l : m_layers)
			{
				delete l;
			}
			m_layers.clear();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void push_layer(layer * value) noexcept
		{
			if (auto const it{ std::find(begin(), end(), value) }; it == end())
			{
				m_layers.emplace(begin() + m_index, value);
				m_index++;
			}
		}

		void pop_layer(layer * value) noexcept
		{
			if (auto const it{ std::find(begin(), end(), value) }; it != end())
			{
				m_layers.erase(it);
				m_index--;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void push_overlay(layer * value) noexcept
		{
			if (auto const it{ std::find(begin(), end(), value) }; it == end())
			{
				m_layers.emplace_back(value);
			}
		}

		void pop_overlay(layer * value) noexcept
		{
			if (auto const it{ std::find(begin(), end(), value) }; it != end())
			{
				m_layers.erase(it);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_bus() const noexcept -> event_bus * { return m_bus; }

		ML_NODISCARD auto get_layers() noexcept -> ds::list<layer *> & { return m_layers; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto begin() noexcept -> iterator { return m_layers.begin(); }
		
		ML_NODISCARD auto begin() const noexcept -> const_iterator { return m_layers.begin(); }
		
		ML_NODISCARD auto cbegin() const noexcept -> const_iterator { return m_layers.cbegin(); }

		ML_NODISCARD auto end() noexcept -> iterator { return m_layers.end(); }
		
		ML_NODISCARD auto end() const noexcept -> const_iterator { return m_layers.end(); }
		
		ML_NODISCARD auto cend() const noexcept -> const_iterator { return m_layers.cend(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		event_bus * const		m_bus		; // 
		ds::list<layer *>	m_layers	; // 
		size_t					m_index		; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_LAYERS_HPP_