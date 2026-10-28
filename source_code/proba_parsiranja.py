import os
import zipfile


def extract_archive(file_path, output_dir):
    with zipfile.ZipFile(file_path, "r") as archive:
        archive.extractall(output_dir)


llsp_file_path = (
    r"C:\Users\dinod\Desktop\PARTICIPANTS_EXTRACTED_Dino"
    r"\10112251_201122525\LEGO_workshop\proba\p.llsp3"
)

output_dir = (
    r"C:\Users\dinod\Desktop\PARTICIPANTS_EXTRACTED_Dino"
    r"\10112251_201122525\LEGO_workshop\proba"
)

# Prvo raspakiraj .llsp3
extract_archive(llsp_file_path, output_dir)

# Zatim raspakiraj scratch.sb3 koji je izvučen iz .llsp3
scratch_path = os.path.join(output_dir, "scratch.sb3")
extract_archive(scratch_path, output_dir)