#pragma once

#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkElevationFilter.h>
#include <vtkRenderer.h>
#include <vtkSTLReader.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkContextMouseEvent.h>
#include <vtkCylinderSource.h>
#include <vtkConeSource.h>
#include <vtkSphereSource.h>
#include <vtkImageWriter.h>
#include <vtkBMPWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkPNMWriter.h>
#include <vtkPostScriptWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkPNGWriter.h>
#include <vtkWindowToImageFilter.h>
#include <vtknlohmann/json.hpp>
#include <array>
#include <string>
#include <fstream>
#include <functional>
#include "Interactor.h"

#include <chrono>
#include <thread>

using json = nlohmann::json;

class KeyPressInteractorStyle;

class Application {
private:
	json settings;
	std::string pathToSettings;

	vtkNew<vtkNamedColors> colors;
	vtkNew<vtkActor> actor;
	vtkNew<vtkPolyDataMapper> mapper;
	vtkNew<vtkRenderer> renderer;
	vtkNew<vtkRenderWindow> renderWindow;
	vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

	void UpdateJson();
	vtkNew<KeyPressInteractorStyle>GetStyle();
	vtkNew<vtkDiscretizableColorTransferFunction> GetColorTable();
	

public:
	Application() = delete;
	Application(std::string PathToSettings);

	void Start();
	void OffScreenRendering();

	void SaveScreen(std::string filename);

	void UpdateSettings();
	void ChangeParallelProjection();
	void PrintClippingRange();
	void ChangeClippingRangeMode();
	void ChangeColorMode();
};