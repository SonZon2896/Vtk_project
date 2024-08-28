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
    rendererOutline = vtkRenderer::New();
    renderWindow = vtkRenderWindow::New();
    renderWindowInteractor = vtkRenderWindowInteractor::New();
    interactorStyle = InteractorStyleProject::New();

    renderer->SetBackground(colors->GetColor3d("SkyBlue").GetData());
    renderer->UseHiddenLineRemovalOn();

    vtkNew<vtkRenderStepsPass> basicPasses;
    vtkNew<vtkOutlineGlowPass> glowPass;
    glowPass->SetDelegatePass(basicPasses);
    glowPass->SetOutlineIntensity(50.);

    rendererOutline->SetPass(glowPass);
    rendererOutline->SetLayer(1);

    renderWindow->AddRenderer(rendererOutline);
    renderWindow->AddRenderer(renderer);
    renderWindow->SetNumberOfLayers(2);
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

void Application::AddObject(std::string fileName)
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

        AddObject(polyData);
        if (importer->IsQuadTriangles())
            AddQuadraticTriangles(importer->GetQuadTriangles());
        std::cout << "File readed" << std::endl;
    }
    else
    {
        std::cout << "File " << fileName << " not found" << std::endl;
    }
}

void Application::AddObject(vtkSP<vtkPolyData> source, bool enableScalars)
{
    std::cout << "Adding Object" << std::endl;

    auto bounds = source->GetBounds();

    std::cout << "bounds: ";
    for (int i = 0; i < 6; ++i)
        std::cout << bounds[i] << ", ";
    std::cout << std::endl;

    vtkNew<vtkPolyDataMapper> mapper;
    if (enableScalars)
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

        CreateIsolines(elevationFilter->GetPolyDataOutput());
    }

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    renderer->AddActor(actor);

    CreateClipping(source);
    CreateOutline(source);
    CreateGrid(source);
    CreateProportions(actor);

    mainActors.push_back(actor);

    std::cout << "Object Created" << std::endl;
}

void Application::AddQuadraticTriangles(std::vector<vtkSP<vtkUnstructuredGrid>> grids)
{
    std::cout << "Adding quadric triangles" << std::endl;

    std::vector<vtkSP<vtkActor>> quadricActor;
    for (auto grid : grids)
    {
        vtkNew<vtkTessellatorFilter> tessellate;
        tessellate->SetInputData(grid);
        tessellate->SetChordError(0.000002);

        vtkNew<vtkDataSetMapper> mapper;
        mapper->SetInputConnection(tessellate->GetOutputPort());
        mapper->ScalarVisibilityOff();

        // Create an actor for the grid
        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(colors->GetColor3d("Silver").GetData());

        renderer->AddActor(actor);
        quadricActor.push_back(actor);
    }
    quadricActors.push_back(quadricActor);

    std::cout << "quadric triangles created" << std::endl;
}

void Application::Start()
{
    std::cout << "Start" << std::endl;

    CreateSliders();
    CreateButtons();
    CreateFPSCounter();

    std::cout << "Rendering all" << std::endl;

    renderWindow->Render();

    HideAll();
    ShowMainActorsOn();
    ChangeVision(0);

    std::cout << "Start rendering only model" << std::endl;

    renderWindow->Render();
    rendererOutline->SetActiveCamera(renderer->GetActiveCamera());

    std::cout << "End rendering" << std::endl;

    renderWindowInteractor->Start();
}

void Application::OffScreenRendering()
{
    ChangeVision(0);
    renderWindow->OffScreenRenderingOn();
    renderWindow->Render();
}

void Application::SaveScreen(std::string fileName)
{
    bool rgba = false;
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

        auto start_time = std::chrono::high_resolution_clock::now();
        window_to_image_filter->Update();
        auto time = std::chrono::high_resolution_clock::now() - start_time;
        std::cout << "Extracting buffer: " << std::chrono::duration_cast<std::chrono::milliseconds>(time).count() << " milliseconds" << std::endl;

        vtkImageData* imageData = window_to_image_filter->GetOutput();
        std::cout << "Writing buffer with resoulution: " << imageData->GetDimensions()[0] << "x" << imageData->GetDimensions()[1] << std::endl;

        std::ofstream out;
        out.open("buffer.txt");
        if (out.is_open())
        {
            start_time = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < imageData->GetPointData()->GetScalars()->GetNumberOfTuples(); ++i)
            {
                double* color = imageData->GetPointData()->GetScalars()->GetTuple(i);
                out << (char)color[0] << (char)color[1] << (char)color[2];
            }
            time = std::chrono::high_resolution_clock::now() - start_time;
            std::cout << "Writing buffer: " << std::chrono::duration_cast<std::chrono::milliseconds>(time).count() << " milliseconds" << std::endl;
        }
        out.close();

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
    //settings = json::parse(f);
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

