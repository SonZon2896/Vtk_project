#include "InteractorStyleProject.h"

InteractorStyleProject::InteractorStyleProject()
{
	pointPicker = vtkPointPicker::New();
	textProperty = vtkTextProperty::New();
	textMapper = vtkTextMapper::New();
	textActor = vtkActor2D::New();

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

void InteractorStyleProject::OnLeftButtonUp()
{
	if (enableSelection == true)
	{
		int x, y;
		this->interactor->GetEventPosition(x, y);

		pointPicker->Pick(x, y, 0, renderer);

		if (pointPicker->GetPointId() >= 0)
		{
			selectedPoint = pointPicker->GetPointId();

			textMapper->SetInput(std::to_string(selectedPoint).c_str());
			textActor->SetPosition(x, y);
			textActor->VisibilityOn();
		}
	}

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