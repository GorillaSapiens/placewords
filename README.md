# S2 PlaceWords — The Free & Open What3Words Alternative

**S2 PlaceWords** (or just **PlaceWords**) is a fully open-source system for uniquely identifying every 5-meter square on Earth with a simple series of 3 words.  
Unlike What3Words, PlaceWords is **free, offline-friendly, patent-safe, and community-driven** — no proprietary lock-in, no secret algorithms, no lawsuits.

---

## 🚀 Why PlaceWords?
- **Free & Open** – Licensed under Apache 2.0, so you can use, modify, and share without fear.  
- **Offline-Capable** – Works without an internet connection; ideal for remote work, disaster relief, and outdoor adventures.  
- **Patent-Safe** – Does **not** infringe on [What3Words patents](https://patents.google.com/patent/CA2909524A1/en).  
- **Interoperable & Future-Proof** – Wordlist supports replacements and multiple languages; older coordinates remain readable.  
- **No Central Control** – Self-host, embed in devices, integrate into maps — all without asking permission.

---

## 📍 How It Works
PlaceWords uses [Google’s S2 geometry system](https://s2geometry.io/) to divide the planet into tiny cells:  
1. A Latitude/Longitude pair is converted to an **S2 Cell ID**.  
2. The 3 “face” bits are separated from the following 42 bits (≈ 5m resolution).  
3. These 42 bits are shuffled with a Linear Feedback Shift Register (LFSR) for even distribution.  
4. The result produces **three 15-bit numbers**, each mapped to a word from a **32,768-word list**.  
5. (Optional) A **4th word** can be added for sub-5 cm accuracy.  

The process is **fully reversible**, letting you go from words back to coordinates.

---

## 🌍 Features
- Minimalist built-in S2 library for portability (swap with full Google S2 if desired).  
- Custom wordlists (replace offensive words, add new languages).  
- Stable results — older PlaceWords will remain valid in future versions.  
- Works anywhere on Earth, even without GPS service.

---

## 🔧 Getting Started
Clone the repo:  
```bash
git clone https://github.com/GorillaSapiens/placewords.git
```
Build and run (see included examples).

---

## 🤝 Community & Contributions
We welcome contributions, bug reports, and new wordlists.  
If you build something cool with PlaceWords, let us know — we’ll feature it.

**Contact:** adam@wozniakconsulting.com (Adam Wozniak)  
**Bitcoin donations:** `1FDHVjmLSMQF2kwtPYkBufMbz8CvN5aw2f`
