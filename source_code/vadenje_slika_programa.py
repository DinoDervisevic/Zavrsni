import os
import shutil
import zipfile
from collections import defaultdict
from datetime import datetime
import tkinter as tk
from tkinter import filedialog


TIMESTAMP_FORMAT = "%Y-%m-%d-%H-%M-%S,%f"


def choose_folder():
    root = tk.Tk()
    root.withdraw()

    folder = filedialog.askdirectory(
        title="Odaberi folder učenika"
    )

    root.destroy()
    return folder


def clean_directory(path):
    if os.path.isdir(path):
        shutil.rmtree(path)

    os.makedirs(path, exist_ok=True)


def extract_archive(archive_path, output_dir):
    with zipfile.ZipFile(archive_path, "r") as archive:
        archive.extractall(output_dir)


def find_file(directory, filename):
    for root, _, files in os.walk(directory):
        if filename in files:
            return os.path.join(root, filename)

    return None


def parse_snapshot_timestamp(filename):
    timestamp_text = filename.split(" ")[0]

    try:
        # Isti format koji koristi tvoj parser
        unix_timestamp = float(
            timestamp_text[:10]
            + "."
            + timestamp_text[10:]
        )

        return datetime.fromtimestamp(unix_timestamp)

    except (ValueError, OSError):
        return None


def get_all_snapshots(snapshots_dir):
    snapshots = []

    for filename in os.listdir(snapshots_dir):
        if not filename.lower().endswith((".llsp3", ".llsp")):
            continue

        timestamp = parse_snapshot_timestamp(filename)

        if timestamp is None:
            print(f"Preskačem snapshot bez validnog vremena: {filename}")
            continue

        snapshots.append((timestamp, filename))

    return sorted(snapshots)


def parse_log_file(log_path):
    with open(log_path, "r", encoding="utf-8") as file:
        lines = [
            line.strip()
            for line in file
            if line.strip()
        ]

    task_periods = defaultdict(list)

    current_task = None
    current_start = None

    for line in lines:
        if "Task:" not in line:
            continue

        parts = line.split()

        if len(parts) < 3:
            print(f"Preskačem neispravan redak: {line}")
            continue

        try:
            timestamp = parts[0]
            task_id = int(parts[2]) + 1
            datetime.strptime(timestamp, TIMESTAMP_FORMAT)
        except (ValueError, IndexError):
            print(f"Preskačem neispravan redak: {line}")
            continue

        if current_task is not None and current_start is not None:
            task_periods[current_task].append(
                (
                    datetime.strptime(
                        current_start,
                        TIMESTAMP_FORMAT,
                    ),
                    datetime.strptime(
                        timestamp,
                        TIMESTAMP_FORMAT,
                    ),
                )
            )

        current_task = task_id
        current_start = timestamp

    if current_task is not None and current_start is not None:
        task_periods[current_task].append(
            (
                datetime.strptime(
                    current_start,
                    TIMESTAMP_FORMAT,
                ),
                datetime.max,
            )
        )

    return dict(task_periods)


def snapshots_for_period(snapshots, start, end):
    return [
        (timestamp, filename)
        for timestamp, filename in snapshots
        if start <= timestamp <= end
    ]


def export_icon_for_snapshot(
    snapshot_path,
    output_root,
    task_id,
    snapshot_timestamp,
    snapshot_filename,
    temporary_dir,
):
    clean_directory(temporary_dir)

    try:
        # Raspakiraj .llsp3/.llsp
        extract_archive(snapshot_path, temporary_dir)

        # Pronađi scratch.sb3
        scratch_path = find_file(
            temporary_dir,
            "scratch.sb3",
        )

        if scratch_path is None:
            print(
                f"  Nema scratch.sb3: "
                f"{snapshot_filename}"
            )
            return False

        # Raspakiraj scratch.sb3
        extract_archive(scratch_path, temporary_dir)

        # Pronađi icon.svg
        icon_path = find_file(
            temporary_dir,
            "icon.svg",
        )

        if icon_path is None:
            print(
                f"  Nema icon.svg: "
                f"{snapshot_filename}"
            )
            return False

        task_directory = os.path.join(
            output_root,
            f"Task_{task_id}",
        )
        os.makedirs(task_directory, exist_ok=True)

        safe_timestamp = snapshot_timestamp.strftime(
            "%Y-%m-%d_%H-%M-%S"
        )

        original_name = os.path.splitext(
            snapshot_filename
        )[0]

        output_filename = (
            f"Task_{task_id}_"
            f"{safe_timestamp}_"
            f"{original_name}.svg"
        )

        output_path = os.path.join(
            task_directory,
            output_filename,
        )

        shutil.copyfile(icon_path, output_path)

        return True

    except (OSError, zipfile.BadZipFile) as error:
        print(
            f"  Greška za {snapshot_filename}: "
            f"{error}"
        )
        return False


def main():
    folder_path = choose_folder()

    if not folder_path:
        print("Nije odabran folder.")
        return

    snapshots_dir = os.path.join(
        folder_path,
        "Lego Spike",
    )

    log_path = os.path.join(
        folder_path,
        "zadatci_metadata_clean.txt",
    )

    if not os.path.isdir(snapshots_dir):
        print(
            f"Nedostaje folder: {snapshots_dir}"
        )
        return

    if not os.path.isfile(log_path):
        print(
            f"Nedostaje metadata file: {log_path}"
        )
        return

    snapshots = get_all_snapshots(snapshots_dir)
    task_periods = parse_log_file(log_path)

    if not snapshots:
        print("Nema validnih snapshotova.")
        return

    if not task_periods:
        print("Nema pronađenih zadataka u metadata fileu.")
        return

    output_root = os.path.join(
        folder_path,
        "icons",
    )

    temporary_dir = os.path.join(
        folder_path,
        "_icon_extraction_temp",
    )

    clean_directory(output_root)

    total_exported = 0

    for task_id, periods in sorted(task_periods.items()):
        task_snapshots = []

        for start, end in periods:
            task_snapshots.extend(
                snapshots_for_period(
                    snapshots,
                    start,
                    end,
                )
            )

        # Ako se isti snapshot pojavi u dva perioda,
        # zadrži ga samo jednom.
        task_snapshots = list(
            dict(task_snapshots).items()
        )

        print(
            f"Task {task_id}: "
            f"{len(task_snapshots)} snapshotova"
        )

        for timestamp, filename in task_snapshots:
            snapshot_path = os.path.join(
                snapshots_dir,
                filename,
            )

            exported = export_icon_for_snapshot(
                snapshot_path,
                output_root,
                task_id,
                timestamp,
                filename,
                temporary_dir,
            )

            if exported:
                total_exported += 1

    if os.path.isdir(temporary_dir):
        shutil.rmtree(temporary_dir)

    print()
    print(
        f"Završeno. Izvezeno ikona: "
        f"{total_exported}"
    )
    print(f"Izlazni folder: {output_root}")


if __name__ == "__main__":
    main()