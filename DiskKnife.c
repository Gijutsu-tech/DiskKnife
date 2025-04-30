#include <stdio.h>  // Standard input and output
#include <stdlib.h> // For system() function
#include <string.h> // For more convinient string formatting
#include <unistd.h> // For checking if running as root

// For better convinience: as format function takes an integer as the argument to select file system -
#define FORMAT_FAT32 1
#define FORMAT_EXT4 2
#define FORMAT_NTFS 3
// For better convinience:
#define MBR 1
#define GPT 2

// Function declarations
void displayMenu(void);

int lsblk(void);

int df(void);

int getFormatPartInput(void);
int formatPart(char partition[50], int fileSystem);

int mountPart(char partition[50]);
int unmountPart(char partition[50]);
int handleMountUnmount(void);

int getPartTableInput(void);
int createPartTable(int tableType, char device[50]);
void clrBuffer(void);

int getBurnIsoInput(void);
int burnIso(char drive[50], char isoPath[100]);

// Main function
int main(void)
{
    printf("\n<==== DiskKnife ====>\nPrecision partitioning and formatting tool\n\n");

    // Check if the program is running as root
    if (getuid() == 0)
    {
        displayMenu();
    }
    else
    {
        fprintf(stderr, "Run DiskKnife as \"sudo ./DiskKnife\"\n");
        return 1;
    }

    return 0;
}

// Display main menu
void displayMenu(void)
{
    int option;

    // Do-while loop to show the menu again, if input was invalid -
    do
    {
        printf("1. List block devices\n");
        printf("2. Show disk usage\n");
        printf("3. Mount/unmount partitions\n");
        printf("4. Create and modify partition tables\n");
        printf("5. Format partitions\n");
        printf("6. Burn windows ISO\n");
        printf("7. Exit\n");

        printf("\nChoose an option: ");
        if (scanf("%d", &option) != 1)
        {
            // Input wasn't valid, clear buffer
            clrBuffer();
            printf("\nInvalid input. Please enter a valid number.\n");
            continue; // show menu again once
        }

        switch (option)
        {
        case 1:
            lsblk();
            break;

        case 2:
            df();
            break;

        case 3:
            handleMountUnmount();
            break;

        case 4:
            getPartTableInput();
            break;

        case 5:
            getFormatPartInput();
            break;

        case 6:
            getBurnIsoInput();
            break;

        case 7:
            printf("Goodbye!\n");
            exit(0); // exit(0); exits the whole program unlike return 0; exiting the function only

        default:
            break;
        }
    } while (1);
}

// List block devices
int lsblk(void)
{
    char repeat;

    printf("Listing available block devices...\n");

    do
    {
        clrBuffer(); // clear buffer

        // Run lsblk to list devices
        int result = system("lsblk -o LABEL,NAME,SIZE,PTTYPE,FSTYPE,MOUNTPOINT");

        // Check if the command was successful
        if (result != 0)
        {
            printf("Error: lsblk command failed!\n");
            exit(1);
        }

        // Ask the user if they want to run the command again
        printf("\nDo you want to list the block devices again? (y/n): ");

        scanf("%c", &repeat);

        if (repeat == 'y' || repeat == 'Y')
        {
            continue;
        }
        else if (repeat == 'n' || repeat == 'N')
        {
            break;
        }
        else
        {
            printf("Invalid input\n");
            break;
        }
    } while (1);

    printf("\n");
    clrBuffer();
    return 0;
}

// Show all mounted disks and their disk usage
int df(void)
{
    char repeat;

    printf("Listing mounted disks and their usage...\n\n");

    do
    {
        clrBuffer();

        // Run df to list mounted disk and show disk usage and store the returned value in 'result' variable
        int result = system("df -h --output=source,size,used,avail,pcent,target");

        // Check if the command was successful
        if (result != 0)
        {
            printf("Error: df command failed!\n");
            exit(1);
        }

        // Ask the user if they want to run the command again
        printf("\nDo you want to show disk usage again? (y/n): ");
        scanf("%c", &repeat);
        printf("\n");

        if (repeat == 'y' || repeat == 'Y')
        {
            continue;
        }
        else if (repeat == 'n' || repeat == 'N')
        {
            break;
        }
        else
        {
            printf("Invalid input\n");
            break;
        }
    } while (1);

    printf("\n");
    clrBuffer();
    return 0;
}

