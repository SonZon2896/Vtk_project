#pragma once

#include "includes.h"

#define ENABLE_FPS_LIMIT
#define FPS_LIMIT 90;
//#undef ENABLE_FPS_LIMIT // ���������������� ���� �� ����� ����� ���

class Application {
private:
	//json settings;
	std::string pathToSettings;

	std::vector<vtkSP<vtkDiscretizableColorTransferFunction>> ctfs;
	std::vector<vtkSP<vtkActor>> mainActors;
	std::vector<vtkSP<vtkActor>> clippingActors;
	std::vector<vtkSP<vtkImplicitPlaneWidget2>> clippingPlaneWidgets;
	std::vector<vtkSP<vtkActor>> isolinesActors;
	std::vector<std::pair<vtkSP<vtkActor>, vtkSP<vtkActor>>> gridActors;
	std::vector<vtkSP<vtkActor>> outlinesActors;
	std::vector<vtkSP<vtkProp3D>> propotionsActors;
	std::vector<std::vector<vtkSP<vtkActor>>> quadricActors;

	vtkSP<vtkNamedColors> colors;
	vtkSP<vtkRenderer> renderer;
	vtkSP<vtkRenderer> rendererOutline;
	vtkSP<vtkRenderWindow> renderWindow;
	vtkSP<vtkRenderWindowInteractor> renderWindowInteractor;
	vtkSP<InteractorStyleProject> interactorStyle;
	vtkSP<vtkPlane> clipPlane;

	vtkSP<vtkSliderWidget> changeVisionSliderWidget;
	std::vector<vtkSP<vtkButtonWidget>> buttons;
	vtkSP<vtkTextMapper> fpsTextMapper;
	vtkSP<vtkActor2D> fpsTextActor;

	void UpdateJson();

	vtkSP<vtkDCTF> GetCTF(double minValue, double maxValue);
	inline vtkSP<vtkDCTF> GetCTF(double* range) { return GetCTF(range[0], range[1]); }

	void CreateClipping(vtkSP<vtkPolyData> source);
	void CreateIsolines(vtkSP<vtkPolyData> source);
	void CreateGrid(vtkSP<vtkPolyData> source);
	void CreateOutline(vtkSP<vtkPolyData> source);
	void CreateProportions(vtkSP<vtkActor> actor);

	vtkSP<vtkButtonWidget> CreateButton(double x, double y, std::string buttonName = "");

	void CreateSliders();
	void CreateButtons();
	void CreateFPSCounter();

public:
	Application();

	void AddSettings(std::string path_to_settings);
	void AddObject(std::string path_to_file);
	void AddObject(vtkSP<vtkPolyData> source, bool enableScalars = false);
	void AddQuadraticTriangles(std::vector<vtkSP<vtkUnstructuredGrid>> grids);

	void Start();
	void OffScreenRendering();

	void SaveScreen(std::string filename);

	void UpdateSettings();

	void ShowMainActors(bool flag);
	void ShowMainActorsOn() { ShowMainActors(true); }
	void ShowMainActorsOff() { ShowMainActors(false); }

	void ShowIsolines(bool flag);
	void ShowIsolinesOn() { ShowIsolines(true); }
	void ShowIsolinesOff() { ShowIsolines(false); }

	void ShowGrid(bool flag);
	void ShowGridOn() { ShowGrid(true); }
	void ShowGridOff() { ShowGrid(false); }

	void ShowClipping(bool flag);
	void ShowClippingOn() { ShowClipping(true); }
	void ShowClippingOff() { ShowClipping(false); }

	void ShowOutlines(bool flag);
	void ShowOutlinesOn() { ShowOutlines(true); }
	void ShowOutlinesOff() { ShowOutlines(false); }

	void ShowProportions(bool flag);
	void ShowProportionsOn() { ShowProportions(true); }
	void ShowProportionsOff() { ShowProportions(false); }

	void ShowQuadric(bool flag);
	void ShowQuadricOn() { ShowQuadric(true); }
	void ShowQuadricOff() { ShowQuadric(false); }

	void ChangeProjection(bool mode);
	void ChangeProjectionToParallel() { ChangeProjection(0); }
	void ChangeProjectionToPerspective() { ChangeProjection(1); }

	void HideAll();

	void ChangeVision(unsigned int mode);
};