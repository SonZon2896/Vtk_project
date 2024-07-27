#pragma once

#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkSphereSource.h>
#include <vtkCylinderSource.h>
#include <vtkTubeFilter.h>
#include <vtkCallbackCommand.h>
#include <vtkRibbonFilter.h>
#include <vtkGlyph3D.h>

class UpdateMeshGridCallback : public vtkCallbackCommand
{
private:
    vtkSmartPointer<vtkGlyph3D> spheres;

public:
    double sphereScale = .01;

    static UpdateMeshGridCallback* New() { return new UpdateMeshGridCallback; }
    
    virtual void Execute(vtkObject* caller, unsigned long, void*);

    void SetSphereSource(vtkGlyph3D* spheresGlyph) { spheres = spheresGlyph; }
};