#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

class $modify(ZoomPauseLayer, PauseLayer) {

    struct Fields {
        float m_currentZoom = 1.0f;
    };

    bool init(bool p0) {
        if (!PauseLayer::init(p0)) return false;
        m_fields->m_currentZoom = 1.0f;
        return true;
    }

    void scrollWheel(float dy, float dx) {
        auto zoomSpeed = static_cast<float>(Mod::get()->getSettingValue<double>("zoom-speed"));
        auto maxZoom = static_cast<float>(Mod::get()->getSettingValue<double>("max-zoom"));
        auto minZoom = static_cast<float>(Mod::get()->getSettingValue<double>("min-zoom"));

        if (dy > 0) {
            m_fields->m_currentZoom += zoomSpeed;
        } else {
            m_fields->m_currentZoom -= zoomSpeed;
        }

        if (m_fields->m_currentZoom > maxZoom) m_fields->m_currentZoom = maxZoom;
        if (m_fields->m_currentZoom < minZoom) m_fields->m_currentZoom = minZoom;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        float cx = winSize.width / 2.0f;
        float cy = winSize.height / 2.0f;

        this->setScale(m_fields->m_currentZoom);
        this->setPosition(
            cx - (cx * m_fields->m_currentZoom),
            cy - (cy * m_fields->m_currentZoom)
        );
    }

    void onResume(CCObject* sender) {
        if (Mod::get()->getSettingValue<bool>("reset-on-close")) {
            m_fields->m_currentZoom = 1.0f;
            this->setScale(1.0f);
            this->setPosition(0.0f, 0.0f);
        }
        PauseLayer::onResume(sender);
    }

    void onQuit(CCObject* sender) {
        if (Mod::get()->getSettingValue<bool>("reset-on-close")) {
            m_fields->m_currentZoom = 1.0f;
            this->setScale(1.0f);
            this->setPosition(0.0f, 0.0f);
        }
        PauseLayer::onQuit(sender);
    }
};
