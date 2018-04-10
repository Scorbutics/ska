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
		View(ViewT&&) noexcept = default;

		ViewT& operator=(ViewT&&) noexcept = default;
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
		using Parent::erase;
		using Parent::begin;
		using Parent::end;
		using Parent::cbegin;
		using Parent::cend;
		using Parent::size;
		using Parent::push_back;
		using Parent::emplace_back;
		using Parent::clear;
		using Parent::empty;

		explicit Vector2(const std::size_t lineSize) :
			m_view(*this, lineSize),
			m_lineSize(lineSize) { }

		Vector2() :
			Vector2(0) { }

		Vector2(const std::size_t lineSize, std::initializer_list<T> list) :
			Parent(std::move(list)),
			m_view(*this, lineSize),
			m_lineSize(lineSize){ }

		explicit Vector2(Vector2<T>&& v) noexcept :
			m_view(*this, v.m_lineSize)  {
			*this = std::move(v);
		}
		Vector2(const Vector2<T>&) = default;
		
		Vector2<T>& operator=(Vector2<T>&& v) noexcept {
			std::swap(m_lineSize, v.m_lineSize);
			m_view = ViewV(*this, m_lineSize);
			std::vector<T>::operator=(std::move(v));
			return *this;
		}

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

		void resize(const std::size_t width, const std::size_t height) {
			setLineSize(width);
			Parent::resize(width * height);
		}

		void reserve(const std::size_t width, const std::size_t height){
			setLineSize(width);
			Parent::reserve(width * height);
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
