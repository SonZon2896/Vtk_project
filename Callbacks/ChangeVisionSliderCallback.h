#pragma once

#include "../includes.h"

class ChangeVisionSliderCallback : public vtkCommand
{
private:
    unsigned int prevMode = 0;

public:
    Application* app;
    
    static ChangeVisionSliderCallback* New() { return new ChangeVisionSliderCallback; }
    virtual void Execute(vtkObject* caller, unsigned long, void*);
};