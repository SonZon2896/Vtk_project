#pragma once

#include <vtkSliderRepresentation2D.h>
#include <vtkCommand.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkSliderWidget.h>
#include <functional>
#include "Application.h"

class ChangeClippingSliderCallback : public vtkCommand
{
public:
	std::vector<vtkSP<vtkActor>> clippingActors;
	vtkSP<vtkPlane> clipPlane;

	static auto New() { return new ChangeClippingSliderCallback; }
	virtual void Execute(vtkObject* caller, unsigned long, void*);
};