#include "CSV3DImporter.h"

vtkStandardNewMacro(CSV3DImporter);

void AddPoint(std::string str, vtkPoints *points)
{
	double *point = new double[3];
	char* iter = &str[0];
	
	while (*iter != ';')
		++iter;
	++iter;
	for (int i = 0; i < 3; ++i)
	{
		std::string numStr = "";
		while (*iter != ';')
		{
			if (*iter == '\0')
			{
				points->InsertNextPoint(point);
				return;
			}
			numStr += *iter;
			++iter;
		}
		++iter;
		point[i] = std::stod(numStr);
	}
	points->InsertNextPoint(point);
}

void AddPolygon(std::string str, vtkCellArray *cellArray)
{
	vtkNew<vtkIdList> points;
	for (char* iter = &str[0]; *iter != '\0'; ++iter)
	{
		std::string numStr = "";
		while (*iter != ';')
		{
			if (*iter == '\0')
			{
				points->InsertNextId(std::stoi(numStr));
				cellArray->InsertNextCell(points);
				return;
			}
			numStr += *iter;
			++iter;
		}
		points->InsertNextId(std::stoi(numStr));
	}
	cellArray->InsertNextCell(points);
}

void AddLine(std::string str, int mode_number, vtkCellArray* cellArray)
{
	vtkNew<vtkIdList> points;
	points->InsertNextId(mode_number);
	points->InsertNextId(std::stoi(str));
	cellArray->InsertNextCell(points);
}

enum Mode {
	nodes = 1,
	tetrahedrons,
	triangles,
	plane_triangles,
	face,
	edge
};

void CSV3DImporter::Update()
{
	std::ifstream file(fileName);
	std::string fileString;
	if (file.is_open())
	{
		vtkNew<vtkPoints> pointsTemp;
		vtkNew<vtkCellArray> vertsTemp;
		vtkNew<vtkCellArray> linesTemp;
		vtkNew<vtkCellArray> polysTemp;

		Mode mode;
		int mode_number;
		while (file >> fileString)
		{
			if (fileString[0] == '#')
			{
				if (fileString.find("#nodes") != std::string::npos)
					mode = nodes;
				else if (fileString.find("#tetrahedrons") != std::string::npos)
					mode = tetrahedrons;
				else if (fileString.find("#triangles") != std::string::npos)
					mode = triangles;
				else if (fileString.find("#plane_triangles") != std::string::npos)
					mode = plane_triangles;
				else if (fileString.find("#face") != std::string::npos)
				{
					mode = face;
					mode_number = std::stoi(std::string(&fileString[6]));
				}
				else if (fileString.find("#edge") != std::string::npos)
				{
					mode = edge;
					mode_number = std::stoi(std::string(&fileString[6]));
				}
				continue;
			}
			double *point;
			vtkSmartPointer<vtkIdList> idList;
			switch (mode)
			{
			case nodes:
				if (fileString[0] < '0' || fileString[0] > '9')
					break;
				AddPoint(fileString, pointsTemp);
				break;
			case tetrahedrons:
				//AddPolygon(fileString, polysTemp);
				break;
			case triangles:
				AddPolygon(fileString, polysTemp);
				break;
			case plane_triangles:
				break;
			case face:
				break;
			case edge:
				AddLine(fileString, mode_number, linesTemp);
				break;
			default:
				break;
			}
			if (file.eof())
				break;
		}
		points = pointsTemp;
		verts = vertsTemp;
		lines = linesTemp;
		polys = polysTemp;
	}
	file.close();

	std::cout << "Reading end" << std::endl;
}