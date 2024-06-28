#include "Application.h"

//#define WATCH_RENDER_TIME

Application::Application(std::string filename)
{
    pathToSettings = filename;
    UpdateJson();
}

void CallbackFunction(vtkObject* caller, long unsigned int vtkNotUsed(eventId),
    void* vtkNotUsed(clientData), void* vtkNotUsed(callData))
{
    vtkRenderer* renderer = static_cast<vtkRenderer*>(caller);

    double timeInSeconds = renderer->GetLastRenderTimeInSeconds();
    double fps = 1.0 / timeInSeconds;
    std::cout << "FPS: " << fps << std::endl;

    std::cout << "Callback" << std::endl;
}

void Application::Start()
{
    std::array<unsigned char, 4> bkg{ 82, 87, 110, 255 };
    vtkNew<vtkNamedColors> colors;
    colors->SetColor("ParaViewBkg", bkg.data());

    renderer->SetBackground(colors->GetColor3d("ParaViewBkg").GetData());

    renderWindow->SetSize(1280, 800);
    renderWindow->SetWindowName("Vtk Project");
    renderWindow->AddRenderer(renderer);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    auto style = GetStyle();
    renderWindowInteractor->SetInteractorStyle(style);

    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetResolution(8);
    //cylinder->SetDirection(0, 1, 0);
    cylinder->SetHeight(20);
    cylinder->Update();

    auto bounds = cylinder->GetOutput()->GetBounds();

    vtkNew<vtkElevationFilter> elevation_filter;
    elevation_filter->SetLowPoint(0, bounds[2], 0);
    elevation_filter->SetHighPoint(0, bounds[3], 0);
    elevation_filter->SetInputConnection(cylinder->GetOutputPort());

    auto ctf = GetColorTable();

    mapper->SetInputConnection(elevation_filter->GetOutputPort());
    mapper->SetLookupTable(ctf);
    mapper->SetColorModeToMapScalars();
    mapper->InterpolateScalarsBeforeMappingOn();

    actor->SetMapper(mapper);

    renderer->AddActor(actor);

#ifdef WATCH_RENDER_TIME
    std::chrono::high_resolution_clock timer;
    for (int i = 0; i < 10; ++i)
    {
        auto start = timer.now();
        renderWindow->Render();
        auto result = timer.now() - start;
        std::cout << "Default Rendering time: " << std::chrono::duration_cast<std::chrono::milliseconds>(result).count() << std::endl;
    }
    std::cout << "Default GetRenderingBackEnd: " << renderWindow->GetRenderingBackend() << std::endl;

    SaveScreen("DefaultRender.jpg");
#else
    vtkNew<vtkCallbackCommand> callback;

    callback->SetCallback(CallbackFunction);
    renderer->AddObserver(vtkCommand::EndEvent, callback);
    renderWindow->Render();
    renderWindowInteractor->Start();
#endif
}

void Application::OffScreenRendering()
{
    std::array<unsigned char, 4> bkg{ 82, 87, 110, 255 };
    vtkNew<vtkNamedColors> colors;
    colors->SetColor("ParaViewBkg", bkg.data());

    renderer->SetBackground(colors->GetColor3d("ParaViewBkg").GetData());

    renderWindow->SetSize(1280, 800);
    renderWindow->SetWindowName("Vtk Project");
    renderWindow->AddRenderer(renderer);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    auto style = GetStyle();
    renderWindowInteractor->SetInteractorStyle(style);

    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetResolution(8);
    //cylinder->SetDirection(0, 1, 0);
    cylinder->SetHeight(20);
    cylinder->Update();

    auto bounds = cylinder->GetOutput()->GetBounds();

    vtkNew<vtkElevationFilter> elevation_filter;
    elevation_filter->SetLowPoint(0, bounds[2], 0);
    elevation_filter->SetHighPoint(0, bounds[3], 0);
    elevation_filter->SetInputConnection(cylinder->GetOutputPort());

    auto ctf = GetColorTable();

    mapper->SetInputConnection(elevation_filter->GetOutputPort());
    mapper->SetLookupTable(ctf);
    mapper->SetColorModeToMapScalars();
    mapper->InterpolateScalarsBeforeMappingOn();

    actor->SetMapper(mapper);

    renderer->AddActor(actor);

    renderWindow->OffScreenRenderingOn();
#ifdef WATCH_RENDER_TIME
    std::chrono::high_resolution_clock timer;
    for (int i = 0; i < 10; ++i)
    {
        auto start = timer.now();
        renderWindow->Render();
        auto result = timer.now() - start;
        std::cout << "Off Screen Rendering time: " << std::chrono::duration_cast<std::chrono::milliseconds>(result).count() << std::endl;
    }
    std::cout << "Off Screen GetRenderingBackEnd: " << renderWindow->GetRenderingBackend() << std::endl;

    SaveScreen("Off Screen Render.jpg");
#else
    renderWindow->Render();
#endif
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
    style->AddKeyBind("x", std::bind(&Application::ChangeParallelProjection, this));
    style->AddKeyBind("c", std::bind(&Application::PrintClippingRange, this));
    style->AddKeyBind("v", std::bind(&Application::ChangeClippingRangeMode, this));
    style->AddKeyBind("b", std::bind(&Application::ChangeColorMode, this));
    return style;
}

