#include "Application.h"

vtkSP<vtkImageData> CreateImage(int r, int g, int b)
{
    vtkNew<vtkImageData> image;

    std::array<unsigned char, 3> color{ r, g, b };

    // Specify the size of the image data.
    image->SetDimensions(2, 2, 1);
    image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

    int* dims = image->GetDimensions();

    // Fill the image with
    for (int y = 0; y < dims[1]; y++)
    {
        for (int x = 0; x < dims[0]; x++)
        {
            unsigned char* pixel = static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
            for (int i = 0; i < 3; ++i)
            {
                pixel[i] = color[i];
            }
        }
    }

    return image;
}

Application::Application()
{
    std::cout << "Initialize Application" << std::endl;

    colors = vtkNamedColors::New();
    renderer = vtkRenderer::New();
    renderWindow = vtkRenderWindow::New();
    renderWindowInteractor = vtkRenderWindowInteractor::New();
    interactorStyle = InteractorStyleProject::New();

    renderer->SetBackground(0., 127., 127.);
    renderer->UseHiddenLineRemovalOn();

    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(1280, 800);
    renderWindow->SetWindowName("Vtk Project");

    renderWindowInteractor->SetRenderWindow(renderWindow);

    interactorStyle->SetWindowInteractor(renderWindowInteractor);
    interactorStyle->SetRenderer(renderer);
}

void Application::AddSettings(std::string filename)
{
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
        polyData->SetPolys(importer->GetPolys());

        AddObject(polyData, false, enableGrid);

        std::cout << "File readed" << std::endl;
    }
    else
    {
        std::cout << "File " << fileName << " not found" << std::endl;
    }
}

void Application::AddObject(vtkSP<vtkPolyData> source, bool enableIsolines, bool enableGrid)
{
    std::cout << "Adding Object" << std::endl;

    //std::cout << "scalars: ";
    //std::cout << source->GetPointData()->GetScalars()->GetNumberOfValues();
    //source->GetPointData()->GetScalars()->PrintValues(std::cout);

    auto bounds = source->GetBounds();

    std::cout << "bounds: ";
    for (int i = 0; i < 6; ++i)
        std::cout << bounds[i] << ", ";
    std::cout << std::endl;

    vtkNew<vtkPolyDataMapper> mapper;
    if (enableIsolines == true)
    {
        auto ctf = GetCTF(source->GetScalarRange());

        mapper->SetInputData(source);
        mapper->SetLookupTable(ctf);
        mapper->SetColorModeToMapScalars();
        mapper->InterpolateScalarsBeforeMappingOn();
        mapper->SetScalarModeToUsePointData();
        mapper->ScalarVisibilityOn();
        mapper->UseLookupTableScalarRangeOn();

        CreateIsolines(source);
    }
    else
    {
        vtkNew<vtkElevationFilter> elevationFilter;
        elevationFilter->SetLowPoint(0, bounds[2], 0);
        elevationFilter->SetHighPoint(0, bounds[3], 0);
        elevationFilter->SetInputData(source);

        auto ctf = GetCTF(elevationFilter->GetScalarRange());

        mapper->SetInputConnection(elevationFilter->GetOutputPort());
        mapper->SetLookupTable(ctf);
        mapper->SetColorModeToMapScalars();
        mapper->InterpolateScalarsBeforeMappingOn();
        mapper->SetScalarModeToUsePointData();
    }

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    renderer->AddActor(actor);

    if (enableGrid == true)
    {
        CreateGrid(source);
    }
    CreateClipping(source);

    mainActors.push_back(actor);

    std::cout << "Object Created" << std::endl;
}

void Application::Start()
{
    std::cout << "Start" << std::endl;

    CreateSliders();
    CreateButtons();
    CreateFPSCounter();

    std::cout << "Rendering with isolines and grid" << std::endl;

    renderWindow->Render();

    ChangeVisionToGradient();

    std::cout << "Start rendering without isolines and grid" << std::endl;

    renderWindow->Render();

    std::cout << "End rendering" << std::endl;

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

vtkSP<vtkDCTF> Application::GetCTF(double minValue, double maxValue)
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
    case 0: // Gradient
        for (auto mainActor : mainActors)
            mainActor->GetMapper()->ScalarVisibilityOn();
        for (auto ctf : ctfs)
            ctf->DiscretizeOff();
        ShowMainActorsOn();
        ShowIsolinesOff();
        ShowGridOff();
        ShowClippingOff();
        interactorStyle->enableSelection = false;
        break;
    case 1: // Discrete
        for (auto mainActor : mainActors)
            mainActor->GetMapper()->ScalarVisibilityOn();
        for (auto ctf : ctfs)
            ctf->DiscretizeOn();
        ShowMainActorsOn();
        ShowIsolinesOff();
        ShowGridOff();
        ShowClippingOff();
        interactorStyle->enableSelection = false;
        break;
    case 2: // Isolines
        for (auto mainActor : mainActors)
        {
            mainActor->GetMapper()->ScalarVisibilityOff();
            mainActor->GetProperty()->SetColor(0, 0, 0);
        }
        ShowMainActorsOn();
        ShowIsolinesOn();
        ShowGridOff();
        ShowClippingOff();
        interactorStyle->enableSelection = false;
        break;
    case 3: // Grid
        for (auto mainActor : mainActors)
        {
            mainActor->GetMapper()->ScalarVisibilityOff();
            mainActor->GetProperty()->SetColor(0, 0, 0);
        }
        ShowMainActorsOn();
        ShowIsolinesOff();
        ShowGridOn();
        ShowClippingOff();
        interactorStyle->enableSelection = true;
        break;
    case 4: // Clipping
        ShowMainActorsOff();
        ShowIsolinesOff();
        ShowGridOff();
        ShowClippingOn();
        interactorStyle->enableSelection = false;
        break;
    default:
        return;
    }
    renderWindow->Render();
}

