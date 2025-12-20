# Automatsko ocjenjivanje LEGO Spike Scratch programa

Ovaj alat omogućuje automatsko ocjenjivanje rješenja zadataka s LEGO Spike radionica na temelju spremljenih snapshotova (llsp3 datoteka) i logova (zadaci_metadata.txt) o radu učenika.

## **Struktura podataka**

Za pravilan rad simulatora, direktorij u kojem se nalaze podaci iz radionice mora biti strukturiran ovako:

```text
|-- radionica
    |-- snapshots
        |-- snapshot_data
    |-- zadaci_metadata.txt
```

- **snapshots/**: folder sa svim llsp3 snapshotovima
- **snapshot_data/**: pomoćni folder za raspakiravanje llsp3 datoteke koji će stvoriti program ako ne postoji
- **zadatci_metadata.txt**: log datoteka s vremenskim periodima za svaki zadatak

## **Pokretanje**

1. Pokreni skriptu **final_parsiranje.py**
2. U prozoru za odabir foldera, odaberi filder *radionica*, sa strukturom kao na slici gore
3. Nakon provjere zadataka, ispisati će se  za svaki zadatak je li riješen (Solved) ili nije (Not solved).