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

try:
    ctypes.windll.shcore.SetProcessDpiAwareness(1)
except Exception:
    pass

folder_path = filedialog.askdirectory(title="Odaberi glavni folder s podacima")
if not folder_path:
    print("Nije odabran folder.")
    exit()

snapshots_dir = os.path.join(folder_path, "koraci")
# Putanja do direktorija sa snapshotima
log_path = os.path.join(folder_path, "zadatci.txt")
# Putanja do datoteke sa logovima

if not os.path.isdir(snapshots_dir):
    print(f"Greška: Folder sa snapshotovima ne postoji: {snapshots_dir}")
    sys.exit(1)
if not os.path.isfile(log_path):
    print(f"Greška: Log datoteka ne postoji: {log_path}")
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

def get_all_snapshots():
    llsp_files = [f for f in os.listdir(snapshots_dir) if f.endswith('.llsp')]

    timestamp_to_file = {}

    for filename in llsp_files:
        # Pretpostavljamo da je ime oblika: "TIMESTAMP ostatak.llsp"
        timestamp_str = filename.split(' ')[0]
        try:
            # Ako želiš i datetime objekt:
            unix_time = float(timestamp_str[:10] + '.' + timestamp_str[10:])
            dt = datetime.utcfromtimestamp(unix_time) + timedelta(hours=2)
            
            timestamp_to_file[dt] = filename
        except ValueError:
            print(f"Preskačem {filename}, nije validan timestamp.")

    
    return timestamp_to_file

def parse_log_file():
    with open(log_path, encoding='utf8') as f:
        lines = [line.strip() for line in f if line.strip()]

    # Pronađi zadnji session
    session_indices = [i for i, line in enumerate(lines) if line.startswith("NEW SESSION STARTED")]
    last_session_start = session_indices[-1]
    session_lines = lines[last_session_start+1:]

    task_periods = defaultdict(list)
    current_task = 0
    current_start = None

    for i, line in enumerate(session_lines):
        if "zadatak:" in line:
            # Primjer: 2025-04-03-10-51-43,6 Task: 0 Next  Time_spent_on_previous_task: 00:00:00,004
            parts = line.split()
            timestamp = parts[0]
            task_id = int(parts[2])
            action = parts[3]

            if action in ("Next", "Prev"):
                if current_start is None:
                    current_start = timestamp
                # Zatvori prethodni task period
                if current_task is not None and current_start is not None:
                    task_periods[current_task].append((current_start, timestamp))

                current_task = task_id
                current_start = timestamp

    now_str = datetime.now().strftime("%Y-%m-%d-%H-%M-%S,%f")[:-3]
    task_periods[current_task].append((current_start, now_str))
    return dict(task_periods)

def find_latest_snapshot_in_period(snapshots, start, end):
    # Pretpostavljamo da su snapshots: dict {timestamp (datetime): filename}
    # start, end su stringovi u formatu "YYYY-MM-DD-HH-MM-SS,ms"
    fmt = "%Y-%m-%d-%H-%M-%S,%f"
    start_dt = datetime.strptime(start, fmt)
    end_dt = datetime.strptime(end, fmt)
    # Filtriraj sve snapshotove prije kraja perioda (i opcionalno nakon početka)
    candidates = [(ts, fname) for ts, fname in snapshots.items() if ts <= end_dt]
    if not candidates:
        # Ako nema nijednog, možeš vratiti None ili probati najnoviji prije end_dt
        return None
    # Vrati najnoviji (najveći timestamp)
    return max(candidates, key=lambda x: x[0])


# Putanja do .llsp datoteke
llsp_file_path = os.path.join(folder_path, "koraci")

output_dir = os.path.join(folder_path, "koraci", "extraction_folder")

sb3_path = os.path.join(folder_path, "koraci", "extraction_folder")

# Ekstraktiraj .llsp datoteku

all_snapshots = get_all_snapshots()
# for ts, fname in sorted(all_snapshots.items()):
#     print(ts, fname)

task_periods = parse_log_file()
# for task, periods in task_periods.items():
#     print(f"Task {task}:")
#     for start, end in periods:
#         print(f"  {start} - {end}")
    
results = {}
for i in range(6):
    results["Task " + str(i)] = -1

# Rjesenje je samo zadnji screenshot za neki task
def evaluate_final_snapshot(task, start, end, all_snapshots, llsp_file_path, sb3_path, output_dir):
    # Pronađi najnoviji snapshot u periodu
    final_file = find_latest_snapshot_in_period(all_snapshots, start, end)
    # Simuliraj i provjeri rješenje
    if final_file:
        ts, fname = final_file
        clean_dir(output_dir)
        extract_sb3(llsp_file_path, fname, output_dir)
        extract_sb3(sb3_path, "scratch.sb3", output_dir)
        result = subprocess.run(
            ["simulacija_ljiljana.exe", str(task)],
            capture_output=True, text=True
        )
        is_correct = result.stdout.strip() == "1"
        return is_correct
    return False

# Ako je barem jedan screenshot u periodu tačan, task je riješen
def evaluate_all_snapshots(task, start, end, all_snapshots, llsp_file_path, sb3_path, output_dir):
    # Pronađi sve snapshotove u periodu
    snapshots_in_period = [
        (ts, fname) for ts, fname in all_snapshots.items()
        if datetime.strptime(start, "%Y-%m-%d-%H-%M-%S,%f") <= ts <= datetime.strptime(end, "%Y-%m-%d-%H-%M-%S,%f")
    ]
    maxi_score = -1
    final_file = "Nema dobrog rjesenja"
    # Analiziraj svaki snapshot u periodu
    for ts, fname in sorted(snapshots_in_period):
        clean_dir(output_dir)
        extract_sb3(llsp_file_path, fname, output_dir)
        extract_sb3(sb3_path, "scratch.sb3", output_dir)
        result = subprocess.run(
            ["simulacija_ljiljana.exe", str(task)],
            capture_output=True, text=True
        )
        score = int(result.stdout.strip())
        if score > maxi_score:
            maxi_score = score
            final_file = fname
    return maxi_score, final_file

for task, periods in task_periods.items():
    for start, end in periods:
        maxi_score, final_file = evaluate_all_snapshots(task, start, end, all_snapshots, llsp_file_path, sb3_path, output_dir)
        if results["Task " + str(task)][0] < maxi_score:
            results["Task " + str(task)] = (maxi_score, final_file)

always_true = []

for task, result in results.items():
    print(f"{task}: {result}")

