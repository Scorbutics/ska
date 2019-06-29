#include "GUI/__internalConfig/LoggerConfig.h"
#include "Core/Window.h"
#include "Core/Inputs/InputContextManager.h"
#include "GUI.h"
#include "Windows/GUIScrollButtonWindowIG.h"
#include "Events/FocusEvent.h"
#include "Core/Data/Events/GUIEvent.h"
#include "Windows/BalloonDialog.h"
#include "Core/Utils/SkaConstants.h"

#define SCROLL_BUTTON_SPEED 3
static constexpr auto TAILLEECRANMINX = 32*15;
static constexpr auto TAILLEECRANMINY = 32*13;

std::string ska::GUI::MENU_DEFAULT_THEME_PATH = "resources/Menu/default_theme/";

ska::GUI::GUI(GameEventDispatcher& ged) :
	ska::Observer<GUIEvent>(std::bind(&GUI::onGUIEvent, this, std::placeholders::_1)),
	ska::Observer<GameEvent>(std::bind(&GUI::onGameEvent, this, std::placeholders::_1)),
	ska::Observer<InputMouseEvent>(std::bind(&GUI::refreshMouse, this, std::placeholders::_1)),
	ska::Observer<InputKeyEvent>(std::bind(&GUI::refreshKeyboard, this, std::placeholders::_1)),
    m_hide(false),
    m_mouseCursor(MENU_DEFAULT_THEME_PATH + "mouse_cursor"),
    m_ged(ged),
    m_hovered(nullptr),
    m_clicked(nullptr),
    m_lastFocused(nullptr),
    m_wMaster(this, this, this, Rectangle{ 0, 0, TAILLEECRANMINX, TAILLEECRANMINY }) {

    m_wFocusable = &m_wMaster.addWidget<TimeScrollableWindowIG<KeyEventListener>>(Rectangle{ 0, 0, TAILLEECRANMINX, TAILLEECRANMINY });
    DrawableFixedPriority::setPriority(std::numeric_limits<int>().max());

	m_ged.addMultipleObservers<GUIEvent, GameEvent, InputMouseEvent, InputKeyEvent>(*this, *this, *this, *this);
}

bool ska::GUI::onGameEvent(GameEvent& ge) {
	if (ge.getEventType() == GameEventType::GAME_WINDOW_READY ||
		ge.getEventType() == GameEventType::GAME_WINDOW_RESIZED) {
		m_wMaster.setWidth(ge.windowWidth);
		m_wMaster.setHeight(ge.windowHeight);
		return onScreenResized(ge.windowWidth, ge.windowHeight);
	}
	return false;
}

ska::GUI::~GUI() {
	m_ged.removeMultipleObservers<GUIEvent, GameEvent, InputMouseEvent, InputKeyEvent>(*this, *this, *this, *this);
}


bool ska::GUI::refreshKeyboard(InputKeyEvent& ike) {
	const auto& playerIcm = ike.icm;
	const auto& textTyped = playerIcm.getTextInput();
    //TODO r��crire
	const auto& isDeleting = playerIcm.getActions()[DeleteChar];
	if (isDeleting) {
		KeyEvent ke(KEY_DOWN, L"", SDL_SCANCODE_BACKSPACE);
		KeyObservable::notifyObservers(ke);
	}

	const auto& isValidating = playerIcm.getActions()[DoAction];
	if (isValidating) {
		KeyEvent ke(KEY_DOWN, L"", SDL_SCANCODE_RETURN);
		KeyObservable::notifyObservers(ke);
	}

	if (!textTyped.empty()) {
		KeyEvent ke(TEXT_TYPED, textTyped, -1);
		KeyObservable::notifyObservers(ke);
	}
	return true;
}

bool ska::GUI::refreshMouse(InputMouseEvent& ime) {
	auto& playerIcm = ime.icm;
	const auto& in = playerIcm.getActions();
	const auto& moveWindow = playerIcm.getToggles()[MoveWindow];
	const auto& mousePos = playerIcm.getRanges()[MousePos];
	const auto& lastMousePos = playerIcm.getRanges()[LastMousePos];

	if (m_lastLastMousePos != lastMousePos || lastMousePos != mousePos) {
		m_mouseCursor.move(mousePos);

		/* Toujours utiliser lastMousePos et non pas mousePos pour les hover :
		*  on consid�re mousePos comme la prochaine position de la souris en terme d'�v�nements.
		*  On a donc un retard d'une frame sur tous les �v�nements d�clench�s, mais cela permet de pouvoir r�agir
		*  aux changements brusques de position de souris */
		HoverEvent he(MOUSE_ENTER, lastMousePos, mousePos, mousePos - lastMousePos);
		HoverObservable::notifyObservers(he);

		HoverEvent hove(MOUSE_OVER, lastMousePos, mousePos, mousePos - lastMousePos);
		HoverObservable::notifyObservers(hove);
		
		if (m_hovered != nullptr) {
			for (auto it = m_hovered; it != hove.getTarget() && it != nullptr; it = it->getParent()) {
				if (hove.getTarget() == nullptr || !it->isAParent(*hove.getTarget())) {
					HoverEvent heOut(MOUSE_OUT, it->getAbsolutePosition(), mousePos, mousePos - lastMousePos);
					it->directNotify(heOut);
				}
			}
		}
		m_hovered = hove.getTarget();

	}
    m_lastLastMousePos = lastMousePos;

	const auto click = in[LClic];
	if (click) {
		Point<int> pMp (mousePos);
		if (m_clicked == nullptr) {
			ClickEvent ce(MOUSE_CLICK, pMp);
			ClickObservable::notifyObservers(ce);
			m_clicked = ce.getTarget();
		}

		auto lastFocused = m_lastFocused;
		if (m_clicked != nullptr) {
			FocusEvent fe(m_clicked, MOUSE_FOCUS);
			m_clicked->directNotify(fe);
			if(fe.getTarget() != nullptr) {
				playerIcm.disableContext(EnumContextManager::CONTEXT_MAP, true);
			}

			m_lastFocused = m_clicked;
		}

		FocusEvent fbe(m_clicked, MOUSE_BLUR);
		if (lastFocused != nullptr && lastFocused != m_clicked) {
			lastFocused->directNotify(fbe);
		}

		if (fbe.getTarget() != nullptr) {
			playerIcm.disableContext(EnumContextManager::CONTEXT_MAP, false);
		}
	}


	if (m_clicked != nullptr && !moveWindow) {
        Point<int> pMp (mousePos);
		ClickEvent ce(MOUSE_RELEASE, pMp);
		ce.setTarget(m_clicked);
		m_clicked = nullptr;
		ClickObservable::notifyObservers(ce);
	}

	return true;
}

