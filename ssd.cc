#include "ssd.hh"
#include <iostream>
#include <chrono>
#include <thread>

#include <nlohmann/json.hpp>

using json = nlohmann::json;


namespace ssd {

// FlashMemory class implementation

FlashMemory::FlashMemory(uint64_t sizeBytes, uint64_t pageSizeBytes, double readLatency, double writeLatency, double deleteLatency)
    : sizeInBytes(sizeBytes), pageSizeBytes(pageSizeBytes), readLatency(readLatency), writeLatency(writeLatency), deleteLatency(deleteLatency) {
    // Constructor implementation
}


void FlashMemory::writeDataBlock(int mainAddress, int attrAddress, const std::string& mainData, const std::string& attrData, uint64_t dataSizeBytes, json& jsonData) {
    if (getSizeInBytes() + dataSizeBytes * 2 <= sizeInBytes) {
        dataBlocks[mainAddress] = std::make_pair(DataBlockType::MainData, mainData);
        dataBlocks[attrAddress] = std::make_pair(DataBlockType::AttributeData, attrData);

        // Build bidirectional connections
        mainToAttributeMap[mainAddress] = attrAddress;
        attributeToMainMap[attrAddress] = mainAddress;
        
        // Serialize the data and add it to the jsonData object
        jsonData["DataBlocks"][std::to_string(mainAddress)] = {
            {"Type", static_cast<int>(DataBlockType::MainData)},
            {"Data", mainData}
        };

        jsonData["DataBlocks"][std::to_string(attrAddress)] = {
            {"Type", static_cast<int>(DataBlockType::AttributeData)},
            {"Data", attrData}
        };

        jsonData["MainToAttributeMap"][std::to_string(mainAddress)] = attrAddress;
        jsonData["AttributeToMainMap"][std::to_string(attrAddress)] = mainAddress;
        

        std::cout<<std::endl << "Main Data Block written to address " << mainAddress << ": " << mainData << std::endl;
        std::cout << "Attribute Data Block written to address " << attrAddress << ": " << attrData << std::endl;

        // Simulate write latency
        std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>((writeLatency/pageSizeBytes/16) * 1e6)));
    } else {
        std::cout << "Flash memory is full. Cannot write data." << std::endl;
    }
}

std::pair<DataBlockType, std::string> FlashMemory::readMainDataBlock(int mainAddress) {
    // Simulate read latency
    std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(readLatency/pageSizeBytes/16 * 1e6)));

    if (dataBlocks.find(mainAddress) != dataBlocks.end()) {
        return dataBlocks[mainAddress];
    } else {
        return std::make_pair(DataBlockType::MainData, "Main Data not found at address " + std::to_string(mainAddress));
    }
}

std::pair<DataBlockType, std::string> FlashMemory::readAttributeDataBlock(int attrAddress) {
    // Simulate read latency
    std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>((readLatency/pageSizeBytes/16) * 1e6)));

    if (dataBlocks.find(attrAddress) != dataBlocks.end()) {
        return dataBlocks[attrAddress];
    } else {
        return std::make_pair(DataBlockType::AttributeData, "Attribute Data not found at address " + std::to_string(attrAddress));
    }
}

void FlashMemory::deleteMainDataBlock(int mainAddress) {
    if (dataBlocks.find(mainAddress) != dataBlocks.end()) {
        // Delete both main data and attribute data
        int attrAddress = mainToAttributeMap[mainAddress];
        dataBlocks.erase(mainAddress);
        dataBlocks.erase(attrAddress);

        // Remove bidirectional connections
        mainToAttributeMap.erase(mainAddress);
        attributeToMainMap.erase(attrAddress);

        std::cout<<std::endl  << "Main Data and Attribute Data deleted from address " << mainAddress << std::endl;

        // Simulate delete latency
        std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(deleteLatency * 1e6)));
    } else {
        std::cout << "Main Data not found at address " << mainAddress << std::endl;
    }
}


uint64_t FlashMemory::getSizeInBytes() const {
    uint64_t currentSize = 0;
    for (const auto& pair : dataBlocks) {
        currentSize += pair.second.second.size();
    }
    return currentSize;
}

double FlashMemory::getReadLatency() const {
    return readLatency;
}

double FlashMemory::getWriteLatency() const {
    return writeLatency;
}

double FlashMemory::getDeleteLatency() const {
    return deleteLatency;
}

// DRAM class implementation

