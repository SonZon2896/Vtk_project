#pragma once

#include <vtkWorldPointPicker.h>
#include <vtkPointPicker.h>
#include <vtkPicker.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorObserver.h>
#include <vtkRenderWIndow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
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
#include <vtkMath.h>
#include <string>

#define vtkSP vtkSmartPointer

class InteractorStyleProject : public vtkInteractorStyleTrackballCamera
{
private:
	// Эта строчка нужна только для того чтобы релиз версия работала
	// Ни в коем случае не удалять
	void *nothing;

	vtkRenderer *renderer;
	vtkRenderWindowInteractor *interactor;
	vtkIdType selectedPoint;

	vtkSP<vtkTextProperty> textProperty;
	vtkSP<vtkTextMapper> textMapper;
	vtkSP<vtkActor2D> textActor;

	vtkSP<vtkPointPicker> pointPicker;
	vtkSP<vtkWorldPointPicker> worldPicker;

	InteractorStyleProject();

	void PickPoint();

public:
	vtkPolyData *polyData;
	bool enableSelection = false;

	static vtkStandardNewMacro(InteractorStyleProject)

	void SetRenderer(vtkRenderer* ren);
	void SetWindowInteractor(vtkRenderWindowInteractor* rwi);

	virtual void OnLeftButtonUp() override;
	virtual void OnLeftButtonDown() override;
	virtual void OnRightButtonUp() override;
	virtual void OnRightButtonDown() override;
	virtual void OnMouseWheelForward() override;
	virtual void OnMouseWheelBackward() override;
	virtual void OnMiddleButtonUp() override;
	virtual void OnMiddleButtonDown() override;
};