void ska::GUI::refresh(unsigned int ellapsedTime) {
	if (m_hide) {
		return;
	}

	for(auto& wName : m_windowsToDelete) {
		const auto* w = getWindow(wName);
		const auto wasAParent = m_hovered != nullptr ? w->isAParent(*m_hovered) : false;
		const auto* windowRemoved = removeWindow(wName);
		if(m_hovered == windowRemoved || wasAParent) {
			m_hovered = nullptr;
			m_lastFocused = nullptr;
		} else if (windowRemoved == m_clicked) {
			m_clicked = nullptr;
		}
	}
    m_windowsToDelete.clear();

	/*refreshMouse();
	refreshKeyboard();*/

	//Time-based events
	TimeEvent te(ellapsedTime);
	TimeObservable::notifyObservers(te);

}


//Reset des informations de notre pok�mon (� chaque refresh)
//TODO
void ska::GUI::hide(bool x) {
    m_hide = x;
}

void ska::GUI::render(Renderer& renderer) const {
	if (m_hide) {
		return;
	}

	m_wMaster.render(renderer);

	for (auto& w : m_topWindowWidgets) {
		w->render(renderer);
	}

	m_mouseCursor.render(renderer);
}

bool ska::GUI::isVisible() const {
    return !m_hide;
}

void ska::GUI::windowSorter(Widget* tthis, ClickEvent& e) {
	if (e.getState() == MOUSE_CLICK) {
		pushWindowToFront(tthis);
	}
}

bool ska::GUI::onGUIEvent(GUIEvent& ge) {
    if(ge.type == GUIEventType::REMOVE_WINDOW) {
        m_windowsToDelete.push_back(ge.windowName);
    }

	if(ge.type == GUIEventType::ADD_BALLOON) {
 		auto& bd = addWindow<BalloonDialog>(ge.windowName, Rectangle{ 0, 64, 320, 64 }, ge.text, ge.delay, 16);
		bd.addHandler<TimeEventListener>([&](Widget* tthis, TimeEvent&) {
			auto& balloon = static_cast<BalloonDialog&>(*tthis);
			if(balloon.isExpired()) {
				balloon.show(false);
			}
		});
		ge.balloonHandle = &bd;
	}

	if(ge.type == GUIEventType::REFRESH_BALLOON) {
		auto bd = static_cast<BalloonDialog*>(getWindow(ge.windowName));
		if (bd != nullptr) {
			bd->move(Point<int>(ge.balloonPosition.x, ge.balloonPosition.y - ge.balloonHandle->getBox().h));
			if (!bd->isVisible()) {
				ge.balloonHandle = nullptr;
				GUIEvent geI(GUIEventType::REMOVE_WINDOW);
				geI.windowName = ge.windowName;
				onGUIEvent(geI);
			}
		}
	}
    return true;
}

ska::Widget* ska::GUI::frontWindow() {
    return m_wFocusable->backWidget();
}

void ska::GUI::pushWindowToFront(Widget* w) {
	auto firstWidget = frontWindow();
	if (w != firstWidget) {
		const auto firstPriority = firstWidget->getPriority();
		const auto priority = w->getPriority();
		firstWidget->setPriority(priority);
		w->setPriority(firstPriority);
		w->focus(true);
		firstWidget->focus(false);
	}
}

ska::Widget* ska::GUI::addTopWidget(std::unique_ptr<Widget>& w) {
	m_topWindowWidgets.push_back(move(w));
	return m_topWindowWidgets.back().get();
}

bool ska::GUI::onScreenResized(unsigned int, unsigned int) {
	return true;
}

unsigned int ska::GUI::getMaxHeight() const {
	return m_wMaster.getBox().h;
}

unsigned int ska::GUI::getMaxWidth() const {
	return m_wMaster.getBox().w;
}
