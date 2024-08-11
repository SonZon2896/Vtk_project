#pragma once

#include "../includes.h"

class ChangeClippingButtonCallback : public vtkCommand
{
public:
	vtkSP<vtkSliderWidget> sliderWidget;
	std::vector<vtkSP<vtkActor>> clippingActors;
	std::vector<vtkSP<vtkActor>> mainActors;

	static auto New() { return new ChangeClippingButtonCallback; }
	virtual void Execute(vtkObject* caller, unsigned long, void*);
};