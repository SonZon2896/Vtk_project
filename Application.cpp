#include "Application.h"

Application::Application()
{
    auto style = GetStyle();
    renderWindowInteractor->SetInteractorStyle(style);

    std::array<unsigned char, 4> bkg{ 82, 87, 110, 255 };
    colors->SetColor("ParaViewBkg", bkg.data());

    renderWindow->SetSize(1280, 800);
    renderWindow->SetWindowName("Vtk Project");
    renderWindowInteractor->SetRenderWindow(renderWindow);
}

void Application::AddSettings(std::string filename)
{
    pathToSettings = filename;
    UpdateJson();
}

void Application::AddObject(std::string fileName, bool enableIsolines, bool enableGrid)
{
    if (fileName.empty())
        return;

    std::string ext;
    auto found = fileName.find_last_of(".");
    if (found == std::string::npos)
        return;

    ext = fileName.substr(found, fileName.size());

    if (ext == ".3ds")
    {
        vtkNew<vtk3DSImporter> importer;
        importer->SetFileName(fileName.c_str());
        importer->ComputeNormalsOn();
        importer->SetRenderWindow(renderWindow);
        importer->Update();
    }
    else if (ext == ".csv3d")
    {
        vtkNew<CSV3DImporter> importer;
        importer->SetFileName(fileName);
        importer->Update();

        vtkNew<vtkPolyData> polyData;
        polyData->SetPoints(importer->GetPoints());
        polyData->SetVerts(importer->GetVerts());
        polyData->SetLines(importer->GetLines());
        polyData->SetPolys(importer->GetPolys());

        AddObject(polyData, enableIsolines, enableGrid);
    }
}

void Application::AddObject(vtkSP<vtkPolyData> source, bool enableIsolines, bool enableGrid)
{
    auto bounds = source->GetBounds();

    vtkNew<vtkElevationFilter> elevationFilter;
    elevationFilter->SetLowPoint(0, bounds[2], 0);
    elevationFilter->SetHighPoint(0, bounds[3], 0);
    elevationFilter->SetInputData(source);

    auto ctf = GetCTF(source->GetScalarRange());

    vtkNew<vtkPolyDataMapper> mapper;
    if (enableIsolines)
        mapper->SetInputData(source);
    else
        mapper->SetInputConnection(elevationFilter->GetOutputPort());
    mapper->SetLookupTable(ctf);
    mapper->SetColorModeToMapScalars();
    mapper->InterpolateScalarsBeforeMappingOn();
    mapper->SetScalarModeToUsePointData();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->SetObjectName("Main");

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("ParaViewBkg").GetData());

    if (enableIsolines)
        CreateIsolines(source, renderer);
    if (enableGrid)
        CreateGrid(source, renderer);

    renderWindow->AddRenderer(renderer);
}

void Application::Start()
{
    renderWindow->Render();
    renderWindowInteractor->Start();
}

void Application::OffScreenRendering()
{
    renderWindow->OffScreenRenderingOn();
    renderWindow->Render();
}

void Application::SaveScreen(std::string fileName)
{
    bool rgba = true;
    if (!fileName.empty())
    {
        std::string fn = fileName;
        std::string ext;
        auto found = fn.find_last_of(".");
        if (found == std::string::npos)
        {
            ext = ".png";
            fn += ext;
        }
        else
        {
            ext = fileName.substr(found, fileName.size());
        }
        auto writer = vtkSmartPointer<vtkImageWriter>::New();
        if (ext == ".bmp")
        {
            writer = vtkSmartPointer<vtkBMPWriter>::New();
        }
        else if (ext == ".jpg")
        {
            writer = vtkSmartPointer<vtkJPEGWriter>::New();
        }
        else if (ext == ".pnm")
        {
            writer = vtkSmartPointer<vtkPNMWriter>::New();
        }
        else if (ext == ".ps")
        {
            if (rgba)
            {
                rgba = false;
            }
            writer = vtkSmartPointer<vtkPostScriptWriter>::New();
        }
        else if (ext == ".tiff")
        {
            writer = vtkSmartPointer<vtkTIFFWriter>::New();
        }
        else
        {
            writer = vtkSmartPointer<vtkPNGWriter>::New();
        }
        vtkNew<vtkWindowToImageFilter> window_to_image_filter;
        window_to_image_filter->SetInput(renderWindow);
        window_to_image_filter->SetScale(1); // image quality
        if (rgba)
        {
            window_to_image_filter->SetInputBufferTypeToRGBA();
        }
        else
        {
            window_to_image_filter->SetInputBufferTypeToRGB();
        }
        // Read from the front buffer.
        window_to_image_filter->ReadFrontBufferOff();
        window_to_image_filter->Update();

        writer->SetFileName(fn.c_str());
        writer->SetInputConnection(window_to_image_filter->GetOutputPort());
        writer->Write();
    }
    else
    {
        std::cerr << "No filename provided." << std::endl;
    }
}

void Application::UpdateJson()
{
    std::ifstream f(pathToSettings);
    settings = json::parse(f);
}

