import csv
import ctypes
import os
import shutil
import subprocess
import sys
import tkinter as tk
import zipfile
from collections import defaultdict
from datetime import datetime
from tkinter import filedialog
import re


try:
    ctypes.windll.shcore.SetProcessDpiAwareness(1)
except AttributeError:
    pass


# Simulator se nalazi u istom folderu kao i ova Python skripta.
EXE_PATH = os.path.normpath(
    os.path.join(os.path.dirname(__file__), "simulacija.exe")
)


# Zadaci koji se stvarno evaluiraju.
VALID_TASKS = {1, 2, 3, 4, 6, 7, 11, 12, 13}


# Nazivi flagova za svaki zadatak.
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


TIMESTAMP_FORMAT = "%Y-%m-%d-%H-%M-%S,%f"

def log_unexpected_error(log_path, student_folder, task, file_name, error):
    with open(log_path, "a", encoding="utf-8") as error_file:
        error_file.write(
            f"Student folder: {student_folder}\n"
            f"Task: {task}\n"
            f"File: {file_name}\n"
            f"Error: {error}\n"
            f"{'-' * 80}\n"
        )

def log_unexpected_stderr(stderr, error_log_path, student_folder, task, file_name):
    """
    Zapisuje neočekivane greške iz stderr-a.
    Nepoznati opcode se namjerno ignorira.
    """
    if not stderr:
        return

    unexpected_lines = []

    for line in stderr.splitlines():
        stripped_line = line.strip()

        if not stripped_line:
            continue

        # Nepoznati blok za sada namjerno ignoriramo.
        is_unknown_block = (
            "[BLOCK ERROR]" in stripped_line
            and (
                "bad_function_call" in stripped_line
                or "Bad function call for opcode" in stripped_line
            )
        )

        if is_unknown_block:
            continue

        unexpected_lines.append(stripped_line)

    if unexpected_lines:
        log_unexpected_error(
            error_log_path,
            student_folder,
            task,
            file_name,
            "Neočekivani STDERR:\n"
            + "\n".join(unexpected_lines),
        )


def extract_sb3(file_dir, file_name, output_dir):
    """
    Ekstraktira ZIP/.sb3/.llsp3 datoteku u output_dir.
    """
    file_path = os.path.join(file_dir, file_name)

    with zipfile.ZipFile(file_path, "r") as zip_ref:
        zip_ref.extractall(output_dir)


def clean_dir(path):
    """
    Briše sadržaj foldera, ali ostavlja sam folder.
    """
    os.makedirs(path, exist_ok=True)

    for name in os.listdir(path):
        full_path = os.path.join(path, name)

        if os.path.isfile(full_path) or os.path.islink(full_path):
            os.remove(full_path)
        elif os.path.isdir(full_path):
            shutil.rmtree(full_path)


def get_all_snapshots(snapshots_dir):
    """
    Učitava sve .llsp3 snapshotove iz foldera.

    Očekivani oblik imena datoteke je npr:

        175335889894454070 Project 4.llsp3

    Prvi dio imena koristi se kao Unix timestamp u mikrosekundama.
    """
    if not os.path.isdir(snapshots_dir):
        return {}

    snapshot_files = [
        file_name
        for file_name in os.listdir(snapshots_dir)
        if file_name.lower().endswith((".llsp3", ".llsp"))
    ]

    timestamp_to_file = {}

    for file_name in snapshot_files:
        timestamp_string = file_name.split(" ")[0]

        try:
            unix_timestamp = float(
                timestamp_string[:10] + "." + timestamp_string[10:]
            )
            timestamp = datetime.fromtimestamp(unix_timestamp)
            timestamp_to_file[timestamp] = file_name
        except (ValueError, OverflowError):
            print(
                f"    Preskačem snapshot '{file_name}': "
                "neispravan timestamp."
            )

    return timestamp_to_file


