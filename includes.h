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
#include <vtkCellData.h>
#include <vtkStripper.h>
#include <vtkImageWriter.h>
#include <vtkImageData.h>
#include <vtkBMPWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkPNMWriter.h>
#include <vtkPostScriptWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkIndent.h>
#include <vtkPNGWriter.h>
#include <vtkWindowToImageFilter.h>
#include <vtkCallbackCommand.h>
#include <vtkContourFilter.h>
#include <vtkRandomSequence.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtknlohmann/json.hpp>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
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
#include <vtkVertexGlyphFilter.h>
#include <vtkStdString.h>
#include <vtkIdFilter.h>
#include <vtkProperty2D.h>
#include <vtkTable.h>
#include <vtkCoordinate.h>
#include <vtkClipPolyData.h>
#include <vtkPlane.h>
#include <vtkPointPicker.h>
#include <vtkWorldPointPicker.h>
#include <vtkTextMapper.h>
#include <vtkSliderWidget.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkImplicitPlaneWidget2.h>
#include <vtkImplicitPlaneRepresentation.h>
#include <vtkRenderStepsPass.h>
#include <vtkOutlineGlowPass.h>
#include <vtkArrowSource.h>
#include <vtkCaptionActor2D.h>
#include <vtkTextActor.h>
#include <vtkLinearTransform.h>
#include <vtkTransform.h>

#include <array>
#include <string>
#include <fstream>
#include <functional>
#include <chrono>
#include <thread>
#include <math.h>

#define vtkSP vtkSmartPointer
#define vtkDCTF vtkDiscretizableColorTransferFunction
#define SILVER_COLOR 192. / 255., 192. / 255., 192. / 255.
#define ORANGE_COLOR 255. / 255., 165. / 255., 0. / 255.
# define M_PI 3.1415

using json = nlohmann::json;

class CSV3DImporter;
class Interactor;
class InteractorStyleProject;
class ChangeVisionSliderCallback;
class ChangeClippingButtonCallback;
class ChangeClippingSliderCallback;
class Application;

#include "Application.h"
#include "Importers/CSV3DImporter.h"
#include "Interactors/Interactor.h"
#include "Interactors/InteractorStyleProject.h"
#include "Callbacks/ChangeVisionSliderCallback.h"
#include "Callbacks/ChangeClippingButtonCallback.h"
#include "Callbacks/ChangeClippingPlaneCallback.h"
#include "Actors/ProportionsActor.h"
#include "Actors/ProportionsPieceActor.h"

