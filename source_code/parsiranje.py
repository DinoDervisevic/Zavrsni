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
import csv

try:
    ctypes.windll.shcore.SetProcessDpiAwareness(1)
except Exception:
    pass


FLAG_NAMES = {
    1: [
        "Robot se pokrenuo",
        "Dosegnuta ciljna udaljenost",
    ],
    2: [
        "Robot se pokrenuo",
        "Dosegnuta ciljna udaljenost",
    ],
    3: [
        "Robot se pomaknuo",
        "Pokret nakon pritiska senzora",
        "Dosegnut rozi spremnik",
        "Povratak u bazu",
        "Ostao u bazi",
    ],
    4: [
        "Košarica otvorena",
        "Košarica zatvorena",
    ],
    6: [
        "Dosegnut plavi spremnik",
        "Košarica otvorena",
        "Košarica zatvorena",
        "Povratak u bazu",
    ],
    7: [
        "Dosegnut rozi spremnik",
        "Košarica otvorena",
        "Košarica zatvorena",
        "Povratak u bazu",
    ],
    11: [
        "Pokret nakon boje",
        "Dosegnut žuti spremnik",
        "Košarica otvorena",
        "Košarica zatvorena",
        "Povratak u bazu",
    ],
    12: [
        "Pokret nakon boje",
        "Dosegnut crni spremnik",
        "Košarica otvorena",
        "Košarica zatvorena",
        "Povratak u bazu",
    ],
    13: [
        "Sve dionice počele nakon boje",
        "Prva dionica riješena",
        "Druga dionica riješena",
        "Treća dionica riješena",
    ],
}

exe_path = os.path.join(os.path.dirname(__file__), "simulacija.exe")

folder_path = filedialog.askdirectory(title="Odaberi glavni folder s podacima")
if not folder_path:
    print("Nije odabran folder.")
    exit()

snapshots_dir = os.path.join(folder_path, "Lego Spike")
# Putanja do direktorija sa snapshotima
log_path = os.path.join(folder_path, "zadatci_metadata_clean.txt")
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
    llsp_files = [f for f in os.listdir(snapshots_dir) if f.endswith('.llsp3')]

    timestamp_to_file = {}

    for filename in llsp_files:
        # Pretpostavljamo da je ime oblika: "TIMESTAMP ostatak.llsp"
        timestamp_str = filename.split(' ')[0]
        try:
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
        if "Task:" in line:
            parts = line.split()
            timestamp = parts[0]
            task_id = int(parts[2]) + 1
            # action = parts[3]  # Next ili Prev, ignorira se ako je TRUE ili FALSE
            # if action not in ["Next", "Prev"]:
            #     continue

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
    candidates = [(ts, fname) 
                  for ts, fname in snapshots.items() 
                  if start_dt <= ts.replace(tzinfo=None) <= end_dt]
    if not candidates:
        # Ako nema nijednog, možeš vratiti None ili probati najnoviji prije end_dt
        return None
    # Vrati najnoviji (najveći timestamp)
    return max(candidates, key=lambda x: x[0])

# Putanja do .llsp datoteke
output_dir = os.path.join(folder_path, "Lego Spike", "extraction_folder")

sb3_path = os.path.join(folder_path, "Lego Spike", "extraction_folder")

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

VALID_TASKS = {1, 2, 3, 4, 6, 7, 11, 12, 13}

results = {}
for i in VALID_TASKS:
    results[f"Task {i}"] = (-1, [], "Nema")

# Parsiranje izlaza simulacije
def parse_simulation_output(stdout_text):
    lines = stdout_text.strip().splitlines()

    if not lines:
        return -1, []

    try:
        score = int(lines[0].strip())
    except ValueError:
        return -1, []

    flags = []

    for line in lines[1:]:
        value = line.strip().lower()

        if value == "true":
            flags.append(True)
        elif value == "false":
            flags.append(False)

    return score, flags

def evaluate_final_snapshot(task, start, end, all_snapshots, snapshots_dir, sb3_path, output_dir):
    # Pronađi najnoviji snapshot u periodu
    final_file = find_latest_snapshot_in_period(all_snapshots, start, end)
    # Simuliraj i provjeri rješenje
    if final_file:
        ts, fname = final_file
        clean_dir(output_dir)
        extract_sb3(snapshots_dir, fname, output_dir)
        extract_sb3(sb3_path, "scratch.sb3", output_dir)
        result = subprocess.run(
            [exe_path, str(task), json_file_path],
            capture_output=True, text=True
        )
        return parse_simulation_output(result.stdout)
    return -1, []