DRAM::DRAM(uint64_t sizeBytes, double readBandwidth, double writeBandwidth, double deleteBandwidthy)
    : sizeInBytes(sizeBytes), readBandwidth(readBandwidth), writeBandwidth(writeBandwidth), deleteBandwidth(deleteBandwidth) {
    // Constructor implementation
}

void DRAM::storeData(const std::string& data, uint64_t dataSizeBytes) {

    // Calculate write latency based on read bandwidth
    double writeLatency = static_cast<double>(dataSizeBytes) / (writeBandwidth*1024*1024*1024); // In seconds. The write bandwidth is converted from GB/s into B/s

    // Simulate write latency
    std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(writeLatency * 1e6)));

    if (data.size() <= sizeInBytes) {
        tempData = data;
    } else {
        std::cout << "DRAM is too small to store this data." << std::endl;
    }
}

std::string DRAM::retrieveData(uint64_t dataSizeBytes) const {

    // Calculate read latency based on read bandwidth
    double readLatency = static_cast<double>(dataSizeBytes) / (readBandwidth*1024*1024*1024); // In seconds. The read bandwidth is converted from GB/s into B/s
    
    // Simulate DRAM read latency
    std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(readLatency * 1e6)));
    std::string retrievedData = tempData;
    std::cout << "Retrieved data from DRAM: " << retrievedData << std::endl;
    return retrievedData;
}

double DRAM::getReadLatency() const {
    return readBandwidth;
}

double DRAM::getWriteLatency() const {
    return writeBandwidth;
}

double DRAM::getDeleteLatency() const {
    return deleteBandwidth;
}

// Processor class implementation

Processor::Processor(int speed) : clockSpeed(speed) {
    // Constructor implementation
}

int Processor::getCurrentFrequency() const {
    return clockSpeed;
}

double Processor::readMainData(FlashMemory& flashMemory, DRAM& dram, int mainAddress, uint64_t dataSizeBytes) {


    // Calculate delay based on clock speed
    double delay = static_cast<double>(dataSizeBytes) / clockSpeed; // In seconds
    //std::cout<< std::endl << "processor time to read " << delay<< std::endl;

    // Simulate CPU processing time
    auto start = std::chrono::high_resolution_clock::now();
  //  std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(delay * 1e6)));


    // Record CPU frequency at this point
    frequencyHistory.push_back(clockSpeed);

    auto data = flashMemory.readMainDataBlock(mainAddress);
    std::string dataDescription = "Main Data Block";
    if (data.first == DataBlockType::AttributeData) {
        dataDescription = "Attribute Data Block";
    }
    
    dram.storeData(data.second,dataSizeBytes);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    std::cout<< std::endl << "Read data from Main Data Block address " << mainAddress << " (" << dataDescription << "): " << data.second << std::endl;
    std::cout << "Latency for Read Main Data: " << elapsed.count() << " seconds" << std::endl;
    return elapsed.count();
}

double Processor::readAttributeData(FlashMemory& flashMemory, DRAM& dram, int attrAddress, uint64_t dataSizeBytes) {

    // Calculate delay based on clock speed
    double delay = static_cast<double>(dataSizeBytes) / clockSpeed; // In seconds

    // Simulate CPU processing time
    auto start = std::chrono::high_resolution_clock::now();
   // std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(delay * 1e6)));
   // auto end = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> elapsed = end - start;

    // Record CPU frequency at this point
    frequencyHistory.push_back(clockSpeed);

    auto data = flashMemory.readAttributeDataBlock(attrAddress);
    std::string dataDescription = "Attribute Data Block";
    if (data.first == DataBlockType::MainData) {
        dataDescription = "Main Data Block";
    }
    dram.storeData(data.second,dataSizeBytes);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    std::cout<< std::endl << "Read data from Attribute Data Block address " << attrAddress << " (" << dataDescription << "): " << data.second << std::endl;
    std::cout << "Latency for Read Attribute Data: " << elapsed.count() << " seconds" << std::endl;
    return elapsed.count();
}

