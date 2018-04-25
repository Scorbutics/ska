#pragma once

#include <vector>
#include <gsl/pointers>

namespace ska {

	template <class Container>
	struct View {
		View(Container& container, const std::size_t lineSize, const std::size_t index) :
			container(&container),
			lineSize(lineSize),
			index(index) {
		}

		const gsl::not_null<Container*> container;
		const std::size_t lineSize;
		const std::size_t index;

		auto& operator[] (const std::size_t x) {
			return (*container)(lineSize * x + index);
		}

		const auto& operator[] (const std::size_t x) const {
			return (*container)(lineSize * x + index);
		}

	};

	template <class Container>
	View<Container> MakeView(Container& container, const std::size_t lineSize, const std::size_t index) {
		return { container, lineSize, index };
	}

	template <class T>
	class Vector2 : private std::vector<T> {
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
			m_lineSize(lineSize) { }

		Vector2() :
			Vector2(0) { }

		Vector2(const std::size_t lineSize, std::initializer_list<T> list) :
			Parent(std::move(list)),
			m_lineSize(lineSize){ }

		explicit Vector2(Vector2<T>&& v) noexcept {
			*this = std::move(v);
		}
		Vector2(const Vector2<T>&) = default;
		
		Vector2<T>& operator=(Vector2<T>&& v) noexcept {
			std::swap(m_lineSize, v.m_lineSize);
			std::vector<T>::operator=(std::move(v));
			return *this;
		}

		Vector2<T>& operator=(const Vector2<T>&) = default;
		
		~Vector2() = default;

		auto operator[] (const std::size_t x) {
			return MakeView(*this, m_lineSize, x);
		}

		auto operator[] (const std::size_t x) const {
			return MakeView(*this, m_lineSize, x);
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
		}

		std::size_t lineSize() const {
			return m_lineSize == 0 && size() != 0 ? size() : m_lineSize;
		}

		bool has(const std::size_t x, const std::size_t y) const {
			return x * y < size();
		}

	private:
		std::size_t m_lineSize{};

	};
}
