#pragma once

#include <vtkSliderRepresentation2D.h>
#include <vtkCommand.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkSliderWidget.h>
#include <functional>
#include "Application.h"

class Application;

class ChangeVisionSliderCallback : public vtkCommand
{
private:
    unsigned int prevMode = 0;

public:
    Application* app;
    
    static ChangeVisionSliderCallback* New() { return new ChangeVisionSliderCallback; }
    virtual void Execute(vtkObject* caller, unsigned long, void*);
};