#pragma once
namespace ska {
	class CameraSystem;

	/**
	 * \brief Indicates that a class needs a camera system
	 */
	class CameraAware {
	public:
	    CameraAware() = default;
	    virtual ~CameraAware() = default;
		virtual void linkCamera(CameraSystem* cs) = 0;
	};
}
