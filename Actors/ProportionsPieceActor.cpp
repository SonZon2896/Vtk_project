#include "ProportionsPieceActor.h"

ProportionsPieceActor::ProportionsPieceActor()
{
	cylinderLengthSource = vtkCylinderSource::New();
	cylinderWidthSource = vtkCylinderSource::New();
	coneSource = vtkConeSource::New();
	proportionLabel = vtkCaptionActor2D::New();
	rightLine = vtkActor::New();
	leftLine = vtkActor::New();
	centerLine = vtkActor::New();
	leftCone = vtkActor::New();
	rightCone = vtkActor::New();

	cylinderLengthSource->SetResolution(8);
	cylinderLengthSource->Update();

	cylinderWidthSource->SetResolution(8);
	cylinderWidthSource->Update();

	coneSource->SetResolution(8);
	coneSource->Update();

	vtkNew<vtkPolyDataMapper> leftLineMapper;
	leftLineMapper->SetInputConnection(cylinderLengthSource->GetOutputPort());
	leftLine->SetMapper(leftLineMapper);
	leftLine->GetProperty()->SetColor(1., 0., 0.);

	vtkNew<vtkPolyDataMapper> rightLineMapper;
	rightLineMapper->SetInputConnection(cylinderLengthSource->GetOutputPort());
	rightLine->SetMapper(rightLineMapper);
	rightLine->GetProperty()->SetColor(0., 0., 1.);

	vtkNew<vtkPolyDataMapper> centerLineMapper;
	centerLineMapper->SetInputConnection(cylinderWidthSource->GetOutputPort());
	centerLine->SetMapper(centerLineMapper);
	centerLine->GetProperty()->SetColor(0., 1., 0.);

	vtkNew<vtkPolyDataMapper> leftConeMapper;
	leftConeMapper->SetInputConnection(coneSource->GetOutputPort());
	leftCone->SetMapper(leftConeMapper);
	leftCone->GetProperty()->SetColor(1., 0., 0.);

	vtkNew<vtkPolyDataMapper> rightConeMapper;
	rightConeMapper->SetInputConnection(coneSource->GetOutputPort());
	rightCone->SetMapper(rightConeMapper);
	rightCone->GetProperty()->SetColor(0., 0., 1.);

	//proportionLabel->ThreeDimensionalLeaderOff();
	//proportionLabel->LeaderOff();
	//proportionLabel->BorderOff();

	position = new double[3] {0., 0., 0.};
	orientation = new double[3] { 0., 0., 0.};

	SetLength(.5);
	SetWidth(1.);
	SetRadius(.01);
	SetConePart(.1);
}

double* rotate(double posX, double posY, double posZ, double* rot)
{

	double alpha = rot[0] / 180. * M_PI;
	double beta = rot[1] / 180. * M_PI;
	double gamma = rot[2] / 180. * M_PI;

	double* newPos = new double[3];
	newPos[0] = posX * cos(beta) * cos(gamma) +
		posY * (sin(alpha) * sin(beta) * cos(gamma) + sin(gamma) * cos(alpha)) +
		posZ * (sin(alpha) * sin(gamma) - sin(beta) * cos(alpha) * cos(gamma));
	newPos[1] = posX * -sin(gamma) * cos(beta) +
		posY * (-sin(alpha) * sin(beta) * sin(gamma) + cos(alpha) * cos(gamma)) +
		posZ * (sin(alpha) * cos(gamma) + sin(beta) * sin(gamma) * cos(alpha));
	newPos[2] = posX * sin(beta) +
		posY * -sin(alpha) * cos(beta) +
		posZ * cos(alpha) * cos(beta);

	return newPos;
}

double* add(double* vec1, double* vec2)
{
	for (int i = 0; i < 3; ++i)
		vec1[i] += vec2[i];
	return vec1;
}

