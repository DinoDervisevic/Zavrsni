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

folder_path = filedialog.askdirectory(title="Odaberi glavni folder s podacima")
if not folder_path:
    print("Nije odabran folder.")
    exit()

snapshots_dir = os.path.join(folder_path, "koraci")
# Putanja do direktorija sa snapshotima
log_path = os.path.join(folder_path, "zadatci.txt")
# Putanja do datoteke sa logovima

num = 0

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
    llsp_files = [f for f in os.listdir(snapshots_dir) if f.endswith('.llsp3')]

    timestamp_to_file = {}

    for filename in llsp_files:
        # Pretpostavljamo da je ime oblika: "TIMESTAMP ostatak.llsp"
        timestamp_str = filename.split(' ')[0]
        try:
            # Ako želiš i datetime objekt:
            unix_time = float(timestamp_str[:10] + '.' + timestamp_str[10:])
            dt = datetime.fromtimestamp(unix_time)
            
            timestamp_to_file[dt] = filename
        except ValueError:
            print(f"Preskačem {filename}, nije validan timestamp.")

    
    return timestamp_to_file

def parse_log_file():
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
            # action = parts[3]  # Next ili Prev - nije bitno za logiku

            # Ako smo već radili na nekom zadatku, zatvori taj period
            if current_task is not None and current_start is not None:
                task_periods[current_task].append((current_start, timestamp))

            # Započni novi period za novi zadatak
            current_task = task_id
            current_start = timestamp

    # Zatvori zadnji period (do sada)
    if current_task is not None and current_start is not None:
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
    candidates = [(ts, fname) for ts, fname in snapshots.items() if ts.replace(tzinfo=None) <= end_dt]
    if not candidates:
        # Ako nema nijednog, možeš vratiti None ili probati najnoviji prije end_dt
        return None
    # Vrati najnoviji (najveći timestamp)
    return max(candidates, key=lambda x: x[0])


# Putanja do .llsp datoteke
llsp_file_path = os.path.join(folder_path, "koraci")

output_dir = os.path.join(folder_path, "koraci", "extraction_folder")

sb3_path = os.path.join(folder_path, "koraci", "extraction_folder")

json_file_path = os.path.normpath(os.path.join(output_dir, "project.json"))
exe_path = os.path.normpath(exe_path)

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
for i in range(1, 7):
    results["Task " + str(i)] = (-1, "Nema")

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
            [exe_path, str(task), json_file_path],
            capture_output=True, text=True
        )
        is_correct = result.stdout.strip() == "1"
        return is_correct
    return False

# Ako je barem jedan screenshot u periodu tačan, task je riješen
def evaluate_all_snapshots(task, start, end, all_snapshots, llsp_file_path, sb3_path, output_dir):
    # Pronađi sve snapshotove u periodu
    start_dt = datetime.strptime(start, "%Y-%m-%d-%H-%M-%S,%f")
    end_dt = datetime.strptime(end, "%Y-%m-%d-%H-%M-%S,%f")
    snapshots_in_period = [
        (ts, fname) for ts, fname in all_snapshots.items()
        if start_dt <= ts.replace(tzinfo=None) <= end_dt
    ]
    maxi_score = -1
    final_file = "Nema dobrog rjesenja"
    global num
    # Analiziraj svaki snapshot u periodu
    for ts, fname in sorted(snapshots_in_period):
        clean_dir(output_dir)
        extract_sb3(llsp_file_path, fname, output_dir)
        extract_sb3(output_dir, "scratch.sb3", output_dir)
        result = subprocess.run(
            [exe_path, str(task), json_file_path],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            cwd=os.path.dirname(exe_path),
            env={**os.environ, "PATH": os.path.dirname(exe_path) + ";" + os.environ["PATH"]}
        )
        # print("Evaluating Task", task, "Snapshot:", fname)
        # print("STDOUT:", repr(result.stdout))
        # print("STDERR:", repr(result.stderr))
        # print("RETURNCODE:", result.returncode)
        # num += 1
        # print("Evaluated", num, "snapshots so far.")
        # print("-----------------------------------")
        
        score = int(result.stdout.strip())
        if score > maxi_score:
            maxi_score = score
            final_file = fname
    return maxi_score, final_file

def debug_print_task_periods(task_periods, all_snapshots):
    """
    Ispisuje za svaki zadatak:
    - Sve periode kada se radio
    - Sve snapshotove koji pripadaju tom periodu s njihovim vremenima
    """
    print("\n" + "="*80)
    print("DEBUG: Pregled zadataka i snapshotova")
    print("="*80)
    
    for task in sorted(task_periods.keys()):
        periods = task_periods[task]
        print(f"\n{'='*40}")
        print(f"ZADATAK {task}")
        print(f"{'='*40}")
        print(f"Broj perioda: {len(periods)}")
        
        total_snapshots_for_task = 0
        
        for period_idx, (start, end) in enumerate(periods):
            print(f"\n  Period {period_idx + 1}:")
            print(f"    Početak: {start}")
            print(f"    Kraj:    {end}")
            
            # Pronađi snapshotove u ovom periodu
            start_dt = datetime.strptime(start, "%Y-%m-%d-%H-%M-%S,%f")
            end_dt = datetime.strptime(end, "%Y-%m-%d-%H-%M-%S,%f")
            
            snapshots_in_period = [
                (ts, fname) for ts, fname in all_snapshots.items()
                if start_dt <= ts.replace(tzinfo=None) <= end_dt
            ]
            
            print(f"    Broj snapshotova u periodu: {len(snapshots_in_period)}")
            
            if snapshots_in_period:
                print(f"    Snapshotovi:")
                for ts, fname in sorted(snapshots_in_period):
                    # Formatiraj vrijeme čitljivije
                    time_str = ts.strftime("%Y-%m-%d %H:%M:%S")
                    print(f"      [{time_str}] {fname}")
            else:
                print(f"    (Nema snapshotova u ovom periodu)")
            
            total_snapshots_for_task += len(snapshots_in_period)
        
        print(f"\n  UKUPNO snapshotova za zadatak {task}: {total_snapshots_for_task}")
    
    print("\n" + "="*80)
    print(f"UKUPNO snapshotova: {len(all_snapshots)}")
    print("="*80 + "\n")


