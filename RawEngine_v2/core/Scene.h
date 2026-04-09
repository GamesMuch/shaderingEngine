#pragma once
#include <list>

namespace core {
    class Scene {
        std::list<gameObject> ObjectList;
        public:
        template<typename... objects>
        Scene(objects&&... object) {
            (ObjectList.emplace_back(std::forward<objects>(object)), ...);
        }
        void Render() {
            for (gameObject Object : ObjectList) {
                Object.render();
            }
        }
        std::list<gameObject> getObjects() {
            return ObjectList;
        }
    };
}

