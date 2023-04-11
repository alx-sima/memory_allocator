#### Nume: Sima Alexandru

#### Grupă: 312CA

# Virtual Memory Allocator (Tema 1)

## Descriere

- ### Scurtă descriere

  Programul simulează un alocator de memorie, adică o interfață prin care
  utilizatorul poate aloca și elibera blocuri de memorie, modifica permisiunile
  blocurilor și scrie/citi date în acestea.

- ### Structură

  Codul este organizat în următoarele module:

  - `io`: Operații de input-output ale programului;
  - `list`: Operații și structuri de date pentru liste generice;
  - `mem_alloc`: Operații de alocare și stergere a (mini)blocurilor;
  - `mem_io`: Operații de citire-scriere în blocuri de memorie;
  - `mem_prot`: Operații de setare/verificare a permisiunilor blocurilor de
    memorie;
  - `utils`: Funcții cu caracter general.
  - `vma`: Operații de alocare/dealocare a blocurilor de memorie.

- ### Funcționare

  - Programul acceptă urmatoarele comenzi, citirea lor facându-se linie cu
    linie[1][4]:

    - `ALLOC_ARENA <dimensiune>`: Se alocă o arena (în care se vor aloca mai
      departe blocurile de memorie) cu dimensiunea specificată.
    - `DEALLOC_ARENA`: Se dealocă arena și tot conținutul ei și se inchide
      programul.
    - `ALLOC_BLOCK <adresă> <dimensiune>`: Se alocă un bloc cu `adresa` și
      `dimensiunea` specificate.
      Se afișează un mesaj de eroare dacă `adresa` este invalida, blocul se
      suprapune cu un altul sau ar ieși din arenă.
    - `FREE_BLOCK <adresă>`: Se eliberează blocul alocat.
      Se afisează un mesaj de eroare dacă `adresa` este invalidă.
    - `READ <adresă> <dimensiune>`:
    - `WRITE <adresă> <dimensiune> <date>...`:
    - `PMAP`: Afișează un memory dump al arenei (informații despre blocuri,
      miniblocuri și memorie utilizată).
    - `MPROTECT <addresă> <perm1> [ | <perm2>...]`:

  - Programul începe cu alocarea unei arene cu dimensiune specificată
    (echivalentul mapării unui memory object).

  - La alocarea unui bloc, acesta se introduce în lista de blocuri,
    verificându-se dacă se suprapune cu blocurile vecine și unindu-se cu
    blocurile adiacente. Practic, lista de blocuri reprezintă zonele contigue de
    memorie (pentru eficientizarea căutărilor), iar cele de miniblocuri
    reprezintă alocările în sine.

    Pentru ușurință, listele sunt menținute sortate dupa adresa de start a
    (mini)blocului.

  - La dealocarea unui minibloc, dacă blocul se golește, se elibereaza și
    acesta, sau, dacă miniblocul este la mjlocul blocului, se sparge în 2,
    blocul din dreapta conținând lista din continuarea miniblocului.

  - La citirea din memorie[2], se verifică dacă adresa este validă (se află într-un
    minibloc), apoi se verifică dacă miniblocurile din care se citește au
    permisiunile necesare (`PROT_READ`). Dacă prin citire s-ar ieși din bloc, se
    afișează un avertisment.

  - La scrierea in memorie, se verifică dacă adresa este alocată (se află
    într-un minibloc), apoi se verifică dacă toate blocurile în care se scrie au
    permisiunile necesare (`PROT_WRITE`). Se efectuează scrierea, afișându-se un
    avertisment dacă datele au fost trunchiate (nu încap în bloc).

  - La schimbarea permisiunilor, se seteaza un octet de permisiuni prin
    concatenarea biților respectivi (aplicând OR pe biți).

  - Pentru afisarea hărții de memorie, se parcurge lista de blocuri[3],
    afișându-se informații despre fiecare.

---

## Comentarii

- Deși este o temă pentru liste, cel mai mult timp l-am petrecut la citirea
  octeților de scris cu comanda `WRITE`.

- [1]: Nu am înțeles de ce se afișează câte un `INVALID_COMMAND` pentru fiecare
  token din comanda invalidă, în loc de 1/comandă, cum ar fi și normal (testul
  0).

- [2]: Pentru citirea și scrierea în memorie, cât și pentru citirea datelor de
- scris în memorie, am prelucrat informația in "batchuri", copiind bloc cu bloc
- (respectiv rând cu rând), apoi concatenand rezultatul. 

- [3]: Am implementat o funcție generică pentru liste (`apply_func`), care
  aplică o funcție dată ca parametru tuturor nodurilor listei. Astfel, pentru
  printarea hărții de memorie, se apeleaza `apply_func` pe lista de blocuri cu o
  funcție de printat un block, care la randul ei apelează `apply_func` pe lista
  de miniblocuri din acesta.

## Resurse

- [4]: În temele precedente am constatat ca e dificil să citești o linie în C
  fără să ai o restricție a dimensiunii, așa că foloseam o funcție inspirată
  dintr-o întrebare de pe StackOverflow pe care o găsisem în clasa a XI-a. La
  tema aceasta, îmbunătățit-o, facând mai puține alocări (folosind același
  buffer care se realocă doar când se citește un string mai mare decât el).
  Sursa de inspirație este funcția `getline`, care, din păcate, nu este
  standard.