def debug_print_results(results, all_snapshots):
    """
    Ispisuje rezultate evaluacije s detaljima o najboljim snapshotovima
    """
    print("\n" + "="*80)
    print("DEBUG: Rezultati evaluacije")
    print("="*80)
    
    for task_name, (score, filename) in sorted(results.items()):
        print(f"\n{task_name}:")
        print(f"  Score: {score}")
        print(f"  Najbolji file: {filename}")
        
        # Pronađi timestamp tog filea
        if filename != "Nema" and filename != "Nema dobrog rjesenja":
            for ts, fname in all_snapshots.items():
                if fname == filename:
                    time_str = ts.strftime("%Y-%m-%d %H:%M:%S")
                    print(f"  Vrijeme snapshota: {time_str}")
                    break
    
    print("\n" + "="*80)

debug_print_task_periods(task_periods, all_snapshots)

for task, periods in task_periods.items():
    if task < 1 or task > 6:  # Preskoči fake zadatke
        continue
    for start, end in periods:
        maxi_score, final_file = evaluate_all_snapshots(task, start, end, all_snapshots, llsp_file_path, sb3_path, output_dir)
        if results["Task " + str(task)][0] < maxi_score:
            results["Task " + str(task)] = (maxi_score, final_file)

#always_true = []

for task, result in results.items():
    print(f"{task}: {result}")

def evaluate_all_snapshots_debug(task, start, end, all_snapshots, llsp_file_path, sb3_path, output_dir):
    """
    Verzija s više debug ispisa da vidimo što se točno događa
    """
    start_dt = datetime.strptime(start, "%Y-%m-%d-%H-%M-%S,%f")
    end_dt = datetime.strptime(end, "%Y-%m-%d-%H-%M-%S,%f")
    
    print(f"\n>>> EVALUACIJA ZADATKA {task}")
    print(f"    Period: {start} -> {end}")
    print(f"    start_dt (naive): {start_dt}")
    print(f"    end_dt (naive): {end_dt}")
    
    # Debug: ispiši sve snapshotove i njihove usporedbe
    print(f"\n    Provjera svih snapshotova:")
    for ts, fname in sorted(all_snapshots.items()):
        ts_naive = ts.replace(tzinfo=None)
        in_range = start_dt <= ts_naive <= end_dt
        print(f"      {fname[:40]:40} | ts={ts} | naive={ts_naive} | in_range={in_range}")
    
    snapshots_in_period = [
        (ts, fname) for ts, fname in all_snapshots.items()
        if start_dt <= ts.replace(tzinfo=None) <= end_dt
    ]
    
    print(f"\n    Odabrani snapshotovi za evaluaciju: {len(snapshots_in_period)}")
    for ts, fname in sorted(snapshots_in_period):
        print(f"      {fname}")
    
    maxi_score = -1
    final_file = "Nema dobrog rjesenja"
    
    for ts, fname in sorted(snapshots_in_period):
        clean_dir(output_dir)
        extract_sb3(llsp_file_path, fname, output_dir)
        
        # Debug: provjeri što je ekstraktirano
        print(f"\n    Ekstraktirano iz {fname}:")
        for f in os.listdir(output_dir):
            print(f"      - {f}")
        
        extract_sb3(output_dir, "scratch.sb3", output_dir)
        
        # Debug: provjeri project.json postoji
        if os.path.exists(json_file_path):
            print(f"    project.json POSTOJI")
        else:
            print(f"    project.json NE POSTOJI!")
        
        # Debug: ispiši točnu komandu
        cmd = [exe_path, str(task), json_file_path]
        print(f"    Komanda: {' '.join(cmd)}")
        
        result = subprocess.run(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            cwd=os.path.dirname(exe_path),
        )
        
        print(f"    STDOUT: {repr(result.stdout)}")
        print(f"    STDERR: {repr(result.stderr)}")
        print(f"    RETURN: {result.returncode}")
        
        try:
            score = int(result.stdout.strip())
        except ValueError:
            print(f"    GREŠKA: Ne mogu parsirati score iz '{result.stdout}'")
            score = -1
        
        print(f"    Score: {score}")
        
        if score > maxi_score:
            maxi_score = score
            final_file = fname
    
    print(f"\n>>> REZULTAT ZA ZADATAK {task}: score={maxi_score}, file={final_file}")
    return maxi_score, final_file


# Testiraj samo jedan zadatak detaljno
def test_single_task(task_num):
    """Pozovi ovako: test_single_task(4) za debug zadatka 4"""
    if task_num not in task_periods:
        print(f"Zadatak {task_num} nije u task_periods!")
        print(f"Dostupni zadaci: {list(task_periods.keys())}")
        return
    
    for start, end in task_periods[task_num]:
        evaluate_all_snapshots_debug(task_num, start, end, all_snapshots, llsp_file_path, sb3_path, output_dir)

# Odkomentiraj ovu liniju i pokreni za debug jednog zadatka:
# test_single_task(4)