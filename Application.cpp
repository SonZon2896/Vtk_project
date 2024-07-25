#include "Application.h"

Application::Application()
{
    std::cout << "Initialize Application" << std::endl;

    colors = vtkNamedColors::New();
    renderWindow = vtkRenderWindow::New();
    renderWindowInteractor = vtkRenderWindowInteractor::New();

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->SetSize(1280, 800);
    renderWindow->SetWindowName("Vtk Project");
    renderWindowInteractor->SetRenderWindow(renderWindow);
}

void Application::AddSettings(std::string filename)
{
    std::cout << "Entered in function 'AddSettings'" << std::endl;

    pathToSettings = filename;
    UpdateJson();
}

void Application::AddObject(std::string fileName, bool enableIsolines, bool enableGrid)
{
    std::cout << "Start read object in file" << std::endl;

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

        std::cout << "File readed" << std::endl;
        return;
    }
    else if (ext == ".csv3d")
    {
        std::cout << "read csv3d file" << std::endl;

        vtkNew<CSV3DImporter> importer;
        importer->fileName = fileName;
        importer->Update();

        vtkNew<vtkPolyData> polyData;
        polyData->SetPoints(importer->GetPoints());
        polyData->SetVerts(importer->GetVerts());
        polyData->SetLines(importer->GetLines());
        polyData->SetPolys(importer->GetPolys());

        AddObject(polyData, false, enableGrid);

        std::cout << "File readed" << std::endl;
        return;
    }
    else
    {
        std::cout << "File " << fileName << " not found" << std::endl;
        return;
    }
}

void Application::AddObject(vtkSP<vtkPolyData> source, bool enableIsolines, bool enableGrid)
{
    std::cout << "Adding Object" << std::endl;

    auto bounds = source->GetBounds();

    vtkNew<vtkElevationFilter> elevationFilter;
    elevationFilter->SetLowPoint(0, bounds[2], 0);
    elevationFilter->SetHighPoint(0, bounds[3], 0);
    elevationFilter->SetInputData(source);

    auto ctf = GetCTF(source->GetScalarRange());

    vtkNew<vtkPolyDataMapper> mapper;
    if (enableIsolines == true)
        mapper->SetInputData(source);
    else
        mapper->SetInputConnection(elevationFilter->GetOutputPort());
    mapper->SetLookupTable(ctf);
    mapper->SetColorModeToMapScalars();
    mapper->InterpolateScalarsBeforeMappingOn();
    mapper->SetScalarModeToUsePointData();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(0., 127., 127.);

    if (enableIsolines == true)
    {
        CreateIsolines(source, renderer);
    }
    if (enableGrid == true)
    {
        CreateGrid(source, renderer);
    }

    renderWindow->AddRenderer(renderer);

    mainActors.push_back(actor);

    std::cout << "Object Created" << std::endl;
}

void Application::Start()
{
    std::cout << "Entered in function 'Start'" << std::endl;

    CreateSlider();

    renderWindow->Render();
    renderWindowInteractor->Start();
}

void Application::OffScreenRendering()
{
    std::cout << "Entered in function 'OffScreenRendering'" << std::endl;

    renderWindow->OffScreenRenderingOn();
    renderWindow->Render();
}

void Application::SaveScreen(std::string fileName)
{
    std::cout << "Entered in function 'SaveScreen'" << std::endl;

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
    std::cout << "Entered in function 'UpdateJson'" << std::endl;

    std::ifstream f(pathToSettings);
    settings = json::parse(f);
}

vtkSP<vtkDCTF> Application::GetCTF(double minValue, double maxValue)
{
    std::cout << "Entered in function 'GetCTF'" << std::endl;

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
    std::cout << "Entered in function 'UpdateSettings'" << std::endl;

    if (pathToSettings.empty())
        return;

    UpdateJson();

    

    renderWindow->Render();
}

void Application::ChangeProjection(unsigned int mode)
{
    std::cout << "Entered in function 'ChangeProjection'" << std::endl;

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
    std::cout << "Entered in function 'ChangeVision'" << std::endl;

    switch (mode)
    {
    case 0: // Gradient
        for (auto mainActor : mainActors)
            mainActor->GetMapper()->ScalarVisibilityOn();
        for (auto ctf : ctfs)
            ctf->DiscretizeOff();
        ShowIsolinesOff();
        ShowGridOff();
        ShowLabelsOff();
        break;
    case 1: // Discrete
        for (auto mainActor : mainActors)
            mainActor->GetMapper()->ScalarVisibilityOn();
        for (auto ctf : ctfs)
            ctf->DiscretizeOn();
        ShowIsolinesOff();
        ShowGridOff();
        ShowLabelsOff();
        break;
    case 2: // Isolines
        for (auto mainActor : mainActors)
        {
            mainActor->GetMapper()->ScalarVisibilityOff();
            mainActor->GetProperty()->SetColor(0, 0, 0);
        }
        ShowIsolinesOn();
        ShowGridOff();
        ShowLabelsOff();
        break;
    case 3: // Grid
        for (auto mainActor : mainActors)
        {
            mainActor->GetMapper()->ScalarVisibilityOff();
            mainActor->GetProperty()->SetColor(0, 0, 0);
        }
        ShowIsolinesOff();
        ShowGridOn();
        ShowLabelsOff();
        break;
    case 4: // Grid with Labels
        for (auto mainActor : mainActors)
        {
            mainActor->GetMapper()->ScalarVisibilityOff();
            mainActor->GetProperty()->SetColor(0, 0, 0);
        }
        ShowIsolinesOff();
        ShowGridOn();
        ShowLabelsOn();
        break;
    default:
        return;
    }
    renderWindow->Render();
}

