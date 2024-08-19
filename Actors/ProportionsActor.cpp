#include "ProportionsActor.h"

ProportionsActor::ProportionsActor()
{
	for (int i = 0; i < 3; ++i)
		proportionsPieceActors.push_back(ProportionsPieceActor::New());
}

void ProportionsActor::UpdateProps()
{
	if (inputActor == nullptr)
		return;

	double* bounds = inputActor->GetBounds();
	double* pos = inputActor->GetCenter();

	//proportionsPieceActors[0]->SetPosition(pos[0], bounds[3], pos[2]);
	//proportionsPieceActors[0]->SetLength((bounds[3] - bounds[2]) / 2);
	proportionsPieceActors[0]->SetPosition(pos[0], pos[1], pos[2]);
	proportionsPieceActors[0]->SetLength((bounds[3] - bounds[2]) / 2 + bounds[3] - pos[1]);
	proportionsPieceActors[0]->SetWidth(bounds[1] - bounds[0]);

	//proportionsPieceActors[1]->SetPosition(bounds[0], pos[1], pos[2]);
	proportionsPieceActors[1]->SetPosition(pos[0], pos[1], pos[2]);
	proportionsPieceActors[1]->SetOrientation(0., 90., 90.);
	//proportionsPieceActors[1]->SetLength((bounds[1] - bounds[0]) / 2);
	proportionsPieceActors[1]->SetLength((bounds[1] - bounds[0]) / 2 + bounds[1] - pos[0]);
	proportionsPieceActors[1]->SetWidth(bounds[5] - bounds[4]);

	//proportionsPieceActors[2]->SetPosition(pos[0], pos[1], bounds[5]);
	proportionsPieceActors[2]->SetPosition(pos[0], pos[1], pos[2]);
	proportionsPieceActors[2]->SetOrientation(90., 0., 90.);
	//proportionsPieceActors[2]->SetLength((bounds[5] - bounds[4]) / 2);
	proportionsPieceActors[2]->SetLength((bounds[5] - bounds[4]) / 2 + bounds[5] - pos[2]);
	proportionsPieceActors[2]->SetWidth(bounds[3] - bounds[2]);

	double minRange = bounds[1] - bounds[0];
	for (int i = 1; i < 3; ++i)
		minRange = std::min(minRange, bounds[i * 2 + 1] - bounds[i * 2]);

	for (auto proportionsPieceActor : proportionsPieceActors)
		proportionsPieceActor->SetRadius(minRange / 300.);
}

double* ProportionsActor::GetBounds()
{
	UpdateProps();

	double* maxBounds = new double[6] {0., 0., 0., 0., 0., 0.};
	double bounds[6];
	for (vtkProp3D* prop : proportionsPieceActors)
	{
		prop->GetBounds(bounds);
		for (int i = 0; i < 3; ++i)
		{
			maxBounds[i * 2] = std::min(maxBounds[i * 2], bounds[i * 2]);
			maxBounds[i * 2 + 1] = std::max(maxBounds[i * 2 + 1], bounds[i * 2 + 1]);
		}
	}

	for (int i = 0; i < 6; ++i)
		this->Bounds[i] = maxBounds[i];
	delete maxBounds;

	return Bounds;
}

void ProportionsActor::GetActors(vtkPropCollection* props)
{
	UpdateProps();

	for (vtkProp3D* prop : proportionsPieceActors)
		props->AddItem(prop);
}

int ProportionsActor::RenderOpaqueGeometry(vtkViewport* viewport)
{
	UpdateProps();

	int renderedSomething = 0;

	for (vtkProp3D* prop : proportionsPieceActors)
		renderedSomething |= prop->RenderOpaqueGeometry(viewport);


	return renderedSomething > 0 ? 1 : 0;
}

int ProportionsActor::RenderTranslucentPolygonalGeometry(vtkViewport* viewport)
{
	UpdateProps();

	int renderedSomething = 0;

	for (vtkProp3D* prop : proportionsPieceActors)
		renderedSomething |= prop->RenderTranslucentPolygonalGeometry(viewport);
	
	return renderedSomething > 0 ? 1 : 0;
}

int ProportionsActor::RenderOverlay(vtkViewport* viewport)
{
	UpdateProps();

	int renderedSomething = 0;

	for (vtkProp3D* prop : proportionsPieceActors)
		renderedSomething |= prop->RenderOverlay(viewport);

	return renderedSomething > 0 ? 1 : 0;
}

vtkTypeBool ProportionsActor::HasTranslucentPolygonalGeometry()
{
	UpdateProps();

	int renderedSomething = 0;

	for (vtkProp3D* prop : proportionsPieceActors)
		renderedSomething |= prop->HasTranslucentPolygonalGeometry();
	
	return renderedSomething > 0 ? 1 : 0;
}

void ProportionsActor::ReleaseGraphicsResources(vtkWindow* win)
{
	UpdateProps();

	for (vtkProp3D* prop : proportionsPieceActors)
		prop->ReleaseGraphicsResources(win);
}

vtkMTimeType ProportionsActor::GetMTime()
{
	vtkMTimeType mTime = this->Superclass::GetMTime();
	return mTime;
}

vtkMTimeType ProportionsActor::GetRedrawMTime()
{
	vtkMTimeType mTime = this->GetMTime();
	return mTime;
}