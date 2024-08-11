#include "InteractorStyleProject.h"

InteractorStyleProject::InteractorStyleProject()
{
	textProperty = vtkTextProperty::New();
	textMapper = vtkTextMapper::New();
	textActor = vtkActor2D::New();

	pointPicker = vtkPointPicker::New();
	worldPicker = vtkWorldPointPicker::New();

	pointPicker->SetTolerance(.003);

	textMapper->SetTextProperty(textProperty);
	textActor->SetMapper(textMapper);
	textActor->VisibilityOff();
}

void InteractorStyleProject::SetRenderer(vtkRenderer* ren)
{
	renderer = ren;
	renderer->AddActor(textActor);
}

void InteractorStyleProject::SetWindowInteractor(vtkRenderWindowInteractor* rwi)
{
	interactor = rwi;
	interactor->SetInteractorStyle(this);
}

void InteractorStyleProject::PickPoint()
{
	if (enableSelection == true)
	{
		int x, y;
		interactor->GetEventPosition(x, y);

		pointPicker->Pick(x, y, 0, renderer);
		worldPicker->Pick(x, y, 0, renderer);

		double* pickerPos = pointPicker->GetPickPosition();
		double* worldPickerPos = worldPicker->GetPickPosition();
		double* cameraPos = renderer->GetActiveCamera()->GetPosition();
		double pickersDist = vtkMath::Distance2BetweenPoints(pickerPos, worldPickerPos);
		double cameraDist = vtkMath::Distance2BetweenPoints(cameraPos, worldPickerPos);

		if (pointPicker->GetPointId() >= 0 && cameraDist * 0.003 > pickersDist)
		{
			selectedPoint = pointPicker->GetPointId();

			textMapper->SetInput(std::to_string(selectedPoint).c_str());
			textActor->SetPosition(x, y);
			textActor->VisibilityOn();
		}
	}
}

void InteractorStyleProject::OnLeftButtonUp()
{
	PickPoint();

	vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
}

void InteractorStyleProject::OnLeftButtonDown()
{
	textActor->VisibilityOff();

	vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void InteractorStyleProject::OnRightButtonUp()
{
	textActor->VisibilityOff();

	vtkInteractorStyleTrackballCamera::OnRightButtonUp();
}
void InteractorStyleProject::OnRightButtonDown()
{
	textActor->VisibilityOff();

	vtkInteractorStyleTrackballCamera::OnRightButtonDown();
}
void InteractorStyleProject::OnMouseWheelForward()
{
	textActor->VisibilityOff();

	vtkInteractorStyleTrackballCamera::OnMouseWheelForward();
}
void InteractorStyleProject::OnMouseWheelBackward()
{
	textActor->VisibilityOff();

	vtkInteractorStyleTrackballCamera::OnMouseWheelBackward();
}
void InteractorStyleProject::OnMiddleButtonUp()
{
	textActor->VisibilityOff();

	vtkInteractorStyleTrackballCamera::OnMiddleButtonUp();
}
void InteractorStyleProject::OnMiddleButtonDown()
{
	textActor->VisibilityOff();

	vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();
}