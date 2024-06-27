#pragma once

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkObject.h>
#include <vtkObjectFactory.h>
#include <map>
#include <functional>
#include "Application.h"

class Application;

// Define interaction style.
class KeyPressInteractorStyle : public vtkInteractorStyleTrackballCamera
{
private:
    std::map<std::string, std::function<void()>> keyBinds;

public:
    static KeyPressInteractorStyle* New();
    vtkTypeMacro(KeyPressInteractorStyle, vtkInteractorStyleTrackballCamera)

    virtual void OnKeyPress() override;
    void AddKeyBind(std::string key, std::function<void()>);
};
