int createPartTable(void)
{
    int tableType;
    char partName[50];
    char partTableCommand[100];
    char confirm[4];
    do
    {
        clr_buffer();

        printf("\nEnter the path of the partition (eg., /dev/sdc1): ");
        fgets(partName, sizeof(partName), stdin);
        partName[strcspn(partName, "\n")] = 0;

        printf("Availaible options: \n");
        printf("\t1. MBR (DOS)\n");
        printf("\t2. GPT\n");

        printf("Choose an option (1 or 2): ");
        scanf("%d", &tableType);

        printf("WARNING: Formatting will erase all data on %s. Continue? (yes/no): ", partName);
        clr_buffer();
        fgets(confirm, sizeof(confirm), stdin);

        if (strncmp(confirm, "yes", 3) == 0)
        {
            if (tableType == 1)
            {
                snprintf(partTableCommand, sizeof(partTableCommand), "sudo parted %s mklabel msdos", partName);
                system(partTableCommand);
                return 0;
            }
            else if (tableType == 2)
            {
                snprintf(partTableCommand, sizeof(partTableCommand), "sudo parted %s mklabel gpt", partName);
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
        else
        {
            printf("Aborted.");
            return 1;
        }
    } while (1);
}