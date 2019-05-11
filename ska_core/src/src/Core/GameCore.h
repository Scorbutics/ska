#pragma once
#include <memory>

#include "State/StateHolder.h"
#include "Core/Utils/TimeUtils.h"
#include "GameApp.h"

namespace ska {
	/**
     * \brief Main class of the engine that holds the game engine loop and instantiate every core systems.
     * \tparam EventDispatcher The event dispatcher to use
     *
     * This class holds an instance of each type template-specified and transmits its to every new game state.
     * It's often a good idea to subclass it if you need accesses to the game window / event dispatcher outside of any state.
     */
    template <class EventDispatcher, class ... Modules>
    class GameCore :
		public GameApp {

    protected:
        using GameConf = ska::GameConfiguration<EventDispatcher, Modules...>;
		using GameConfPtr = std::unique_ptr<GameConf>;
    public:

        /**
         * \brief Constructor that builds a basic context from a file.
         */
	    explicit GameCore(GameConfPtr&& gc) :
			m_gameConfig(std::forward<GameConfPtr>(gc)),
    		m_eventDispatcher(m_gameConfig->getEventDispatcher()),
    		m_stateHolder(m_eventDispatcher){
        }

		~GameCore() override = default;

        /**
         * \brief Loop on an internal refresh to know if we need to continue to run the application or not.
         */
        void run() override {
            auto continuer = true;
            while (continuer) {
                continuer = refreshInternal();
            }
        }

        /**
		 * \brief Creates a state and navigates to it.
         * \tparam StateT state type
         * \tparam Args additional parameters to the state constructor
         * \param args Args&&... additional parameters to the state constructor
         * \return StateT& The resulting state reference
         */
		template<class StateT, class ... Args>
		StateT& navigateToState(Args&&... args) {
			auto sc = std::make_unique<StateT>(std::forward<Args>(args)...);
			auto result = sc.get();
			m_stateHolder.nextState(std::move(sc));
			return *result;
		}

    private:
		bool reduceAccumulatorAndEventUpdate(float& accumulator, const float ti) {
			while (accumulator >= ti) {
				if(eventUpdate(ti) || m_accumulatorReset) { m_accumulatorReset = false; return true; }
				accumulator -= ti;
			}
			return false;
		}

        bool refreshInternal() {
  	        unsigned long t0 = TimeUtils::getTicks();
			const auto ti = ticksWanted();
			auto accumulator = ti;

			for (;;) {
				const unsigned long t = TimeUtils::getTicks();
				const auto ellapsedTime = t - t0;
				t0 = t;

				accumulator += ellapsedTime;
				if(reduceAccumulatorAndEventUpdate(accumulator, ti)) {
					graphicUpdate(ellapsedTime);
					break;
				}

				graphicUpdate(ellapsedTime);
			}
	
			return true;
        }

        void graphicUpdate(unsigned int ellapsedTime) {
			m_gameConfig->graphicUpdate(ellapsedTime, m_stateHolder);
        }

        bool eventUpdate(unsigned int ellapsedTime) {
			const auto stateChange = m_stateHolder.update();
			m_stateHolder.eventUpdate(ellapsedTime);
			m_gameConfig->eventUpdate(ellapsedTime);
			return stateChange;
        }

		GameConfPtr m_gameConfig;
		bool m_accumulatorReset = false;
    protected:
        EventDispatcher& m_eventDispatcher;
		void resetAccumulator() {
			m_accumulatorReset = true;
		}

	private:
        StateHolder m_stateHolder;
		
    };
}
