#pragma once
#include <memory>

#include "State/StateHolder.h"
#include "../Exceptions/StateDiedException.h"
#include "../Data/Events/WorldEvent.h"
#include "../Data/Events/SoundEvent.h"
#include "../Inputs/RawInputListener.h"
#include "../Inputs/InputContextManager.h"
#include "../Utils/TimeUtils.h"
#include "../GameApp.h"
#include "State/StateData.h"

namespace ska {
	/**
     * \brief Main class of the engine that holds the game engine loop and instantiate every core systems.
     * \tparam EntityManager The entity manager to use
     * \tparam EventDispatcher The event dispatcher to use
     * \tparam DrawableContainer The drawable container to use
     * \tparam SoundManager The sound manager to use
     *
     * This class holds an instance of each type template-specified and transmits its to every new game state.
     * It's often a good idea to subclass it if you need accesses to the game window / event dispatcher outside of any state.
     */
    template <class EventDispatcher>
    class GameCore :
		public GameApp {

    protected:
        using GameConf = ska::GameConfiguration<EventDispatcher>;

    public:

        /**
         * \brief Constructor that builds a basic context from a file.
         */
	    explicit GameCore(GameConf&& gc) :
    		m_eventDispatcher(gc.getEventDispatcher()),
    		m_stateHolder(m_eventDispatcher),
    		m_gameConfig(std::forward<GameConf>(gc)){
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
        bool refreshInternal() {
  	        unsigned long t0 = TimeUtils::getTicks();
			const auto ti = ticksWanted();
			auto accumulator = ti;

			try {
                for (;;) {
	                const unsigned long t = TimeUtils::getTicks();

					const auto ellapsedTime = t - t0;
					t0 = t;

                	accumulator += ellapsedTime;

					while (accumulator >= ti) {
						eventUpdate(static_cast<unsigned int>(ti));
						accumulator -= ti;
                    }

					graphicUpdate(ellapsedTime);
                }
            } catch (StateDiedException&) {
                return true;
            }
        }

        void graphicUpdate(unsigned int ellapsedTime) {
            //TODO : éviter "getModules"
            auto& modules = m_gameConfig.getModules();
            for(auto& module : modules) {
                //TODO : modules graphiques != modules logiques
                module->graphicUpdate(ellapsedTime, m_stateHolder);
            }
        }

        void eventUpdate(unsigned int ellapsedTime) {
			m_stateHolder.update();
            m_stateHolder.eventUpdate(ellapsedTime);

            //TODO : éviter "getModules"
            auto& modules = m_gameConfig.getModules();
            for(auto& module : modules) {
                //TODO : modules graphiques != modules logiques
                module->eventUpdate(ellapsedTime);
            }
        }

    protected:
        EventDispatcher& m_eventDispatcher;

	private:
        StateHolder m_stateHolder;
		GameConf m_gameConfig;
    };
}
