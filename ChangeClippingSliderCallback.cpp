#include "ChangeClippingSliderCallback.h"

void ChangeClippingSliderCallback::Execute(vtkObject* caller, unsigned long, void*)
{
	vtkSP<vtkSliderWidget> sliderWidget = static_cast<vtkSliderWidget*>(caller);
	double value = static_cast<vtkSliderRepresentation2D*>(sliderWidget->GetRepresentation())->GetValue();
	
	clipPlane->SetOrigin(0, value, 0);
	
	for (auto clippingActor : clippingActors)
	{
		auto clipper = reinterpret_cast<vtkClipPolyData*>(clippingActor->GetMapper()->GetInput());
		clipper->SetClipFunction(clipPlane);
	}
}