void Application::CreateClipping(vtkSP<vtkPolyData> source)
{
    clipPlane = vtkPlane::New();
    clipPlane->SetNormal(0., -1., 0.);
    clipPlane->SetOrigin(0., 0., 0.);

    vtkNew<vtkClipPolyData> clipper;
    clipper->SetInputData(source);
    clipper->SetClipFunction(clipPlane);
    clipper->GenerateClippedOutputOn();

    vtkNew<vtkPolyDataMapper> clippingMapper;
    clippingMapper->SetInputConnection(clipper->GetOutputPort());

    vtkNew<vtkProperty> backFaces;
    backFaces->SetSpecular(0.0);
    backFaces->SetDiffuse(0.0);
    backFaces->SetAmbient(1.0);
    backFaces->SetAmbientColor(50. / 256., 50. / 256., 50. / 256.);


    vtkNew<vtkActor> clippingActor;
    clippingActor->SetMapper(clippingMapper);
    clippingActor->VisibilityOff();
    clippingActor->SetBackfaceProperty(backFaces);

    std::cout << "bounds: ";
    for (int i = 0; i < 6; ++i)
        std::cout << source->GetBounds()[i] << ", ";
    std::cout << std::endl;

    vtkNew<vtkImplicitPlaneRepresentation> planeRepr;
    planeRepr->SetPlaceFactor(1.25);
    planeRepr->PlaceWidget(source->GetBounds());
    planeRepr->SetNormal(clipPlane->GetNormal());
    planeRepr->DrawOutlineOff();

    vtkNew<ChangeClippingPlaneCallback> planeCb;
    planeCb->clipPlane = clipPlane;

    clippingPlaneWidget = vtkImplicitPlaneWidget2::New();
    clippingPlaneWidget->SetInteractor(renderWindowInteractor);
    clippingPlaneWidget->SetRepresentation(planeRepr);
    clippingPlaneWidget->AddObserver(vtkCommand::InteractionEvent, planeCb);
    clippingPlaneWidget->EnabledOff();

    renderer->AddActor(clippingActor);

    clippingActors.push_back(clippingActor);
}

void Application::CreateIsolines(vtkSP<vtkPolyData> source)
{
    std::cout << "Creating isolines" << std::endl;

    double range[2];
    source->GetScalarRange(range);

    std::cout << "\tScalar range is: " << range[0] << ", " << range[1] << std::endl;

    vtkNew<vtkContourFilter> contourFilter;
    contourFilter->SetInputData(source);
    contourFilter->GenerateValues(25, range[0], range[1]);
    contourFilter->ComputeScalarsOn();
    contourFilter->ComputeGradientsOff();
    contourFilter->ComputeNormalsOff();
    contourFilter->UseScalarTreeOff();
    contourFilter->FastModeOff();

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

    renderer->AddActor(isolinesActor);

    isolinesActors.push_back(isolinesActor);

    std::cout << "created isolines" << std::endl;
}

void Application::CreateGrid(vtkSP<vtkPolyData> source)
{
    std::cout << "Creating Grid" << std::endl;

    std::cout << "Creating Edges" << std::endl;

    vtkNew<vtkExtractEdges> extract;
    extract->SetInputData(source);
    vtkNew<vtkPolyDataMapper> mapEdges;
    mapEdges->SetInputConnection(extract->GetOutputPort());
    mapEdges->ScalarVisibilityOff();
    vtkNew<vtkActor> edgesActor;
    edgesActor->SetMapper(mapEdges);
    edgesActor->GetProperty()->SetColor(51. / 255., 161. / 255., 201. / 255.);

    std::cout << "Creating Vertexes" << std::endl;

    vtkNew<vtkVertexGlyphFilter> glyphFilter;
    glyphFilter->SetInputData(source);
    glyphFilter->Update();
    vtkNew<vtkPolyDataMapper> mapBalls;
    mapBalls->SetInputConnection(glyphFilter->GetOutputPort());
    mapBalls->ScalarVisibilityOff();
    vtkNew<vtkActor> VertexesActor;
    VertexesActor->SetMapper(mapBalls);
    VertexesActor->GetProperty()->SetColor(255. / 255., 105. / 255., 180. / 255.);
    VertexesActor->GetProperty()->SetPointSize(10);
    VertexesActor->GetProperty()->RenderPointsAsSpheresOn();

    renderer->AddActor(edgesActor);
    renderer->AddActor(VertexesActor);

    gridActors.push_back(std::make_pair<vtkActor*, vtkActor*>(edgesActor, VertexesActor));

    std::cout << "Grid Created" << std::endl;
}