// NOTE: The functions for gathering input and those for executing operations are separated to enhance reusability and maintainability.

// Get the user input for formatPart()
int getFormatPartInput(void)
{
    clrBuffer();

    char partition[50]; // Path of the partition which is to be formatted
    int fileSystem;     // Integer representing the filesystem type
    char confirm[4];

    // Get the partition path from the user
    printf("Enter the path of the partition (eg., /dev/sdc1): ");
    fgets(partition, sizeof(partition), stdin);
    partition[strcspn(partition, "\n")] = 0; // Remove \n from partition

    // Show menu of available filesystems and get the filesystem type
    printf("Available filesystem types: \n");
    printf("\t1. FAT32\n");
    printf("\t2. ext4\n");
    printf("\t3. NTFS\n");
    printf("Choose the filesystem for the partition from the options listed above: ");
    scanf("%d", &fileSystem);

    // Confirm before formatting
    printf("WARNING: Formatting will erase all data on %s. Continue? (yes/no): ", partition);
    clrBuffer();
    fgets(confirm, sizeof(confirm), stdin);

    // Call formatPart() only if user types "yes" exactly or abort
    if (strncmp(confirm, "yes", 3) == 0)
    {
        formatPart(partition, fileSystem);
    }
    else
    {
        printf("Aborted formatting.\n");
    }
}

// Format partitions with available filesystems
int formatPart(char partition[50], int fileSystem)
{
    char command[100]; // Command to be executed to format the partition
    int result;        // Variable to store the returned value obtained after formatting

    unmountPart(partition); // Unmount partition before formatting

    printf("Formatting...");
    // Format partition based on the filesystem number obtained from getFormatPartInput()
    if (fileSystem == FORMAT_FAT32)
    {
        sprintf(command, "mkfs.vfat -F 32 %s", partition); // Write the command to be used to format partition
        result = system(command);                          // Run the command and store the returned value in result
        if (result == 0)
        {
            printf("Formatted %s to FAT32 filesystem.\n", partition);
        }
        else
        {
            printf("An error occurred while formatting!");
            exit(1);
        }
    }
    else if (fileSystem == FORMAT_EXT4)
    {
        sprintf(command, "mkfs.ext4 %s", partition);
        result = system(command);
        if (result == 0)
        {
            printf("Formatted %s to ext4 filesystem.\n", partition);
        }
        else
        {
            printf("An error occurred while formatting!");
            exit(1);
        }
    }
    else if (fileSystem == FORMAT_NTFS)
    {
        sprintf(command, "mkntfs --fast %s 2>/dev/null", partition);
        result = system(command);
        if (result == 0)
        {
            printf("Formatted %s to ntfs filesystem.\n", partition);
        }
        else
        {
            printf("An error occurred while formatting!");
            exit(1);
        }
    }
    else
    {
        // Abort if an invalid option was selected in getFormatPartInput()
        printf("Invalid option!\n");
        clrBuffer();
        return 1;
    }
    return 0;
}

// Mount partition to the /run/media/user_name/drive_label
int mountPart(char partition[50])
{
    char mountCommand[100]; // Command to be executed to mount the partition
    snprintf(mountCommand, sizeof(mountCommand), "udisksctl mount -b %s", partition);
    system(mountCommand); // Run the command

    return 0;
}

// Unmount partition if mounted
int unmountPart(char partition[50])
{
    char unmountCommand[100]; // Command to be executed to unmount the partition
    snprintf(unmountCommand, sizeof(unmountCommand), "umount %s > /dev/null 2>&1", partition);
    system(unmountCommand); // Run the command and store the returned value in unmountResult

    printf("Successfully unmounted %s\n", partition);
    return 0;
}

