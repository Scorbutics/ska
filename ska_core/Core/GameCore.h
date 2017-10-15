#pragma once
#include <memory>

#include "State/StateHolder.h"
#include "../Exceptions/StateDiedException.h"
#include "../Data/Events/WorldEvent.h"
#include "../Data/Events/SoundEvent.h"
#include "../Inputs/RawInputListener.h"
#include "../Inputs/InputContextManager.h"
#include "../Utils/TimeUtils.h"
#include "Window.h"
#include "../GameApp.h"
#include "State/StateData.h"
#include "../Inputs/Readers/IniReader.h"
#include "../Exceptions/FileException.h"

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
    template <class EntityManager, class EventDispatcher, class DrawableContainer, class SoundManager>
    class GameCore :
		public GameApp,
		public Observer<StateEvent> {
    public:

        /**
         * \brief No-arg constructor that builds a basic context from a file.
         */
        GameCore() :
			Observer<StateEvent>(std::bind(&GameCore::onStateEvent, this, std::placeholders::_1)),
			m_entityManager(m_eventDispatcher),
    		m_soundManager(m_eventDispatcher),
            m_playerICM(m_rawInputListener, m_eventDispatcher),
			m_stateHolder(m_eventDispatcher) {

            //TODO better handling
#ifdef __ANDROID__
            int widthBlocks = 34;
            int heightBlocks = 53;
#else
            int widthBlocks = 30;
			int heightBlocks = 20;
#endif

			std::string title = "Default title";
			try {
				IniReader reader("gamesettings.ini");
				widthBlocks = reader.get<int>("Window width_blocks");
				heightBlocks = reader.get<int>("Window height_blocks");
				title = reader.get<std::string>("Window title");
			} catch (FileException& fe) {
				std::cerr << "Error while loading game settings : " << fe.what() << std::endl;
			}

			createWindow(title, widthBlocks, heightBlocks);

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
		void createWindow(const std::string& title, unsigned int wBlocks, unsigned int hBlocks) {
			static constexpr auto taillebloc = 32;
			m_mainWindow = std::make_unique<Window>(title, wBlocks * taillebloc, hBlocks * taillebloc);
		}

        bool refreshInternal() {
	        unsigned long t0 = ska::TimeUtils::getTicks();
			const auto ti = ticksWanted();
			auto accumulator = ti;

			try {
                for (;;) {
                    unsigned long t = ska::TimeUtils::getTicks();

					const auto ellapsedTime = t - t0;
					t0 = t;

                	accumulator += ellapsedTime;

					while (accumulator >= ti) {
						eventUpdate(static_cast<unsigned int>(ti));
						accumulator -= ti;
                    }

					graphicUpdate(ellapsedTime);
                }
            } catch (ska::StateDiedException&) {
                return true;
            }
        }

        void graphicUpdate(unsigned int ellapsedTime) {
        	m_stateHolder.graphicUpdate(ellapsedTime, m_drawables);
            m_drawables.draw();
            m_drawables.clear();

			m_mainWindow->display();
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
		std::unique_ptr<Window> m_mainWindow;

	private:
		EntityManager m_entityManager;
        DrawableContainer m_drawables;
        SoundManager m_soundManager;

        RawInputListener m_rawInputListener;
        InputContextManager m_playerICM;

        StateHolder m_stateHolder;

    };
}