void Application::ShowMainActors(bool flag)
{
    for (auto mainActor : mainActors)
    {
        if (flag)
        {
            mainActor->VisibilityOn();
        }
        else
        {
            mainActor->VisibilityOff();
        }
    }
}

void Application::ShowIsolines(bool flag)
{
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

void Application::ShowClipping(bool flag)
{
    for (auto clippingActor : clippingActors)
    {
        if (flag)
        {
            clippingPlaneWidget->EnabledOn();
            for (auto clippingActor : clippingActors)
                clippingActor->VisibilityOn();
        }
        else
        {
            clippingPlaneWidget->EnabledOff();
            for (auto clippingActor : clippingActors)
                clippingActor->VisibilityOff();
        }
    }
}

void Application::CreateSliders()
{
    std::cout << "Creating Sliders" << std::endl;

    vtkNew<vtkSliderRepresentation2D> sliderRepresentation;
    sliderRepresentation->SetMinimumValue(0);
    sliderRepresentation->SetMaximumValue(4);
    sliderRepresentation->SetTitleText("Vision mode");

    sliderRepresentation->GetPoint1Coordinate()
        ->SetCoordinateSystemToNormalizedDisplay();
    sliderRepresentation->GetPoint2Coordinate()
        ->SetCoordinateSystemToNormalizedDisplay();
    sliderRepresentation->GetPoint1Coordinate()->SetValue(.1, .1);
    sliderRepresentation->GetPoint2Coordinate()->SetValue(.9, .1);

    vtkNew<ChangeVisionSliderCallback> callback;
    callback->app = this;

    changeVisionSliderWidget = vtkSliderWidget::New();
    changeVisionSliderWidget->SetInteractor(renderWindowInteractor);
    changeVisionSliderWidget->SetRepresentation(sliderRepresentation);
    changeVisionSliderWidget->AddObserver(vtkCommand::InteractionEvent, callback);
    changeVisionSliderWidget->EnabledOn();
}

void Application::CreateButtons()
{
    //vtkNew<vtkCoordinate> coordinate;
    //coordinate->SetCoordinateSystemToNormalizedDisplay();
    //coordinate->SetValue(.92, .26);

    //double bds[6];
    //double sz = 100.0;
    //bds[0] = coordinate->GetComputedDisplayValue(renderer)[0] - sz;
    //bds[1] = bds[0] + sz;
    //bds[2] = coordinate->GetComputedDisplayValue(renderer)[1] - sz;
    //bds[3] = bds[2] + sz;
    //bds[4] = bds[5] = 0.0;

    //vtkSP<vtkImageData> red = CreateImage(255, 0, 0);
    //vtkSP<vtkImageData> green = CreateImage(0, 255, 0);

    //vtkNew<vtkTexturedButtonRepresentation2D> clippingBR;
    //clippingBR->SetNumberOfStates(2);
    //clippingBR->SetButtonTexture(0, red);
    //clippingBR->SetButtonTexture(1, green);
    //clippingBR->PlaceWidget(bds);

    //vtkNew<ChangeClippingButtonCallback> clippingCb;
    //clippingCb->clippingPlaneWidget = clippingPlaneWidget;
    //clippingCb->mainActors = mainActors;
    //clippingCb->clippingActors = clippingActors;

    //changeClippingButton = vtkButtonWidget::New();
    //changeClippingButton->SetInteractor(renderWindowInteractor);
    //changeClippingButton->SetRepresentation(clippingBR);
    //changeClippingButton->AddObserver(vtkCommand::StateChangedEvent, clippingCb);
    //changeClippingButton->EnabledOn();

}

void FPSCounterCallback(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
    vtkRenderer* renderer = static_cast<vtkRenderer*>(caller);
    vtkTextMapper* fpsTextMapper = static_cast<vtkTextMapper*>(clientData);

    double time = renderer->GetLastRenderTimeInSeconds(); // Get the time required for the last rendering call
    int fps = 1.0 / time;

    fpsTextMapper->SetInput(("FPS: " + std::to_string(fps)).c_str());
}

void Application::CreateFPSCounter()
{
    fpsTextMapper = vtkTextMapper::New();
    fpsTextActor = vtkActor2D::New();

    vtkNew<vtkTextProperty> textProperty;
    textProperty->BoldOn();
    textProperty->SetFontSize(25);

    fpsTextMapper->SetTextProperty(textProperty);
    fpsTextActor->SetMapper(fpsTextMapper);
    fpsTextActor->SetPosition(25, 25);
    renderer->AddActor(fpsTextActor);

    vtkNew<vtkCallbackCommand> callback;
    callback->SetCallback(FPSCounterCallback);
    callback->SetClientData(fpsTextMapper);
    renderer->AddObserver(vtkCommand::EndEvent, callback);
}