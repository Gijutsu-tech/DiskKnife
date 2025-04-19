#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int lsblk(void);
int df(void);
int formatPart(void);
void clr_buffer(void);

int main()
{
    int option;

    printf("\n==== DiskKnife ====\nPrecision partitioning and formatting tool\n");

    do
    {
        printf("\n1. List block devices\n");
        printf("2. Show disk usage\n");
        printf("3. Format a partition\n");
        printf("4. Exit\n");

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
            formatPart();
        }
        else if (option == 4)
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
            continue;
        }

    } while (1);
}

// Function to clear the \n buffer
void clr_buffer(void)
{
    while (getchar() != '\n')
        ; // Clear out the buffer
}