#pragma once

#include "includes.h"

class Application {
private:
	json settings;
	std::string pathToSettings;

	std::vector<vtkSP<vtkDiscretizableColorTransferFunction>> ctfs;
	std::vector<vtkSP<vtkActor>> mainActors;
	std::vector<vtkSP<vtkActor>> clippingActors;
	std::vector<vtkSP<vtkActor>> isolinesActors;
	std::vector<std::pair<vtkSP<vtkActor>, vtkSP<vtkActor>>> gridActors;

	vtkSP<vtkNamedColors> colors;
	vtkSP<vtkRenderer> renderer;
	vtkSP<vtkRenderWindow> renderWindow;
	vtkSP<vtkRenderWindowInteractor> renderWindowInteractor;
	vtkSP<InteractorStyleProject> interactorStyle;
	vtkSP<vtkPlane> clipPlane;

	vtkSP<vtkSliderWidget> changeVisionSliderWidget;
	vtkSP<vtkSliderWidget> changeClippingSliderWidget;
	vtkSP<vtkButtonWidget> changeClippingButton;
	vtkSP<vtkTextMapper> fpsTextMapper;
	vtkSP<vtkActor2D> fpsTextActor;

	void UpdateJson();

	vtkSP<vtkDCTF> GetCTF(double minValue, double maxValue);
	inline vtkSP<vtkDCTF> GetCTF(double* range) { return GetCTF(range[0], range[1]); }

	void CreateClipping(vtkSP<vtkPolyData> source);
	void CreateIsolines(vtkSP<vtkPolyData> source);
	void CreateGrid(vtkSP<vtkPolyData> source);

	void CreateSliders();
	void CreateButtons();
	void CreateFPSCounter();

public:
	Application();

	void AddSettings(std::string path_to_settings);
	void AddObject(std::string path_to_file, bool enableIsolines = false, bool enableGrid = false);
	void AddObject(vtkSP<vtkPolyData> source, bool enableIsolines = false, bool enableGrid = false);

	void Start();
	void OffScreenRendering();

	void SaveScreen(std::string filename);

	void UpdateSettings();

	void ShowIsolines(bool flag);
	void ShowIsolinesOn() { ShowIsolines(true); }
	void ShowIsolinesOff() { ShowIsolines(false); }

	void ShowGrid(bool flag);
	void ShowGridOn() { ShowGrid(true); }
	void ShowGridOff() { ShowGrid(false); }

	void ChangeProjection(unsigned int mode);
	void ChangeProjectionToParallel() { ChangeProjection(0); }
	void ChangeProjectionToPerspective() { ChangeProjection(1); }

	void ChangeVision(unsigned int mode);
	void ChangeVisionToGradient() { ChangeVision(0); }
	void ChangeVisionToDiscrete() { ChangeVision(1); }
	void ChangeVisionToIsolines() { ChangeVision(2); }
	void ChangeVisionToGrid() { ChangeVision(3); }
};