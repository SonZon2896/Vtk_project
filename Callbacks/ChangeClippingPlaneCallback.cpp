#include "ChangeClippingPlaneCallback.h"

void ChangeClippingPlaneCallback::Execute(vtkObject* caller, unsigned long, void*)
{
	auto planeWidget = static_cast<vtkImplicitPlaneWidget2*>(caller);
	auto planeRepr = static_cast<vtkImplicitPlaneRepresentation*>(planeWidget->GetRepresentation());
	
	planeRepr->GetPlane(clipPlane);
}