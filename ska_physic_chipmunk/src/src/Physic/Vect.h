#pragma once
#include <chipmunk.h>
#include "Point.h"

namespace ska {
	namespace cp {
		
		//2D Vector wrapper class
		class Vect {
		public:
			template <class U1, class U2>
			Vect(const U1& x, const U2& y) : 
				m_vect(cpv(x, y)) {
			}

			template <class T>
			Vect(const Point<T>& p) : 
				m_vect(cpv(p.x, p.y)) {
			}

			Vect(cpVect v) : 
				m_vect(std::move(v)) {
			}

			Vect() : 
				m_vect(cpvzero) {
			}

			~Vect() = default;

			inline const cpVect& vect() const {
				return m_vect;
			}

		private:
			cpVect m_vect;
		};
	}
}
