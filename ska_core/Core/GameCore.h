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
#include "Window.h"
#include "../Draw/Renderer.h"

namespace ska {
	class Window;
	class Renderer;

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
    template <class EntityManager, class EventDispatcher, class DrawableContainer, class SoundManager>
    class GameCore :
		public GameApp,
		public Observer<StateEvent> {

	    public:
			using RendererPtr = std::unique_ptr<Renderer>;
			using WindowPtr = std::unique_ptr<Window>;
			using DrawableContainerPtr = std::unique_ptr<DrawableContainer>;

        /**
         * \brief Constructor that builds a basic context from a file.
         */
	    explicit GameCore(RendererPtr&& renderer, WindowPtr&& window) :
			Observer<StateEvent>(std::bind(&GameCore::onStateEvent, this, std::placeholders::_1)),
			m_mainWindow(std::forward<WindowPtr>(window)),
    		m_entityManager(m_eventDispatcher),
            m_soundManager(m_eventDispatcher),
			m_playerICM(m_rawInputListener, m_eventDispatcher),
    		m_stateHolder(m_eventDispatcher),
			m_renderer(std::forward<RendererPtr>(renderer)) {

			m_mainWindow->show();
			m_drawables = std::make_unique<DrawableContainer>(*m_renderer);

            m_eventDispatcher.template addMultipleObservers<SoundEvent, WorldEvent, StateEvent>(m_soundManager, m_soundManager, *this);
        }

        virtual ~GameCore() {
			m_eventDispatcher.template removeMultipleObservers<SoundEvent, WorldEvent, StateEvent>(m_soundManager, m_soundManager, *this);
        }

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
		 * \brief Creates a state and returns without adding it to the state queue.
         * \tparam StateT state type
         * \tparam Args additional parameters to the state constructor
         * \param args Args&&... additional parameters to the state constructor
         * \return std::unique_ptr<StateT> The resulting state
         */
		template<class StateT, class ... Args>
        std::unique_ptr<StateT> makeState(Args&&... args) {
			StateData<EntityManager, EventDispatcher> data(m_entityManager, m_eventDispatcher);
			return std::make_unique<StateT>(data, m_stateHolder, std::forward<Args>(args)...);
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
			auto sc = makeState<StateT, Args...>(std::forward<Args>(args)...);
			auto result = sc.get();
			m_stateHolder.nextState(std::move(sc));
			return *result;
		}

    protected:
		template<class I, class ... Args>
        /**
         * \brief Adds an input context to the game.
         * \tparam I input context type
         * \tparam Args additional parameters to the input context constructor
         * \param em EnumContextManager
         * \param args Args&&... additional parameters to the input context constructor
         *
         * Having multiple input contexts in a game allow to have specific ways to query inputs depending from where we are in the game.
         */
        void addInputContext(EnumContextManager em, Args&&... args) {
            m_playerICM.addContext(em, std::make_unique<I>(m_rawInputListener, std::forward<Args>(args)...));
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
        	m_stateHolder.graphicUpdate(ellapsedTime, *m_drawables);
            m_drawables->draw();
			m_renderer->update();
			m_drawables->clear();
        }

        void eventUpdate(unsigned int ellapsedTime) {
			m_entityManager.refresh();
			/* Raw input acquisition */
			m_playerICM.refresh();

			m_stateHolder.update();
            m_stateHolder.eventUpdate(ellapsedTime);
        }

		bool onStateEvent(StateEvent& se) {
			if(se.type == FIRST_STATE_LOADED) {
				GameEvent ge(GAME_WINDOW_READY);
				ge.windowWidth = m_mainWindow->getWidth();
				ge.windowHeight = m_mainWindow->getHeight();
				m_eventDispatcher.Observable<GameEvent>::notifyObservers(ge);
				return true;
			}
			return false;
		}

	protected:
		EventDispatcher m_eventDispatcher;
		WindowPtr m_mainWindow;

	private:
		EntityManager m_entityManager;
		DrawableContainerPtr m_drawables;
        SoundManager m_soundManager;

        RawInputListener m_rawInputListener;
        InputContextManager m_playerICM;

        StateHolder m_stateHolder;

		RendererPtr m_renderer;

    };
}