void Application::ChangeProjection(bool mode)
{
    renderer->GetActiveCamera()->SetParallelProjection(mode);
    
    renderWindow->Render();
}

void Application::HideAll()
{
    ShowMainActorsOff();
    ShowIsolinesOff();
    ShowGridOff();
    ShowClippingOff();
    ShowOutlinesOff();
    ShowProportionsOff();
    ShowQuadricOff();
    interactorStyle->enableSelection = false;
}

void Application::ChangeVision(unsigned int mode)
{
    switch (mode)
    {
    case 0: // Standart
        for (auto mainActor : mainActors)
        {
            mainActor->GetMapper()->ScalarVisibilityOff();
            mainActor->GetProperty()->SetColor(colors->GetColor3d("Silver").GetData());
        }
        ShowIsolinesOff();
        break;
    case 1: // Gradient
        for (auto mainActor : mainActors)
            mainActor->GetMapper()->ScalarVisibilityOn();
        for (auto ctf : ctfs)
            ctf->DiscretizeOff();
        ShowIsolinesOff();
        break;
    case 2: // Discrete
        for (auto mainActor : mainActors)
            mainActor->GetMapper()->ScalarVisibilityOn();
        for (auto ctf : ctfs)
            ctf->DiscretizeOn();
        ShowIsolinesOff();
        break;
    case 3: // Isolines
        for (auto mainActor : mainActors)
        {
            mainActor->GetMapper()->ScalarVisibilityOff();
            mainActor->GetProperty()->SetColor(colors->GetColor3d("Silver").GetData());
        }
        ShowIsolinesOn();
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
    backFaces->SetAmbientColor(colors->GetColor3d("Silver").GetData());

    vtkNew<vtkActor> clippingActor;
    clippingActor->SetMapper(clippingMapper);
    clippingActor->VisibilityOff();
    clippingActor->SetBackfaceProperty(backFaces);
    clippingActor->GetProperty()->SetColor(colors->GetColor3d("Silver").GetData());

    vtkNew<vtkImplicitPlaneRepresentation> planeRepr;
    planeRepr->SetPlaceFactor(1.25);
    planeRepr->PlaceWidget(source->GetBounds());
    planeRepr->SetNormal(clipPlane->GetNormal());
    planeRepr->DrawOutlineOff();

    vtkNew<ChangeClippingPlaneCallback> planeCb;
    planeCb->clipPlane = clipPlane;

    vtkNew<vtkImplicitPlaneWidget2> clippingPlaneWidget;
    clippingPlaneWidget->SetInteractor(renderWindowInteractor);
    clippingPlaneWidget->SetRepresentation(planeRepr);
    clippingPlaneWidget->AddObserver(vtkCommand::InteractionEvent, planeCb);
    clippingPlaneWidget->EnabledOff();

    renderer->AddActor(clippingActor);

    clippingActors.push_back(clippingActor);
    clippingPlaneWidgets.push_back(clippingPlaneWidget);
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

void Application::CreateOutline(vtkSP<vtkPolyData> source)
{
    std::cout << "Creating Outline" << std::endl;

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->LightingOff();
    actor->GetProperty()->SetColor(colors->GetColor3d("Orange").GetData());
    actor->VisibilityOff();

    rendererOutline->AddActor(actor);

    outlinesActors.push_back(actor);

    std::cout << "Outline created" << std::endl;
}

void Application::CreateProportions(vtkSP<vtkActor> actor)
{
    std::cout << "Creating Proportions" << std::endl;

    vtkNew<ProportionsActor> proportionsActor;
    proportionsActor->SetActor(actor);
    renderer->AddActor(proportionsActor);

    propotionsActors.push_back(proportionsActor);

    std::cout << "Proportions created" << std::endl;
}

void Application::ShowMainActors(bool flag)
{
    for (auto mainActor : mainActors)
        mainActor->SetVisibility(flag);
}

void Application::ShowIsolines(bool flag)
{
    for (auto isolinesActor : isolinesActors)
        isolinesActor->SetVisibility(flag);
    renderWindow->Render();
}

void Application::ShowGrid(bool flag)
{
    for (auto gridActor : gridActors)
    {
        gridActor.first->SetVisibility(flag);
        gridActor.second->SetVisibility(flag);
        interactorStyle->enableSelection = flag;
    }
    renderWindow->Render();
}

void Application::ShowClipping(bool flag)
{
    for (int i = 0; i < clippingActors.size(); ++i)
    {
        clippingPlaneWidgets[i]->SetEnabled(flag);
        clippingActors[i]->SetVisibility(flag);
    }
}

void Application::ShowOutlines(bool flag)
{
    for (auto outlineActor : outlinesActors)
        outlineActor->SetVisibility(flag);
}

void Application::ShowProportions(bool flag)
{
    for (auto propotionsActor : propotionsActors)
        propotionsActor->SetVisibility(flag);
}

void Application::ShowQuadric(bool flag)
{
    for (auto quadricActor : quadricActors)
        for (auto polygon : quadricActor)
            polygon->SetVisibility(flag);
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

#define CREATE_SIMPLE_BUTTON_CALLBACK(name) void name(vtkObject* caller, unsigned long eventId, void* clientData, void* callData) \
{ \
    auto app = static_cast<Application*>(clientData); \
    auto button = static_cast<vtkButtonWidget*>(caller); \
    auto repr = static_cast<vtkTexturedButtonRepresentation2D*>(button->GetRepresentation()); \
    int state = repr->GetState(); 

CREATE_SIMPLE_BUTTON_CALLBACK(ChangeProjectionCallback)
    app->ChangeProjection(state);
}
CREATE_SIMPLE_BUTTON_CALLBACK(ChangeGridCallback)
    app->ShowGrid(state);
}
CREATE_SIMPLE_BUTTON_CALLBACK(ChangeOutlineCallback)
    app->ShowOutlines(state);
}
CREATE_SIMPLE_BUTTON_CALLBACK(ChangeProportionsCallback)
    app->ShowProportions(state);
}
struct AppWithButton
{
    Application* app;
    vtkButtonWidget* button;

