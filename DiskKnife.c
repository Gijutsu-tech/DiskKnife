#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int lsblk(void);
int df(void);
int formatPart(void);
int mountPart(char partName[50]);
int unmountPart(char partName[50]);
int handleMountUnmount();
void clr_buffer(void);

int main(void)
{
    int option;

    printf("\n<==== DiskKnife ====>\nPrecision partitioning and formatting tool\n");

    do
    {
        printf("\n1. List block devices\n");
        printf("2. Show disk usage\n");
        printf("3. Mount/unmount partitions\n");
        printf("4. Format partitions\n");
        printf("5. Exit\n");

        printf("\nChoose an option: ");
        if (scanf("%d", &option) != 1)
        {
            // Input wasn't a number, clear buffer
            clr_buffer();
            printf("\nInvalid input. Please enter a valid number.\n");
            continue; // show menu again once
        }

        if (option == 1)
        {
            lsblk();
        }
        else if (option == 2)
        {
            df();
        }
        else if (option == 3)
        {
            handleMountUnmount();
        }
        else if (option == 4)
        {
            formatPart();
        }
        else if (option == 5)
        {
            printf("\nGoodbye!\n");
            break;
        }
        else
        {
            printf("\nInvalid input. Please enter a valid number.\n");
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
        int result = system("lsblk -o LABEL,NAME,SIZE,FSTYPE,MOUNTPOINT");

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

// Format partitions with common filesystems
int formatPart(void)
{
    clr_buffer();

    char partName[50];
    int fileSystem;
    char confirm[4];
    char command[100];
    int result;

    printf("Enter the path of the partition (eg., /dev/sdc1): ");
    fgets(partName, sizeof(partName), stdin);
    partName[strcspn(partName, "\n")] = 0;

    do
    {
        printf("Availaible filesystem types: \n");
        printf("\t1. FAT32\n");
        printf("\t2. ext4\n");

        printf("Choose the filesystem for the partition from the options listed above: ");
        scanf("%d", &fileSystem);

        if (fileSystem == 1)
        {
            printf("WARNING: Formatting will erase all data on %s. Continue? (yes/no): ", partName);
            clr_buffer();
            fgets(confirm, sizeof(confirm), stdin);

            unmountPart(partName);

            if (strncmp(confirm, "yes", 3) == 0)
            {
                printf("Formatting...\n");
                sprintf(command, "sudo mkfs.vfat %s", partName);
                result = system(command);
                if (result == 0)
                {
                    printf("Formatted %s to FAT32 filesystem.\n", partName);
                }
                break;
            }
            else
            {
                printf("Aborted formatting.\n");
                break;
            }
        }
        else if (fileSystem == 2)
        {
            printf("WARNING: Formatting will erase all data on %s. Continue? (yes/no): ", partName);
            clr_buffer();
            fgets(confirm, sizeof(confirm), stdin);

            unmountPart(partName);

            if (strncmp(confirm, "yes", 3) == 0)
            {
                printf("Formatting...\n");
                sprintf(command, "sudo mkfs.ext4 %s", partName);
                result = system(command);
                if (result == 0)
                {
                    printf("Formatted %s to ext4 filesystem.\n", partName);
                }
                break;
            }
            else
            {
                printf("Aborted formatting.\n");
                break;
            }
        }
        else
        {
            printf("Invalid option!\n");
            clr_buffer();
        }

    } while (1);
    
}

// Mount partition to the /run/media/user_name/drive_label
int mountPart(char partName[50])
{
    char mountCommand[100];
    snprintf(mountCommand, sizeof(mountCommand), "udisksctl mount -b %s", partName);
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
int unmountPart(char partName[50])
{
    // Unmount the partition first
    char unmountCommand[100];
    snprintf(unmountCommand, sizeof(unmountCommand), "umount %s", partName);
    int unmountResult = system(unmountCommand);

    if (unmountResult != 0)
    {
        return 1;
    }
    else
    {
        printf("Successfully unmounted %s\n", partName);
        return 0;
    }
}

// Handle mounting and unmounting
int handleMountUnmount()
{
    int mountingOption;
    char partName[50];

    do
    {
        clr_buffer();

        printf("\nEnter the path of the partition (eg., /dev/sdc1): ");
        fgets(partName, sizeof(partName), stdin);
        partName[strcspn(partName, "\n")] = 0;

        printf("Availaible options: \n");
        printf("\t1. Mount the partition.\n");
        printf("\t2. Unmount the partition.\n");

        printf("Choose an option (1 or 2): ");
        scanf("%d", &mountingOption);

        if (mountingOption == 1)
        {
            mountPart(partName);
            return 0;
        }
        else if (mountingOption == 2)
        {
            int unmountPartResult = unmountPart(partName);
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

// Function to clear the \n buffer
void clr_buffer(void)
{
    while (getchar() != '\n')
        ; // Clear out the buffer
}