void ProportionsPieceActor::UpdateProps()
{
	proportionLabel->SetCaption(std::to_string(width).c_str());

	cylinderWidthSource->SetHeight(width * (1 - conePart * 2));
	cylinderWidthSource->SetRadius(radius);
	cylinderWidthSource->Update();

	cylinderLengthSource->SetHeight(length);
	cylinderLengthSource->SetRadius(radius);
	cylinderLengthSource->Update();

	coneSource->SetRadius(radius * 2);
	coneSource->SetHeight(width * conePart);
	coneSource->Update();

	leftLine->SetPosition(add(rotate(-width / 2, length / 2, 0., orientation), position));
	rightLine->SetPosition(add(rotate(width / 2, length / 2, 0., orientation), position));
	centerLine->SetPosition(add(rotate(0., length, 0., orientation), position));
	leftCone->SetPosition(add(rotate(-width / 2 * (1 - conePart), length, 0., orientation), position));
	rightCone->SetPosition(add(rotate(width / 2 * (1 - conePart), length, 0., orientation), position));

	leftLine->RotateZ(-orientation[2]);
	leftLine->RotateY(orientation[1]);
	leftLine->RotateX(orientation[0]);

	rightLine->RotateZ(-orientation[2]);
	rightLine->RotateY(orientation[1]);
	rightLine->RotateX(orientation[0]);

	centerLine->RotateY(orientation[1]);
	centerLine->RotateX(orientation[0]);
	centerLine->RotateZ(-orientation[2] - 90.);

	leftCone->RotateY(orientation[1] + 180.);
	leftCone->RotateX(orientation[0]);
	leftCone->RotateZ(orientation[2]);

	rightCone->RotateY(orientation[1]);
	rightCone->RotateX(orientation[0]);
	rightCone->RotateZ(-orientation[2]);

	proportionLabel->SetAttachmentPoint(add(rotate(0., length, 0., orientation), position));
	proportionLabel->SetPosition(-160., 0);
}

void ProportionsPieceActor::SetLength(double length)
{
	this->length = length;
	UpdateProps();
}

void ProportionsPieceActor::SetWidth(double width)
{
	this->width = width;
	UpdateProps();
}

void ProportionsPieceActor::SetRadius(double radius)
{
	this->radius = radius;
	UpdateProps();
}

void ProportionsPieceActor::SetConePart(double conePart)
{
	this->conePart = conePart;
	UpdateProps();
}

void ProportionsPieceActor::SetRenderer(vtkRenderer* renderer)
{
	renderer->AddActor(proportionLabel);
	renderer->AddActor(rightLine);
	renderer->AddActor(leftLine);
	renderer->AddActor(centerLine);
	renderer->AddActor(leftCone);
	renderer->AddActor(rightCone);
}

void ProportionsPieceActor::SetPosition(double* position)
{
	if (this->position != nullptr)
		delete this->position;
	this->position = position;
	UpdateProps();
}

void ProportionsPieceActor::SetOrientation(double* orientation)
{
	if (this->orientation != nullptr)
		delete this->orientation;
	this->orientation = orientation;
	UpdateProps();
}

