#include "Interactor.h"

void KeyPressInteractorStyle::OnKeyPress()
{
    // Get the keypress.
    vtkRenderWindowInteractor* rwi = this->Interactor;
    std::string key = rwi->GetKeySym();

    // Output the key that was pressed.
    std::cout << "Pressed " << key << std::endl;

    for (auto item : keyBinds)
        if (key == item.first)
        {
            std::cout << "Entered function on key " << key << std::endl;
            item.second();
            break;
        }

    // Forward events.
    vtkInteractorStyleTrackballCamera::OnKeyPress();
}
vtkStandardNewMacro(KeyPressInteractorStyle);

void KeyPressInteractorStyle::AddKeyBind(std::string key, std::function<void()> cb)
{
    keyBinds[key] = cb;
}