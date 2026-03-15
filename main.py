import os
try:
    from PIL import Image, ImageDraw
except ImportError:
    print("Brak biblioteki Pillow. Zainstaluj ją wpisując: pip install pillow")
    exit()

# Funkcja rysująca pojedynczy kwadrat 32x32 piksele
def create_tile(filename, bg_color, text=""):
    # Tworzenie obrazka RGB o wymiarach 32x32
    img = Image.new('RGB', (32, 32), color=bg_color)
    
    # Dodawanie tekstu, jeśli został podany
    if text:
        draw = ImageDraw.Draw(img)
        # Rysowanie tekstu mniej więcej na środku kafelka
        draw.text((12, 8), text, fill=(0, 0, 0)) 
        
    img.save(filename)

# Konfiguracja kolorów dla dwóch różnych skórek
skins = {
    "classic": {"hidden": (192, 192, 192), "empty": (128, 128, 128)},
    "modern": {"hidden": (100, 150, 200), "empty": (50, 100, 150)}
}

# Generowanie plików dla każdej skórki
for skin_name, colors in skins.items():
    path = f"assets/{skin_name}"
    os.makedirs(path, exist_ok=True) # Tworzenie folderów
    
    # Podstawowe kafelki
    create_tile(f"{path}/hidden.png", colors["hidden"])
    create_tile(f"{path}/empty.png", colors["empty"])
    create_tile(f"{path}/mine.png", (255, 50, 50), "X") # Czerwona mina (X)
    create_tile(f"{path}/flag.png", (50, 255, 50), "F") # Zielona flaga (F)
    
    # Cyfry od 1 do 8 oznaczające sąsiadujące miny
    for i in range(1, 9):
        create_tile(f"{path}/{i}.png", colors["empty"], str(i))

print("Gotowe! Wygenerowano foldery assets/classic oraz assets/modern wraz z teksturami.")