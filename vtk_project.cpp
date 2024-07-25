#include "Application.h"
#include <chrono>
#include <thread>

#define PATHTOSETTINGS "C:\\Users\\123va\\source\\repos\\CMakeProject1\\settings.json"

int main(int argc, char* argv[])
{
    Application app;
    //app.AddSettings(PATHTOSETTINGS);
    
    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetResolution(8);
    cylinder->SetHeight(20);
    cylinder->Update();

    vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
    randomSequence->SetSeed(2);

    vtkNew<vtkPlaneSource> plane;
    plane->SetXResolution(5);
    plane->SetYResolution(5);
    plane->Update();

    vtkNew<vtkDoubleArray> randomScalars;
    randomScalars->SetNumberOfComponents(1);
    randomScalars->SetName("Isovalues");
    for (int i = 0; i < plane->GetOutput()->GetNumberOfPoints(); i++)
    {
        randomScalars->InsertNextTuple1(randomSequence->GetRangeValue(-100., 100.));
        randomSequence->Next();
    }
    plane->GetOutput()->GetPointData()->SetScalars(randomScalars);

    vtkNew<vtkSphereSource> sphere;
    sphere->SetPhiResolution(25);
    sphere->SetThetaResolution(25);
    sphere->Update();

    //app.AddObject(cylinder->GetOutput(), false, true);
    //app.AddObject(plane->GetOutput(), true, true);
    //app.AddObject("Examples\\WP - new geom 09 01 24_+10.csv3d", false, true);
    app.AddObject(sphere->GetOutput(), false, true);

    app.Start();

    return EXIT_SUCCESS;
}