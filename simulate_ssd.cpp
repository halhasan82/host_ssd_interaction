#include "ssd.hh"
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Function to execute write commands from a trace file
int executeWriteCommands(ssd::FlashMemory& flash, ssd::DRAM& dram, ssd::Processor& processor, ssd::PCIeInterface& pcieInterface) {
    // Read and execute write commands from the trace file
    std::ifstream traceFile("write_trace.txt");
    if (!traceFile.is_open()) {
        std::cerr << "Failed to open write trace file." << std::endl;
        return 1;
    }
    
    // Create a JSON object to poplate data
    json jsonData;
    
    // Specify the file path where you want to save the JSON data
    std::string filePath = "serialized_data.json";
    
    // Open a file stream for writing
    std::ofstream outputFile(filePath);

    std::string line;
    while (std::getline(traceFile, line)) {
        int mainDataAddress, attributeDataAddress;
        std::string mainDataToTransfer, attributeDataToTransfer;
        uint64_t mainDataSizeBytes, attributeDataSizeBytes;

        // Parse the command line from the trace file
        std::istringstream iss(line);
        iss >> mainDataAddress >> attributeDataAddress >> mainDataToTransfer >> attributeDataToTransfer >> mainDataSizeBytes >> attributeDataSizeBytes;
        
        pcieInterface.transferDataToDRAM (dram, mainDataToTransfer, mainDataSizeBytes);

        // Execute the write command
        double writeLatency = processor.writeData(flash, dram, mainDataAddress, attributeDataAddress, mainDataToTransfer, attributeDataToTransfer, mainDataSizeBytes, jsonData);
        



        // Output the write latency for this command
        std::cout << "Write Latency for Command: " << writeLatency << " seconds" << std::endl;
    }
    
    
            // Serialize the jsonData object to a formatted JSON string
        std::string jsonString = jsonData.dump(4);
        
        // Check if the file stream is open
        if (outputFile.is_open()) { // Write the JSON data to the file
        
    	outputFile << jsonString;// jsonData.dump(4); // Use dump(4) for pretty formatting with 4 spaces as an example
   
   	 
    	std::cout << "JSON data has been successfully written to " << filePath << std::endl;
	} else {
 	   std::cerr << "Unable to open the file for writing." << std::endl;
	}
    
    	outputFile.close();
    
    return 0;
}


// Function to execute read commands for attribute data
int executeReadAttributeCommands(ssd::FlashMemory& flash, ssd::DRAM& dram, ssd::Processor& processor, ssd::PCIeInterface& pcieInterface) {

// Open the data_addresses.txt file
     	std::ifstream dataAddressesFile("data_addresses.txt");
     	
     	if (!dataAddressesFile.is_open()) {
      	std::cerr << "Failed to open data_addresses.txt file." << std::endl;
     	 return 1;
     	 }

	// Variables to store data from data_addresses.txt
	uint64_t mainDataAddress, attributeDataAddress, mainDataSizeBytes, attributeDataSizeBytes;

	// Read mainDataAddress, attributeDataAddress, mainDataSizeBytes, and attributeDataSizeBytes
	while (dataAddressesFile >> mainDataAddress >> attributeDataAddress >> mainDataSizeBytes >> attributeDataSizeBytes) {
 	   // Execute readAttribute command with attributeDataAddress and attributeDataSizeBytes
    	double readAttributeLatency = processor.readAttributeData(flash, dram, attributeDataAddress, attributeDataSizeBytes);
    	
    	// After executing readAttributeData or readMainData, retrieve data and append to a file
	std::ofstream retrievedAttributeDataFile("retrievedAttributeData.txt");//, std::ios::app);


	if (retrievedAttributeDataFile.is_open()) {
 	   // Execute readAttribute command with attributeDataAddress and attributeDataSizeBytes
	    double readAttributeLatency = processor.readAttributeData(flash, dram, attributeDataAddress, attributeDataSizeBytes);

 	   // Use transferDataToHost to retrieve data from DRAM
 	   std::string retrievedData;
  	  pcieInterface.transferDataToHost(dram, retrievedData, attributeDataSizeBytes);

	    // Append the retrieved data to the attributeData.txt file
  	  //retrievedAttributeDataFile << "Attribute Data at Address " << attributeDataAddress << ": " << retrievedData << std::endl;
  	  retrievedAttributeDataFile << attributeDataAddress << " "<<retrievedData << std::endl;

 	   // Close the attributeData.txt file
  	  retrievedAttributeDataFile.close();
	} else {
   	 std::cerr << "Failed to open attributeData.txt file." << std::endl;
  	  return 1;
	}


	}

	// Close the data_addresses.txt file
	dataAddressesFile.close();
	
	return 0;

}


