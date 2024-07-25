#pragma once

#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkSphereSource.h>
#include <vtkCylinderSource.h>
#include <vtkTubeFilter.h>
#include <vtkCallbackCommand.h>

class UpdateMeshGridCallback : public vtkCallbackCommand
{
private:
    vtkSmartPointer<vtkSphereSource> sphereSource;
    vtkSmartPointer<vtkTubeFilter> cylinderSource;

    double sphereScale = .003;
    double cylinderScale = .0003;

public:
    static UpdateMeshGridCallback* New() { return new UpdateMeshGridCallback; }
    
    virtual void Execute(vtkObject* caller, unsigned long, void*);

    void SetSphereSource(vtkSphereSource* sphereSrc) { sphereSource = sphereSrc; }
    auto GetSphereSource() { return sphereSource; }
    void SetCylinderSource(vtkTubeFilter* cylinderSrc) { cylinderSource = cylinderSrc; }
    auto GetCylinderSource() { return cylinderSource; }
    void SetSphereScale(double scale) { sphereScale = scale; }
    auto GetSphereScale() { return sphereScale; }
    void SetCylinderScale(double scale) { cylinderScale = scale; }
    auto GetCylinederScale() { return cylinderScale; }

};