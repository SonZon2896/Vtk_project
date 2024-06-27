#include "Application.h"

#define PATHTOSETTINGS "C:\\Users\\123va\\source\\repos\\CMakeProject1\\settings.json"

//using json = nlohmann::json;
//
//void FillJson(std::string filename)
//{
//    json data;
//    data["Projection"]["IsEnabled"] = true;
//    data["Projection"]["ParallelScale"] = 10;
//    data["Projection"]["ClippingRange"]["Min"] = -1000;
//    data["Projection"]["ClippingRange"]["Max"] = 1000;
//    std::ofstream output_file(filename);
//    if (!output_file.is_open()) {
//        std::cout << "\n Failed to open output file";
//    }
//    else {
//        output_file << std::setw(2) << data;
//        output_file.close();
//    }
//}
//
//json GetJson(std::string filename)
//{
//    std::ifstream fstream(filename);
//    json data = json::parse(fstream);
//    return data;
//}

int main(int argc, char* argv[])
{
    Application app(PATHTOSETTINGS);

    return EXIT_SUCCESS;
}