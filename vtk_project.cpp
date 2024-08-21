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

//    if (argc < 2)
//    {
//        vtkSP<vtkPolyData> standartObject = CreateStandartObject();
//#ifdef DEBUG
//        std::cout << "using debug" << std::endl;
//        app.AddObject(standartObject, false, true, true);
//#elif RELEASE
//        std::cout << "using release" << std::endl;
//        app.AddObject(standartObject, false, true, false);
//#endif
//    }
//    else if (is(argv[1], "with_scalars"))
//    {
//        vtkSP<vtkPolyData> standartObject = CreateStandartObject();
//        vtkSP<vtkDoubleArray> scalars = GetScalars(standartObject->GetNumberOfPoints());
//        standartObject->GetPointData()->SetScalars(scalars);
//#ifdef DEBUG
//        app.AddObject(standartObject, true, true, true);
//#elif RELEASE
//        app.AddObject(standartObject, true, true, false);
//#endif
//    }
//    else
//    {
//#ifdef DEBUG
//        app.AddObject(argv[1], true, true, true);
//#elif RELEASE
//        app.AddObject(argv[1], true, true, false);
//#endif
//    }
    app.AddObject("Examples\\3D_fork_0.csv3d", true, true, false);

    app.Start();

    return EXIT_SUCCESS;
}