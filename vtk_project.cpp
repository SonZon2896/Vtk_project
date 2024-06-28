#include "Application.h"
#include <chrono>
#include <thread>

#define PATHTOSETTINGS "C:\\Users\\123va\\source\\repos\\CMakeProject1\\settings.json"
//#define WATCH_RENDER_TIME

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
#ifdef WATCH_RENDER_TIME
    std::chrono::high_resolution_clock timer;

    Application app(PATHTOSETTINGS);
    Application app2(PATHTOSETTINGS);
    
    auto start = timer.now();
    app.Start();
    auto result = timer.now() - start;
    std::cout << "Default Full Rendering time: " << std::chrono::duration_cast<std::chrono::milliseconds>(result).count() << std::endl;
    start = timer.now();

    app2.OffScreenRendering();
    result = timer.now() - start;
    std::cout << "Off Screen Full Rendering time: " << std::chrono::duration_cast<std::chrono::milliseconds>(result).count() << std::endl;

#else
    Application app(PATHTOSETTINGS);
    app.Start();
#endif

    return EXIT_SUCCESS;
}