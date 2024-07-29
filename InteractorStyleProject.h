#pragma once

#include <vtkPointPicker.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWIndow.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>
#include <string>

#define vtkSP vtkSmartPointer

class InteractorStyleProject : public vtkInteractorStyleTrackballCamera
{
private:
	vtkSP<vtkPointPicker> pointPicker;
	vtkSP<vtkRenderer> renderer;
	vtkIdType selectedPoint;

	vtkSP<vtkTextProperty> textProperty;
	vtkSP<vtkTextMapper> textMapper;
	vtkSP<vtkActor2D> textActor;

public:
	vtkSP<vtkPolyData> polyData;
	bool enableSelection = false;

	static vtkStandardNewMacro(InteractorStyleProject)
	InteractorStyleProject();
	void SetRenderer(vtkRenderer* ren);

	void OnLeftButtonUp() override;
	void OnLeftButtonDown() override;
	void OnRightButtonUp() override;
	void OnRightButtonDown() override;
	void OnMouseWheelForward() override;
	void OnMouseWheelBackward() override;
	void OnMiddleButtonUp() override;
	void OnMiddleButtonDown() override;
};