#ifndef _ML_SCENE_HPP_
#define _ML_SCENE_HPP_

#include <modus_core/detail/EventSystem.hpp>
#include <modus_core/detail/LoopSystem.hpp>
#include <modus_core/detail/Matrix.hpp>
#include <entt/entt.hpp>

namespace ml
{
	struct entity;

	struct ML_CORE_API scene : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		scene() noexcept : m_registry{} {}

		scene(scene && other) noexcept : m_registry{ std::move(other.m_registry) } {}

		~scene() noexcept override = default;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_registry() noexcept -> entt::registry & { return m_registry; }

		ML_NODISCARD auto get_registry() const noexcept -> entt::registry const & { return m_registry; }

		ML_NODISCARD entity new_entity(ds::string const & name = {}) noexcept;

		void delete_entity(entity const & value) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_runtime_update();

		void on_editor_update();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void from_json(json const & j);

		void to_json(json & j) const;

		ML_NODISCARD json to_json() const noexcept { json j; this->to_json(j); return j; }

		std::istream & deserialize(std::istream & in)
		{
			this->from_json(json::parse(in));
			return in;
		}

		std::ostream & serialize(std::ostream & out) const
		{
			return out << this->to_json();
		}

		bool load_from_file(fs::path const & path)
		{
			std::ifstream f{ path };
			ML_defer(&f) { f.close(); };
			if (!f) { return debug::failure("failed reading scene: \'{0}\'", path); }
			deserialize(f);
			return true;
		}

		bool save_to_file(fs::path const & path) const
		{
			std::ofstream f{ path };
			ML_defer(&f) { f.close(); };
			if (!f) { return debug::failure("failed writing scene: \'{0}\'", path); }
			serialize(f);
			return true;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		template <class T> void on_component_added(entity, T &) {}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		friend entity;

		entt::registry m_registry; // registry

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

namespace ml
{
	inline void from_json(json const & j, scene & v) noexcept { v.from_json(j); }

	inline void to_json(json & j, scene const & v) noexcept { v.to_json(j); }
}

#endif // !_ML_SCENE_HPP_