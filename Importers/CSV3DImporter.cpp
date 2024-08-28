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
				point[i] = std::stod(numStr);
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

void AddQuadTriangle(std::string str, std::vector<vtkSP<vtkIdList>>& cellArray)
{
	char* iter = &str[0];
	while (*iter != ';')
		++iter;
	++iter;

	vtkNew<vtkIdList> points;
	for (; *iter != '\0'; ++iter)
	{
		std::string numStr = "";
		while (*iter != ';')
		{
			if (*iter == '\0')
			{
				points->InsertNextId(std::stoi(numStr));
				cellArray.push_back(points);
				return;
			}
			numStr += *iter;
			++iter;
		}
		points->InsertNextId(std::stoi(numStr));
	}
	cellArray.push_back(points);
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
	nodes_values,
	tetrahedrons,
	triangles,
	plane_triangles,
	quad_nodes,
	quad_triangles,
	face,
	edge
};

void CSV3DImporter::Update()
{
	std::ifstream file(fileName);
	std::string fileString;
	if (file.is_open())
	{
		vtkNew<vtkPoints> quadPointsTemp;
		std::vector<vtkSP<vtkIdList>> quadPolysTemp;
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
				else if (fileString.find("#quad_nodes") != std::string::npos)
				{
					isQuadTriangles = true;
					mode = quad_nodes;
				}
				else if (fileString.find("#quad_triangles") != std::string::npos)
				{
					isQuadTriangles = true;
					mode = quad_triangles;
				}
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

			switch (mode)
			{
			case nodes:
				AddNodesTitles(fileString);
				mode = nodes_values;
				break;
			case nodes_values:
				AddNodesValues(fileString);
				break;
			case tetrahedrons:
				//AddPolygon(fileString, polysTemp);
				break;
			case triangles:
				AddPolygon(fileString, polysTemp);
				break;
			case plane_triangles:
				break;
			case quad_nodes:
				AddPoint(fileString, quadPointsTemp);
				break;
			case quad_triangles:
				AddQuadTriangle(fileString, quadPolysTemp);
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

		quadPoints = quadPointsTemp;
		quadPolys = quadPolysTemp;
		verts = vertsTemp;
		lines = linesTemp;
		polys = polysTemp;
	}
	file.close();

	CreatePoints();
	CreateScalars();
	CreatePolyData();

	std::cout << "Reading end" << std::endl;
}

void CSV3DImporter::AddNodesTitles(std::string input)
{
	std::string temp;
	for (auto ch : input)
	{
		if (ch != ';')
			temp += ch;
		else
		{
			nodesTitles.push_back(temp);
			temp = "";
		}
	}
	if (temp != "")
		nodesTitles.push_back(temp);
	return;
}

void CSV3DImporter::AddNodesValues(std::string input)
{
	std::vector<double> tempVector;
	std::string temp;
	for (auto ch : input)
	{
		if (ch != ';')
			temp += ch;
		else
		{
			tempVector.push_back(std::stod(temp));
			temp = "";
		}
	}
	if (temp != "")
		tempVector.push_back(std::stod(temp));
	nodesValues.push_back(tempVector);
}

int CSV3DImporter::CreatePoints()
{
	int xId = -1;
	int yId = -1;
	int zId = -1;
	for (int i = 0; i < nodesTitles.size(); ++i)
	{
		if (nodesTitles[i] == "x")
			xId = i;
		else if (nodesTitles[i] == "y")
			yId = i;
		else if (nodesTitles[i] == "z")
			zId = i;
	}

	if (xId == -1 || yId == -1 || zId == -1)
	{
		std::cerr << "CSV3DImporter error: coordinates not found" << std::endl;
		return -1;
	}

	points = vtkPoints::New();

	for (auto values : nodesValues)
		points->InsertNextPoint(values[xId], values[yId], values[zId]);

	points->Print(std::cout);

	return 0;
}

void CSV3DImporter::CreateScalars()
{
	int TId = -1;
	for (int i = 0; i < nodesTitles.size(); ++i)
	{
		if (nodesTitles[i] == "T")
			TId = i;
	}
	if (TId == -1)
		return;

	isScalars = true;
	scalars = vtkDoubleArray::New();

	for (auto values : nodesValues)
		scalars->InsertNextTuple1(values[TId]);

	scalars->Print(std::cout);
}

void CSV3DImporter::CreatePolyData()
{
	if (points == nullptr)
		return;

	polyData = vtkPolyData::New();
	polyData->SetPoints(points);
	polyData->SetPolys(polys);
	
	if (IsScalars())
		polyData->GetPointData()->SetScalars(scalars);

	polyData->Print(std::cout);
}

std::vector<vtkSP<vtkUnstructuredGrid>> CSV3DImporter::CreateQuadTriangles()
{
	std::cout << "creating grid for quad triangles" << std::endl;

	std::vector<vtkSP<vtkUnstructuredGrid>> grids;

	for (auto cell : quadPolys)
	{
		vtkNew<vtkQuadraticTriangle> quadTriangle;

		for (int j = 0; j < 6; ++j)
		{
			quadTriangle->GetPointIds()->SetId(j, j);
		}

		quadTriangle->GetPoints()->SetPoint(0, points->GetPoint(cell->GetId(0)));
		quadTriangle->GetPoints()->SetPoint(1, points->GetPoint(cell->GetId(1)));
		quadTriangle->GetPoints()->SetPoint(2, points->GetPoint(cell->GetId(2)));
		quadTriangle->GetPoints()->SetPoint(3, quadPoints->GetPoint(cell->GetId(3)));
		quadTriangle->GetPoints()->SetPoint(4, quadPoints->GetPoint(cell->GetId(4)));
		quadTriangle->GetPoints()->SetPoint(5, quadPoints->GetPoint(cell->GetId(5)));


		vtkNew<vtkUnstructuredGrid> grid;
		grid->SetPoints(quadTriangle->GetPoints());
		grid->InsertNextCell(quadTriangle->GetCellType(), quadTriangle->GetPointIds());

		grids.push_back(grid);
	}

	std::cout << "grid for quad triangles created" << std::endl;

	return grids;
}