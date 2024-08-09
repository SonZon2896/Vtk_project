#include "ChangeClippingButtonCallback.h"

void ChangeClippingButtonCallback::Execute(vtkObject* caller, unsigned long, void*)
{
	vtkSP<vtkButtonWidget> buttonWidget =  static_cast<vtkButtonWidget*>(caller);
	vtkSP<vtkTexturedButtonRepresentation2D> repr = reinterpret_cast<vtkTexturedButtonRepresentation2D*>(buttonWidget->GetRepresentation());
	int state = repr->GetState();

	switch (state)
	{
	case 0:
		sliderWidget->EnabledOff();
		for (auto mainActor : mainActors)
			mainActor->VisibilityOn();
		for (auto clippingActor : clippingActors)
			clippingActor->VisibilityOff();
		break;
	case 1:
		sliderWidget->EnabledOn();
		for (auto mainActor : mainActors)
			mainActor->VisibilityOff();
		for (auto clippingActor : clippingActors)
			clippingActor->VisibilityOn();
		break;
	default:
		break;
	}
}