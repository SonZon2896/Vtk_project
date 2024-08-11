#pragma once

#include "../includes.h"

class ChangeClippingPlaneCallback : public vtkCommand
{
public:
	vtkSP<vtkPlane> clipPlane;

	static auto New() { return new ChangeClippingPlaneCallback; }
	virtual void Execute(vtkObject* caller, unsigned long, void*);
};