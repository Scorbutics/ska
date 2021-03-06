#pragma once
#include "Button.h"
namespace ska {
	class CheckBox : public Button {
	public:
		CheckBox(WidgetPanel& parent, Point<int> relativePos, const std::string& placeHolderStyleName, const Rectangle* clip);
		virtual ~CheckBox() = default;
		
		bool getValue() const;
		void forceValue(bool b);

		void render(Renderer& renderer) const override;
	private:
		bool m_value;
		Texture m_check;
	};
}