def evaluate_all_snapshots(task, start, end, all_snapshots, snapshots_dir, sb3_path, output_dir):
    # Pronađi sve snapshotove u periodu
    start_dt = datetime.strptime(start, "%Y-%m-%d-%H-%M-%S,%f")
    end_dt = datetime.strptime(end, "%Y-%m-%d-%H-%M-%S,%f")
    snapshots_in_period = [
        (ts, fname) for ts, fname in all_snapshots.items()
        if start_dt <= ts.replace(tzinfo=None) <= end_dt
    ]
    best_score = -1
    best_flag_count = -1
    best_flags = []
    best_file = "Nema dobrog rjesenja"
    # Analiziraj svaki snapshot u periodu
    for ts, fname in sorted(snapshots_in_period):
        clean_dir(output_dir)
        try:
            extract_sb3(snapshots_dir, fname, output_dir)
            extract_sb3(output_dir, "scratch.sb3", output_dir)
        except (OSError, zipfile.BadZipFile) as error:
            print(f"Greška pri raspakiravanju {fname}: {error}")
            continue
        try:
            result = subprocess.run(
                [exe_path, str(task), json_file_path],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                timeout=30,
                cwd=os.path.dirname(exe_path),
                env={**os.environ, "PATH": os.path.dirname(exe_path) + ";" + os.environ["PATH"]}
            )
        except subprocess.TimeoutExpired:
            print(f"Timeout za file {fname}")
            continue
        except OSError as error:
            print(f"Greška pri pokretanju simulatora: {error}")
            continue
        # print("Evaluating Task", task, "Snapshot:", fname)
        # print("STDOUT:", repr(result.stdout))
        # print("STDERR:", repr(result.stderr))
        # print("RETURNCODE:", result.returncode)
        # num += 1
        # print("Evaluated", num, "snapshots so far.")
        # print("-----------------------------------")

        if result.returncode != 0:
            print(f"Greška u simulatoru za file {fname}")
            print(result.stderr)
            continue
        
        score, flags = parse_simulation_output(result.stdout)
        
        if score < 0:
            print(f"Neispravan izlaz simulatora za file {fname}")
            print(f"STDOUT: {result.stdout!r}")
            print(f"STDERR: {result.stderr!r}")
            continue
        
        flag_count = sum(flags)

        is_better = (
            flag_count > best_flag_count
            or (
                flag_count == best_flag_count
                and score > best_score
            )
        )

        if is_better:
            best_flag_count = flag_count
            best_score = score
            best_flags = flags
            best_file = fname
    return best_score, best_flags, best_file

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


def write_results_to_csv(
    results,
    output_path,
    flag_names=None,
    student_name="",
):
    """
    Zapisuje rezultate u čitljiv tekstualni format.
    """
    if flag_names is None:
        flag_names = {}

    separator = "=" * 40

    with open(
        output_path,
        "w",
        encoding="utf-8",
    ) as results_file:
        if student_name:
            results_file.write(f"student: {student_name}\n")
            results_file.write(f"{separator}\n")

        def task_number(task_name):
            return int(task_name.split()[-1])

        for task_name in sorted(results, key=task_number):
            score, flags, filename = results[task_name]
            task_id = task_number(task_name)
            names = flag_names.get(task_id, [])

            results_file.write(f"task {task_id}\n")
            results_file.write(f"score {score}\n")

            number_of_flags = max(
                len(flags),
                len(names),
            )

            for index in range(number_of_flags):
                if index < len(names):
                    flag_name = names[index]
                else:
                    flag_name = f"flag{index + 1}"

                if index < len(flags):
                    flag_value = flags[index]
                else:
                    flag_value = False

                results_file.write(
                    f"{flag_name}: "
                    f"{'true' if flag_value else 'false'}\n"
                )

            results_file.write(f"{separator}\n")

for task, periods in task_periods.items():
    if task not in VALID_TASKS:
        continue
    for start, end in periods:
        score, flags, filename = evaluate_all_snapshots(task, start, end, all_snapshots, snapshots_dir, sb3_path, output_dir)
        # Za sada gledam samo flagove
        result_key = f"Task {task}"

        previous_score, previous_flags, previous_file = results[result_key]

        previous_flag_count = sum(previous_flags)
        current_flag_count = sum(flags)

        is_better = (
            current_flag_count > previous_flag_count
            or (
                current_flag_count == previous_flag_count
                and score > previous_score
            )
        )

        if is_better:
            results[result_key] = (score, flags, filename)

for task, result in results.items():
    score, flags, filename = result

    print(
        f"{task}: score={score}, "
        f"flags={flags}, "
        f"file={filename}"
    )


write_results_to_csv(
    results,
    os.path.join(folder_path, "rezultati.csv"),
    flag_names=FLAG_NAMES,
    student_name=os.path.basename(folder_path),
)