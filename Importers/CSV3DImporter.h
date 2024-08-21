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

	bool isQuadTriangles = false;

	std::vector<vtkSP<vtkUnstructuredGrid>> CreateQuadTriangles();
public:
	std::string fileName;

	static CSV3DImporter* New();
	vtkTypeMacro(CSV3DImporter, vtkImporter);

	void Update();
	vtkPoints* GetPoints() { return points; }
	auto GetQuadTriangles() { return CreateQuadTriangles(); }
	bool IsQuadTriangles() { return isQuadTriangles; }
	vtkCellArray* GetVerts() { return verts; }
	vtkCellArray* GetLines() { return lines; }
	vtkCellArray* GetPolys() { return polys; }
};