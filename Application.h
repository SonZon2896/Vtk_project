#pragma once

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkNamedColors.h>
#include <vtkColor.h>
#include <vtkButtonRepresentation.h>
#include <vtkButtonWidget.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkCellIterator.h>
#include <vtkCellArrayIterator.h>
#include <vtkTextProperty.h>
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
#include <vtkPlaneSource.h>
#include <vtkCubeSource.h>
#include <vtkStripper.h>
#include <vtkImageWriter.h>
#include <vtkImageData.h>
#include <vtkBMPWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkPNMWriter.h>
#include <vtkPostScriptWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkPNGWriter.h>
#include <vtkWindowToImageFilter.h>
#include <vtkCallbackCommand.h>
#include <vtkContourFilter.h>
#include <vtkRandomSequence.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtknlohmann/json.hpp>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>
#include <vtkLabeledDataMapper.h>
#include <vtkDelaunay3D.h>
#include <vtkExtractEdges.h>
#include <vtkExtractPoints.h>
#include <vtkTubeFilter.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph3DMapper.h>
#include <vtkRibbonFilter.h>
#include <vtkRendererCollection.h>
#include <vtkCollectionIterator.h>
#include <vtkActorCollection.h>
#include <vtkActor2DCollection.h>
#include <vtk3DSImporter.h>
#include <vtkTableToPolyData.h>
#include <vtkDelimitedTextReader.h>
#include <vtkPointInterpolator.h>
#include <vtkSelectVisiblePoints.h>
#include <vtkTexturedButtonRepresentation2D.h>
#include <vtkButtonWidget.h>
#include <vtkStdString.h>
#include <vtkIdFilter.h>
#include <vtkProperty2D.h>
#include <vtkTable.h>
#include <vtkCoordinate.h>
#include <array>
#include <string>
#include <fstream>
#include <functional>
#include "Interactor.h"
#include "CSV3DImporter.h"
#include "UpdateMeshGridCallback.h"
#include "ChangeVisionSliderCallback.h"

#include <chrono>
#include <thread>

#define vtkSP vtkSmartPointer
#define vtkDCTF vtkDiscretizableColorTransferFunction

#define DotRadius 10
#define EdgeRadius 7

using json = nlohmann::json;

class KeyPressInteractorStyle;

class Application {
private:
	json settings;
	std::string pathToSettings;

	std::vector<vtkSP<vtkDiscretizableColorTransferFunction>> ctfs;
	std::vector<vtkSP<vtkActor>> mainActors;
	std::vector<vtkSP<vtkActor>> isolinesActors;
	std::vector<std::pair<vtkSP<vtkActor>, vtkSP<vtkActor>>> gridActors;
	std::vector<vtkSP<vtkActor2D>> labelsActors;

	vtkSP<vtkNamedColors> colors;
	vtkSP<vtkRenderWindow> renderWindow;
	vtkSP<vtkRenderWindowInteractor> renderWindowInteractor;
	vtkSP<vtkSliderWidget> changeVisionSliderWidget;

	void UpdateJson();
	vtkSP<vtkDCTF> GetCTF(double minValue, double maxValue);
	inline vtkSP<vtkDCTF> GetCTF(double* range) { return GetCTF(range[0], range[1]); }

	void CreateIsolines(vtkSP<vtkPolyData> source, vtkSP<vtkRenderer> renderer);
	void CreateGrid(vtkSP<vtkPolyData> source, vtkSP<vtkRenderer> renderer);

	void CreateSlider();
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

	void ShowLabels(bool flag);
	void ShowLabelsOn() { ShowLabels(true); }
	void ShowLabelsOff() { ShowLabels(false); }

	void ChangeProjection(unsigned int mode);
	void ChangeProjectionToParallel() { ChangeProjection(0); }
	void ChangeProjectionToPerspective() { ChangeProjection(1); }

	void ChangeVision(unsigned int mode);
	void ChangeVisionToGradient() { ChangeVision(0); }
	void ChangeVisionToDiscrete() { ChangeVision(1); }
	void ChangeVisionToIsolines() { ChangeVision(2); }
	void ChangeVisionToGrid() { ChangeVision(3); }
};