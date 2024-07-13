#pragma once

#include <vtkImporter.h>
#include <vtkIOImportModule.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkObject.h>
#include <vtkObjectFactory.h>
#include <vector>

class CSV3DImporter : public vtkImporter
{
protected:

private:
	CSV3DImporter() = default;
	CSV3DImporter(const CSV3DImporter&) = delete;
	void operator=(const CSV3DImporter&) = delete;

	std::string fileName;

	vtkSmartPointer<vtkPoints> points;
	vtkSmartPointer<vtkCellArray> verts;
	vtkSmartPointer<vtkCellArray> lines;
	vtkSmartPointer<vtkCellArray> polys;

public:
	static CSV3DImporter* New();
	vtkTypeMacro(CSV3DImporter, vtkImporter);

	void SetFileName(std::string fileName) { this->fileName = fileName; }
	std::string GetFileName() { return fileName; }

	void Update();
	vtkPoints* GetPoints() { return points; }
	vtkCellArray* GetVerts() { return verts; }
	vtkCellArray* GetLines() { return lines; }
	vtkCellArray* GetPolys() { return polys; }
};