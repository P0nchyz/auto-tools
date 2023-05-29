import pyperclip

def compress_html(file_path):
    output = ""
    with open(file_path, 'r') as file:
        for line in file:
            line = line.replace("\\", "\\\\")
            line = line.replace('"', '\\"')
            line = line.strip()
            output += line
    return output

# Example usage
input_file = "index.html"
output_file = "output.txt"
compressed_data = compress_html(input_file)

with open(output_file, 'w') as file:
    file.write(compressed_data)

# Copy the compressed data to the clipboard
pyperclip.copy(compressed_data)

print("HTML file compressed successfully and saved as output.txt!")
print("The compressed data has been copied to the clipboard.")