// Handle mounting and unmounting
int handleMountUnmount(void)
{
    int option;         // Variable to store the option for mount or unmount
    char partition[50]; // Partition path
    char repeat;

    // Do-while loop to show the interface again, if the user wants to mount or unmount again
    do
    {
        clrBuffer();

        // Get the path of the partition and remove \n from it
        printf("Enter the path of the partition (eg., /dev/sdc1): ");
        fgets(partition, sizeof(partition), stdin);
        partition[strcspn(partition, "\n")] = 0;

        // Display the menu of available options
        printf("Available options: \n");
        printf("\t1. Mount the partition.\n");
        printf("\t2. Unmount the partition.\n");

        printf("Choose an option (1 or 2): ");
        scanf("%d", &option);

        // Mount or unmount based on 1 or 2 respectively
        if (option == 1)
        {
            int mountPartResult = mountPart(partition);
            if (mountPartResult != 0)
            {
                printf("Mounting the partition has been failed!\n");
                exit(1);
            }
        }
        else if (option == 2)
        {
            int unmountPartResult = unmountPart(partition);
            if (unmountPartResult != 0)
            {
                printf("Unmounting the partition has been failed!\n");
                exit(1);
            }
        }
        else
        {
            printf("Invalid option chosen, choose 1 or 2!\n");
            clrBuffer();
            return 1;
        }

        // Ask the user if they want to run the command again
        printf("\nDo you want to mount/unmount any other partition? (y/n): ");
        clrBuffer();
        scanf("%c", &repeat);

        if (repeat == 'y' || repeat == 'Y')
        {
            continue;
        }
        else if (repeat == 'n' || repeat == 'N')
        {
            break;
        }
        else
        {
            printf("Invalid input\n");
            break;
        }
        clrBuffer();
    } while (1);
}

// Get the user input for createPartTable()
int getPartTableInput(void)
{
    int tableType;   // Partition table type (i.e., MBR or GPT)
    char device[50]; // Path of the device
    char confirm[4];
    char repeat;

    // Do-while loop to show the interface again, if the user wants to create partition table again
    do
    {
        clrBuffer();

        // Get the path of device to create partition table in
        printf("\nEnter the path of the device (eg., /dev/sdc): ");
        fgets(device, sizeof(device), stdin);
        device[strcspn(device, "\n")] = 0;

        // Show menu of available options of parition tables
        printf("Available options: \n");
        printf("\t1. MBR (DOS)\n");
        printf("\t2. GPT\n");

        printf("Choose an option (1 or 2): ");
        scanf("%d", &tableType);

        // Show warning
        printf("WARNING: This will erase all data on %s. Continue? (yes/no): ", device);
        clrBuffer();
        fgets(confirm, sizeof(confirm), stdin);

        // Create partition table only if the user enter 'yes'
        if (strncmp(confirm, "yes", 3) == 0)
        {
            createPartTable(tableType, device);
        }
        else
        {
            printf("Aborted.");
            return 1;
        }

        // Ask the user if they want to run the command again
        printf("\nDo you want to mount/unmount any other partition? (y/n): ");
        clrBuffer();
        scanf("%c", &repeat);

        if (repeat == 'y' || repeat == 'Y')
        {
            continue;
        }
        else if (repeat == 'n' || repeat == 'N')
        {
            break;
        }
        else
        {
            printf("Invalid input\n");
            break;
        }
        clrBuffer();
    } while (1);
}

// Create a partition table
int createPartTable(int tableType, char device[50])
{
    char partTableCommand[100]; // Command to create partition table
    int result;                 // Result of creating partition table

    // Format partition based on the filesystem number obtained from getPartTableInput()
    if (tableType == MBR)
    {
        // Write the command to be used to format -
        snprintf(partTableCommand, sizeof(partTableCommand), "parted %s mklabel msdos", device);
        result = system(partTableCommand); // Run the command
        return 0;
    }
    else if (tableType == GPT)
    {
        snprintf(partTableCommand, sizeof(partTableCommand), "parted %s mklabel gpt", device);
        result = system(partTableCommand);
        return 0;
    }
    else
    {
        // Abort the function if invalid option is chosen in getPartTableInput()
        printf("Invalid option chosen, choose 1 or 2!\n");
        clrBuffer();
        return 1;
    }
}

// Get the user input for burnISO()
int getBurnIsoInput(void)
{
    clrBuffer();

    char drive[50];    // Path of the usb drive
    char isoPath[100]; // Path of the ISO

    // Get the path of the USB drive from user and remove \n from it
    printf("Enter the path of the USB drive: ");
    fgets(drive, sizeof(drive), stdin);
    drive[strcspn(drive, "\n")] = 0;

    // Get the path of the ISO from user and remove \n from it
    printf("Enter the path of the ISO file (*Path length MUST be lsser than 100 characters*): ");
    fgets(isoPath, sizeof(isoPath), stdin);
    isoPath[strcspn(isoPath, "\n")] = 0;

    // Burn the ISO
    burnIso(drive, isoPath);
}

