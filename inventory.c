#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the structure for an inventory item
struct item {
    int itemID;
    char name[50];
    char description[100];
    int quantity;
    float price;
    char category[50];
    int expiryDate[3];
};

// Functions used
void addNewItem(FILE *fp);
void updateItem(FILE *fp);
void deleteItem(FILE *fp);
void displayInventory(FILE *fp);
void identifySurplusItems(FILE *fp);
void listLowQuantityItems(FILE *fp);
void searchItemByID(FILE *fp);
void expireditems(FILE *fp); 
void searchItemByCategory(FILE *fp);
void TotalcostofInventory(FILE *fp);

int main() {
    FILE *fp;
    fp = fopen("inventory.txt", "r+");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    int choice;
    do {
        printf("\n==== Inventory Management System ====\n");
        printf("\n1. Add item\n");
        printf("2. Update item\n");
        printf("3. Delete item\n");
        printf("4. Display inventory\n");
        printf("5. Identify surplus items\n");
        printf("6. List low-quantity range items\n");
        printf("7. Search Item by ID\n");
        printf("8. Expired items \n");
        printf("9. search item by category\n");
        printf("10. Total cost of inventory\n");

        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addNewItem(fp);
                break;
            case 2:
                updateItem(fp);
                break;
            case 3:
                deleteItem(fp);
                break;
            case 4:
                displayInventory(fp);
                break;
            case 5:
                identifySurplusItems(fp);
                break;
            case 6:
                listLowQuantityItems(fp);
                break;
            case 7:
                searchItemByID(fp);
                break;
            case 8:
                expireditems(fp);   
                break;
            case 9:
                searchItemByCategory(fp);   
                break; 
            case 10:
                TotalcostofInventory(fp);   
                break;    
            case 0:
                printf("Exiting the program. Goodbye!\n");
                break;
            default:
                printf("Invalid choice! Please enter a valid option.\n");
                break;
        }
    } while (choice != 0);

    fclose(fp);
    return 0;
}


//Adding a new item
void addNewItem(FILE *fp) {
    struct item newItem;

    printf("\nEnter Item ID: ");
    scanf("%d", &newItem.itemID);

    getchar(); // Consume any leftover newline character from the buffer
    printf("Enter Item Name: ");
    fgets(newItem.name, sizeof(newItem.name), stdin);
    newItem.name[strcspn(newItem.name, "\n")] = 0; // Remove newline character at the end

    printf("Enter Description: ");
    fgets(newItem.description, sizeof(newItem.description), stdin);
    newItem.description[strcspn(newItem.description, "\n")] = 0; // Remove newline character at the end


    printf("Enter Quantity: ");
    scanf("%d", &newItem.quantity);

    printf("Enter Price: ");
    scanf("%f", &newItem.price);

    getchar(); // Consume any leftover newline character from the buffer
    printf("Enter Category: ");
    fgets(newItem.category, sizeof(newItem.category), stdin);
    newItem.category[strcspn(newItem.category, "\n")] = 0; // Remove newline character at the end

    printf("Enter Expiry Date (dd mm yyyy): ");
    scanf("%d %d %d", &newItem.expiryDate[0], &newItem.expiryDate[1], &newItem.expiryDate[2]);

    fseek(fp, 0, SEEK_END); // Move to the end of the file
   fprintf(fp, "%d \"%s\" \"%s\" %d %.2f \"%s\" %d %d %d\n", newItem.itemID, newItem.name, newItem.description,
            newItem.quantity, newItem.price, newItem.category, newItem.expiryDate[0], newItem.expiryDate[1], newItem.expiryDate[2]);
    printf("\nItem added successfully!\n");
}

