
# DiskKnife 🔪  
*A clean cut through partition management.*

DiskKnife is a sleek, menu-driven command-line partition management tool written in pure C 🧠⚙️  
Designed to be beginner-friendly yet powerful, it helps you list block devices, view disk usage, format partitions, and more — right from your terminal.

> ⚠️ **Warning:** This tool is powerful. Use with caution. Always double-check before formatting or modifying real partitions!

---

## 💡 Features

- 🧾 List block devices (via `lsblk`)
- 📊 Show disk usage (via `df`)
- 💽 Format partitions to:
  - `FAT32`
  - `ext4`
  - `NTFS`
- 🔧 Create partition tables (MBR/GPT)
- ➕ Create and delete partitions
- 📁 Mount and unmount partitions safely
- 🔥 **Burn Windows ISOs to USB (UEFI-only)**  
   *Split-WIM compatible with dual FAT32+NTFS partitions!*
- ✅ Confirmation prompts to prevent mistakes
- 🤖 Modular structure with clean, readable code

---

## 🛠️ Usage

### 🧪 Compile
```bash
gcc -o DiskKnife DiskKnife.c
```

### 🚀 Run
```bash
sudo ./DiskKnife
```
> ⚙️ Root access is required for formatting, mounting, and burning ISOs.

---

## 🧪 Safe Testing with Loop Devices

Avoid risking real drives. You can test on virtual block devices:

```bash
dd if=/dev/zero of=disk.img bs=1M count=100
losetup /dev/loop0 disk.img
```

Perfect for experimenting without frying your actual setup 🔥

---

## 🔮 Roadmap

- [x] NTFS support (`mkntfs` via `ntfs-3g`)
- [x] Partition table creation (MBR/GPT)
- [x] Basic partition creation
- [x] **🔥 Windows ISO burning** *(FAT32+NTFS dual partition, UEFI only)*
- [ ] Improved error handling/logging
- [ ] ISO burning UI enhancements
- [ ] Maybe a minimal GUI in the far future

---

## 🤝 Contributing

Wanna join the rebellion? Fork it, clone it, tweak it, and send a PR.  
Good code, good vibes only 🖤

```bash
git clone https://github.com/Gijutsu-tech/DiskKnife.git
```

---

## 🧑‍💻 Author

Made with 💜, Linux, and *a whole lotta printf()*  
By **[Gijutsu-tech](https://github.com/Gijutsu-tech)**  
_“A PC is like an AC. It's useless if you open Windows. - The Arch User”_