// Burn windows ISOs!!!!
int burnIso(char drive[50], char isoPath[100])
{
    /* NOTE : This function is designed to burn Windows ISOs ONLY ON UEFI systems with following steps -
            1. Create GPT partition table
            2. Create a FAT32 partition of 100MiB to store the bootx64.efi
            3. Create NTFS partition taking rest of the space to store files which may have file size larger than 4GB such as install.wim
            4. Mount the EFI(FAT32) and Windows(NTFS) partition to /mnt/efi and /mnt/windows and mount the ISO to /mnt/iso
            5. Copy bootx64.efi to EFI partition and rest of the files to Windows partitions
            6. Profit, lol */

    char cmd[512];  // Command to burn the ISO
    char part1[51]; // Path of the first partition - FAT32 filesystem
    char part2[51]; // Path of the second partition - NTFS filesystem

    printf("The drive is %s and ISO is %s\n", drive, isoPath);

    // Create GPT partition table
    printf("[1/5] : Creating partition table...\n");
    createPartTable(GPT, drive);

    // Create EFI (FAT32) and windows (NTFS) partitions with proper alignment
    printf("[2/5] : Creating partitions...\n");
    snprintf(cmd, sizeof(cmd), "parted %s --align optimal mkpart ESP fat32 1MiB 101MiB", drive);
    system(cmd); // Create EFI partition of 100MB for bootx64.efi
    snprintf(cmd, sizeof(cmd), "parted %s set 1 boot on", drive);
    system(cmd); // Set boot flag
    snprintf(cmd, sizeof(cmd), "parted %s set 1 esp on", drive);
    system(cmd); // Set esp flag - optional
    snprintf(cmd, sizeof(cmd), "parted %s --align optimal mkpart primary ntfs 101MiB 100%%", drive);
    system(cmd); // Create windows partition occupying rest of the space on drive

    // Refresh partition table
    system("partprobe");

    // Define partition paths
    if (strncmp(drive, "/dev/loop", 9) == 0)
    {
        snprintf(part1, sizeof(part1), "%sp1", drive);
        snprintf(part2, sizeof(part2), "%sp2", drive);
    }
    else if (strncmp(drive, "/dev/nvme", 9) == 0)
    {
        printf("You MUST select an USB drive!");
        exit(1);
    }
    else
    {
        snprintf(part1, sizeof(part1), "%s1", drive);
        snprintf(part2, sizeof(part2), "%s2", drive);
    }

    // Create mount directories
    system("mkdir -p /mnt/efi");
    system("mkdir -p /mnt/windows");
    system("mkdir -p /mnt/iso");

    // Format partitions
    printf("[3/5] : Formatting partitions...\n");
    formatPart(part1, FORMAT_FAT32); // Format EFI partition as FAT32
    formatPart(part2, FORMAT_NTFS);  // Format windows partition as NTFS

    // Mount partitions
    printf("[4/5] : Mounting partitions...\n");
    snprintf(cmd, sizeof(cmd), "mount %s /mnt/efi", part1);
    system(cmd); // Mount EFI partition at /mnt/efi
    snprintf(cmd, sizeof(cmd), "mount %s /mnt/windows", part2);
    system(cmd); // Mount windows partition at windows

    // Mount the ISO
    snprintf(cmd, sizeof(cmd), "mount -o loop %s /mnt/iso", isoPath);
    system(cmd); // Mount the ISO at /mnt/iso

    // Copy files from ISO to the NTFS partition
    printf("[5/5] : Burning the ISO...\n");
    system("rsync -ah --progress /mnt/iso/* /mnt/windows");

    // Copy the bootloader to the EFI partition
    system("mkdir -p /mnt/efi/BOOT");
    system("rsync -ah --progress /mnt/windows/efi/boot/bootx64.efi /mnt/efi/BOOT/");

    // Unmount partitions
    system("umount /mnt/iso");
    system("umount /mnt/efi");
    system("umount /mnt/windows");

    return 0;
}

// Function to clear the buffer
void clrBuffer()
{
    int c;
    // Check if there's input to clear (non-empty buffer)
    if (feof(stdin) || ferror(stdin))
        return; // Exit if EOF or error
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}