//Updating an item
void updateItem(FILE *fp) {

    int searchID;
    printf("\nEnter Item ID to update: ");
    scanf("%d", &searchID);

    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("Error creating temporary file!\n");
        return;
    }

    struct item currentItem;
    int found = 0;

    rewind(fp); // Go to the beginning of the file

    while (fscanf(fp, "%d \"%49[^\"]\" \"%99[^\"]\" %d %f \"%49[^\"]\" %d %d %d", 
                  &currentItem.itemID, currentItem.name, currentItem.description,
                  &currentItem.quantity, &currentItem.price, currentItem.category, 
                  &currentItem.expiryDate[0], &currentItem.expiryDate[1], &currentItem.expiryDate[2]) !=EOF) {
        if (currentItem.itemID == searchID) {
            found = 1;

            getchar(); // Clear input buffer

            printf("Enter updated Item Name: ");
            fgets(currentItem.name, sizeof(currentItem.name), stdin);
            currentItem.name[strcspn(currentItem.name, "\n")] = 0;

            printf("Enter updated Description: ");
            fgets(currentItem.description, sizeof(currentItem.description), stdin);
            currentItem.description[strcspn(currentItem.description, "\n")] = 0;

            printf("Enter updated Quantity: ");
            scanf("%d", &currentItem.quantity);

            printf("Enter updated Price: ");
            scanf("%f", &currentItem.price);

            getchar(); // Clear input buffer

            printf("Enter updated Category: ");
            fgets(currentItem.category, sizeof(currentItem.category), stdin);
            currentItem.category[strcspn(currentItem.category, "\n")] = 0;

            printf("Enter updated Expiry Date (dd mm yyyy): ");
            scanf("%d %d %d", &currentItem.expiryDate[0], &currentItem.expiryDate[1], &currentItem.expiryDate[2]);
        }
        fprintf(tempFile, "%d \"%s\" \"%s\" %d %.2f \"%s\" %d %d %d\n", 
                currentItem.itemID, currentItem.name, currentItem.description,
                currentItem.quantity, currentItem.price, currentItem.category, 
                currentItem.expiryDate[0], currentItem.expiryDate[1], currentItem.expiryDate[2]);
    }

    fclose(fp);
    fclose(tempFile);

    if (found) {
        printf("Item updated successfully!\n");
        remove("inventory.txt");
        rename("temp.txt", "inventory.txt");
    } else {
        printf("Item with ID %d not found!\n", searchID);
        remove("temp.txt");
    }

    // Reopen the file for further operations
    fp = fopen("inventory.txt", "r+");
    if (fp == NULL) {
        perror("Error opening file");
        exit(1);
    }
}



//Display the entire inventory
void displayInventory(FILE *fp) {
    rewind(fp); // Reset the file pointer to the beginning of the file
    struct item currentItem;

    printf("\n%-10s %-20s %-30s %-10s %-10s %-20s %-15s\n", "ID", "Name", "Description", "Quantity", "Price", "Category", "Expiry Date");

   while (fscanf(fp, "%d \"%49[^\"]\" \"%99[^\"]\" %d %f \"%49[^\"]\" %d %d %d", 
                  &currentItem.itemID, currentItem.name, currentItem.description,
                  &currentItem.quantity, &currentItem.price, currentItem.category, 
                  &currentItem.expiryDate[0], &currentItem.expiryDate[1], &currentItem.expiryDate[2]) !=EOF) {
        printf("%-10d %-20s %-30s %-10d %-10.2f %-20s %02d-%02d-%04d\n", currentItem.itemID, currentItem.name, currentItem.description,
               currentItem.quantity, currentItem.price, currentItem.category, currentItem.expiryDate[0], currentItem.expiryDate[1], currentItem.expiryDate[2]);
    }
}



//Identifying surplus items (quantity > 50)
void identifySurplusItems(FILE *fp){
    rewind(fp);
    struct item currentItem;
    printf("\nSurplus Items (Quantity > 50):\n");
    printf("\n%-10s %-20s %-30s %-10s %-10s %-20s %-15s\n", "ID", "Name", "Description", "Quantity", "Price", "Category", "Expiry Date");
   while (fscanf(fp, "%d \"%49[^\"]\" \"%99[^\"]\" %d %f \"%49[^\"]\" %d %d %d", 
                  &currentItem.itemID, currentItem.name, currentItem.description,
                  &currentItem.quantity, &currentItem.price, currentItem.category, 
                  &currentItem.expiryDate[0], &currentItem.expiryDate[1], &currentItem.expiryDate[2])!= EOF) {
        if (currentItem.quantity > 50) {
            printf("%-10d %-20s %-30s %-10d %-10.2f %-20s %02d-%02d-%04d\n", currentItem.itemID, currentItem.name, currentItem.description,
                   currentItem.quantity, currentItem.price, currentItem.category, currentItem.expiryDate[0], currentItem.expiryDate[1], currentItem.expiryDate[2]);
        }
    }
}