vtkNew<vtkDiscretizableColorTransferFunction> Application::GetColorTable()
{
    vtkNew<vtkDiscretizableColorTransferFunction> ctf;

    ctf->SetColorSpaceToLab();
    ctf->SetScaleToLinear();

    ctf->SetNanColor(0, 0, 0);
    ctf->SetAboveRangeColor(0, 0, 0);
    ctf->UseAboveRangeColorOn();
    ctf->SetBelowRangeColor(0, 0, 0);
    ctf->UseBelowRangeColorOn();

    ctf->AddRGBPoint(0, 0.05639999999999999, 0.05639999999999999, 0.47);
    ctf->AddRGBPoint(0.17159223942480895, 0.24300000000000013, 0.4603500000000004,
        0.81);
    ctf->AddRGBPoint(0.2984914818394138, 0.3568143826543521, 0.7450246485363142,
        0.954367702893722);
    ctf->AddRGBPoint(0.4321287371255907, 0.6882, 0.93, 0.9179099999999999);
    ctf->AddRGBPoint(0.5, 0.8994959551205902, 0.944646394975174,
        0.7686567142818399);
    ctf->AddRGBPoint(0.5882260353170073, 0.957107977357604, 0.8338185108985666,
        0.5089156299842102);
    ctf->AddRGBPoint(0.7061412605695164, 0.9275207599610714, 0.6214389091739178,
        0.31535705838676426);
    ctf->AddRGBPoint(0.8476395308725272, 0.8, 0.3520000000000001,
        0.15999999999999998);
    ctf->AddRGBPoint(1, 0.59, 0.07670000000000013, 0.11947499999999994);

    ctf->SetNumberOfValues(9);
    ctf->DiscretizeOff();

    return ctf;
}

void Application::UpdateSettings()
{
    std::cout << "Updating settings...\n";
    UpdateJson();

    auto camera = renderer->GetActiveCamera();
    camera->SetParallelProjection(settings["Projection"]["IsEnabled"]);
    camera->SetParallelScale(settings["Projection"]["ParallelScale"]);
    camera->SetClippingRange(settings["Projection"]["ClippingRange"]["Min"],
                             settings["Projection"]["ClippingRange"]["Max"]);

    actor->GetProperty()->SetColor(
        colors->GetColor3d(settings["Actor"]["Color"]).GetData());
    actor->GetProperty()->SetOpacity((double)settings["Actor"]["Opacity"] / 100);

    renderWindow->Render();
}

void Application::ChangeParallelProjection()
{
    auto camera = renderer->GetActiveCamera();
    bool flag = camera->GetParallelProjection();
    camera->SetParallelProjection(!flag);
    
    renderWindow->Render();
}

void Application::PrintClippingRange()
{
    auto clippingRange = renderer->GetActiveCamera()->GetClippingRange();
    std::cout << "Now ClippingRange is " << clippingRange[0] << ", " << clippingRange[1] << ".\n";
}

void Application::ChangeClippingRangeMode()
{
    auto style = (vtkInteractorStyle *)(renderWindowInteractor->GetInteractorStyle());
    bool flag = style->GetAutoAdjustCameraClippingRange();
    style->SetAutoAdjustCameraClippingRange(!flag);

    renderWindow->Render();
}

void Application::ChangeColorMode()
{
    auto colorTable = dynamic_cast<vtkDiscretizableColorTransferFunction*>(mapper->GetLookupTable());
    bool flag = colorTable->GetDiscretize();
    colorTable->SetDiscretize(!flag);

    renderWindow->Render();
}