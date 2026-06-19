#pragma once

#include "Ferret/Core/Event/Event.h"
#include "Ferret/Core/Event/KeyEvent.h"
#include "Ferret/Layer/Layer.h"

namespace Ferret
{
    class FerretLayer : public Layer
    {
    public:
        virtual void OnAttach() override;
        virtual void OnDetach() override;

        virtual void OnUpdate() override;
        virtual void OnUIRender() override;
        virtual void OnEvent(Event& event) override;

        bool OnKeyPressedEvent(KeyPressedEvent& e);

        static void LogExample();
    private:
    };
}