vtkNew<KeyPressInteractorStyle> Application::GetStyle()
{
    vtkNew<KeyPressInteractorStyle> style;
    style->AddKeyBind("z", std::bind(&Application::UpdateSettings, this));
    style->AddKeyBind("x", std::bind(&Application::ChangeProjectionToParallel, this));
    style->AddKeyBind("c", std::bind(&Application::ChangeProjectionToPerspective, this));
    style->AddKeyBind("v", std::bind(&Application::ChangeVisionToGradient, this));
    style->AddKeyBind("b", std::bind(&Application::ChangeVisionToDiscrete, this));
    style->AddKeyBind("n", std::bind(&Application::ChangeVisionToIsolines, this));
    style->AddKeyBind("m", std::bind(&Application::ChangeVisionToGrid, this));
    style->AddKeyBind("h", std::bind(&Application::ChangeIsolines, this));
    style->AddKeyBind("g", std::bind(&Application::ChangeGrid, this));
    return style;
}

vtkNew<vtkDCTF> Application::GetCTF(double minValue, double maxValue)
{
    vtkNew<vtkDiscretizableColorTransferFunction> ctf;
    double oneRange = (maxValue - minValue) / 8;

    ctf->SetColorSpaceToLab();
    ctf->SetScaleToLinear();

    ctf->SetNanColor(0, 0, 0);
    ctf->SetAboveRangeColor(0, 0, 0);
    ctf->UseAboveRangeColorOn();
    ctf->SetBelowRangeColor(0, 0, 0);
    ctf->UseBelowRangeColorOn();

    ctf->AddRGBPoint(minValue + oneRange * 0, 0.05639999999999999, 0.05639999999999999, 0.47);
    ctf->AddRGBPoint(minValue + oneRange * 1, 0.24300000000000013, 0.4603500000000004, 0.81);
    ctf->AddRGBPoint(minValue + oneRange * 2, 0.3568143826543521, 0.7450246485363142, 0.954367702893722);
    ctf->AddRGBPoint(minValue + oneRange * 3, 0.6882, 0.93, 0.9179099999999999);
    ctf->AddRGBPoint(minValue + oneRange * 4, 0.8994959551205902, 0.944646394975174, 0.7686567142818399);
    ctf->AddRGBPoint(minValue + oneRange * 5, 0.957107977357604, 0.8338185108985666, 0.5089156299842102);
    ctf->AddRGBPoint(minValue + oneRange * 6, 0.9275207599610714, 0.6214389091739178, 0.31535705838676426);
    ctf->AddRGBPoint(minValue + oneRange * 7, 0.8, 0.3520000000000001, 0.15999999999999998);
    ctf->AddRGBPoint(minValue + oneRange * 8, 0.59, 0.07670000000000013, 0.11947499999999994);

    ctf->SetNumberOfValues(9);
    ctf->DiscretizeOff();

    ctfs.push_back(ctf);
    return ctf;
}

void Application::UpdateSettings()
{
    if (pathToSettings.empty())
        return;

    UpdateJson();

    

    renderWindow->Render();
}

void Application::ChangeProjection(unsigned int mode)
{
    if (mode > 1)
        return;

    vtkRendererCollection *renderers = renderWindow->GetRenderers();
    vtkCollectionIterator *renderersIter = renderers->NewIterator();
    for (renderersIter->GoToFirstItem(); !renderersIter->IsDoneWithTraversal(); renderersIter->GoToNextItem())
    {
        auto renderer = static_cast<vtkRenderer*>(renderersIter->GetCurrentObject());
        auto camera = renderer->GetActiveCamera();
        if (mode == 0)
            camera->SetParallelProjection(true);
        if (mode == 1)
            camera->SetParallelProjection(false);
    }
    
    renderWindow->Render();
}

void Application::ChangeVision(unsigned int mode)
{
    switch (mode)
    {
    case 0:
        for (auto ctf : ctfs)
            ctf->DiscretizeOff();
        break;
    case 1:
        for (auto ctf : ctfs)
            ctf->DiscretizeOn();
        break;
    case 2:
        ShowIsolinesOn();
        break;
    case 3:
        ShowGridOn();
        break;
    default:
        return;
    }
    renderWindow->Render();
}

void Application::CreateIsolines(vtkSP<vtkPolyData> source, vtkSP<vtkRenderer> renderer)
{
    double range[2];
    source->GetScalarRange(range);

    vtkNew<vtkContourFilter> contourFilter;
    contourFilter->SetInputData(source);
    contourFilter->GenerateValues(10, range[0], range[1]);

    vtkNew<vtkStripper> stripper;
    stripper->SetInputConnection(contourFilter->GetOutputPort());
    stripper->Update();

    vtkNew<vtkLookupTable> surfaceLUT;
    surfaceLUT->SetRange(range);
    surfaceLUT->Build();

    vtkNew<vtkPolyDataMapper> isoMapper;
    isoMapper->SetInputConnection(stripper->GetOutputPort());
    isoMapper->SetLookupTable(surfaceLUT);
    isoMapper->ScalarVisibilityOn();
    isoMapper->SetScalarRange(range);

    vtkNew<vtkActor> isolinesActor;
    isolinesActor->SetObjectName("Isolines");
    isolinesActor->SetMapper(isoMapper);
    isolinesActor->GetProperty()->SetLineWidth(2);

    renderer->AddActor(isolinesActor);
}

