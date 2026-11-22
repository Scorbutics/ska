#pragma once
#include <memory>

#include "State/StateHolder.h"
#include "Core/Utils/TimeUtils.h"
#include "Core/Draw/FrameClock.h"
#include "GameApp.h"
#include "Core/Exceptions/TerminateProcessException.h"
#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#endif

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
#if defined(__EMSCRIPTEN__)
			// The browser calls a frame at each refresh of the display : the loop cannot block it. run does not return
			// (the C++ stack is unwound : the game is not on it)
			emscripten_set_main_loop_arg([](void* core) {
				auto& self = *static_cast<GameCore*>(core);
				try {
					self.frame();
				} catch (TerminateProcessException& tpe) {
					emscripten_cancel_main_loop();
					self.onTerminate(tpe);
				} catch (GenericException& e) {
					emscripten_cancel_main_loop();
					self.onException(e);
				}
			}, this, 0, true);
#else
			for (;;) {
				frame();
			}
#endif
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
		bool reduceAccumulatorAndEventUpdate(float& accumulator, float ti) {
			while (accumulator >= ti) {
				const auto stateChange = eventUpdate(static_cast<unsigned int>(ti));
				FrameClock::logicUpdated();
				if(stateChange || m_accumulatorReset) { m_accumulatorReset = false; return true; }
				accumulator -= ti;
			}
			return false;
		}

		// One frame : the logic updates due since the last frame (fixed time steps), then the drawing
		void frame() {
			const auto ti = ticksWanted();
			if (!m_frameStarted) {
				// The start, or a new state : the time starts again
				m_frameTime = TimeUtils::getTicks();
				m_accumulator = ti;
				m_frameStarted = true;
			}
			const unsigned long t = TimeUtils::getTicks();
			const auto ellapsedTime = t - m_frameTime;
			m_frameTime = t;

			m_accumulator += ellapsedTime;
			if (reduceAccumulatorAndEventUpdate(m_accumulator, ti)) {
				FrameClock::drawing(1.F);
				graphicUpdate(ellapsedTime);
				m_frameStarted = false;
				return;
			}

			// The moving things are drawn between their last two logic states (FrameClock)
			FrameClock::drawing(m_accumulator / ti);
			graphicUpdate(ellapsedTime);
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
		bool m_frameStarted = false;
		unsigned long m_frameTime = 0;
		float m_accumulator = 0.F;
    protected:
        EventDispatcher& m_eventDispatcher;
		void resetAccumulator() {
			m_accumulatorReset = true;
		}

	private:
        StateHolder m_stateHolder;
		
    };
}
