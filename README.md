# DiskKnife 🔪

DiskKnife is a simple, command-line based partition management tool written in C.  
It allows you to safely list block devices, view disk usage, and format partitions with supported filesystems — all in a user-friendly menu-driven interface.

> ⚠️ This tool is intended for educational purposes and should be used with caution. Formatting or modifying real partitions can result in data loss.

---

## Features

- 🧱 List block devices (`lsblk`)
- 💾 Show disk usage (`df`)
- 🧽 Format partitions to:
  - FAT32
  - ext4
- ✅ Confirmation prompts for safety
- 🧪 Safe to test using loop devices like `/dev/loop0`

---

## Usage

### 🔧 Compile the project

```
gcc -o DiskKnife DiskKnife.c
```
### 🚀 Run the tool

```
sudo ./DiskKnife
```
Root access (sudo) is required for formatting partitions.

___

## Notes

To avoid bricking or damaging real devices, you can test with loop devices:
```
dd if=/dev/zero of=disk.img bs=1M count=100
losetup /dev/loop0 disk.img
```

___

## Roadmap

1. NTFS formatting support (check for ntfs-3g)

2. Mount/unmount partitions

3. Create/delete partitions

4. Better error handling

6. Burn ISOs to make bootable drives. (advanced)

6. Add GUI (maybe?)

___

## Contributing

Pull requests are welcome! If you’d like to add features or improve things, feel free to fork and make changes.

___

## Author

Made with ❤️ by Gijutsu-tech!

____
