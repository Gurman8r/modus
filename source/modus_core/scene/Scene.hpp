#ifndef _ML_SCENE_HPP_
#define _ML_SCENE_HPP_

#include <modus_core/detail/EventSystem.hpp>
#include <modus_core/detail/LoopSystem.hpp>
#include <modus_core/detail/Matrix.hpp>
#include <entt/entt.hpp>

namespace ml
{
	struct entity;

	struct ML_CORE_API scene : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual ~scene() noexcept override = default;

		scene() noexcept : m_registry{} {}

		scene(scene && other) noexcept : m_registry{ std::move(other.m_registry) } {}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD entity new_entity(ds::string const & name = {}) noexcept;

		void delete_entity(entity const & value) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_runtime_update();

		void on_editor_update();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_registry() const noexcept -> entt::registry *
		{
			return const_cast<entt::registry *>(&m_registry);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		template <class T> void on_component_added(entity, T &) { static_assert(!"on_component_added"); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		friend entity;

		entt::registry m_registry; // registry

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

namespace ml
{
	inline void from_json(json const & j, scene & v) noexcept {}

	inline void to_json(json & j, scene const & v) noexcept {}
}

#endif // !_ML_SCENE_HPP_