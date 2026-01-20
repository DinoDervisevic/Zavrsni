import shutil
import zipfile
import os
import sys
from datetime import datetime, timedelta
from collections import defaultdict
import subprocess
import tkinter as tk
from tkinter import filedialog
import ctypes
from datetime import timezone

try:
    ctypes.windll.shcore.SetProcessDpiAwareness(1)
except Exception:
    pass

exe_path = os.path.join(os.path.dirname(__file__), "simulacija_ljiljana.exe")

# Odaberi glavni folder
root_folder = filedialog.askdirectory(title="Odaberi glavni folder s podacima (koji sadrži kodove i metapodaci folder)")
if not root_folder:
    print("Nije odabran folder.")
    exit()

# Pronađi metapodaci folder
metapodaci_folder = os.path.join(root_folder, "metapodaci")
if not os.path.isdir(metapodaci_folder):
    # Pokušaj s velikim slovima
    metapodaci_folder = os.path.join(root_folder, "Metapodaci")
    if not os.path.isdir(metapodaci_folder):
        print(f"Greška: Ne mogu pronaći 'metapodaci' folder u {root_folder}")
        sys.exit(1)

def extract_llsp(file_path, output_dir):
    with zipfile.ZipFile(file_path, 'r') as zip_ref:
        zip_ref.extractall(output_dir)

def extract_sb3(file_dir, file_name, output_dir):
    file_path = os.path.join(file_dir, file_name)
    with zipfile.ZipFile(file_path, 'r') as zip_ref:
        zip_ref.extractall(output_dir)

def clean_dir(path):
    os.makedirs(path, exist_ok=True)
    for name in os.listdir(path):
        full = os.path.join(path, name)
        if os.path.isfile(full) or os.path.islink(full):
            os.remove(full)
        else:
            shutil.rmtree(full)

def get_all_snapshots(snapshots_dir):
    if not os.path.isdir(snapshots_dir):
        return {}
    
    llsp_files = [f for f in os.listdir(snapshots_dir) if f.endswith('.llsp3') or f.endswith('.llsp')]
    timestamp_to_file = {}

    for filename in llsp_files:
        timestamp_str = filename.split(' ')[0]
        try:
            unix_time = float(timestamp_str[:10] + '.' + timestamp_str[10:])
            dt = datetime.fromtimestamp(unix_time)
            timestamp_to_file[dt] = filename
        except ValueError:
            print(f"  Preskačem {filename}, nije validan timestamp.")

    return timestamp_to_file

def parse_log_file(log_path):
    if not os.path.isfile(log_path):
        return {}
    
    with open(log_path, encoding='utf8') as f:
        lines = [line.strip() for line in f if line.strip()]

    task_periods = defaultdict(list)
    current_task = None
    current_start = None

    for i, line in enumerate(lines):
        if "zadatak:" in line:
            parts = line.split()
            timestamp = parts[0]
            task_id = int(parts[2])

            if current_task is not None and current_start is not None:
                task_periods[current_task].append((current_start, timestamp))

            current_task = task_id
            current_start = timestamp

    if current_task is not None and current_start is not None:
        now_str = datetime.now().strftime("%Y-%m-%d-%H-%M-%S,%f")[:-3]
        task_periods[current_task].append((current_start, now_str))
    
    return dict(task_periods)

def evaluate_all_snapshots(task, start, end, all_snapshots, llsp_file_path, output_dir, json_file_path, student_code):
    start_dt = datetime.strptime(start, "%Y-%m-%d-%H-%M-%S,%f")
    end_dt = datetime.strptime(end, "%Y-%m-%d-%H-%M-%S,%f")
    
    snapshots_in_period = [
        (ts, fname) for ts, fname in all_snapshots.items()
        if start_dt <= ts.replace(tzinfo=None) <= end_dt
    ]
    
    maxi_score = -1
    final_file = "Nema dobrog rjesenja"
    
    print(f"    Zadatak {task}: {len(snapshots_in_period)} snapshotova u periodu")
    
    for idx, (ts, fname) in enumerate(sorted(snapshots_in_period)):
        print(f"      [{idx+1}/{len(snapshots_in_period)}] {fname[:50]}...", end=" ", flush=True)
        
        clean_dir(output_dir)
        try:
            extract_sb3(llsp_file_path, fname, output_dir)
            extract_sb3(output_dir, "scratch.sb3", output_dir)
        except Exception as e:
            print(f"GREŠKA ekstrakcija: {e}")
            continue
        
        try:
            result = subprocess.run(
                [exe_path, str(task), json_file_path],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                cwd=os.path.dirname(exe_path),
                env={**os.environ, "PATH": os.path.dirname(exe_path) + ";" + os.environ["PATH"]},
                timeout=30  # TIMEOUT 30 sekundi
            )
        except subprocess.TimeoutExpired:
            print("TIMEOUT!")
            continue
        except Exception as e:
            print(f"GREŠKA subprocess: {e}")
            continue
        
        try:
            score = int(result.stdout.strip())
        except ValueError:
            score = -1
        if score == -1:
            print(f"\n\n{'='*60}")
            print(f"KRITINA GREKA!")
            print(f"Študent folder: {student_code}")
            print(f"Zadatak: {task}")
            print(f"File: {fname}")
            print(f"Score: {score}")
            print(f"STDOUT: {repr(result.stdout)}")
            print(f"STDERR: {repr(result.stderr)}")
            print(f"{'='*60}\n")
            sys.exit(1)
        
        print(f"score={score}")
        
        if score > maxi_score:
            maxi_score = score
            final_file = fname
    
    return maxi_score, final_file

