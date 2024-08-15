#pragma once

#include "../includes.h"

class ProportionsPieceActor : public vtkObject
{
private:
	ProportionsPieceActor();

	vtkSP<vtkPolyData> source;

	vtkSP<vtkCylinderSource> cylinderLengthSource;
	vtkSP<vtkCylinderSource> cylinderWidthSurce;
	vtkSP<vtkConeSource> coneSource;

	double conePart;
	double length;
	double width;
	double radius;

	vtkSP<vtkCaptionActor2D> proportionLabel;
	vtkSP<vtkActor> rightLine;
	vtkSP<vtkActor> leftLine;
	vtkSP<vtkActor> centerLine;
	vtkSP<vtkActor> leftCone;
	vtkSP<vtkActor> rightCone;
public:
	static auto New() { return new ProportionsPieceActor(); }

	void SetLength(double length);
	void SetWidth(double width);
	void SetRadius(double radius);
	void SetConePart(double conePart);

	void SetRenderer(vtkRenderer* renderer);
};