#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <filesystem>
#include <string>

// Helper function to generate linearly spaced points
std::vector<double> linspace(double start, double end, size_t points) {
    std::vector<double> res(points);
    double step = (end - start) / (points - 1);
    for (size_t i = 0; i < points; ++i) {
        res[i] = start + step * i;
    }
    return res;
}

// Pass strings and vectors by const reference to avoid expensive copying
void write_csv(const std::string& filename, const std::vector<double>& vals) {
    // Note: using std::ios::out to overwrite instead of append, 
    // since we create a new file per timestep.
    std::ofstream myFile(filename, std::ios::out);
    if (!myFile.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }
    
    for (size_t i = 0; i < vals.size(); ++i) {
        myFile << vals[i] << (i == vals.size() - 1 ? "" : ",");
    }
    myFile << "\n";
    myFile.close();
}

int main() {
    // 1. Setup portable relative directories
    std::string workingDir = "./data/";
    std::string inputFileLocation = workingDir + "simulation_Parameters.txt";
    std::string simulationDataLocation = workingDir + "CSV_data";
    std::string simulationImagesLocation = workingDir + "Images";
    std::string intialDataLocation = workingDir + "InitialConditions.csv";

    
    std::filesystem::create_directories(simulationDataLocation);
    std::filesystem::create_directories(simulationImagesLocation);

    // 2. Read simulation parameters
    std::ifstream infile(inputFileLocation);
    if (!infile.is_open()) {
        std::cerr << "Error: Could not find " << inputFileLocation << "\n"
                  << "Please create the file or run the setup script." << std::endl;
        return 1;
    }

    std::string parameterName;
    double parameterValue;
    std::vector<double> simulationParameters(5, 0.0);
    int inputCount = 0;

    while (infile >> parameterName >> parameterValue && inputCount < 5) {
        simulationParameters[inputCount] = parameterValue;
        inputCount++;
    }
    infile.close();

    // 3. Declaration of driving variables
    double maximumLength = simulationParameters[0];
    double waveLength    = simulationParameters[1];
    double waveSpeed     = simulationParameters[2];
    double CFL           = simulationParameters[3];
    double T             = simulationParameters[4];

    std::cout << "\n------- Simulation Parameters -------" << std::endl;
    std::cout << "Domain Length : " << maximumLength << std::endl;
    std::cout << "Wavelength    : " << waveLength << std::endl;
    std::cout << "Wave Speed    : " << waveSpeed << std::endl;
    std::cout << "CFL Condition : " << CFL << std::endl;
    std::cout << "Total Time (T): " << T << std::endl;
    std::cout << "-------------------------------------\n" << std::endl;

    // 4. Derived physical and spatial variables
    double dx = waveLength * 0.01;
    double dt = CFL * dx / waveSpeed;
    double lowerLimit = maximumLength * 0.3;
    double upperLimit = lowerLimit + (waveLength / 2.0);
    double centreOfPulse = (upperLimit + lowerLimit) / 2.0;
    double variancePulse = dx * 5.0;
    
    size_t totalSpatialPoints = static_cast<size_t>(std::floor(maximumLength / dx));

    // Precompute constant multipliers for the hot-loop
    double CFL_sq = CFL * CFL;

    // 5. Initialize Amplitude Vectors
    std::vector<double> x = linspace(0, maximumLength, totalSpatialPoints);
    std::vector<double> uPresent(totalSpatialPoints, 0.0);
    
    std::cout << "Setting Gaussian pulse as initial condition..." << std::endl;
    for (size_t i = 0; i < totalSpatialPoints; ++i) {
        if (x[i] <= upperLimit && x[i] >= lowerLimit) {
            uPresent[i] = std::exp(-std::pow((x[i] - centreOfPulse), 2) / (2 * std::pow(variancePulse, 2)));
        }
    }

    // uPast and uFuture start identical to uPresent
    std::vector<double> uPast = uPresent;
    std::vector<double> uFuture = uPresent;

    write_csv(intialDataLocation, uPresent);

    std::cout << "Initialization complete. Starting simulation...\n" << std::endl;

    // 6. Main Simulation Loop
    int timeStepCounter = 0;
    double t = 0.0;

    while (t < T) {
        t += dt;
        
        std::string fileLocation = simulationDataLocation + "/time_" + std::to_string(timeStepCounter) + ".csv";
        std::cout << "TIME = " << t << " \t Saving: time_" << timeStepCounter << ".csv" << std::endl;

        // O(1) Memory Swapping: 
        // Moves the data pointers instead of copying thousands of array elements.
        std::swap(uPast, uPresent);
        std::swap(uPresent, uFuture); // uFuture now holds old memory ready to be overwritten
        
        write_csv(fileLocation, uPresent);

        // Compute uFuture for interior points
        for (size_t i = 1; i < totalSpatialPoints - 1; ++i) {
            uFuture[i] = 2.0 * uPresent[i] - uPast[i] + 
                         (CFL_sq * (uPresent[i + 1] + uPresent[i - 1] - 2.0 * uPresent[i]));
        } 

        // Apply Periodic Boundary Conditions
        uFuture[0] = 2.0 * uPresent[0] - uPast[0] + 
                     (CFL_sq * (uPresent[1] + uPresent[totalSpatialPoints - 1] - 2.0 * uPresent[0])); 

        uFuture[totalSpatialPoints - 1] = 2.0 * uPresent[totalSpatialPoints - 1] - uPast[totalSpatialPoints - 1] + 
                     (CFL_sq * (uPresent[0] + uPresent[totalSpatialPoints - 2] - 2.0 * uPresent[totalSpatialPoints - 1])); 

        timeStepCounter++;
    }

    std::cout << "\nSimulation Finished Successfully." << std::endl;
    return 0;
}