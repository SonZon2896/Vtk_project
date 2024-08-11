#include "ChangeClippingSliderCallback.h"

void ChangeClippingSliderCallback::Execute(vtkObject* caller, unsigned long, void*)
{
	vtkSliderWidget *sliderWidget = static_cast<vtkSliderWidget*>(caller);
	double value = static_cast<vtkSliderRepresentation2D*>(sliderWidget->GetRepresentation())->GetValue();
	
	clipPlane->SetOrigin(0, value, 0);
}