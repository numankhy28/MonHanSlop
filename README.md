# 2D Monster Hunter Mobile Prototype (Raylib + C++)

2D mobil Monster Hunter esintili oyun prototipi. C++ ve Raylib kullanılarak geliştirilmiştir.

---

## 🎮 Mimari ve Sınıf Yapısı

Proje, katı bir sorumluluk ayrımı (Separation of Concerns) ve tek bir merkezi render pipeline prensibine sadık kalınarak tasarlanmıştır:

1. **`ScreenManager` (Mobil Otomatik Ekran Ölçekleme)**
   - Sanal çözünürlük: **1280x720 (16:9 Landscape)**.
   - Farklı telefon ekranları, çentikler veya pencereler için `scale = min(screenW/virtualW, screenH/virtualH)` ile otomatik letterbox hesaplar.
   - Dokunmatik (Touch) ve fare koordinatlarını fiziksel ekrandan sanal oyun tuvaline (`ScreenToVirtual`) dönüştürür.

2. **`RenderManager` (Merkezi Render Yönetimi)**
   - **Tüm oyun tek bir render fonksiyonunda (`RenderManager::Render`) yönetilir.**
   - Sınıflara (Character, DragonBoss, Map vb.) asla kendi render loopları (`BeginDrawing`/`EndDrawing`) verilmemiştir.
   - 8 yönlü mavi nişan topu (Aim Indicator), yüksek kaliteli 2D anime karakter spriteları, ejderha bossu, genişletilmiş katana kavis dalgası, 8 yöne uçan oklar ve arayüz tek merkezden çizilir.

3. **`VirtualJoystick` & Lineer Cebir Normalizasyonu**
   - Mobil yatay ekranlar için sol altta dokunmatik / sürüklenebilir sanal joystick.
   - **Lineer Cebir ile Normalizasyon:** Çapraz hareketlerde hızın $\sqrt{2} \approx 1.414$ katına çıkması engellenmiştir ($\vec{v} / \|\vec{v}\|$). Hız her yönde sabittir.
   - PC testi için WASD / Ok tuşları desteği.

4. **Yüksek Kaliteli 2D Anime Karakter & Boss Spriteları (`assets/`)**
   - **Kadın Avcı (Female Hunter):**
     - Doğu (East) yönüne bakan tam boy 2D anime karakter sanatı.
     - Kask yok, dalgalanan altın/kumral at kuyruğu saçı.
     - Crop-top ejderha pullu avcı zırhı (açıkta kalan bel/karın), omuzluk, ok kovanı ve devasa oyulmuş **Great Bow (Büyük Avcı Yayı)**.
     - Şeffaf arka planlı yüksek çözünürlüklü PNG formatında (`assets/female_hunter_east.png`).
   - **Erkek Avcı (Male Hunter):**
     - Doğu (East) yönüne bakan tam boy 2D anime savaşçı sanatı.
     - Kask yok, stilize dağınık koyu saçlar, odaklanmış bakışlar.
     - Ronin / Wyvern plaka palto-zırhı, çelik göğüslük ve çizmeler.
     - Dövüş duruşunda kavranmış, ışıltılı keskin çelik **Katana**.
     - Şeffaf arka planlı yüksek çözünürlüklü PNG formatında (`assets/male_hunter_east.png`).
   - **Ejderha Bossu (Rathalos-Stili Ateş Wyvern'i):**
     - Batı (West) yönüne bakan görkemli ve tehditkar 2D anime boss sanatı.
     - İri kıvrık boynuzlar, keskin çene ve duman tüten burun delikleri.
     - Devasa deri kanatlar, kaslı pençeler ve dikenli ağır kuyruk.
     - Şeffaf arka planlı yüksek çözünürlüklü PNG formatında (`assets/dragon_boss.png`).

5. **8 Yönlü Saldırı Vektörü & Mavi Nişan Topu (Aim Indicator)**
   - Karakterin önünde beliren ışıldayan mavi enerji küresi saldırıların nereye gideceğini açıkça gösterir.
   - Oklar ve Katana kavisleri bu 8 yönlü vektöre göre açılanır.
   - Katana kavis menzili **150 piksele** çıkarılmış ve hasarı artırılmıştır.
   - Kaçınma (Evade Dash) **3 saniyelik reload/cooldown süresi** ve görsel dolum sayacı içerir.

6. **Oyun Sonu & Yeniden Başlama (Game Over / Victory)**
   - Ejderha öldüğünde zafer, karakter canı bittiğinde yenilgi ekranı gelir.
   - **"RESTART HUNT"** butonu veya klavyede **`R` / `Space` / `Enter`** ile anında sıfırlanabilir.

---

## 🕹️ Kontroller
- **Hareket / Nişan:** Sol alttaki sanal joystick veya PC'de **W, A, S, D** / **Yön Tuşları**.
- **Saldırı (Attack):** Sağ alttaki kırmızı **SHOOT / SLASH** butonu (veya PC'de **`J`** / **`Enter`** tuşu).
- **Kaçınma (Evade):** Sağ alttaki mavi **EVADE** butonu (veya PC'de **`Space`** / **`K`** tuşu) — **3s bekleme süreli**.
- **Karakter Değiştirme:** Sağ üstteki **CHANGE HUNTER** butonu (veya PC'de **`C`** tuşu).
- **Yeniden Başlatma:** Oyun bittiğinde **"RESTART HUNT"** butonu (veya **`R`** tuşu).

---

## 🚀 Derleme ve Çalıştırma
Klasördeki `build.bat` dosyasına çift tıklayabilir veya terminalde çalıştırabilirsiniz:
```cmd
build.bat --run
```
veya doğrudan:
```cmd
run.bat
```
