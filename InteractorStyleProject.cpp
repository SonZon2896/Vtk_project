#include "InteractorStyleProject.h"

InteractorStyleProject::InteractorStyleProject()
{
	textProperty = vtkTextProperty::New();
	textMapper = vtkTextMapper::New();
	textActor = vtkActor2D::New();

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

	vtkNew<vtkPointPicker> picker;
	picker->SetTolerance(.003);
	interactor->SetPicker(picker);
}

void InteractorStyleProject::OnLeftButtonUp()
{
	if (enableSelection == true)
	{
		int x, y;
		this->interactor->GetEventPosition(x, y);

		vtkPointPicker* picker = dynamic_cast<vtkPointPicker *>(interactor->GetPicker());

		picker->Pick(x, y, 0, renderer);

		if (picker->GetPointId() >= 0)
		{
			selectedPoint = picker->GetPointId();

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