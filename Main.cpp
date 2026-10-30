
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <iomanip>
using namespace std;

// Abstract class
class Advisor {
public:
    virtual void giveAdvice() const = 0;
    virtual ~Advisor() {}
};

// Base class
class Crop {
protected:
    string name, season;
    double area, yieldPerAcre;
    static int count;

public:
    // Default constructor
    Crop() : name("Unknown"), season("Unknown"),
             area(1), yieldPerAcre(1) {
        count++;
    }

    // Parameterized constructor
    Crop(string n, double a, string s, double y)
        : name(n), season(s), area(a), yieldPerAcre(y) {
        if (a <= 0 || y <= 0)
            throw invalid_argument("Area and yield must be positive.");
        count++;
    }

    virtual ~Crop() { count--; }

    double totalYield() const {
        return area * yieldPerAcre;
    }

    // Runtime polymorphism
    virtual void display() const {
        cout << fixed << setprecision(2);
        cout << "\nCrop: " << name
             << "\nArea: " << area << " acres"
             << "\nSeason: " << season
             << "\nYield per acre: " << yieldPerAcre
             << "\nTotal yield: " << totalYield() << " units\n";
    }

    // Operator overloading
    bool operator>(const Crop& other) const {
        return totalYield() > other.totalYield();
    }

    // File handling
    virtual void save(ofstream& file) const {
        file << name << '\n' << area << '\n'
             << season << '\n' << yieldPerAcre << '\n';
    }

    static int getCount() {
        return count;
    }
};

int Crop::count = 0;

// Derived class: CerealCrop
class CerealCrop : public Crop, public Advisor {
    string fertilizerType;

public:
    CerealCrop(string n, double a, string s, double y,
               string f)
        : Crop(n, a, s, y), fertilizerType(f) {}

    void display() const override {
        Crop::display();
        cout << "Fertilizer: " << fertilizerType << '\n';
    }

    void giveAdvice() const override {
        cout << "Advice: Use suitable fertilizers and "
             << "monitor soil nutrients regularly.\n";
    }

    void save(ofstream& file) const override {
        file << "Cereal\n";
        Crop::save(file);
        file << fertilizerType << '\n';
    }
};

// Derived class: VegetableCrop
class VegetableCrop : public Crop, public Advisor {
    int irrigationFrequency;

public:
    VegetableCrop(string n, double a, string s, double y,
                  int irrigation)
        : Crop(n, a, s, y),
          irrigationFrequency(irrigation) {
        if (irrigation <= 0)
            throw invalid_argument(
                "Irrigation frequency must be positive.");
    }

    void display() const override {
        Crop::display();
        cout << "Irrigation frequency: "
             << irrigationFrequency << " times per week\n";
    }

    void giveAdvice() const override {
        cout << "Advice: Maintain proper soil moisture "
             << "and avoid overwatering.\n";
    }

    void save(ofstream& file) const override {
        file << "Vegetable\n";
        Crop::save(file);
        file << irrigationFrequency << '\n';
    }
};

int main() {
    try {
        vector<unique_ptr<Crop>> crops;

        // Constructor calls
        crops.push_back(make_unique<CerealCrop>(
            "Wheat", 5, "Winter", 20, "NPK"));

        crops.push_back(make_unique<VegetableCrop>(
            "Tomato", 3, "Summer", 15, 4));

        cout << "===== SMART FARM MANAGEMENT SYSTEM =====\n";

        // Runtime polymorphism
        for (const auto& crop : crops) {
            crop->display();
        }

        cout << "\n===== FARMING ADVICE =====\n";
        for (const auto& crop : crops) {
            const Advisor* advisor =
                dynamic_cast<const Advisor*>(crop.get());
            if (advisor)
                advisor->giveAdvice();
        }

        // Operator overloading
        cout << "\n===== YIELD COMPARISON =====\n";
        if (*crops[0] > *crops[1])
            cout << "Wheat has a higher total yield.\n";
        else
            cout << "Tomato has a higher or equal total yield.\n";

        // Static member
        cout << "\nNumber of crops: "
             << Crop::getCount() << '\n';

        // Save crop data to file
        ofstream outFile("crops.txt");
        if (!outFile)
            throw runtime_error("Cannot open file for writing.");

        for (const auto& crop : crops)
            crop->save(outFile);

        outFile.close();
        cout << "\nCrop data saved to crops.txt\n";

        // Retrieve and display saved data
        ifstream inFile("crops.txt");
        if (!inFile)
            throw runtime_error("Cannot open file for reading.");

        cout << "\n===== SAVED CROP DATA =====\n";
        string line;
        while (getline(inFile, line))
            cout << line << '\n';

        inFile.close();
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
