#include "Application.h"
#include <chrono>
#include <thread>

#define PATHTOSETTINGS "C:\\Users\\123va\\source\\repos\\CMakeProject1\\settings.json"

bool is(const char* str1, const char* str2)
{
    for (int i = 0; str1[i] == str2[i]; ++i)
    {
        if (str1[i] == '\0')
            return true;
    }
    return false;
}

int main(int argc, char* argv[])
{
    Application app;

    if (argc < 2)
    {
        vtkNew<vtkSphereSource> sphere;
        sphere->SetPhiResolution(25);
        sphere->SetThetaResolution(25);
        sphere->Update();

        app.AddObject(sphere->GetOutput(), false, true);
    }
    else if (is(argv[1], "plane"))
    {
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

        app.AddObject(plane->GetOutput(), true, true);
    }
    else
    {
        app.AddObject(argv[1], true, true);
    }

    app.Start();

    return EXIT_SUCCESS;
}