    AppWithButton(Application* app, vtkButtonWidget* button)
    {
        this->app = app;
        this->button = button;
    }
};

void ChangeQuadElemsCallback(vtkObject* caller, unsigned long eventId, void* clientData, void* callData) {
    auto app = static_cast<Application*>(static_cast<AppWithButton*>(clientData)->app);
    auto button = static_cast<vtkButtonWidget*>(caller); 
    auto repr = static_cast<vtkTexturedButtonRepresentation2D*>(button->GetRepresentation()); 
    int state = repr->GetState();
    auto clippingButton = reinterpret_cast<vtkButtonWidget*>(static_cast<AppWithButton*>(clientData)->button);
    auto clippingRepresentation = static_cast<vtkTexturedButtonRepresentation2D*>(clippingButton->GetRepresentation());
    if (state && clippingRepresentation->GetState())
    {
        clippingRepresentation->SetState(0);
        clippingButton->InvokeEvent(vtkCommand::StateChangedEvent);
    }
    app->ShowMainActors(!state);
    app->ShowQuadric(state);
}
void ChangeClippingCallback(vtkObject* caller, unsigned long eventId, void* clientData, void* callData) {
    auto app = static_cast<Application*>(static_cast<AppWithButton*>(clientData)->app);
    auto button = static_cast<vtkButtonWidget*>(caller);
    auto repr = static_cast<vtkTexturedButtonRepresentation2D*>(button->GetRepresentation());
    int state = repr->GetState();
    auto quadElemsButton = reinterpret_cast<vtkButtonWidget*>(static_cast<AppWithButton*>(clientData)->button);
    auto quadElemsRepresentation = static_cast<vtkTexturedButtonRepresentation2D*>(quadElemsButton->GetRepresentation());
    if (state && quadElemsRepresentation->GetState())
    {
        quadElemsRepresentation->SetState(0);
        quadElemsButton->InvokeEvent(vtkCommand::StateChangedEvent);
    }
    app->ShowMainActors(!state);
    app->ShowClipping(state);
}

