#include "ProportionsPieceActor.h"

ProportionsPieceActor::ProportionsPieceActor()
{
	cylinderLengthSource = vtkCylinderSource::New();
	cylinderWidthSurce = vtkCylinderSource::New();
	coneSource = vtkConeSource::New();
	proportionLabel = vtkCaptionActor2D::New();
	rightLine = vtkActor::New();
	leftLine = vtkActor::New();
	centerLine = vtkActor::New();
	leftCone = vtkActor::New();
	rightCone = vtkActor::New();

	cylinderLengthSource->SetResolution(8);
	cylinderLengthSource->Update();

	cylinderWidthSurce->SetResolution(8);
	cylinderWidthSurce->Update();

	coneSource->SetResolution(8);
	coneSource->Update();

	vtkNew<vtkPolyDataMapper> leftLineMapper;
	leftLineMapper->SetInputConnection(cylinderLengthSource->GetOutputPort());
	leftLine->SetMapper(leftLineMapper);

	vtkNew<vtkPolyDataMapper> rightLineMapper;
	rightLineMapper->SetInputConnection(cylinderLengthSource->GetOutputPort());
	rightLine->SetMapper(rightLineMapper);

	vtkNew<vtkPolyDataMapper> centerLineMapper;
	centerLineMapper->SetInputConnection(cylinderWidthSurce->GetOutputPort());
	centerLine->SetMapper(centerLineMapper);

	vtkNew<vtkPolyDataMapper> leftConeMapper;
	leftConeMapper->SetInputConnection(coneSource->GetOutputPort());
	leftCone->SetMapper(leftConeMapper);

	vtkNew<vtkPolyDataMapper> rightConeMapper;
	rightConeMapper->SetInputConnection(coneSource->GetOutputPort());
	rightCone->SetMapper(rightConeMapper);

	//proportionLabel->ThreeDimensionalLeaderOff();
	//proportionLabel->LeaderOff();
	//proportionLabel->BorderOff();
	proportionLabel->SetPosition(-160., 0);

	leftLine->SetOrientation(0., 0., 0);
	rightLine->SetOrientation(0., 0., 0);
	centerLine->SetOrientation(0., 0., 90.);
	leftCone->SetOrientation(0., 180., 0.);
	rightCone->SetOrientation(0., 0., 0.);

	SetLength(.5);
	SetWidth(1.);
	SetRadius(.01);
	SetConePart(.1);
}

void ProportionsPieceActor::SetLength(double length)
{
	this->length = length;

	cylinderLengthSource->SetHeight(length);
	cylinderLengthSource->Update();

	leftLine->SetPosition(-width / 2, length / 2, 0.);
	rightLine->SetPosition(width / 2, length / 2, 0.);
	centerLine->SetPosition(0., length, 0.);
	leftCone->SetPosition(-width / 2, length, 0.);
	rightCone->SetPosition(width / 2, length, 0.);

	proportionLabel->SetAttachmentPoint(0., length, 0);
}

void ProportionsPieceActor::SetWidth(double width)
{
	this->width = width;
	proportionLabel->SetCaption(std::to_string(width).c_str());

	cylinderWidthSurce->SetHeight(width);
	cylinderWidthSurce->Update();

	leftLine->SetPosition(-width / 2, length / 2, 0.);
	rightLine->SetPosition(width / 2, length / 2, 0.);
	centerLine->SetPosition(0., length, 0.);
	leftCone->SetPosition(-width / 2 * (1 - conePart), length, 0.);
	rightCone->SetPosition(width / 2 * (1 - conePart), length, 0.);
}

void ProportionsPieceActor::SetRadius(double radius)
{
	this->width = width;

	cylinderWidthSurce->SetRadius(radius);
	cylinderWidthSurce->Update();
	cylinderLengthSource->SetRadius(radius);
	cylinderLengthSource->Update();
	coneSource->SetRadius(radius * 2);
	coneSource->Update();
}

void ProportionsPieceActor::SetConePart(double conePart)
{
	this->conePart = conePart;

	cylinderWidthSurce->SetHeight(width * (1 - conePart * 2));
	coneSource->SetHeight(width * conePart);

	leftCone->SetPosition(-width / 2 * (1 - conePart), length, 0.);
	rightCone->SetPosition(width / 2 * (1 - conePart), length, 0.);
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

//double* ProportionsPieceActor::GetBounds()
//{
//	vtkProp3D* props[5] = { rightLine, leftLine, centerLine, leftCone, rightCone };
//
//	double *maxBounds = new double[6];
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
//	return maxBounds;
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