def parse_log_file(log_path):
    """
    Parsira metadata log i vraća periode za svaki zadatak.

    Povratna vrijednost:

        {
            1: [
                ("2025-12-01-10-35-10,3", "2025-12-01-10-38-09,7"),
                ...
            ],
            2: [
                ...
            ]
        }

    Svaki Task zapis predstavlja početak novog trenutnog zadatka.
    Prethodni zadatak završava u trenutku novog Task zapisa.
    """
    if not os.path.isfile(log_path):
        return {}

    with open(log_path, "r", encoding="utf-8") as metadata_file:
        lines = [
            line.strip()
            for line in metadata_file
            if line.strip()
        ]

    task_periods = defaultdict(list)

    current_task = None
    current_start = None
    last_task_timestamp = None

    for line in lines:
        if "Task:" not in line:
            continue

        parts = line.split()

        # Očekivani format:
        #
        # timestamp Task: task_id action Time_spent_on_previous_task: ...
        #
        # Primjer:
        #
        # 2025-12-01-10-29-42,1 Task: 0 Next ...
        if len(parts) < 3:
            print(f"    Preskačem neispravan metadata zapis: {line}")
            continue

        timestamp = parts[0]

        try:
            task_id = int(parts[2])
            datetime.strptime(timestamp, TIMESTAMP_FORMAT)
        except (ValueError, IndexError):
            print(f"    Preskačem neispravan metadata zapis: {line}")
            continue

        last_task_timestamp = timestamp

        if current_task is not None and current_start is not None:
            task_periods[current_task].append(
                (current_start, timestamp)
            )

        current_task = task_id
        current_start = timestamp

    # Zadnji period ne treba završavati s datetime.now(), jer bi se tada
    # mogao proširiti na vrijeme nakon završetka rada pristupnika.
    if (
        current_task is not None
        and current_start is not None
        and last_task_timestamp is not None
    ):
        task_periods[current_task].append(
            (current_start, last_task_timestamp)
        )

    return dict(task_periods)


def get_snapshots_in_period(all_snapshots, start, end):
    """
    Vraća snapshotove čiji timestamp pripada zadanom periodu.
    """
    start_datetime = datetime.strptime(start, TIMESTAMP_FORMAT)
    end_datetime = datetime.strptime(end, TIMESTAMP_FORMAT)

    return [
        (timestamp, file_name)
        for timestamp, file_name in all_snapshots.items()
        if (
            start_datetime
            <= timestamp.replace(tzinfo=None)
            <= end_datetime
        )
    ]


def parse_simulation_output(stdout_text):
    """
    Parsira izlaz simulatora.

    Očekivani format:

        score
        true
        false
        true
        ...

    Vraća:

        (score, flags)
    """
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


def evaluate_all_snapshots(
    task,
    start,
    end,
    all_snapshots,
    snapshots_dir,
    output_dir,
    error_log_path,
    student_folder,
):
    """
    Evaluira sve snapshotove koji pripadaju jednom periodu zadatka.

    Najbolji rezultat bira se ovim redoslijedom:

    1. veći broj pozitivnih flagova
    2. ako je broj flagova jednak, veći score
    """
    snapshots_in_period = get_snapshots_in_period(
        all_snapshots,
        start,
        end,
    )

    best_score = -1
    best_flag_count = -1
    best_flags = []
    best_file = "Nema dobrog rjesenja"

    print(
        f"      Zadatak {task}: "
        f"{len(snapshots_in_period)} snapshotova"
    )

    for index, (timestamp, file_name) in enumerate(
        sorted(snapshots_in_period)
    ):
        # print(
        #     f"        [{index + 1}/{len(snapshots_in_period)}] "
        #     f"{file_name}"
        # )

        clean_dir(output_dir)

        try:
            # Snapshot je .llsp3 ZIP arhiva.
            extract_sb3(
                snapshots_dir,
                file_name,
                output_dir,
            )

            # Iz snapshot projekta izvlači se scratch.sb3.
            extract_sb3(
                output_dir,
                "scratch.sb3",
                output_dir,
            )
        except (OSError, zipfile.BadZipFile, KeyError) as error:
            log_unexpected_error(
                error_log_path,
                student_folder,
                task,
                file_name,
                f"Greška pri raspakiravanju: {error}",
            )
            continue

        json_file_path = os.path.join(
            output_dir,
            "project.json",
        )

        if not os.path.isfile(json_file_path):
            log_unexpected_error(
                error_log_path,
                student_folder,
                task,
                file_name,
                "Nije pronađen project.json.",
            )
            continue

        try:
            result = subprocess.run(
                [
                    EXE_PATH,
                    str(task),
                    json_file_path,
                ],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                cwd=os.path.dirname(EXE_PATH),
                env={
                    **os.environ,
                    "PATH": (
                        os.path.dirname(EXE_PATH)
                        + ";"
                        + os.environ.get("PATH", "")
                    ),
                },
                timeout=30,
                check=False,
            )
        except subprocess.TimeoutExpired:
            error = "Simulator timeout nakon 30 sekundi."

            log_unexpected_error(
                error_log_path,
                student_folder,
                task,
                file_name,
                error,
            )
            continue
        except OSError as error:
            log_unexpected_error(
                error_log_path,
                student_folder,
                task,
                file_name,
                error,
            )
            continue

        stderr = result.stderr.strip()

        # Provjerava stderr bez obzira na return code.
        # Ovo hvata i greške koje simulator interno uhvati,
        # ali ipak završi s returncode == 0.
        log_unexpected_stderr(
            stderr=stderr,
            error_log_path=error_log_path,
            student_folder=student_folder,
            task=task,
            file_name=file_name,
        )

        if result.returncode != 0:
            log_unexpected_error(
                error_log_path,
                student_folder,
                task,
                file_name,
                (
                    f"Return code: {result.returncode}\n"
                    f"STDERR:\n{stderr}\n"
                    f"STDOUT:\n{result.stdout}"
                ),
            )
            continue

        score, flags = parse_simulation_output(result.stdout)

        if score < 0:
            log_unexpected_error(
                error_log_path,
                student_folder,
                task,
                file_name,
                (
                    f"Neispravan izlaz simulatora.\n"
                    f"STDOUT:\n{result.stdout}\n"
                    f"STDERR:\n{result.stderr}"
                ),
            )

            print(
                f"        Simulator je vratio neispravan izlaz "
                f"za '{file_name}'."
            )
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
            best_file = file_name

    return best_score, best_flags, best_file