void Application::CreateGrid(vtkSP<vtkPolyData> source, vtkSP<vtkRenderer> renderer)
{
    vtkNew<vtkExtractEdges> extract;
    extract->SetInputData(source);
    vtkNew<vtkTubeFilter> tubes;
    tubes->SetInputConnection(extract->GetOutputPort());
    tubes->SetRadius(0.01);
    tubes->SetNumberOfSides(3);
    vtkNew<vtkPolyDataMapper> mapEdges;
    mapEdges->SetInputConnection(tubes->GetOutputPort());
    vtkNew<vtkActor> edgesActor;
    edgesActor->SetObjectName("Edges");
    edgesActor->SetMapper(mapEdges);
    edgesActor->GetProperty()->SetColor(colors->GetColor3d("peacock").GetData());
    edgesActor->GetProperty()->SetSpecularColor(1, 1, 1);
    edgesActor->GetProperty()->SetSpecular(0.3);
    edgesActor->GetProperty()->SetSpecularPower(20);
    edgesActor->GetProperty()->SetAmbient(0.2);
    edgesActor->GetProperty()->SetDiffuse(0.8);

    vtkNew<vtkSphereSource> ball;
    ball->SetRadius(0.025);
    ball->SetThetaResolution(2);
    ball->SetPhiResolution(2);
    vtkNew<vtkGlyph3D> balls;
    balls->SetInputData(source);
    balls->SetSourceConnection(ball->GetOutputPort());
    vtkNew<vtkPolyDataMapper> mapBalls;
    mapBalls->SetInputConnection(balls->GetOutputPort());
    vtkNew<vtkActor> VertexesActor;
    VertexesActor->SetObjectName("Vertexes");
    VertexesActor->SetMapper(mapBalls);
    VertexesActor->GetProperty()->SetColor(colors->GetColor3d("hot_pink").GetData());

    vtkNew<vtkLabeledDataMapper> labelMapper;
    labelMapper->SetInputData(source);

    vtkNew<vtkActor2D> labelsActor;
    labelsActor->SetObjectName("Labels");
    labelsActor->SetMapper(labelMapper);

    renderer->AddActor(edgesActor);
    renderer->AddActor(labelsActor);
    renderer->AddActor(VertexesActor);

    edgesActor->VisibilityOff();
    VertexesActor->VisibilityOff();
    labelsActor->VisibilityOff();

    vtkNew<UpdateMeshGridCallback> callback;
    callback->SetSphereSource(ball);
    callback->SetCylinderSource(tubes);

    renderer->AddObserver(vtkCommand::StartEvent, callback);
}

void Application::ShowIsolines(bool flag)
{
    auto renderersIter = renderWindow->GetRenderers()->NewIterator();
    for (renderersIter->GoToFirstItem(); !renderersIter->IsDoneWithTraversal(); renderersIter->GoToNextItem())
    {
        auto renderer = static_cast<vtkRenderer *>(renderersIter->GetCurrentObject());
        auto actorsIter = renderer->GetActors()->NewIterator();
        for (actorsIter->GoToFirstItem(); !actorsIter->IsDoneWithTraversal(); actorsIter->GoToNextItem())
        {
            auto actor = static_cast<vtkActor*>(actorsIter->GetCurrentObject());
            if (actor->GetObjectName() == "Isolines")
            {
                if (flag)
                    actor->VisibilityOn();
                else
                    actor->VisibilityOff();
            }
        }
    }
    renderWindow->Render();
}

void Application::ShowGrid(bool flag)
{
    auto renderersIter = renderWindow->GetRenderers()->NewIterator();
    for (renderersIter->GoToFirstItem(); !renderersIter->IsDoneWithTraversal(); renderersIter->GoToNextItem())
    {
        auto renderer = static_cast<vtkRenderer *>(renderersIter->GetCurrentObject());
        auto actorsIter = renderer->GetActors()->NewIterator();
        for (actorsIter->GoToFirstItem(); !actorsIter->IsDoneWithTraversal(); actorsIter->GoToNextItem())
        {
            auto actor = static_cast<vtkActor *>(actorsIter->GetCurrentObject());
            if (actor->GetObjectName() == "Edges" || actor->GetObjectName() == "Vertexes")
            {
                if (flag)
                    actor->VisibilityOn();
                else
                    actor->VisibilityOff();
            }
        }
        auto actors2DIter = renderer->GetActors2D()->NewIterator();
        for (actors2DIter->GoToFirstItem(); !actors2DIter->IsDoneWithTraversal(); actors2DIter->GoToNextItem())
        {
            auto actor2D = static_cast<vtkActor2D*>(actors2DIter->GetCurrentObject());
            if (actor2D->GetObjectName() == "Labels")
            {
                if (flag)
                    actor2D->VisibilityOn();
                else
                    actor2D->VisibilityOff();
            }
        }
    }
    renderWindow->Render();
}