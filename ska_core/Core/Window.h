#pragma once

#include <SDL.h>
#include <string>

namespace ska {
	/**
	 * \brief Game window where graphics are rendered, and inputs listened.
	 * 
	 * The game window provides all accesses to basic services about window manipulation.
	 * Rendering and input listening are not covered by this class.
	 * 
	 */
	class Window {
	public:
		Window(const std::string& title, unsigned int w, unsigned int h);
		Window& operator=(const Window&) = delete;
		virtual ~Window();

		/**
         * \brief Window width accessor
         * \return the window width
         */
        unsigned int getWidth() const;
		
		/**
		* \brief Window height accessor
		* \return the window height
		*/
		unsigned int getHeight() const;

		/**
		 * \brief Resizes the window in pixels
		 * \param w Window required width
		 * \param h Window required height
		 */
		void resize(unsigned int w, unsigned int h);

		/**
		 * \brief Displays a native message box, blocking the thread
		 * \param flags Properties to describe the message box
		 * \param title Message box window title
		 * \param message Message to display in the box
		 */
		void showMessageBox(Uint32 flags, const std::string& title, const std::string& message) const ;


		/**
		 * \brief Changes the current top-left program icon
		 * \param filename Path to the image to display as a program icon
		 */
		void setWindowIcon(const std::string& filename);

		SDL_Window* getInstance() const{
			return m_screen;
		}

	private:
		unsigned int m_height;
		unsigned int m_width;

		const std::string m_wName;

		SDL_Window * m_screen;
		SDL_Surface* m_iconFile;

	};
}