void executeDataQuery(ssd::FlashMemory& flash, ssd::DRAM& dram, ssd::Processor& processor, ssd::PCIeInterface& pcieInterface) {
    // Open the "retrievedAttributeData.txt" file
    std::ifstream retrievedAttributeDataFile("retrievedAttributeData.txt");
    if (!retrievedAttributeDataFile.is_open()) {
        std::cerr << "Failed to open retrievedAttributeData.txt file." << std::endl;
        return;
    }

    // Prompt the user for the input parameter to filter against
    std::cout << "Enter the value to filter against: ";
    std::string userInput;
    std::cin >> userInput;

    // Open the "data_addresses.txt" file for searching main data addresses
    std::ifstream dataAddressesFile("data_addresses.txt");
    if (!dataAddressesFile.is_open()) {
        std::cerr << "Failed to open data_addresses.txt file." << std::endl;
        return;
    }

    // Open the "queryResults.txt" file for storing query results
    std::ofstream queryResultsFile("queryResults.txt");
    if (!queryResultsFile.is_open()) {
        std::cerr << "Failed to open queryResults.txt file." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(retrievedAttributeDataFile, line)) {
        std::istringstream iss(line);
        uint64_t attributeDataAddress;
        std::string retrievedData;
        iss >> attributeDataAddress >> retrievedData;

        // Check if the retrieved data matches the user input parameter
        if (retrievedData == userInput) {
            // Search for the attribute data address in "data_addresses.txt"
            std::string dataAddressesLine;
            while (std::getline(dataAddressesFile, dataAddressesLine)) {
                std::istringstream dataIss(dataAddressesLine);
                uint64_t mainDataAddress, attrDataAddress, mainDataSizeBytes, attrDataSizeBytes;
                dataIss >> mainDataAddress >> attrDataAddress >> mainDataSizeBytes >> attrDataSizeBytes;

                // If attribute data address is found, read the corresponding main data
                if (attrDataAddress == attributeDataAddress) {
                    double readLatency = processor.readMainData(flash, dram, mainDataAddress, mainDataSizeBytes);
                    std::string mainData;

                    // Use transferDataToHost to retrieve data from DRAM
                    pcieInterface.transferDataToHost(dram, mainData, mainDataSizeBytes);

                    // Write the main data to "queryResults.txt"
                   // queryResultsFile << "Main Data at Address " << mainDataAddress << ": " << mainData << std::endl;
                   // queryResultsFile << "Latency for Read Main Data: " << readLatency << " seconds" << std::endl;
                   queryResultsFile << mainData << std::endl;

                    // Exit the inner loop since we found a match
                    break;
                }
            }
        }
    }

    // Close the files
    retrievedAttributeDataFile.close();
    dataAddressesFile.close();
    queryResultsFile.close();
}

//function to capture query results to be ready for use
void captureQueryResults (const std::string& mainData){
 
 
 // Your Bash script file path
    const char* scriptPath = "./capture_query_results.sh";

    // Image filename to be passed as an argument
    std::string imageFilename = mainData; // Replace with the actual filename

    // Create a command string to execute the script with the argument
    std::string command = scriptPath;
    command += " ";
    command += imageFilename;

    // Run the Bash script using the system function with the command
    int result = std::system(command.c_str());

    // Check the result of running the script
    if (result == 0) {
        std::cout << "Script executed successfully." << std::endl;
    } else {
        std::cerr << "Script execution failed." << std::endl;
    }
 
 }

