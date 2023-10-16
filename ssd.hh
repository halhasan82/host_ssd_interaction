#ifndef SSD_HH
#define SSD_HH

#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;


namespace ssd {

// Define enum for data block types
enum class DataBlockType {
    MainData,
    AttributeData
};

class FlashMemory {
private:
    //std::unordered_map<int, std::pair<DataBlockType, std::string>> dataBlocks;
    //std::unordered_map<int, int> mainToAttributeMap; // Maps main data block address to attribute data block address
    //std::unordered_map<int, int> attributeToMainMap; // Maps attribute data block address to main data block address
    uint64_t sizeInBytes; // Size of the flash memory in bytes
    uint64_t pageSizeBytes; // Size of the flash page in bytes
    double readLatency;
    double writeLatency;
    double deleteLatency;

public:
    std::unordered_map<int, std::pair<DataBlockType, std::string>> dataBlocks;
    std::unordered_map<int, int> mainToAttributeMap; // Maps main data block address to attribute data block address
    std::unordered_map<int, int> attributeToMainMap; // Maps attribute data block address to main data block address
    
    // Constructor to initialize the size of the flash memory in bytes
    FlashMemory(uint64_t sizeBytes, uint64_t pageSizeBytes, double readLatency, double writeLatency, double deleteLatency);

    // Function to write data to flash memory
    void writeDataBlock(int mainAddress, int attrAddress, const std::string& mainData, const std::string& attrData, uint64_t dataSizeBytes, json& jsonData);

    // Function to read main data from flash memory
    std::pair<DataBlockType, std::string> readMainDataBlock(int mainAddress);

    // Function to read attribute data from flash memory
    std::pair<DataBlockType, std::string> readAttributeDataBlock(int attrAddress);

    // Function to delete main data and its corresponding attribute data from flash memory
    void deleteMainDataBlock(int mainAddress);

    // Function to get the current size of the flash memory in bytes
    uint64_t getSizeInBytes() const;

    // Function to get read latency
    double getReadLatency() const;
    
    // Function to get write latency
    double getWriteLatency() const;

    // Function to get delete latency
    double getDeleteLatency() const;
};

class DRAM {
private:
    std::string tempData;
    uint64_t sizeInBytes; // Size of the DRAM in bytes
    double readBandwidth; // Read bandwidth in gigabytes per second
    double writeBandwidth; // Write bandwidth in gigabytes per second
    double deleteBandwidth; // delete bandwidth in gigabytes per second

public:
    // Constructor to initialize the size of the DRAM in bytes
    DRAM(uint64_t sizeBytes, double readBandwidth, double writeBandwidth, double deleteBandwidth);

    // Function to store data temporarily in DRAM
    void storeData(const std::string& data, uint64_t dataSizeBytes);

    // Function to retrieve data from DRAM
    std::string retrieveData( uint64_t dataSizeBytes) const;

    // Function to get read latency
    double getReadLatency() const;
    
    // Function to get write latency
    double getWriteLatency() const;

    // Function to get delete latency
    double getDeleteLatency() const;
};

class Processor {
private:
    int clockSpeed; // Clock speed of the CPU in Hz
    std::vector<int> frequencyHistory; // Record CPU frequencies over time

public:
    // Constructor to initialize the clock speed
    Processor(int speed);

    // Function to get the current CPU frequency
    int getCurrentFrequency() const;

    // Function to execute read command for main data
    double readMainData(FlashMemory& flashMemory, DRAM& dram, int mainAddress, uint64_t dataSizeBytes);

    // Function to execute read command for attribute data
    double readAttributeData(FlashMemory& flashMemory, DRAM& dram, int attrAddress, uint64_t dataSizeBytes);

    // Function to execute write command for main data and attribute data
    double writeData(FlashMemory& flashMemory, DRAM& dram, int mainAddress, int attrAddress, const std::string& mainData, const std::string& attrData, uint64_t dataSizeBytes, json& jsonData);

    // Function to execute delete command for main data and its corresponding attribute data
    double deleteMainData(FlashMemory& flashMemory, DRAM& dram, int mainAddress, uint64_t dataSizeBytes);

    // Function to calculate the change in processor frequency and identify the peak frequency
    void calculateFrequencyChange();
    
    //Function to process data internally with ISDF layer
    void processISDF(const DRAM& dram, std::string& data, uint64_t dataSizeBytes);
};

// Add this code at the end of ssd.h

class PCIeInterface {
private:
    int generation;        // PCIe generation (e.g., Gen3)
    int numLanes;          // Number of PCIe lanes (e.g., 16 lanes)
    double bandwidthPerLane; // Bandwidth per PCIe lane in GT/s (gigatransfers per second)

public:
    // Constructor to initialize PCIe parameters
    PCIeInterface(int gen, int lanes, double bandwidth);

    // Function to transfer data from host's memory to SSD's DRAM via PCIe
    void transferDataToDRAM(DRAM& dram, const std::string& data, uint64_t dataSizeBytes);

    // Function to transfer data from SSD's DRAM to host's memory via PCIe
    double transferDataToHost(const DRAM& dram, std::string& data, uint64_t dataSizeBytes);
};



} // namespace ssd

#endif // SSD_HH

