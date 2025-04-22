#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int lsblk(void);

int df(void);

int getFormatPartInput(void);
int formatPart(char partPath[50], int fileSystem);

int mountPart(char partPath[50]);
int unmountPart(char partPath[50]);
int handleMountUnmount(void);

int getPartTableInput(void);
int createPartTable(int tableType, char devicePath[50]);
void clr_buffer(void);

// Main function
int main(void)
{
    int option;

    printf("\n<==== DiskKnife ====>\nPrecision partitioning and formatting tool\n");

    do
    {
        printf("\n1. List block devices\n");
        printf("2. Show disk usage\n");
        printf("3. Mount/unmount partitions\n");
        printf("4. Create and modify partition tables\n");
        printf("5. Format partitions\n");
        printf("6. Exit\n");

        printf("\nChoose an option: ");
        if (scanf("%d", &option) != 1)
        {
            // Input wasn't a number, clear buffer
            clr_buffer();
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
            printf("Goodbye!\n");
            return 0;

        default:
            break;
        }
    } while (1);

    return 0;
}

// List block devices
int lsblk(void)
{
    char loopLsblk[2];

    printf("Listing available block devices...\n");

    do
    {
        clr_buffer(); // clear \n buffer

        // Run lsblk to list devices
        int result = system("lsblk -o LABEL,NAME,SIZE,PTTYPE,FSTYPE,MOUNTPOINT");

        // Check if the command was successful
        if (result != 0)
        {
            printf("Error: lsblk command failed!\n");
            return 1;
        }

        // Ask the user if they want to run the command again
        printf("\nDo you want to list the block devices again? (y/n): ");

        fgets(loopLsblk, sizeof(loopLsblk), stdin);

        if (loopLsblk[0] == 'y' || loopLsblk[0] == 'Y')
        {
            continue;
        }
        else if (loopLsblk[0] == 'n' || loopLsblk[0] == 'N')
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
}

// Show all mounted disks and their disk usage
int df(void)
{
    char loopDf[2];

    printf("Listing mounted disks and their usage...\n\n");

    do
    {
        clr_buffer(); // clear \n buffer

        // Run df to list mounted disk and show disk usage
        int result = system("df -h --output=source,size,used,avail,pcent,target");

        // Check if the command was successful
        if (result != 0)
        {
            printf("Error: lsblk command failed!\n");
            return 1;
        }

        // Ask the user if they want to run the command again
        printf("\nDo you want to show disk usage again? (y/n): ");

        fgets(loopDf, sizeof(loopDf), stdin);
        printf("\n");

        if (loopDf[0] == 'y' || loopDf[0] == 'Y')
        {
            continue;
        }
        else if (loopDf[0] == 'n' || loopDf[0] == 'N')
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
}

// Get the input for formatPart()
int getFormatPartInput(void)
{
    clr_buffer();

    char partPath[50];
    int fileSystem;
    char confirm[4];

    printf("Enter the path of the partition (eg., /dev/sdc1): ");
    fgets(partPath, sizeof(partPath), stdin);
    partPath[strcspn(partPath, "\n")] = 0;

    printf("Availaible filesystem types: \n");
    printf("\t1. FAT32\n");
    printf("\t2. ext4\n");

    printf("Choose the filesystem for the partition from the options listed above: ");
    scanf("%d", &fileSystem);

    printf("WARNING: Formatting will erase all data on %s. Continue? (yes/no): ", partPath);
    clr_buffer();
    fgets(confirm, sizeof(confirm), stdin);

    if (strncmp(confirm, "yes", 3) == 0)
    {
        formatPart(partPath, fileSystem);
    }
    else
    {
        printf("Aborted formatting.\n");
    }
}

// Format partitions with available filesystems
int formatPart(char partPath[50], int fileSystem)
{
    char command[100];
    int result;

    if (fileSystem == 1)
    {
        unmountPart(partPath);
        printf("Formatting...\n");
        sprintf(command, "sudo mkfs.vfat %s", partPath);
        result = system(command);
        if (result == 0)
        {
            printf("Formatted %s to FAT32 filesystem.\n", partPath);
        }
    }
    else if (fileSystem == 2)
    {

        unmountPart(partPath);
        printf("Formatting...\n");
        sprintf(command, "sudo mkfs.ext4 %s", partPath);
        result = system(command);
        if (result == 0)
        {
            printf("Formatted %s to ext4 filesystem.\n", partPath);
        }
    }
    else
    {
        printf("Invalid option!\n");
        clr_buffer();
    }
}

// Mount partition to the /run/media/user_name/drive_label
int mountPart(char partPath[50])
{
    char mountCommand[100];
    snprintf(mountCommand, sizeof(mountCommand), "udisksctl mount -b %s", partPath);
    int mountResult = system(mountCommand);

    if (mountResult != 0)
    {
        printf("Mounting the partition has failed!");
        return 1;
    }
    else
    {
        return 0;
    }
}

// Unmount partition if mounted
int unmountPart(char partPath[50])
{
    // Unmount the partition first
    char unmountCommand[100];
    snprintf(unmountCommand, sizeof(unmountCommand), "umount %s", partPath);
    int unmountResult = system(unmountCommand);

    if (unmountResult != 0)
    {
        return 1;
    }
    else
    {
        printf("Successfully unmounted %s\n", partPath);
        return 0;
    }
}

// Handle mounting and unmounting
int handleMountUnmount(void)
{
    int mountingOption;
    char partPath[50];

    do
    {
        clr_buffer();

        printf("\nEnter the path of the partition (eg., /dev/sdc1): ");
        fgets(partPath, sizeof(partPath), stdin);
        partPath[strcspn(partPath, "\n")] = 0;

        printf("Availaible options: \n");
        printf("\t1. Mount the partition.\n");
        printf("\t2. Unmount the partition.\n");

        printf("Choose an option (1 or 2): ");
        scanf("%d", &mountingOption);

        if (mountingOption == 1)
        {
            mountPart(partPath);
            return 0;
        }
        else if (mountingOption == 2)
        {
            int unmountPartResult = unmountPart(partPath);
            if (unmountPartResult != 0)
            {
                printf("Unmounting the partition has been failed!\n");
                return 1;
            }
            return 0;
        }
        else
        {
            printf("Invalid option chosen, choose 1 or 2!\n");
            clr_buffer();
            return 1;
        }

    } while (1);
}

// Get input for createPartTable()
int getPartTableInput(void)
{
    int tableType;
    char devicePath[50];
    char confirm[4];
    do
    {
        clr_buffer();

        printf("\nEnter the path of the device (eg., /dev/sdc): ");
        fgets(devicePath, sizeof(devicePath), stdin);
        devicePath[strcspn(devicePath, "\n")] = 0;

        printf("Availaible options: \n");
        printf("\t1. MBR (DOS)\n");
        printf("\t2. GPT\n");

        printf("Choose an option (1 or 2): ");
        scanf("%d", &tableType);

        printf("WARNING: Creating a partition table will erase all data on %s. Continue? (yes/no): ", devicePath);
        clr_buffer();
        fgets(confirm, sizeof(confirm), stdin);

        if (strncmp(confirm, "yes", 3) == 0)
        {
            createPartTable(tableType, devicePath);
        }
        else
        {
            printf("Aborted.");
            return 1;
        }
    } while (1);
}

// Create a partition table
int createPartTable(int tableType, char devicePath[50]){
    char partTableCommand[100];
    if (tableType == 1)
    {
        snprintf(partTableCommand, sizeof(partTableCommand), "sudo parted %s mklabel msdos", devicePath);
        system(partTableCommand);
        return 0;
    }
    else if (tableType == 2)
    {
        snprintf(partTableCommand, sizeof(partTableCommand), "sudo parted %s mklabel gpt", devicePath);
        system(partTableCommand);
        return 0;
    }
    else
    {
        printf("Invalid option chosen, choose 1 or 2!\n");
        clr_buffer();
        return 1;
    }
}

// Function to clear the \n buffer
void clr_buffer(void)
{
    while (getchar() != '\n')
        ; // Clear out the buffer
}
