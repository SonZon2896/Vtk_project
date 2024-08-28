#pragma once

#include "../includes.h"

class CSV3DImporter : public vtkImporter
{
protected:

private:
	CSV3DImporter() = default;
	CSV3DImporter(const CSV3DImporter&) = delete;
	void operator=(const CSV3DImporter&) = delete;

	vtkSP<vtkPoints> points;
	vtkSP<vtkPoints> quadPoints;
	std::vector<vtkSP<vtkIdList>> quadPolys;
	vtkSP<vtkCellArray> verts;
	vtkSP<vtkCellArray> lines;
	vtkSP<vtkCellArray> polys;

	vtkSP<vtkPolyData> polyData;

	vtkSP<vtkDoubleArray> scalars;

	std::vector<std::string> nodesTitles;
	std::vector<std::vector<double>> nodesValues;

	bool isQuadTriangles = false;
	bool isScalars = false;

	void AddNodesTitles(std::string input);
	void AddNodesValues(std::string input);
	int CreatePoints();
	void CreateScalars();
	void CreatePolyData();

	std::vector<vtkSP<vtkUnstructuredGrid>> CreateQuadTriangles();
public:
	std::string fileName;

	static CSV3DImporter* New();
	vtkTypeMacro(CSV3DImporter, vtkImporter);

	void Update();

	auto GetPolyData() { return polyData; }
	auto GetQuadTriangles() { return CreateQuadTriangles(); }
	bool IsQuadTriangles() { return isQuadTriangles; }
	bool IsScalars() { return isScalars; }
};