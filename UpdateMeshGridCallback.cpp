#include "UpdateMeshGridCallback.h"

void UpdateMeshGridCallback::Execute(vtkObject* caller, unsigned long, void*)
{
    vtkSmartPointer<vtkRenderer> renderer = static_cast<vtkRenderer*>(caller);
    vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
    double *cameraPos = camera->GetPosition();
    double rangeToCamera = sqrt(pow(cameraPos[0], 2) + pow(cameraPos[1], 2) + pow(cameraPos[2], 2));

    sphereSource->SetRadius(rangeToCamera * sphereScale);
    cylinderSource->SetRadius(rangeToCamera * cylinderScale);
}