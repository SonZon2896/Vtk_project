#pragma once

#include "../includes.h"

class ChangeClippingSliderCallback : public vtkCommand
{
public:
	std::vector<vtkSP<vtkActor>> clippingActors;
	vtkSP<vtkPlane> clipPlane;

	static auto New() { return new ChangeClippingSliderCallback; }
	virtual void Execute(vtkObject* caller, unsigned long, void*);
};