def create_empty_results():
    """
    Stvara početni rezultat za sve validne zadatke.
    """
    return {
        f"Task {task_id}": (-1, [], "Nema")
        for task_id in sorted(VALID_TASKS)
    }


def update_best_result(
    results,
    task,
    score,
    flags,
    file_name,
):
    """
    Sprema rezultat samo ako je bolji od prethodno spremljenog
    rezultata za isti zadatak.
    """
    result_key = f"Task {task}"

    previous_score, previous_flags, previous_file = (
        results[result_key]
    )

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
        results[result_key] = (
            score,
            flags,
            file_name,
        )


def task_number(task_name):
    """
    Iz naziva 'Task 13' vraća broj 13.
    """
    return int(task_name.split()[-1])


def write_results_to_csv(
    results,
    output_path,
    student_name,
):
    """
    Zapisuje rezultate jednog pristupnika u CSV datoteku.
    """
    max_flags = 0

    for task_result in results.values():
        if len(task_result) != 3:
            raise ValueError(
                "Rezultat mora imati oblik "
                "(score, flags, filename)."
            )

        _, flags, _ = task_result
        max_flags = max(max_flags, len(flags))

    with open(
        output_path,
        "w",
        encoding="utf-8-sig",
        newline="",
    ) as csv_file:
        writer = csv.writer(
            csv_file,
            delimiter=";",
        )

        header = [
            "student",
            "task",
            "score",
            "filename",
        ]

        for index in range(max_flags):
            header.extend(
                [
                    f"flag_{index + 1}_name",
                    f"flag_{index + 1}_value",
                ]
            )

        writer.writerow(header)

        for task_name in sorted(
            results,
            key=task_number,
        ):
            score, flags, file_name = results[task_name]
            task_id = task_number(task_name)

            names = FLAG_NAMES.get(task_id, [])

            row = [
                student_name,
                task_id,
                score,
                file_name,
            ]

            for index in range(max_flags):
                if index < len(names):
                    flag_name = names[index]
                else:
                    flag_name = f"flag_{index + 1}"

                if index < len(flags):
                    flag_value = flags[index]
                else:
                    flag_value = False

                row.extend(
                    [
                        flag_name,
                        "true" if flag_value else "false",
                    ]
                )

            writer.writerow(row)


def find_metadata_file(workshop_folder):
    """
    Pronalazi metadata datoteku.

    Primarno se koristi zadatci_metadata_clean.txt, a ako ona
    ne postoji, pokušava se zadatci_metadata.txt.
    """
    possible_names = [
        "zadatci_metadata_clean.txt",
        "zadatci_metadata.txt",
    ]

    for file_name in possible_names:
        metadata_path = os.path.join(
            workshop_folder,
            file_name,
        )

        if os.path.isfile(metadata_path):
            return metadata_path

    return None


