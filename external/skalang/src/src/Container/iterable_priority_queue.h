#pragma once
#include <queue>

namespace ska {
	template <class T, class Container = std::vector<T>,
        	class Compare = std::less<T> >
	class iterable_priority_queue {
	protected:
    	Container c;
    	Compare comp;
	public:
    	explicit iterable_priority_queue(Container c_  = Container(),
                            	Compare comp_ = Compare())
        	: c(std::move(c_)), comp(std::move(comp_)) {
    	}
    	
    	bool empty()       const { return c.empty(); }
    	std::size_t size() const { return c.size(); }
    	const T& top()     const { return c.front(); }
    	
    	void push(T x) {
        	c.push_back(std::move(x));
        	std::sort(c.begin(), c.end(), comp);
    	}

    	void pop() {
        	c.pop_back();
        	std::sort(c.begin(), c.end(), comp);
    	}

    	void remove(const T& x) {
        	auto it = std::find(c.begin(), c.end(), x);
        	if (it != c.end()) {
            	erase(it);
        	}
    	}
    	
    	void erase(const decltype(c.begin())& it) {
        	c.erase(it);
        	std::sort(c.begin(), c.end(), comp);
    	}

    	auto begin() {
        	return std::begin(c);
    	}
    	
    	auto end() {
        	return std::end(c);
    	}
	};
}
