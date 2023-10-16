import os
import random

# Define the path to the dataset directory
dataset_directory = "/home/hasan/host_ssd_interaction/dataset"

# Function to get the size of a file in bytes
def get_file_size(file_path):
    return os.path.getsize(file_path)
main_data_address=1
attribute_data_address=2
# Open the write_trace.txt and data_addresses.txt files for writing
with open("write_trace.txt", "w") as write_trace_file, open("data_addresses.txt", "w") as data_addresses_file:
    for folder_name in os.listdir(dataset_directory):
        folder_path = os.path.join(dataset_directory, folder_name)

        # Check if the item in the dataset directory is a directory
        if os.path.isdir(folder_path):
            for image_file_name in os.listdir(folder_path):
                
                # Generate random attribute data size between 4870 and 5780 bytes
                attribute_data_size_bytes = random.randint(460800, 1258291)

                # Get the full path of the image file
                image_file_path = os.path.join(folder_path, image_file_name)

                # Get the size of the image file in bytes
                main_data_size_bytes = get_file_size(image_file_path)
                
                # Write the command to write_trace.txt
                write_trace_file.write(f"{main_data_address} {attribute_data_address} {image_file_name} {folder_name} {main_data_size_bytes} {attribute_data_size_bytes}\n")

                
                 # Write the data addresses to the data_addresses.txt file
                data_addresses_file.write(f"{main_data_address} {attribute_data_address} {main_data_size_bytes} {attribute_data_size_bytes}\n")
                
                main_data_address = main_data_address+2  # You can adjust the address range
                attribute_data_address = attribute_data_address + 2  # Assuming a sequential address layout


# Close the files
write_trace_file.close()
data_addresses_file.close()

