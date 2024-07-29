#pragma once

#include <vtkPointPicker.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorObserver.h>
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
	// ��� ������� ����� ������ ��� ���� ����� ����� ������ ��������
	// �� � ���� ������ �� �������
	vtkPointPicker *pointPicker;

	vtkRenderer *renderer;
	vtkRenderWindowInteractor *interactor;
	vtkIdType selectedPoint;

	vtkTextProperty *textProperty;
	vtkTextMapper *textMapper;
	vtkActor2D *textActor;

	InteractorStyleProject();

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