void Application::CreateIsolines(vtkSP<vtkPolyData> source, vtkSP<vtkRenderer> renderer)
{
    std::cout << "Entered in function 'CreateIsolines'" << std::endl;

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
    isolinesActor->SetMapper(isoMapper);
    isolinesActor->GetProperty()->SetLineWidth(2);
    
    isolinesActor->VisibilityOff();

    renderer->AddActor(isolinesActor);

    isolinesActors.push_back(isolinesActor);
}

void Application::CreateGrid(vtkSP<vtkPolyData> source, vtkSP<vtkRenderer> renderer)
{
    std::cout << "Entered in function 'CreateGrid'" << std::endl;

    std::cout << "Creating Edges" << std::endl;
    vtkNew<vtkExtractEdges> extract;
    extract->SetInputData(source);
    vtkNew<vtkTubeFilter> tubes;
    tubes->SetInputConnection(extract->GetOutputPort());
    tubes->SetRadius(0.01);
    tubes->SetNumberOfSides(3);
    vtkNew<vtkPolyDataMapper> mapEdges;
    mapEdges->SetInputConnection(tubes->GetOutputPort());
    mapEdges->ScalarVisibilityOff();
    vtkNew<vtkActor> edgesActor;
    edgesActor->SetMapper(mapEdges);
    edgesActor->GetProperty()->SetColor(51. / 255., 161. / 255., 201. / 255.);

    std::cout << "Creating Vertexes" << std::endl;
    vtkNew<vtkSphereSource> ball;
    ball->SetRadius(0.025);
    ball->SetThetaResolution(2);
    ball->SetPhiResolution(2);
    vtkNew<vtkGlyph3D> balls;
    balls->SetInputData(source);
    balls->SetSourceConnection(ball->GetOutputPort());
    vtkNew<vtkPolyDataMapper> mapBalls;
    mapBalls->SetInputConnection(balls->GetOutputPort());
    mapBalls->ScalarVisibilityOff();
    vtkNew<vtkActor> VertexesActor;
    VertexesActor->SetMapper(mapBalls);
    VertexesActor->GetProperty()->SetColor(255. / 255., 105. / 255., 180. / 255.);

    std::cout << "Creating Labels" << std::endl;
    vtkNew<vtkIdFilter> idFilter;
    idFilter->SetInputData(source);
    idFilter->PointIdsOn();

    vtkNew<vtkSelectVisiblePoints> visiblePoints;
    visiblePoints->SetInputConnection(idFilter->GetOutputPort());
    visiblePoints->SetRenderer(renderer);

    vtkNew<vtkLabeledDataMapper> labelMapper;
    labelMapper->SetInputConnection(visiblePoints->GetOutputPort());
    labelMapper->SetLabelModeToLabelFieldData();

    vtkNew<vtkActor2D> labelsActor;
    labelsActor->SetMapper(labelMapper);

    renderer->AddActor(edgesActor);
    renderer->AddActor(VertexesActor);
    renderer->AddActor(labelsActor);

    edgesActor->VisibilityOff();
    VertexesActor->VisibilityOff();
    labelsActor->VisibilityOff();

    vtkNew<UpdateMeshGridCallback> callback;
    callback->SetSphereSource(ball);
    callback->SetCylinderSource(tubes);

    renderer->AddObserver(vtkCommand::StartEvent, callback);

    gridActors.push_back(std::make_pair<vtkActor*, vtkActor*>(edgesActor, VertexesActor));
    labelsActors.push_back(labelsActor);

    std::cout << "Grid Created" << std::endl;
}

void Application::ShowIsolines(bool flag)
{
    std::cout << "Entered in function 'ShowIsolines'" << std::endl;

    for (auto isolinesActor : isolinesActors)
    {
        if (flag)
            isolinesActor->VisibilityOn();
        else
            isolinesActor->VisibilityOff();
    }
    renderWindow->Render();
}

void Application::ShowGrid(bool flag)
{
    std::cout << "Entered in function 'ShowGrid'" << std::endl;

    for (auto gridActor : gridActors)
    {
        if (flag)
        {
            gridActor.first->VisibilityOn();
            gridActor.second->VisibilityOn();
        }
        else
        {
            gridActor.first->VisibilityOff();
            gridActor.second->VisibilityOff();
        }
    }
    renderWindow->Render();
}

void Application::CreateSlider()
{
    std::cout << "Entered in function 'CreateSlider'" << std::endl;

    vtkNew<vtkSliderRepresentation2D> sliderRepresentation;
    sliderRepresentation->SetMinimumValue(0);
    sliderRepresentation->SetMaximumValue(4);
    sliderRepresentation->SetTitleText("Vision mode");

    sliderRepresentation->GetPoint1Coordinate()
        ->SetCoordinateSystemToNormalizedDisplay();
    sliderRepresentation->GetPoint1Coordinate()->SetValue(.1, .1);
    sliderRepresentation->GetPoint2Coordinate()
        ->SetCoordinateSystemToNormalizedDisplay();
    sliderRepresentation->GetPoint2Coordinate()->SetValue(.9, .1);

    vtkNew<ChangeVisionSliderCallback> callback;
    callback->app = this;

    changeVisionSliderWidget = vtkSliderWidget::New();
    changeVisionSliderWidget->SetInteractor(renderWindowInteractor);
    changeVisionSliderWidget->SetRepresentation(sliderRepresentation);
    changeVisionSliderWidget->AddObserver(vtkCommand::InteractionEvent, callback);
    changeVisionSliderWidget->EnabledOn();
}

void Application::ShowLabels(bool flag)
{
    std::cout << "Entered in function 'ShowLabels'" << std::endl;

    for (auto labelsActor : labelsActors)
    {
        if (flag)
            labelsActor->VisibilityOn();
        else
            labelsActor->VisibilityOff();
    }
}