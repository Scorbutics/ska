#pragma once
#include <memory>

#include "Exceptions/TerminateProcessException.h"
#include "Ticked.h"
#include "Core/GameConfiguration.h"

namespace ska {

	class GenericException;

    /**
     * \brief Defines the basic behaviour of any application using this engine.
     * As you can see, the "get" static function is not defined here.
     * There is no cpp file attached to this header, meaning that "get" has to be defined by the user application.
     * Instead of creating a "main" function, you have to define this "get" function and also, a GameApp instance.
     */
	class GameApp : public Ticked {

	protected:
		using GameAppPtr = std::unique_ptr<GameApp>;
		explicit GameApp(ska::GameConfiguration&& gc);

		/**
		* \brief User provided static method to inject game application dependency into the main function.
		* \return The created game application.
		*/
		static std::unique_ptr<ska::GameApp> get();

	public:
		/**
		* \brief Instantiates and returns a game application.
		* \return The created game application.
		*/
		static GameAppPtr instantiate() {
			return std::move(get());
		}

        /**
         * \brief Callback function called when a TerminateProcessException occurs and is not caught.
         * \return A return value coded on an int.
         */
		virtual int onTerminate(TerminateProcessException&) { return 0; }

		/**
         * \brief Callback function called when a GenericException occurs and is not caught.
         * \return A return value coded on an int.
         */
		virtual int onException(GenericException&) = 0;

		/**
         * \brief Runs the application. It doesn't return a value, because the application is considered running until an uncaught exception occurs.
         */
		virtual void run() = 0;

		virtual float ticksWanted() const override {
			const static auto FPS = 60U;
			const static auto TICKS = 1000.F / FPS;
			return TICKS;
		}

		virtual ~GameApp();
	
	private:
		GameConfiguration m_gameConfig;
	};
}
