import zipfile

def extract_llsp(file_path, output_dir):
    with zipfile.ZipFile(file_path, 'r') as zip_ref:
        zip_ref.extractall(output_dir)

def extract_sb3(file_path, output_dir):
    with zipfile.ZipFile(file_path, 'r') as zip_ref:
        zip_ref.extractall(output_dir)

# Putanja do .llsp datoteke
llsp_file_path = "C:/Users/amrad/OneDrive/Documents/LEGO Education SPIKE/Project 1.llsp"
llsp3_file_path = "C:/Users/amrad/OneDrive/Documents/LEGO Education SPIKE/Project 3.llsp3"


output_dir = 'C:/Users/amrad/OneDrive/Documents/LEGO Education SPIKE'

sb3_path = "C:/Users/amrad/OneDrive/Documents/LEGO Education SPIKE/scratch.sb3"


import zipfile
import json
import os

def compare_llsp_formats(llsp_path, llsp3_path, output_dir):
    """Usporedi project.json iz .llsp i .llsp3 filea"""
    
    # Ekstraktiraj llsp
    with zipfile.ZipFile(llsp_path, 'r') as z:
        z.extractall(os.path.join(output_dir, "llsp_extracted"))
    
    # Ekstraktiraj llsp3
    with zipfile.ZipFile(llsp3_path, 'r') as z:
        z.extractall(os.path.join(output_dir, "llsp3_extracted"))
    
    # Učitaj oba project.json (ako postoje)
    llsp_scratch = os.path.join(output_dir, "llsp_extracted", "scratch.sb3")
    llsp3_scratch = os.path.join(output_dir, "llsp3_extracted", "scratch.sb3")
    
    # Ekstraktiraj scratch.sb3 iz oba
    if os.path.exists(llsp_scratch):
        with zipfile.ZipFile(llsp_scratch, 'r') as z:
            z.extractall(os.path.join(output_dir, "llsp_scratch"))
    
    if os.path.exists(llsp3_scratch):
        with zipfile.ZipFile(llsp3_scratch, 'r') as z:
            z.extractall(os.path.join(output_dir, "llsp3_scratch"))
    
    # Usporedi project.json
    llsp_json_path = os.path.join(output_dir, "llsp_scratch", "project.json")
    llsp3_json_path = os.path.join(output_dir, "llsp3_scratch", "project.json")
    
    if os.path.exists(llsp_json_path) and os.path.exists(llsp3_json_path):
        with open(llsp_json_path, 'r', encoding='utf-8') as f:
            llsp_json = json.load(f)
        with open(llsp3_json_path, 'r', encoding='utf-8') as f:
            llsp3_json = json.load(f)
        
        # Ispiši razlike u strukturi
        print("=== LLSP keys ===")
        print(llsp_json.keys() if isinstance(llsp_json, dict) else type(llsp_json))
        
        print("\n=== LLSP3 keys ===")
        print(llsp3_json.keys() if isinstance(llsp3_json, dict) else type(llsp3_json))
        
        # Usporedi targets
        if "targets" in llsp_json and "targets" in llsp3_json:
            print(f"\nLLSP targets: {len(llsp_json['targets'])}")
            print(f"LLSP3 targets: {len(llsp3_json['targets'])}")
            
            # Usporedi blokove
            if len(llsp_json['targets']) > 1 and len(llsp3_json['targets']) > 1:
                llsp_blocks = llsp_json['targets'][1].get('blocks', {})
                llsp3_blocks = llsp3_json['targets'][1].get('blocks', {})
                
                print(f"\nLLSP blocks: {len(llsp_blocks)}")
                print(f"LLSP3 blocks: {len(llsp3_blocks)}")
                
                # Pronađi razlike
                llsp_keys = set(llsp_blocks.keys())
                llsp3_keys = set(llsp3_blocks.keys())
                
                only_in_llsp = llsp_keys - llsp3_keys
                only_in_llsp3 = llsp3_keys - llsp_keys
                
                if only_in_llsp:
                    print(f"\nSamo u LLSP: {only_in_llsp}")
                if only_in_llsp3:
                    print(f"\nSamo u LLSP3: {only_in_llsp3}")
                
                # Usporedi jedan blok detaljno
                common_keys = llsp_keys & llsp3_keys
                if common_keys:
                    sample_key = list(common_keys)[0]
                    print(f"\n=== Usporedba bloka {sample_key} ===")
                    print(f"LLSP:  {json.dumps(llsp_blocks[sample_key], indent=2)}")
                    print(f"LLSP3: {json.dumps(llsp3_blocks[sample_key], indent=2)}")

# Pozovi ovako:
compare_llsp_formats(llsp_file_path, llsp3_file_path, output_dir)