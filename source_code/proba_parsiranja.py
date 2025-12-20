import zipfile

def extract_llsp(file_path, output_dir):
    with zipfile.ZipFile(file_path, 'r') as zip_ref:
        zip_ref.extractall(output_dir)

def extract_sb3(file_path, output_dir):
    with zipfile.ZipFile(file_path, 'r') as zip_ref:
        zip_ref.extractall(output_dir)

# Putanja do .llsp datoteke
llsp_file_path = "C:/Users/amrad/OneDrive/Documents/LEGO Education SPIKE/Project 3.llsp3"

output_dir = 'C:/Users/amrad/OneDrive/Documents/LEGO Education SPIKE'

sb3_path = "C:/Users/amrad/OneDrive/Documents/LEGO Education SPIKE/scratch.sb3"

# Ekstraktiraj .llsp datoteku
extract_sb3(llsp_file_path, output_dir)

extract_sb3(sb3_path, output_dir)
