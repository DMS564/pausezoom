#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

class $modify(ZoomPauseLayer, PauseLayer) {

    struct Fields {
        float m_currentZoom = 1.0f;
        float m_targetZoom = 1.0f;
        bool m_isPanning = false;
        CCPoint m_lastMousePos = CCPointZero;
        CCPoint m_panOffset = CCPointZero;
    };

    bool init(bool p0) {
        if (!PauseLayer::init(p0)) return false;

        m_fields->m_currentZoom = 1.0f;
        m_fields->m_targetZoom = 1.0f;
        m_fields->m_panOffset = CCPointZero;

        this->setKeypadEnabled(true);

        // schedule update for smooth zoom
        if (Mod::get()->getSettingValue<bool>("smooth-zoom")) {
            this->schedule(schedule_selector(ZoomPauseLayer::smoothZoomUpdate));
        }

        return true;
    }

    void scrollWheel(float dy, float dx) {
        float zoomSpeed = Mod::get()->getSettingValue<double>("zoom-speed");
        float maxZoom = Mod::get()->getSettingValue<double>("max-zoom");
        float minZoom = Mod::get()->getSettingValue<double>("min-zoom");

        // dy > 0 means scroll up = zoom in, dy < 0 means scroll down = zoom out
        float zoomDelta = dy > 0 ? zoomSpeed : -zoomSpeed;
        m_fields->m_targetZoom = clampf(
            m_fields->m_targetZoom + zoomDelta,
            minZoom,
            maxZoom
        );

        if (!Mod::get()->getSettingValue<bool>("smooth-zoom")) {
            // instant zoom
            m_fields->m_currentZoom = m_fields->m_targetZoom;
            applyZoom();
        }
    }

    void smoothZoomUpdate(float dt) {
        if (std::abs(m_fields->m_currentZoom - m_fields->m_targetZoom) > 0.001f) {
            // lerp towards target
            float lerpFactor = clampf(dt * 12.0f, 0.0f, 1.0f);
            m_fields->m_currentZoom += (m_fields->m_targetZoom - m_fields->m_currentZoom) * lerpFactor;
            applyZoom();
        }
    }

    void applyZoom() {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        CCPoint center = ccp(winSize.width / 2.0f, winSize.height / 2.0f);

        this->setScale(m_fields->m_currentZoom);

        // keep the zoom centered with pan offset
        CCPoint anchoredPos = ccp(
            center.x - (center.x * m_fields->m_currentZoom) + m_fields->m_panOffset.x,
            center.y - (center.y * m_fields->m_currentZoom) + m_fields->m_panOffset.y
        );
        this->setPosition(anchoredPos);
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        // middle mouse / right click for panning when zoomed in
        if (m_fields->m_currentZoom > 1.05f) {
            m_fields->m_isPanning = true;
            m_fields->m_lastMousePos = touch->getLocation();
        }
        return PauseLayer::ccTouchBegan(touch, event);
    }

    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        if (m_fields->m_isPanning && m_fields->m_currentZoom > 1.05f) {
            CCPoint currentPos = touch->getLocation();
            CCPoint delta = ccpSub(currentPos, m_fields->m_lastMousePos);
            m_fields->m_panOffset = ccpAdd(m_fields->m_panOffset, delta);
            m_fields->m_lastMousePos = currentPos;
            applyZoom();
        }
        PauseLayer::ccTouchMoved(touch, event);
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        m_fields->m_isPanning = false;
        PauseLayer::ccTouchEnded(touch, event);
    }

    void onQuit(CCObject* sender) {
        resetZoom();
        PauseLayer::onQuit(sender);
    }

    void onResume(CCObject* sender) {
        resetZoom();
        PauseLayer::onResume(sender);
    }

    void onPracticeMode(CCObject* sender) {
        resetZoom();
        PauseLayer::onPracticeMode(sender);
    }

    void keyDown(enumKeyCodes key) {
        // reset zoom with R key
        if (key == enumKeyCodes::KEY_R) {
            m_fields->m_targetZoom = 1.0f;
            m_fields->m_currentZoom = 1.0f;
            m_fields->m_panOffset = CCPointZero;
            applyZoom();
            return;
        }
        PauseLayer::keyDown(key);
    }

    void resetZoom() {
        if (Mod::get()->getSettingValue<bool>("reset-on-close")) {
            m_fields->m_targetZoom = 1.0f;
            m_fields->m_currentZoom = 1.0f;
            m_fields->m_panOffset = CCPointZero;
            applyZoom();
        }
    }
};