//Listing items with low quantity (quantity < 20)
void listLowQuantityItems(FILE *fp){
    rewind(fp);
    struct item currentItem;
    printf("\nLow Quantity Items (Quantity < 20):\n");
    printf("\n%-10s %-20s %-30s %-10s %-10s %-20s %-15s\n", "ID", "Name", "Description", "Quantity", "Price", "Category", "Expiry Date");
   while (fscanf(fp, "%d \"%49[^\"]\" \"%99[^\"]\" %d %f \"%49[^\"]\" %d %d %d", 
                  &currentItem.itemID, currentItem.name, currentItem.description,
                  &currentItem.quantity, &currentItem.price, currentItem.category, 
                  &currentItem.expiryDate[0], &currentItem.expiryDate[1], &currentItem.expiryDate[2]) != EOF) {
        if (currentItem.quantity < 20) {
            printf("%-10d %-20s %-30s %-10d %-10.2f %-20s %02d-%02d-%04d\n", currentItem.itemID, currentItem.name, currentItem.description,
                   currentItem.quantity, currentItem.price, currentItem.category, currentItem.expiryDate[0], currentItem.expiryDate[1], currentItem.expiryDate[2]);
        }
    }
}

//Search for an item by ID
void searchItemByID(FILE *fp) {
    rewind(fp);
    int searchID;
    printf("\nEnter Item ID to search: ");
    scanf("%d", &searchID);

    struct item currentItem;
    int found = 0;

   while (fscanf(fp, "%d \"%49[^\"]\" \"%99[^\"]\" %d %f \"%49[^\"]\" %d %d %d", 
                  &currentItem.itemID, currentItem.name, currentItem.description,
                  &currentItem.quantity, &currentItem.price, currentItem.category, 
                  &currentItem.expiryDate[0], &currentItem.expiryDate[1], &currentItem.expiryDate[2]) != EOF) {
        if (currentItem.itemID == searchID) {
            printf("\nItem found:\n");
            printf("\n%-10s %-20s %-30s %-10s %-10s %-20s %-15s\n", "ID", "Name", "Description", "Quantity", "Price", "Category", "Expiry Date");
            printf("%-10d %-20s %-30s %-10d %-10.2f %-20s %02d-%02d-%04d\n", currentItem.itemID, currentItem.name, currentItem.description,
                   currentItem.quantity, currentItem.price, currentItem.category, currentItem.expiryDate[0], currentItem.expiryDate[1], currentItem.expiryDate[2]);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Item with ID %d not found.\n", searchID);
    }
}

//Listing expired items 
void expireditems(FILE *fp){
    rewind(fp);
    int currentDay, currentMonth, currentYear;
    printf("\nEnter Current Date (dd mm yyyy): ");
    scanf("%d %d %d", &currentDay, &currentMonth, &currentYear);

    struct item currentItem;
    printf("\nExpired or Near Expiry Items:\n");
    printf("\n%-10s %-20s %-30s %-10s %-10s %-20s %-15s\n", "ID", "Name", "Description", "Quantity", "Price", "Category", "Expiry Date");
   while (fscanf(fp, "%d \"%49[^\"]\" \"%99[^\"]\" %d %f \"%49[^\"]\" %d %d %d", 
                  &currentItem.itemID, currentItem.name, currentItem.description,
                  &currentItem.quantity, &currentItem.price, currentItem.category, 
                  &currentItem.expiryDate[0], &currentItem.expiryDate[1], &currentItem.expiryDate[2]) != EOF) {
        if ((currentItem.expiryDate[2] < currentYear) ||
            (currentItem.expiryDate[2] == currentYear && currentItem.expiryDate[1] < currentMonth) ||
            (currentItem.expiryDate[2] == currentYear && currentItem.expiryDate[1] == currentMonth && currentItem.expiryDate[0] <= currentDay)) {
            printf("%-10d %-20s %-30s %-10d %-10.2f %-20s %02d-%02d-%04d\n", currentItem.itemID, currentItem.name, currentItem.description,
                   currentItem.quantity, currentItem.price, currentItem.category, currentItem.expiryDate[0], currentItem.expiryDate[1], currentItem.expiryDate[2]);
        }
    }
}
void deleteItem(FILE *fp) {
    int deleteID;
    printf("\nEnter Item ID to delete: ");
    scanf("%d", &deleteID);

    // Creating a temporary file to store data of remaining items
    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("Error creating temporary file!\n");
        return;
    }

    struct item currentItem;
    int found = 0;

    rewind(fp); // Go back to the beginning of the file

    // Read each item and write it to the temp file unless it's the item to be deleted
    while (fscanf(fp, "%d \"%49[^\"]\" \"%99[^\"]\" %d %f \"%49[^\"]\" %d %d %d", 
                  &currentItem.itemID, currentItem.name, currentItem.description,
                  &currentItem.quantity, &currentItem.price, currentItem.category, 
                  &currentItem.expiryDate[0], &currentItem.expiryDate[1], &currentItem.expiryDate[2]) != EOF) {
        if (currentItem.itemID == deleteID) {
            found = 1; // Mark as found but don't write it to temp file
            continue;
        }
        fprintf(tempFile, "%d \"%s\" \"%s\" %d %.2f \"%s\" %d %d %d\n", currentItem.itemID, currentItem.name, currentItem.description,
                currentItem.quantity, currentItem.price, currentItem.category, currentItem.expiryDate[0], currentItem.expiryDate[1], currentItem.expiryDate[2]);
    }

    fclose(fp);
    fclose(tempFile);

    // Check if the item was found
    if (found) {
        printf("Item with ID %d deleted successfully!\n", deleteID);
        // Delete the original file and rename the temp file
        remove("inventory.txt");
        rename("temp.txt", "inventory.txt");
    } else {
        printf("Item with ID %d not found.\n", deleteID);
        remove("temp.txt"); // Delete the temp file as no changes are made
    }

    // Reopen the file for further operations
    fp = fopen("inventory.txt", "r+");
    if (fp == NULL) {
        perror("Error opening file");
        exit(1);
   }
}
//Searching an item by category
void searchItemByCategory(FILE *fp) {
    rewind(fp);
    char searchCategory[50];
    printf("Enter Category to search: ");
    scanf("%s", searchCategory);

    struct item currentItem;
    int found = 0;

    printf("Items in Category '%s':\n", searchCategory);
    printf("%-10s %-20s %-30s %-10s %-10s %-20s %-15s\n", "ID", "Name", "Description", "Quantity", "Price", "Category", "Expiry Date");

    while (fscanf(fp, "%d \"%49[^\"]\" \"%99[^\"]\" %d %f \"%49[^\"]\" %d %d %d\n",
                  &currentItem.itemID, currentItem.name, currentItem.description,
                  &currentItem.quantity, &currentItem.price, currentItem.category,
                  &currentItem.expiryDate[0], &currentItem.expiryDate[1], &currentItem.expiryDate[2]) != EOF) {
        if (strcmp(currentItem.category, searchCategory) == 0) {
            printf("%-10d %-20s %-30s %-10d %-10.2f %-20s %02d-%02d-%04d\n", currentItem.itemID, currentItem.name, currentItem.description,
                   currentItem.quantity, currentItem.price, currentItem.category, currentItem.expiryDate[0], currentItem.expiryDate[1], currentItem.expiryDate[2]);
            found = 1;
        }
    }

    if (!found) {
        printf("No items found in category '%s'.\n", searchCategory);
    }
}
//Total cost of inventory
void TotalcostofInventory(FILE *fp) {
    rewind(fp);
    struct item currentItem;
    float totalCost = 0;

    while (fscanf(fp, "%d \"%49[^\"]\" \"%99[^\"]\" %d %f \"%49[^\"]\" %d %d %d", 
                  &currentItem.itemID, currentItem.name, currentItem.description,
                  &currentItem.quantity, &currentItem.price, currentItem.category, 
                  &currentItem.expiryDate[0], &currentItem.expiryDate[1], &currentItem.expiryDate[2]) !=EOF) {
        totalCost += currentItem.quantity * currentItem.price;
    }
    printf("Total Cost of Inventory: %.2f\n",totalCost);
}
