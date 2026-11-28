#pragma once

namespace ska {
	// The drawings sorted by their ground line (the characters, the standing tiles of the maps) : by their pixels from the
	// top of the camera, from this priority (a world of any size : the same priorities ; a drawing a little above the
	// camera stays over the ones under DepthPriority, ex : the flat tiles, a battleground)
	constexpr int DepthPriority = 4096;
    //TODO refacto avec DrawableFixedPriority
    class Priorized {
    public:
	    virtual ~Priorized() = default;

	    Priorized() :
            m_priority(0) {
        }

        virtual void setPriority(int p) {
			m_priority = p;
		}

		virtual int getPriority() const {
			return m_priority;
		}

		template<class T>
        static bool comparatorInf(const T& a, const T& b) {
			int p1 = a->getPriority();
			int p2 = b->getPriority();
			return p1 < p2;
		}

        template<class T>
        static bool comparatorSup(const T& a, const T& b) {
			int p1 = a->getPriority();
			int p2 = b->getPriority();
			return p1 > p2;
		}

		bool operator< (const Priorized &id) {
			int p1 = getPriority();
			int p2 = id.getPriority();
			return p1 < p2;
		}

		bool operator> (const Priorized &id) {
			int p1 = getPriority();
			int p2 = id.getPriority();
			return p1 > p2;
		}

	private:
		int m_priority;
    };
}
