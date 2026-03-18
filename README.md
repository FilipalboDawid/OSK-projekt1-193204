# Saper C++ (Minesweeper)

Projekt implementujący klasyczną grę Saper (Minesweeper) w języku C++. Aplikacja posiada graficzny interfejs użytkownika, rozbudowane opcje konfiguracji rozgrywki oraz lokalny system zapisu wyników. 

## Funkcjonalności
* Cztery poziomy trudności: Początkujący (8x8), Zaawansowany (16x16), Ekspert (30x16) oraz w pełni personalizowana Plansza Użytkownika.
* Dodatkowe mechaniki ułatwiające rozgrywkę (z możliwością konfiguracji):
  * Bezpieczny ruch otwierający (gwarancja braku miny przy pierwszym kliknięciu).
  * Chording (szybkie odkrywanie sąsiednich pól po kliknięciu w cyfrę).
  * Pytajniki (dodatkowy stan oflagowania pola).
  * Możliwość cofania ruchów po detonacji miny (Undo).
  * System podpowiedzi wskazujący bezpieczne pole lub ukrytą minę.
* Lokalna tabela najlepszych wyników dla poszczególnych poziomów trudności.
* Mechanizm aktywnej pauzy (zatrzymanie czasu i ukrycie planszy).
* Dynamiczna zmiana motywów graficznych (Classic, Modern, Green).

## Sterowanie
* Lewy Przycisk Myszy (LPM) - odkrycie pola / interakcja z interfejsem graficznym.
* Prawy Przycisk Myszy (PPM) - oflagowanie miny / postawienie znaku zapytania.
* Klawisz P - aktywacja/dezaktywacja pauzy.
* Klawisz H - użycie podpowiedzi.
* Klawisz F11 - przełączenie trybu pełnoekranowego.
* Klawisz ESC - otwarcie menu podręcznego podczas gry / powrót do głównego menu.

## Wykorzystane biblioteki
* Standardowa biblioteka języka C++ (STL).
* SFML (Simple and Fast Multimedia Library) w wersji 2.6.x - wykorzystano moduły: `System`, `Window` oraz `Graphics` do obsługi zdarzeń, renderowania grafiki 2D oraz zarządzania oknem aplikacji.

## Instrukcja kompilacji i uruchomienia

### System Windows (środowisko MSYS2 / MinGW)
1. Upewnij się, że posiadasz zainstalowany kompilator `g++` oraz bibliotekę SFML dla architektury 64-bitowej (zalecane środowisko MSYS2 UCRT64).
2. (Opcjonalnie) Aby skompilować plik zasobów z ikoną aplikacji, wykonaj polecenie:
   windres icon.rc -o icon.o
3. Skompiluj projekt, uruchamiając terminal w głównym katalogu projektu i wpisując poniższe polecenie:
   g++ src/*.cpp icon.o -o saper.exe -lsfml-graphics -lsfml-window -lsfml-system -mwindows
   (Uwaga: Jeśli pominąłeś krok 2, usuń fragment "icon.o" z powyższej komendy).
4. Do poprawnego działania gry wymagane jest umieszczenie w jednym katalogu: pliku wykonywalnego `saper.exe`, folderu `assets/` oraz plików współdzielonych biblioteki SFML (m.in. `libsfml-graphics-2.dll`, `libsfml-window-2.dll`, `libsfml-system-2.dll`) i kompilatora.
5. Uruchom plik `saper.exe`.

### System Linux (Ubuntu / Zorin OS)
1. Zainstaluj kompilator `g++` oraz pliki nagłówkowe biblioteki SFML, wykonując polecenia w terminalu:
   sudo apt update
   sudo apt install g++ libsfml-dev
2. Przejdź do głównego katalogu projektu i skompiluj kod poleceniem:
   g++ src/*.cpp -o saper -lsfml-graphics -lsfml-window -lsfml-system
3. Upewnij się, że folder `assets/` (zawierający czcionki i grafiki) znajduje się w tym samym katalogu roboczym, co wygenerowany plik wykonywalny `saper`.
4. Uruchom aplikację poleceniem:
   ./saper