//double* ProportionsPieceActor::GetBounds()
//{
//	vtkProp3D* props[5] = { rightLine, leftLine, centerLine, leftCone, rightCone };
//
//	double* maxBounds = new double[6] {0., 0., 0., 0., 0., 0.};
//	double bounds[6];
//	for (auto prop : props)
//	{
//		prop->GetBounds(bounds);
//		for (int i = 0; i < 3; ++i)
//		{
//			maxBounds[i * 2] = std::min(maxBounds[i * 2], bounds[i * 2]);
//			maxBounds[i * 2 + 1] = std::max(maxBounds[i * 2 + 1], bounds[i * 2 + 1]);
//		}
//	}
//
//	for (int i = 0; i < 6; ++i)
//		this->Bounds[i] = maxBounds[i];
//	delete maxBounds;
//
//	return Bounds;
//}
//
//void ProportionsPieceActor::GetActors(vtkPropCollection* props)
//{
//	props->AddItem(proportionLabel);
//	props->AddItem(rightLine);
//	props->AddItem(leftLine);
//	props->AddItem(centerLine);
//	props->AddItem(leftCone);
//	props->AddItem(rightCone);
//}
//
//int ProportionsPieceActor::RenderOpaqueGeometry(vtkViewport* viewport)
//{
//	//UpdateProps();
//
//	int renderedSomething = 0;
//
//	renderedSomething |= rightLine->RenderOpaqueGeometry(viewport);
//	renderedSomething |= leftLine->RenderOpaqueGeometry(viewport);
//	renderedSomething |= centerLine->RenderOpaqueGeometry(viewport);
//	renderedSomething |= leftCone->RenderOpaqueGeometry(viewport);
//	renderedSomething |= rightCone->RenderOpaqueGeometry(viewport);
//	renderedSomething |= proportionLabel->RenderOpaqueGeometry(viewport);
//
//	std::cout << "RenderOpaqueGeometry return " << (renderedSomething) << std::endl;
//	return renderedSomething > 0 ? 1 : 0;
//}
//
//int ProportionsPieceActor::RenderTranslucentPolygonalGeometry(vtkViewport* viewport)
//{
//	//UpdateProps();
//
//	int renderedSomething = 0;
//
//	renderedSomething |= proportionLabel->RenderTranslucentPolygonalGeometry(viewport);
//	renderedSomething |= rightLine->RenderTranslucentPolygonalGeometry(viewport);
//	renderedSomething |= leftLine->RenderTranslucentPolygonalGeometry(viewport);
//	renderedSomething |= centerLine->RenderTranslucentPolygonalGeometry(viewport);
//	renderedSomething |= leftCone->RenderTranslucentPolygonalGeometry(viewport);
//	renderedSomething |= rightCone->RenderTranslucentPolygonalGeometry(viewport);
//
//	std::cout << "RenderTranslucentPolygonalGeometry return " << (renderedSomething) << std::endl;
//	return renderedSomething > 0 ? 1 : 0;
//}
//
//int ProportionsPieceActor::RenderOverlay(vtkViewport* viewport)
//{
//	//UpdateProps();
//
//	int renderedSomething = 0;
//
//	renderedSomething += proportionLabel->RenderOverlay(viewport);
//
//	std::cout << "RenderOverlay return " << (renderedSomething) << std::endl;
//	return renderedSomething > 0 ? 1 : 0;
//}
//
//vtkTypeBool ProportionsPieceActor::HasTranslucentPolygonalGeometry()
//{
//	//UpdateProps();
//
//	int renderedSomething = 0;
//
//	renderedSomething += proportionLabel->HasTranslucentPolygonalGeometry();
//	renderedSomething |= rightLine->HasTranslucentPolygonalGeometry();
//	renderedSomething |= leftLine->HasTranslucentPolygonalGeometry();
//	renderedSomething |= centerLine->HasTranslucentPolygonalGeometry();
//	renderedSomething |= leftCone->HasTranslucentPolygonalGeometry();
//	renderedSomething |= rightCone->HasTranslucentPolygonalGeometry();
//
//	std::cout << "HasTranslucentPolygonalGeometry return " << (renderedSomething > 0 ? 1 : 0) << std::endl;
//	return renderedSomething > 0 ? 1 : 0;
//}
//
//void ProportionsPieceActor::ReleaseGraphicsResources(vtkWindow* win)
//{
//	proportionLabel->ReleaseGraphicsResources(win);
//	rightLine->ReleaseGraphicsResources(win);
//	leftLine->ReleaseGraphicsResources(win);
//	centerLine->ReleaseGraphicsResources(win);
//	leftCone->ReleaseGraphicsResources(win);
//	rightCone->ReleaseGraphicsResources(win);
//}
//
//vtkMTimeType ProportionsPieceActor::GetMTime()
//{
//	vtkMTimeType mTime = this->Superclass::GetMTime();
//	return mTime;
//}
//
////------------------------------------------------------------------------------
//vtkMTimeType ProportionsPieceActor::GetRedrawMTime()
//{
//	vtkMTimeType mTime = this->GetMTime();
//	return mTime;
//}