def process_student(student_folder, error_log_path):
    """
    Obrađuje jednog pristupnika.
    """
    student_name = os.path.basename(
        os.path.normpath(student_folder)
    )

    workshop_folder = os.path.join(
        student_folder,
        "LEGO_workshop",
    )

    snapshots_dir = os.path.join(
        workshop_folder,
        "Lego Spike",
    )

    output_dir = os.path.join(
        workshop_folder,
        "extraction_folder",
    )

    csv_path = os.path.join(
        student_folder,
        "rezultati.csv",
    )

    print()
    print("=" * 80)
    print(f"OBRAĐUJEM PRISTUPNIKA: {student_name}")
    print("=" * 80)

    if not os.path.isdir(workshop_folder):
        print(
            f"  Preskačem '{student_name}': "
            "ne postoji LEGO_workshop folder."
        )
        return False

    if not os.path.isdir(snapshots_dir):
        print(
            f"  Preskačem '{student_name}': "
            f"ne postoji folder '{snapshots_dir}'."
        )
        return False

    metadata_path = find_metadata_file(workshop_folder)

    if metadata_path is None:
        print(
            f"  Preskačem '{student_name}': "
            "nije pronađen metadata file."
        )
        return False

    if not os.path.isfile(EXE_PATH):
        print(
            f"  Greška: simulator ne postoji: {EXE_PATH}"
        )
        return False

    print(f"  Metadata: {metadata_path}")
    print(f"  Snapshoti: {snapshots_dir}")

    all_snapshots = get_all_snapshots(snapshots_dir)

    if not all_snapshots:
        print(
            f"  Preskačem '{student_name}': "
            "nema valjanih snapshotova."
        )
        return False

    task_periods = parse_log_file(metadata_path)

    if all_snapshots:
        last_snapshot_time = max(all_snapshots).strftime( TIMESTAMP_FORMAT)

        for task, periods in task_periods.items():
            if periods:
                start, end = periods[-1]

                if start == end:
                    periods[-1] = (start, last_snapshot_time)

    if not task_periods:
        print(
            f"  Preskačem '{student_name}': "
            "metadata ne sadrži valjane task zapise."
        )
        return False

    results = create_empty_results()

    for task, periods in task_periods.items():
        if task not in VALID_TASKS:
            continue

        for start, end in periods:
            score, flags, file_name = evaluate_all_snapshots(
                task=task,
                start=start,
                end=end,
                all_snapshots=all_snapshots,
                snapshots_dir=snapshots_dir,
                output_dir=output_dir,
                error_log_path=error_log_path,
                student_folder=student_folder,
            )

            update_best_result(
                results=results,
                task=task,
                score=score,
                flags=flags,
                file_name=file_name,
            )

    write_results_to_csv(
        results=results,
        output_path=csv_path,
        student_name=student_name,
    )

    print()
    print(f"  CSV je spremljen u: {csv_path}")
    print("  Rezultati:")

    for task_name in sorted(
        results,
        key=task_number,
    ):
        score, flags, file_name = results[task_name]

        print(
            f"    {task_name}: "
            f"score={score}, "
            f"flags={flags}, "
            f"file={file_name}"
        )

    return True


def find_student_folders(root_folder):
    """
    Vraća podfoldere glavnog foldera.

    Svaki direktni podfolder tretira se kao folder jednog
    pristupnika. Sama provjera LEGO_workshop foldera radi se
    u process_student().
    """
    student_folders = []

    try:
        entries = os.scandir(root_folder)
    except OSError as error:
        print(
            f"Greška pri čitanju glavnog foldera: {error}"
        )
        return student_folders

    with entries:
        for entry in entries:
            if entry.is_dir():
                student_folders.append(entry.path)

    return sorted(
        student_folders,
        key=lambda path: os.path.basename(path).lower(),
    )


def main():
    root = tk.Tk()
    root.withdraw()

    root_folder = filedialog.askdirectory(
        title=(
            "Odaberi glavni folder koji sadrži "
            "foldere pristupnika"
        )
    )

    root.destroy()

    if not root_folder:
        print("Nije odabran folder.")
        return

    if not os.path.isdir(root_folder):
        print(
            f"Odabrani folder ne postoji: {root_folder}"
        )
        return

    student_folders = find_student_folders(root_folder)

    error_log_path = os.path.join(
        root_folder,
        "neocekivane_greske.txt",
    )

    # Prethodni log se briše na početku novog pokretanja.
    with open(error_log_path, "w", encoding="utf-8") as error_file:
        error_file.write("")

    if not student_folders:
        print(
            "U odabranom folderu nisu pronađeni "
            "podfolderi pristupnika."
        )
        return

    processed_count = 0
    skipped_count = 0

    for student_folder in student_folders:
        was_processed = process_student(student_folder, error_log_path=error_log_path)

        if was_processed:
            processed_count += 1
        else:
            skipped_count += 1

    print()
    print("=" * 80)
    print("OBRADA ZAVRŠENA")
    print("=" * 80)
    print(f"Uspješno obrađeno: {processed_count}")
    print(f"Preskočeno: {skipped_count}")


if __name__ == "__main__":
    main()