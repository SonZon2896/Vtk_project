#pragma once

#include "../includes.h"

class ProportionsActor : public vtkProp3D
{
private:
	ProportionsActor();

	vtkSP<vtkActor> inputActor;
	std::vector<vtkSP<ProportionsPieceActor>> proportionsPieceActors;

	vtkSP<vtkCamera> camera;

	void UpdateProps();

public:
	static ProportionsActor* New() { return new ProportionsActor(); }
	
	void SetActor(vtkActor* actor) { inputActor = actor; UpdateProps(); }
	void SetCamera(vtkCamera* camera) { this->camera = camera; UpdateProps(); }

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