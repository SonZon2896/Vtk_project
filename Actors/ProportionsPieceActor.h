#pragma once

#include "../includes.h"

class ProportionsPieceActor : public vtkProp3D
{
private:
	ProportionsPieceActor();

	vtkSP<vtkPolyData> source;

	vtkSP<vtkCylinderSource> cylinderLengthSource;
	vtkSP<vtkCylinderSource> cylinderWidthSource;
	vtkSP<vtkConeSource> coneSource;

	double conePart;
	double length;
	double width;
	double radius;

	double* position;
	double* orientation;

	vtkSP<vtkCaptionActor2D> proportionLabel;
	vtkSP<vtkActor> rightLine;
	vtkSP<vtkActor> leftLine;
	vtkSP<vtkActor> centerLine;
	vtkSP<vtkActor> leftCone;
	vtkSP<vtkActor> rightCone;
public:
	static auto New() { return new ProportionsPieceActor(); }

	void UpdateProps();

	void SetLength(double length);
	void SetWidth(double width);
	void SetRadius(double radius);
	void SetConePart(double conePart);

	//void SetRenderer(vtkRenderer* renderer);
	void SetPosition(double* pos);
	void SetPosition(double x, double y, double z) { return SetPosition(new double[3] {x, y, z}); }
	double* GetPosition() { return position; }
	void SetOrientation(double* orient);
	void SetOrientation(double x, double y, double z) { return SetOrientation(new double[3] {x, y, z}); }

	double* GetBounds() override;
	void GetActors(vtkPropCollection* props) override;
	int RenderOpaqueGeometry(vtkViewport* viewport) override;
	int RenderTranslucentPolygonalGeometry(vtkViewport* viewport) override;
	int RenderOverlay(vtkViewport* viewport) override;
	vtkTypeBool HasTranslucentPolygonalGeometry() override;
	void ReleaseGraphicsResources(vtkWindow* window) override;
	vtkMTimeType GetMTime() override;
	vtkMTimeType GetRedrawMTime() override;
};