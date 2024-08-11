#include "ChangeVisionSliderCallback.h"

void ChangeVisionSliderCallback::Execute(vtkObject* caller, unsigned long, void*)
{
    vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    double value = static_cast<vtkSliderRepresentation2D*>(sliderWidget->GetRepresentation())->GetValue();
    unsigned int mode = static_cast<unsigned int>(value);
    if (mode != prevMode)
    {
        prevMode = mode;
        app->ChangeVision(mode);
    }
}