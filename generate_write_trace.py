import random

# Number of commands to generate
num_commands = 100

# File names and descriptions
file_names = [f"File{i}.img" for i in range(1, num_commands + 1)]
descriptions = ["cat", "dog", "building", "car"]  # Add more descriptions as needed

# Output files
write_trace_file = open("write_trace.txt", "w")
data_addresses_file = open("data_addresses.txt", "w")

main_data_address=1
attribute_data_address=2

for i in range(num_commands):
   # main_data_address = random.randint(1, 1000)  # You can adjust the address range
   # attribute_data_address = main_data_address + 1  # Assuming a sequential address layout

    main_data_size_bytes = random.randint(1, 10)  # Adjust the size range as needed
    attribute_data_size_bytes = random.randint(1, 10)

    #file_name = random.choice(file_names)
    file_name = f"File{i}.img"
    description = random.choice(descriptions)

    # Write the command to the write_trace.txt file
    write_trace_file.write(f"{main_data_address} {attribute_data_address} {file_name} {description} {main_data_size_bytes} {attribute_data_size_bytes}\n")

    # Write the data addresses to the data_addresses.txt file
    data_addresses_file.write(f"{main_data_address} {attribute_data_address} {main_data_size_bytes} {attribute_data_size_bytes}\n")
    
    main_data_address=main_data_address+2
    attribute_data_address=attribute_data_address+2


# Close the files
write_trace_file.close()
data_addresses_file.close()