// Function to execute read commands for attribute data
double readAttributesAndExecuteQuery(ssd::FlashMemory& flash, ssd::DRAM& dram, ssd::Processor& processor, ssd::PCIeInterface& pcieInterface) {


    //query latency
   double TotalReadLatency=0;
   double dataTransferTime=0;

    // Prompt the user for the input parameter to filter against
    std::cout << "Enter the value to filter against: ";
    std::string userInput;
    std::cin >> userInput;
    
        //clear query results directory
        system("./clean_query_directory.sh");
        //std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>((50) * 1e6)));
        // Open the "queryResults.txt" file for storing query results
        std::ofstream queryResultsFile("queryResults.txt");
        if (!queryResultsFile.is_open()) {
        std::cerr << "Failed to open queryResults.txt file." << std::endl;
        return 1;
        }

     // Open the data_addresses.txt file
     	std::ifstream dataAddressesFile("data_addresses.txt");
     	
     	if (!dataAddressesFile.is_open()) {
      	std::cerr << "Failed to open data_addresses.txt file." << std::endl;
     	 return 1;
     	 }

	// Variables to store data from data_addresses.txt
	uint64_t mainDataAddress, attributeDataAddress, mainDataSizeBytes, attributeDataSizeBytes;
	
	// After executing readAttributeData or readMainData, retrieve data and append to a file
	std::ofstream retrievedAttributeDataFile("retrievedAttributeData.txt");//, std::ios::app);

	if (retrievedAttributeDataFile.is_open()) {

	// Read mainDataAddress, attributeDataAddress, mainDataSizeBytes, and attributeDataSizeBytes
	while (dataAddressesFile >> mainDataAddress >> attributeDataAddress >> mainDataSizeBytes >> attributeDataSizeBytes) {
 	   // Execute readAttribute command with attributeDataAddress and attributeDataSizeBytes
    	//double readAttributeLatency = processor.readAttributeData(flash, dram, attributeDataAddress, attributeDataSizeBytes);
    	

 	   // Execute readAttribute command with attributeDataAddress and attributeDataSizeBytes
	    double readAttributeLatency = processor.readAttributeData(flash, dram, attributeDataAddress, attributeDataSizeBytes);
	    
	    TotalReadLatency+=readAttributeLatency;

            std::string retrievedData;
 	   // Use transferDataToHost to retrieve data from DRAM
  	  dataTransferTime+=pcieInterface.transferDataToHost(dram, retrievedData, attributeDataSizeBytes);
  	  
  	   // Use processISDF to proccsses data with internal cpu
  	 // processor.processISDF(dram, retrievedData, attributeDataSizeBytes);

	    // Append the retrieved data to the attributeData.txt file
  	  //retrievedAttributeDataFile << "Attribute Data at Address " << attributeDataAddress << ": " << retrievedData << std::endl;
  	  retrievedAttributeDataFile << attributeDataAddress << " "<<retrievedData << std::endl;

  	  
  	  //here we place code for query filtering
  	  
  	  // Check if the retrieved data matches the user input parameter
        if (retrievedData == userInput) {

	double readMainDataLatency = processor.readMainData(flash, dram, mainDataAddress, mainDataSizeBytes);
	
	TotalReadLatency+=readMainDataLatency;
	
	std::string mainData;
	// Use transferDataToHost to retrieve data from DRAM
	dataTransferTime+=pcieInterface.transferDataToHost(dram, mainData, mainDataSizeBytes);
                    
  
        //write query results into a logging file
        queryResultsFile << mainData << std::endl;

        //run a bash which outputs query results as ready to use by end users
        captureQueryResults(mainData);
        
           
        // Close the queryResults.txt file
  	//queryResultsFile.close();
        
        }
}

	} else {
   	 std::cerr << "Failed to open attributeData.txt file." << std::endl;
  	  return 1;
	}


	// Close the data_addresses.txt file
	dataAddressesFile.close();
	// Close the queryResults.txt file
  	queryResultsFile.close();
  	// Close the attributeData.txt file
  	retrievedAttributeDataFile.close();
	
	return dataTransferTime;
}

