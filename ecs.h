#pragma once
#include <vector>

namespace ecs {

	template <typename ...Component>
	struct Entity : std::reference_wrapper<Component>... {

		Entity(Component &...component) : std::reference_wrapper<Component>(component)...{}

		template <typename Component>
		inline Component &getComponent() {
			return *(std::reference_wrapper<Component>*)this;
		}
	};

	template <typename Container, typename Element, auto Interface>
	struct Iterator {

		Iterator(size_t idx, Container *container)
			: m_current(idx), m_container(container) {}

		inline Iterator &operator++() noexcept {
			++m_current;
			return *this;
		}

		inline Iterator operator++(int) noexcept {
			return Iterator(m_current++, m_container);
		}

		inline Iterator &operator--() noexcept {
			--m_current;
			return *this;
		}

		inline Iterator operator--(int) noexcept {
			return Iterator(m_current--, m_container);
		}

		inline Iterator operator+(const size_t step) const noexcept {
			return Iterator(m_current + step, m_container);
		}

		inline Iterator operator-(const size_t step) const noexcept {
			return Iterator(m_current - step, m_container);
		}

		inline Element operator*() const noexcept {
			return (m_container->*Interface)(m_current);
		}

		inline bool operator==(Iterator rs) const noexcept {
			return m_container == rs.m_container && m_current == rs.m_current;
		}

		inline bool operator!=(Iterator rs) const noexcept {
			return m_container != rs.m_container || m_current != rs.m_current;
		}

	private:
		Container *m_container;
		size_t m_current;
	};

	template <typename ...Component>
	struct GroupOfEntities : private std::vector<Component>... {

		template <typename Component>
		inline auto &list() noexcept {
			return *(std::vector<Component>*)this;
		}

		template <typename Component>
		inline auto begin() noexcept {
			return std::vector<Component>::begin();
		}

		template <typename Component>
		inline auto end() noexcept {
			return std::vector<Component>::end();
		}

		template <typename Component>
		inline Component &get(const size_t idx) {
			return std::vector<Component>::operator[](idx);
		}

		inline Entity<Component...> getEntity(const size_t idx) {

			return Entity<Component...>(
				std::vector<Component>::operator[](idx)...
				);
		}

		inline void addEntity() {
			(std::vector<Component>::push_back({}), ...);

			++m_size;
		}

		inline void addEntity(const Component &...component) {
			(std::vector<Component>::push_back(component), ...);

			++m_size;
		}

		inline void addEntity(Entity<Component...> &&entity) {
			(std::vector<Component>::push_back(entity.getComponent<Component>()), ...);

			++m_size;
		}

		inline void removeEntity(const size_t idx) {
			(std::vector<Component>::erase(std::vector<Component>::begin() + idx), ...);

			--m_size;
		}

		inline void removeSubset(const size_t idx, const size_t size) {

			for(auto i = idx; i < idx + size; ++i)
				(std::vector<Component>::erase(std::vector<Component>::begin() + idx), ...);

			m_size -= size;
		}

		inline size_t size() {
			return m_size;
		}

		inline GroupOfEntities<Component...> getSubset(const size_t idx, const size_t size) const {

			GroupOfEntities<Component...> tmp;

			for(auto i = idx; i < idx + size; ++i)
				(((std::vector<Component>*)&tmp)->push_back(std::vector<Component>::operator[](i)), ...);

			return tmp;
		}

		inline void addSet(const GroupOfEntities<Component...> entities, const size_t idx, const size_t size) { //TO BE TESTED

			for(auto i = idx; i < idx + size; ++i)
				(std::vector<Component>::push_back(entities.get<Component>(i)), ...);

			m_size += size;
		}

		inline void addSet(const GroupOfEntities<Component...> entities) { //TO BE TESTED

			for(auto entity : this)
				(std::vector<Component>::push_back(entity.getComponent<Component>()), ...);

			m_size += size;
		}

		inline Iterator<GroupOfEntities<Component...>, Entity<Component...>,
			&GroupOfEntities<Component...>::getEntity> begin() noexcept {
			return Iterator<GroupOfEntities<Component...>, Entity<Component...>,
				&GroupOfEntities<Component...>::getEntity>(0, this);
		}

		inline Iterator<GroupOfEntities<Component...>, Entity<Component...>,
			&GroupOfEntities<Component...>::getEntity> end() noexcept {
			return Iterator<GroupOfEntities<Component...>, Entity<Component...>,
				&GroupOfEntities<Component...>::getEntity>(m_size, this);
		}

	private:
		size_t m_size = 0;
	};

}