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

exe_path = os.path.join(os.path.dirname(__file__), "simulacija_breakdancing.exe")

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
            print(f"tudent folder: {student_code}")
            print(f"Zadatak: {task}")
            print(f"File: {fname}")
            print(f"Score: {score}")
            print(f"STDOUT: {repr(result.stdout)}")
            print(f"STDERR: {repr(result.stderr)}")
            print(f"{'='*60}\n")
            if not result.stderr.startswith("Bad function call for opcode: ") and not result.stderr.startswith("Program exited via ControlStop") and not fname.endswith(".llsp"):
                sys.exit(1)
        
        print(f"score={score}")
        
        if score > maxi_score:
            maxi_score = score
            final_file = fname
    
    return maxi_score, final_file


# Imena flagova po zadatku (odgovaraju criteria_met[] redom iz C++)
FLAG_NAMES = {
    1: [
        "Correct motor rotation direction",
        "The motor rotates one full rotation",
        "Correct port",
    ],
    2: [
        "Starts ONLY one motor",
        "Starts two motors simultaneously",
        "Starts one motor after the other",
        "Motor port correctly set (1)",
        "Motor port correctly set (2)",
    ],
    3: [
        "Sets the speed before using the motor (1)",
        "Sets the speed before using the motor (2)",
        "Speed 30% correctly set",
        "Speed 50% correctly set",
        "Motor command correctly set to 375 degrees",
        "Motor command correctly set to 5 s",
        "Motor ports correctly set",
    ],
    4: [
        "Sets the speed before using the motor (1)",
        "Sets the speed before using the motor (2)",
        "Speed 20% correctly set",
        "Speed 100% correctly set",
        "Motor command correctly set to 3 rotations",
        "Motor command correctly set to 360 degrees",
        "Motor port correctly set (1)",
        "Motor port correctly set (2)",
        "Starts motors simultaneously",
    ],
    5: [
        "Repeat until command correctly transcribed",
        "Distance sensor command correct",
        "Distance correctly set",
        "Speed set to the correct value",
        "Number of rotations set to the correct value",
        "Ports correctly entered in the speed-setting command",
        "Ports correctly entered in the motor start command",
    ],
    6: [
        "Program detects a hand",
        "Program detects the hand every time, not only once",
        "Distance correctly set to 5 cm",
        "Motors for arms and legs started",
        "Ultrasonic sensor port correctly set",
        "Motor command correctly set to 180 seconds",
        "Motor port correctly set (1)",
        "Motor port correctly set (2)",
    ],
}


def parse_simulation_output(stdout_text):
    """Parsira C++ output: linija 1 = score, ostale linije = true/false flagovi"""
    lines = stdout_text.strip().splitlines()
    if not lines:
        return -1, []
    try:
        score = int(lines[0].strip())
    except ValueError:
        return -1, []
    flags = []
    for line in lines[1:]:
        val = line.strip().lower()
        if val == "true":
            flags.append(True)
        elif val == "false":
            flags.append(False)
    return score, flags


def run_snapshot_for_task(task_id, llsp_file_path, fname, output_dir, json_file_path, student_code):
    """Pokreni simulaciju jednog snapshota za jedan zadatak. Vrati (score, flags_list)."""
    clean_dir(output_dir)
    try:
        extract_sb3(llsp_file_path, fname, output_dir)
        extract_sb3(output_dir, "scratch.sb3", output_dir)
    except Exception as e:
        return -1, []

    try:
        result = subprocess.run(
            [exe_path, str(task_id), json_file_path],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            cwd=os.path.dirname(exe_path),
            env={**os.environ, "PATH": os.path.dirname(exe_path) + ";" + os.environ["PATH"]},
            timeout=30,
        )
    except subprocess.TimeoutExpired:
        return -1, []
    except Exception as e:
        return -1, []

    score, flags = parse_simulation_output(result.stdout)

    if score == -1:
        if (
            not result.stderr.startswith("Bad function call for opcode: ")
            and not result.stderr.startswith("Program exited via ControlStop")
            and not fname.endswith(".llsp")
        ):
            print(f"\n{'='*60}")
            print(f"KRITICNA GRESKA!")
            print(f"Student: {student_code}, Zadatak: {task_id}, File: {fname}")
            print(f"STDOUT: {repr(result.stdout)}")
            print(f"STDERR: {repr(result.stderr)}")
            print(f"{'='*60}\n")

    return score, flags