double Processor::writeData(FlashMemory& flashMemory, DRAM& dram, int mainAddress, int attrAddress, const std::string& mainData, const std::string& attrData, uint64_t dataSizeBytes, json& jsonData) {
    // Calculate delay based on clock speed
    double delay = static_cast<double>(dataSizeBytes) / clockSpeed; // In seconds

    // Simulate CPU processing time
    auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(delay * 1e6)));
  //  auto end = std::chrono::high_resolution_clock::now();
  //  std::chrono::duration<double> elapsed = end - start;

    // Record CPU frequency at this point
    frequencyHistory.push_back(clockSpeed);
    
    // Store the data in DRAM
    dram.storeData((mainData + attrData),dataSizeBytes);

    flashMemory.writeDataBlock(mainAddress, attrAddress, mainData, attrData, dataSizeBytes, jsonData);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    std::cout<< "Latency for Write Data: " << elapsed.count() << " seconds" << std::endl;
    return elapsed.count();
}

double Processor::deleteMainData(FlashMemory& flashMemory, DRAM& dram, int mainAddress, uint64_t dataSizeBytes) {
    // Calculate delay based on clock speed
    double delay = static_cast<double>(dataSizeBytes) / clockSpeed; // In seconds

    // Simulate CPU processing time
    auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(delay * 1e6)));
    //auto end = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> elapsed = end - start;

    // Record CPU frequency at this point
    frequencyHistory.push_back(clockSpeed);

    flashMemory.deleteMainDataBlock(mainAddress);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Latency for Delete Main Data: " << elapsed.count() << " seconds" << std::endl;
    return elapsed.count();
}

void Processor::calculateFrequencyChange() {
    int peakFrequency = 0;
    int initialFrequency = frequencyHistory[0];
    double maxChange = 0;

    std::cout << "Initial CPU Frequency: " << initialFrequency << " Hz" << std::endl;

    peakFrequency = initialFrequency;

    for (int i = 1; i < frequencyHistory.size(); ++i) {
        int currentFrequency = frequencyHistory[i];
        int frequencyChange = currentFrequency - initialFrequency;

        if (std::abs(frequencyChange) > maxChange) {
            maxChange = std::abs(frequencyChange);
            peakFrequency = currentFrequency;
        }

        std::cout << "At time " << i << ", CPU Frequency: " << currentFrequency << " Hz" << std::endl;
    }

    std::cout << "Peak CPU Frequency: " << peakFrequency << " Hz" << std::endl;
}

void Processor::processISDF(const DRAM& dram, std::string& data, uint64_t dataSizeBytes){

    // retrieve data from DRAM for processing
    data = dram.retrieveData(dataSizeBytes);

}

// PCIeInterface class implementation

PCIeInterface::PCIeInterface(int gen, int lanes, double bandwidth)
    : generation(gen), numLanes(lanes), bandwidthPerLane(bandwidth) {
    // Constructor implementation
}

void PCIeInterface::transferDataToDRAM(DRAM& dram, const std::string& data, uint64_t dataSizeBytes) {
    // Calculate transfer time based on PCIe bandwidth
    //double transferTime = static_cast<double>(dataSizeBytes) / (numLanes * bandwidthPerLane*1024*1024*1024); // In seconds. The bandwidth is converted from gigabyte/s to byte/s
    double transferTime = static_cast<double>(dataSizeBytes) / (numLanes* bandwidthPerLane*1024);//;*1024*1024); // In seconds. The bandwidth is converted from gigabyte/s to byte/s
    

    // Simulate data transfer from host's memory to SSD's DRAM via PCIe
    dram.storeData(data, dataSizeBytes);

    // Simulate PCIe transfer time
  //  std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(transferTime * 1e6)));
}

double PCIeInterface::transferDataToHost(const DRAM& dram, std::string& data, uint64_t dataSizeBytes) {
    // Calculate transfer time based on PCIe bandwidth
    //double transferTime = static_cast<double>(dataSizeBytes) / (numLanes * bandwidthPerLane*1024*1024*1024); // In seconds. The bandwidth is converted from gigabyte/s to byte/s
        double transferTime = static_cast<double>(dataSizeBytes) / (numLanes *bandwidthPerLane*1024*124);//*1024); // In seconds. The bandwidth is converted from gigabyte/s to byte/s

    // Simulate data transfer from SSD's DRAM to host's memory via PCIe
    data = dram.retrieveData(dataSizeBytes);

    // Simulate PCIe transfer time
    auto start = std::chrono::high_resolution_clock::now();
    
    std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(transferTime * 1e6)));
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    std::cout << "Data Transfer Time: " << std::fixed<<elapsed.count()<< " seconds" << std::endl;
    
    return elapsed.count();
 
}

} // namespace ssd

