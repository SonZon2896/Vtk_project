#pragma once

#include "../includes.h"

class ChangeClippingButtonCallback : public vtkCommand
{
public:
	vtkSP<vtkImplicitPlaneWidget2> clippingPlaneWidget;
	std::vector<vtkSP<vtkActor>> clippingActors;
	std::vector<vtkSP<vtkActor>> mainActors;

	unsigned long eventMode;

	static auto New() { return new ChangeClippingButtonCallback; }
	virtual void Execute(vtkObject* caller, unsigned long eventId, void*);
};