vtkSP<vtkButtonWidget> Application::CreateButton(double x, double y, std::string buttonName)
{
    vtkSP<vtkImageData> red = CreateImage(255, 0, 0);
    vtkSP<vtkImageData> green = CreateImage(0, 255, 0);

    vtkNew<vtkCoordinate> coordinate;
    coordinate->SetCoordinateSystemToNormalizedDisplay();
    coordinate->SetValue(x, y);

    double bds[6];
    double sz = 100.0;
    bds[0] = coordinate->GetComputedDisplayValue(renderer)[0] - sz;
    bds[1] = bds[0] + sz;
    bds[2] = coordinate->GetComputedDisplayValue(renderer)[1] - sz;
    bds[3] = bds[2] + sz;
    bds[4] = bds[5] = 0.0;

    vtkNew<vtkTexturedButtonRepresentation2D> buttonRepresentation;
    buttonRepresentation->SetNumberOfStates(2);
    buttonRepresentation->SetButtonTexture(0, red);
    buttonRepresentation->SetButtonTexture(1, green);
    buttonRepresentation->PlaceWidget(bds);

    vtkNew<vtkButtonWidget> buttonWidget;
    buttonWidget->SetInteractor(renderWindowInteractor);
    buttonWidget->SetRepresentation(buttonRepresentation);
    buttonWidget->EnabledOn();

    vtkNew<vtkTextActor> text;
    text->SetInput(buttonName.c_str());
    text->SetDisplayPosition(bds[0] + sz / 2, bds[2] + sz / 4);
    text->GetTextProperty()->SetJustificationToCentered();
    text->GetTextProperty()->SetVerticalJustificationToTop();
    text->GetTextProperty()->SetFontSize(16);
    text->GetTextProperty()->SetColor(colors->GetColor3d("White").GetData());
    text->GetTextProperty()->SetShadow(true);
    renderer->AddActor(text);

    buttons.push_back(buttonWidget);

    return buttonWidget;
}

void Application::CreateButtons()
{
    vtkNew<vtkCallbackCommand> quadElemsCallback;
    quadElemsCallback->SetCallback(ChangeQuadElemsCallback);
    vtkNew<vtkCallbackCommand> clippingCallback;
    clippingCallback->SetCallback(ChangeClippingCallback);

    auto clippingButton = CreateButton(.92, .26, "Clipping");
    auto quadElemsButton = CreateButton(.92, .36, "Quad Elems");

    clippingCallback->SetClientData(new AppWithButton(this, quadElemsButton));
    quadElemsCallback->SetClientData(new AppWithButton(this, clippingButton));

    clippingButton->AddObserver(vtkCommand::StateChangedEvent, clippingCallback);
    quadElemsButton->AddObserver(vtkCommand::StateChangedEvent, quadElemsCallback);
    
    vtkNew<vtkCallbackCommand> gridCallback;
    gridCallback->SetCallback(ChangeGridCallback);
    gridCallback->SetClientData(this);

    auto gridButton = CreateButton(.82, .26, "Grid");
    gridButton->AddObserver(vtkCommand::StateChangedEvent, gridCallback);

    vtkNew<vtkCallbackCommand> outlineCallback;
    outlineCallback->SetCallback(ChangeOutlineCallback);
    outlineCallback->SetClientData(this);

    auto outlineButton = CreateButton(.72, .26, "Outline");
    outlineButton->AddObserver(vtkCommand::StateChangedEvent, outlineCallback);

    vtkNew<vtkCallbackCommand> proportionsCallback;
    proportionsCallback->SetCallback(ChangeProportionsCallback);
    proportionsCallback->SetClientData(this);

    auto proportionsButton = CreateButton(.62, .26, "Proportions");
    proportionsButton->AddObserver(vtkCommand::StateChangedEvent, proportionsCallback);

    vtkNew<vtkCallbackCommand> projectionCallback;
    projectionCallback->SetCallback(ChangeProjectionCallback);
    projectionCallback->SetClientData(this);

    auto projectionButton = CreateButton(.52, .26, "Parallel Projection");
    projectionButton->AddObserver(vtkCommand::StateChangedEvent, projectionCallback);
}

void EndRenderCallback(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
    vtkRenderer* renderer = static_cast<vtkRenderer*>(caller);
    vtkTextMapper* fpsTextMapper = static_cast<vtkTextMapper*>(clientData);

    double time_last_render = renderer->GetLastRenderTimeInSeconds(); // Get the time required for the last rendering call
    int fps = 1.0 / time_last_render;

#ifdef ENABLE_FPS_LIMIT
    double render_limit = 1. / FPS_LIMIT;
    if (time_last_render < render_limit)
    {
        double time_to_wait = render_limit - time_last_render;
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((time_to_wait) * 1000)));
        fps = FPS_LIMIT;
    }
#endif

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
    callback->SetCallback(EndRenderCallback);
    callback->SetClientData(fpsTextMapper);
    renderer->AddObserver(vtkCommand::EndEvent, callback);
}