// Function to load serialized data from a JSON file into data structures
void loadSerializedDataFromFile(const std::string& filename, ssd::FlashMemory& flashMemory) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }

        json jsonData;
        file >> jsonData;
        file.close();

        // Update dataBlocks
        for (const auto& item : jsonData["DataBlocks"].items()) {
            int address = std::stoi(item.key());
            int type = item.value()["Type"];
            std::string data = item.value()["Data"];
            ssd::DataBlockType dataType = static_cast<ssd::DataBlockType>(type);
            flashMemory.dataBlocks[address] = std::make_pair(dataType, data);
        }

        // Update mainToAttributeMap
        for (const auto& item : jsonData["MainToAttributeMap"].items()) {
            int mainAddress = std::stoi(item.key());
            int attrAddress = item.value();
            flashMemory.mainToAttributeMap[mainAddress] = attrAddress;
        }

        // Update attributeToMainMap
        for (const auto& item : jsonData["AttributeToMainMap"].items()) {
            int attrAddress = std::stoi(item.key());
            int mainAddress = item.value();
            flashMemory.attributeToMainMap[attrAddress] = mainAddress;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error loading serialized data: " << e.what() << std::endl;
    }
}




int main() {
    // Read the configuration file
    std::ifstream configStream("config.json");
    if (!configStream.is_open()) {
        std::cerr << "Failed to open configuration file." << std::endl;
        return 1;
    }

    json config;
    configStream >> config;

    // Extract parameters for FlashMemory and DRAM
    uint64_t flashSizeBytes = config["FlashMemory"]["sizeBytes"];
    uint64_t flashPageSizeBytes = config["FlashMemory"]["pageSizeBytes"];
    double flashReadLatency = config["FlashMemory"]["readLatency"];
    double flashWriteLatency = config["FlashMemory"]["writeLatency"];
    double flashDeleteLatency = config["FlashMemory"]["deleteLatency"];

    uint64_t dramSizeBytes = config["DRAM"]["sizeBytes"];
    double dramReadBandwidth = config["DRAM"]["readBandwidth"];
    double dramWriteBandwidth = config["DRAM"]["writeBandwidth"];
    double dramDeleteBandwidth = config["DRAM"]["deleteBandwidth"];
  
    int pcieGeneration = config["PCIeInterface"]["generation"];
    int pcieLanes = config["PCIeInterface"]["lanes"];
    double pcieBandwidth = config["PCIeInterface"]["bandwidth"];

    // Initialize flash memory with parameters
    ssd::FlashMemory flash(flashSizeBytes,flashPageSizeBytes, flashReadLatency, flashWriteLatency, flashDeleteLatency);

    // Initialize DRAM with 
    ssd:: DRAM dram(dramSizeBytes, dramReadBandwidth, dramWriteBandwidth, dramDeleteBandwidth); 

    // Initialize processor with a clock speed of 1 MHz
    ssd::Processor processor(1000000);
    
    // Initialize PCIe interface with parameters
    ssd::PCIeInterface pcieInterface(pcieGeneration, pcieLanes, pcieBandwidth);
    
    
    // Specify the JSON file containing the serialized data
    std::string jsonFilename = "serialized_data.json";
    
    
    // Load the serialized data from the JSON file into the data structures
    loadSerializedDataFromFile(jsonFilename, flash);

   
    // Execute write commands from a trace file
    int writeResult= executeWriteCommands(flash, dram, processor,pcieInterface);
    
    // Simulate query time
    auto start = std::chrono::high_resolution_clock::now();


    
     //reading attribute data
    //int readResult= executeReadAttributeCommands(flash, dram, processor,pcieInterface);
     
     //execute data query
     //executeDataQuery(flash, dram, processor,pcieInterface);
     
   /*  double TotalTransferTime= readAttributesAndExecuteQuery(flash, dram, processor,pcieInterface);
     
     auto end = std::chrono::high_resolution_clock::now();
     std::chrono::duration<double> queryResponseTime = end - start; 
     
    // std::cout <<std::endl<< "The total read latency:" <<TotalReadLatency<< std::endl;
     std::cout <<std::endl<< "The data transfer time:" <<TotalTransferTime<< std::endl;
     
     std::cout <<std::endl<< "Query response time:" <<queryResponseTime.count()<< std::endl;*/
    
     
	
	
    
    
   // Calculate and display frequency change
   // processor.calculateFrequencyChange();
    return 0;
}

