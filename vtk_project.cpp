#include "Application.h"

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

inline vtkSP<vtkPolyData> CreateStandartObject()
{
    vtkNew<vtkSphereSource> sphere;
    sphere->SetPhiResolution(15);
    sphere->SetThetaResolution(15);
    sphere->Update();

    return sphere->GetOutput();
}

inline vtkSP<vtkDoubleArray> GetScalars(vtkIdType num)
{
    vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
    randomSequence->SetSeed(6);

    vtkDoubleArray *randomScalars = vtkDoubleArray::New();
    randomScalars->SetNumberOfComponents(1);
    randomScalars->SetNumberOfTuples(num);

    //std::cout << randomScalars->GetNumberOfTuples() << std::endl;
    //std::cout << randomScalars->GetMaxId() << std::endl;

    randomScalars->SetName("values");

    //std::cout << "Add values: ";
    for (int i = 0; i < num; i++)
    {
        double value = randomSequence->GetRangeValue(-1000., 1000.);
        //randomScalars->InsertNextTuple1(value);
        randomScalars->SetTuple1(i, value);
        randomSequence->Next();

        //std::cout << value << ", ";
    }
    //std::cout << std::endl;
    
    //std::cout << randomScalars->GetNumberOfTuples() << std::endl;
    //std::cout << randomScalars->GetMaxId() << std::endl;

    //randomScalars->PrintValues(std::cout);

    return randomScalars;
}

int main(int argc, char* argv[])
{
    Application app;

    if (argc < 2)
    {
        vtkSP<vtkPolyData> standartObject = CreateStandartObject();
        app.AddObject(standartObject, false, true, true);
    }
    else if (is(argv[1], "with_scalars"))
    {
        vtkSP<vtkPolyData> standartObject = CreateStandartObject();
        vtkSP<vtkDoubleArray> scalars = GetScalars(standartObject->GetNumberOfPoints());
        standartObject->GetPointData()->SetScalars(scalars);
        app.AddObject(standartObject, true, true, true);
    }
    else
    {
        app.AddObject(argv[1], true, true, true);
    }

    app.Start();

    return EXIT_SUCCESS;
}