def evaluate_last_snapshot_all_tasks(start, end, all_snapshots, llsp_file_path, output_dir, json_file_path, student_code, task_from_log):
    """
    Uzmi ZADNJI snapshot u periodu [start, end].
    Ovisno o task_from_log:
      - Ako je 1 ili 2: testiraj SVE taskove (1-6)
      - Ako je 3-6: testiraj samo taskove 3-6 (Task 1 i 2 su trivijalni)
    Vrati (best_task_id, best_score, best_flags_list, fname) prema najvisem scoreu.
    Ako nema snapshotova, vrati (0, -1, [], "Nema").
    """
    start_dt = datetime.strptime(start, "%Y-%m-%d-%H-%M-%S,%f")
    end_dt = datetime.strptime(end, "%Y-%m-%d-%H-%M-%S,%f")

    snapshots_in_period = [
        (ts, fname) for ts, fname in all_snapshots.items()
        if start_dt <= ts.replace(tzinfo=None) <= end_dt
    ]

    if not snapshots_in_period:
        print(f"    -> NEMA SNAPSHOTS-A U OVOM PERIODU [{start} -> {end}]")
        return 0, -1, [], "Nema"

    # Uzmi ZADNJI snapshot (po timestampu)
    snapshots_in_period.sort(key=lambda x: x[0])
    last_ts, last_fname = snapshots_in_period[-1]

    print(f"    Zadnji snapshot: {last_fname[:60]}...")
    
    # Odredi koji se taskovi trebaju testirati
    if task_from_log in [1, 2]:
        # Ako su na Easy taskovima, testiraj SVE
        tasks_to_test = range(1, 7)
        print(f"    Log zadatak: {task_from_log} (Easy) -> testiranje svih taskova (1-6)")
    elif task_from_log in [3, 4, 5, 6]:
        # Ako su na teskim taskovima, ignoriraj 1 i 2 (trivijalni)
        tasks_to_test = range(3, 7)
        print(f"    Log zadatak: {task_from_log} (Hard) -> testiranje samo taskova 3-6")
    else:
        tasks_to_test = range(1, 7)

    best_task_id = 0
    best_score = -1
    best_flags = []

    for task_id in tasks_to_test:
        score, flags = run_snapshot_for_task(
            task_id, llsp_file_path, last_fname, output_dir, json_file_path, student_code
        )
        print(f"      Task {task_id}: score={score}")
        if score >= best_score:
            best_score = score
            best_task_id = task_id
            best_flags = flags

    print(f"    -> Najbolji: Task {best_task_id} sa {best_score} bodova")
    return best_task_id, best_score, best_flags, last_fname


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
    normalized = normalized.replace("_", "").replace(" ", "").replace(",", "").replace(".", "").replace("-", "")
    
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
    # results[detected_task_id] = (detected_task_id, score, flags_list, fname)
    results = {}
    
    # Za svaki period (timeframe), uzmi zadnji snapshot i probaj sve taskove
    period_index = 0
    for task_from_log, periods in task_periods.items():
        if task_from_log < 1 or task_from_log > 6:
            continue
        
        for start, end in periods:
            period_index += 1
            print(f"  Period {period_index} (log zadatak {task_from_log}): {start} -> {end}")
            
            best_task_id, best_score, best_flags, fname = evaluate_last_snapshot_all_tasks(
                start, end, all_snapshots,
                snapshots_dir, output_dir, json_file_path,
                student_code=normalize_code(code),
                task_from_log=task_from_log
            )
            
            if best_score < 0:
                continue
            
            # Ako za ovaj detektirani zadatak vec imamo bolji rezultat, preskocimo
            if best_task_id in results and results[best_task_id][1] >= best_score:
                continue
            
            results[best_task_id] = (best_task_id, best_score, best_flags, fname)
    
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
        
        # Ispisi rezultate za ovog studenta
        for task_num in range(1, 7):
            if task_num in results:
                _, score, flags, fname = results[task_num]
                flags_str = ", ".join(
                    f"{name}={'DA' if val else 'NE'}"
                    for name, val in zip(FLAG_NAMES.get(task_num, []), flags)
                )
                print(f"  Zadatak {task_num}: {score} bodova  [{flags_str}]")
            else:
                print(f"  Zadatak {task_num}: nije detektiran")
    
    # Zapisi rezultate u CSV
    output_csv_path = os.path.join(root_folder, "rezultati.csv")
    
    # Pronadi maksimalan broj flagova za header
    max_flags = max(len(v) for v in FLAG_NAMES.values())
    
    with open(output_csv_path, 'w', encoding='utf-8') as f:
        # Header
        header_parts = ["student", "zadatak", "score", "file"]
        for i in range(max_flags):
            header_parts.append(f"flag_{i+1}_name")
            header_parts.append(f"flag_{i+1}_value")
        f.write(";".join(header_parts) + "\n")
        
        for code, results in all_results:
            for task_num in range(1, 7):
                if task_num in results:
                    _, score, flags, fname = results[task_num]
                    flag_names = FLAG_NAMES.get(task_num, [])
                    
                    row_parts = [code, str(task_num), str(score), fname]
                    for i in range(max_flags):
                        if i < len(flag_names):
                            row_parts.append(flag_names[i])
                            row_parts.append("true" if (i < len(flags) and flags[i]) else "false")
                        else:
                            row_parts.append("")
                            row_parts.append("")
                    f.write(";".join(row_parts) + "\n")
                else:
                    row_parts = [code, str(task_num), "-1", "Nema"]
                    row_parts.extend([""] * (max_flags * 2))
                    f.write(";".join(row_parts) + "\n")
    
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