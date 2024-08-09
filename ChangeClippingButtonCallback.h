#pragma once

#include <vtkSliderRepresentation2D.h>
#include <vtkCommand.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkSliderWidget.h>
#include <functional>
#include "Application.h"

class ChangeClippingButtonCallback : public vtkCommand
{
public:
	vtkSP<vtkSliderWidget> sliderWidget;
	std::vector<vtkSP<vtkActor>> clippingActors;
	std::vector<vtkSP<vtkActor>> mainActors;

	static auto New() { return new ChangeClippingButtonCallback; }
	virtual void Execute(vtkObject* caller, unsigned long, void*);
};