def normalize_code(code):
    """Normaliziraj kod: makni '1.' ili '1' s početka i pretvori u ALLCAPS"""
    normalized = code.strip()
    
    # Makni '1.' s početka ako postoji
    if normalized.startswith("1."):
        normalized = normalized[2:].strip()
    # Makni samo '1' s početka ako postoji (ali ne ako je dio koda npr. "123ABC")
    elif normalized.startswith("1 "):
        normalized = normalized[2:].strip()
    elif normalized == "1":
        normalized = ""
    elif len(normalized) > 1 and normalized[0] == '1' and not normalized[1].isdigit():
        # Makni '1' samo ako sljedeći znak nije broj (da ne maknemo iz "123ABC")
        normalized = normalized[1:].strip()
    
    return normalized.upper()

def find_matching_metapodaci_folder(code, metapodaci_folder):
    """Pronađi folder u metapodaci koji odgovara kodu (case-insensitive, ignoriraj 1. prefix)"""
    code_normalized = normalize_code(code)
    
    for folder_name in os.listdir(metapodaci_folder):
        folder_path = os.path.join(metapodaci_folder, folder_name)
        if os.path.isdir(folder_path):
            folder_normalized = normalize_code(folder_name)
            if folder_normalized == code_normalized:
                return folder_path
    
    return None


def process_single_student(code, student_folder, metapodaci_student_folder):
    """Obradi jednog studenta i vrati rezultate"""
    
    snapshots_dir = os.path.join(student_folder)
    log_path = os.path.join(metapodaci_student_folder, "zadatci.txt")
    output_dir = os.path.join(student_folder, "extraction_folder")
    json_file_path = os.path.normpath(os.path.join(output_dir, "project.json"))
    
    # Provjeri postoje li potrebni folderi/datoteke
    if not os.path.isdir(snapshots_dir):
        return None, "Nema student foldera"
    
    if not os.path.isfile(log_path):
        return None, "Nema zadatci.txt datoteke"
    
    # Dohvati snapshotove i periode
    all_snapshots = get_all_snapshots(snapshots_dir)
    if not all_snapshots:
        return None, "Nema snapshotova (.llsp3)"
    
    task_periods = parse_log_file(log_path)
    if not task_periods:
        return None, "Nema podataka o zadacima"
    
    # Inicijaliziraj rezultate
    results = {}
    for i in range(1, 7):
        results[i] = (-1, "Nema")
    
    # Evaluiraj svaki zadatak
    for task, periods in task_periods.items():
        if task < 1 or task > 6:
            continue
        
        for start, end in periods:
            maxi_score, final_file = evaluate_all_snapshots(
                task, start, end, all_snapshots, 
                snapshots_dir, output_dir, json_file_path,
                student_code=normalize_code(code)
            )
            
            if results[task][0] < maxi_score:
                results[task] = (maxi_score, final_file)
    
    return results, None

def main():
    # Pronađi sve studentske foldere (ignoriraj metapodaci folder)
    student_codes = []
    
    for folder_name in os.listdir(root_folder):
        folder_path = os.path.join(root_folder, folder_name)
        
        # Preskoči ako nije direktorij ili ako je metapodaci folder
        if not os.path.isdir(folder_path):
            continue
        if folder_name.upper() == "METAPODACI":
            continue
        
        student_codes.append(folder_name)
    
    print(f"Pronađeno {len(student_codes)} studentskih foldera.")
    print(f"Metapodaci folder: {metapodaci_folder}")
    print("="*60)
    
    # Rezultati za CSV
    all_results = []
    skipped = []
    
    for i, code in enumerate(sorted(student_codes)):
        print(f"\n[{i+1}/{len(student_codes)}] Obrađujem: {code} (normalizirano: {normalize_code(code)})")
        
        student_folder = os.path.join(root_folder, code)
        metapodaci_student_folder = find_matching_metapodaci_folder(code, metapodaci_folder)
        
        if metapodaci_student_folder is None:
            print(f"  PRESKAČEM: Nema odgovarajućeg metapodaci foldera za {code}")
            skipped.append((code, "Nema metapodaci foldera"))
            continue
        
        results, error = process_single_student(code, student_folder, metapodaci_student_folder)
        
        if error:
            print(f"  PRESKAČEM: {error}")
            skipped.append((code, error))
            continue
        
        # Koristi normalizirani kod za rezultate
        all_results.append((normalize_code(code), results))
        
        # Ispiši rezultate za ovog studenta
        for task_num in range(1, 7):
            score, filename = results[task_num]
            print(f"  Zadatak {task_num}: {score} bodova")
    
    # Zapiši rezultate u CSV
    output_csv_path = os.path.join(root_folder, "rezultati.csv")
    
    with open(output_csv_path, 'w', encoding='utf-8') as f:
        for code, results in all_results:
            f.write(f"{code}\n")
            for task_num in range(1, 7):
                score, filename = results[task_num]
                f.write(f"{task_num}: {score};{filename}\n")
            f.write("//////////////////////////////////////////////////////\n")
            f.write("//////////////////////////////////////////////////////\n")
    
    print("\n" + "="*60)
    print(f"ZAVRŠENO!")
    print(f"Obrađeno: {len(all_results)} studenata")
    print(f"Preskočeno: {len(skipped)} studenata")
    print(f"Rezultati zapisani u: {output_csv_path}")
    
    if skipped:
        print("\nPreskočeni studenti:")
        for code, reason in skipped:
            print(f"  {code}: {reason}")

if __name__ == "__main__":
    main()