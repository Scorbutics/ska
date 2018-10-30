#pragma once
#include <string>
#include <cstdint>

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
		Window() = default;
		Window& operator=(const Window&) = delete;
		virtual ~Window() = default;

		virtual void show() =0;

		/**
         * \brief Window width accessor
         * \return the window width
         */
        virtual unsigned int getWidth() const = 0;

		/**
		* \brief Window height accessor
		* \return the window height
		*/
		virtual unsigned int getHeight() const = 0;

		/**
		 * \brief Resizes the window in pixels
		 * \param w Window required width
		 * \param h Window required height
		 */
		virtual void resize(unsigned int w, unsigned int h) = 0;

		/**
		 * \brief Displays a native message box, blocking the thread
		 * \param flags Properties to describe the message box
		 * \param title Message box window title
		 * \param message Message to display in the box
		 */
		virtual void showMessageBox(uint32_t flags, const std::string& title, const std::string& message) const = 0;

		/**
		 * \brief Changes the current top-left program icon
		 * \param filename Path to the image to display as a program icon
		 */
		virtual void setWindowIcon(const std::string& filename) = 0;
		
		virtual void load() {}

	};
}
