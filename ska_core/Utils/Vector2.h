#pragma once

#include <vector>

namespace ska {

	template <class Container, class T>
	class View {
		using ViewT = View<Container, T>;
	public:
		View(Container& container, const std::size_t lineSize) :
			m_lineSize(lineSize),
			m_index(0),
			m_container(container) {
		}
		
		View(const ViewT&) = default;
		View(ViewT&&) = default;

		ViewT& operator=(ViewT&&) = default;
		ViewT& operator=(const ViewT&) = default;

		~View() = default;

		void setOffset(const std::size_t index) {
			m_index = index;
		}

		T& operator[] (const std::size_t x) {
			return m_container.get()(m_lineSize * x + m_index);
		}

		const T& operator[] (const std::size_t x) const {
			return m_container.get()(m_lineSize * x + m_index);
		}

		std::size_t size() const {
			return m_lineSize;
		}

		void setLineSize(const std::size_t lineSize) {
			m_lineSize = lineSize;
		}

	private:
		std::size_t m_lineSize;
		std::size_t m_index;
		std::reference_wrapper<Container> m_container;

	};

	template <class T>
	class Vector2 : private std::vector<T> {
		using ViewV = View<Vector2<T>, T>;
		using Parent = std::vector<T>;
	public:
		using Parent::begin;
		using Parent::end;
		using Parent::cbegin;
		using Parent::cend;
		using Parent::size;
		using Parent::resize;
		using Parent::reserve;
		using Parent::push_back;
		using Parent::emplace_back;
		using Parent::clear;
		using Parent::empty;

		explicit Vector2(const std::size_t lineSize) :
			m_view(*this, lineSize),
			m_lineSize(lineSize) { }

		Vector2() :
			Vector2(0) { }

		Vector2(Vector2<T>&&) = default;
		Vector2(const Vector2<T>&) = default;
		
		Vector2<T>& operator=(Vector2<T>&&) noexcept = default;
		Vector2<T>& operator=(const Vector2<T>&) = default;
		
		~Vector2() = default;

		ViewV& operator[] (const std::size_t x) {
			setupView(x);
			return m_view;
		}

		const ViewV& operator[] (const std::size_t x) const {
			setupView(x);
			return m_view;
		}

		T& operator() (const std::size_t x) {
			return Parent::operator[](x);
		}

		const T& operator() (const std::size_t x) const {
			return Parent::operator[](x);
		}

		void setLineSize(const std::size_t lineSize) {
			m_lineSize = lineSize;
			m_view.setLineSize(lineSize);
		}

		std::size_t lineSize() const {
			return m_lineSize == 0 && size() != 0 ? size() : m_lineSize;
		}

	private:
		void setupView(const std::size_t x) const {
			m_view.setOffset(x);
		}
		mutable ViewV m_view;
		std::size_t m_lineSize;

	};
}
