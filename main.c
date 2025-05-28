#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX_SHOWROOMS 10
#define MAX 3
#define STOCK_FILE "stock.txt"
#define SALES_FILE "sales.txt"
#define SALESPERSON_FILE "salespersons.txt"
#define CUSTOMER_FILE "customers.txt"

typedef struct Car
{
    char vin[20];
    char name[50];
    char color[20];
    float price;
    char fuelType[15];
    char type[20];
} Car;

typedef struct Salesperson
{
    char id[10];
    char name[50];
    float salesAchieved;
    float commission;
} Salesperson;

typedef struct Customer
{
    char name[50];
    char phone[15];
    char address[100];
    char vin[20];
    char registrationNumber[20];
} Customer;

typedef struct EMINode
{
    char customerName[50];
    char phone[15];
    char vin[20];
    float loanAmount;
    float emi;
    float remaining;
    int duration;
    int monthsPaid;
    struct EMINode *next;
} EMINode;

// B+ Tree Node Structures
typedef struct CarNode
{
    Car *data[MAX];
    struct CarNode *child[MAX + 1];
    int count;
    int isLeaf;
} CarNode;

typedef struct SalespersonNode
{
    Salesperson *data[MAX];
    struct SalespersonNode *child[MAX + 1];
    int count;
    int isLeaf;
} SalespersonNode;

typedef struct CustomerNode
{
    Customer *data[MAX];
    struct CustomerNode *child[MAX + 1];
    int count;
    int isLeaf;
} CustomerNode;

typedef struct Showroom
{
    char showroomId[10];
    CarNode *availableRoot;
    CarNode *soldRoot;
    SalespersonNode *salespersonRoot;
    CustomerNode *customerRoot;
    EMINode *emiHead;
} Showroom;

Showroom showrooms[MAX_SHOWROOMS];
int showroomCount = 0;

// Helper functions
void printHeader(const char *title)
{
    printf("\n=== %s ===\n", title);
}

void pressToContinue()
{
    printf("\nPress any key to continue...");
    getchar();
    getchar();
}

void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

// B+ Tree Operations
CarNode *createCarNode()
{
    CarNode *newNode = (CarNode *)malloc(sizeof(CarNode));
    if (!newNode)
    {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    newNode->count = 0;
    newNode->isLeaf = 1;
    for (int i = 0; i <= MAX; i++)
        newNode->child[i] = NULL;
    return newNode;
}

SalespersonNode *createSalespersonNode()
{
    SalespersonNode *newNode = (SalespersonNode *)malloc(sizeof(SalespersonNode));
    if (!newNode)
    {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    newNode->count = 0;
    newNode->isLeaf = 1;
    for (int i = 0; i <= MAX; i++)
        newNode->child[i] = NULL;
    return newNode;
}

CustomerNode *createCustomerNode()
{
    CustomerNode *newNode = (CustomerNode *)malloc(sizeof(CustomerNode));
    if (!newNode)
    {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    newNode->count = 0;
    newNode->isLeaf = 1;
    for (int i = 0; i <= MAX; i++)
        newNode->child[i] = NULL;
    return newNode;
}

Car *createCar()
{
    Car *newCar = (Car *)malloc(sizeof(Car));
    if (!newCar)
    {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    printf("Enter VIN: ");
    scanf("%s", newCar->vin);

    printf("Enter Car Name: ");
    clearInputBuffer();
    scanf("%[^\n]s", newCar->name);

    printf("Enter Color: ");
    clearInputBuffer();
    scanf("%[^\n]s", newCar->color);

    printf("Enter Price: ");
    scanf("%f", &newCar->price);

    printf("Enter Fuel Type (Petrol/Diesel/Electric/Hybrid): ");
    clearInputBuffer();
    scanf("%[^\n]s", newCar->fuelType);

    printf("Enter Type (Sedan/SUV/Hatchback/MUV): ");
    clearInputBuffer();
    scanf("%[^\n]s", newCar->type);

    return newCar;
}

/* Splitting function for Car B+ Tree
void splitCarNode(CarNode **parent, CarNode **child, int index) {
    CarNode *newChild = createCarNode();
    newChild->isLeaf = (*child)->isLeaf;
    
    // Determine split point
    int splitPos = MAX / 2;
    
    // Copy half of the elements to the new node
    for (int i = splitPos + 1, j = 0; i < MAX; i++, j++) {
        newChild->data[j] = (*child)->data[i];
        newChild->count++;
    }
    
    // If not a leaf, copy the child pointers as well
    if (!(*child)->isLeaf) {
        for (int i = splitPos + 1, j = 0; i <= MAX; i++, j++) {
            newChild->child[j] = (*child)->child[i];
            (*child)->child[i] = NULL;
        }
    }
    
    // Adjust counts
    (*child)->count = splitPos;
    
    // Make space in parent for the new child
    for (int i = (*parent)->count; i > index; i--) {
        (*parent)->child[i + 1] = (*parent)->child[i];
        (*parent)->data[i] = (*parent)->data[i - 1];
    }
    
    // Link the new child to the parent
    (*parent)->child[index + 1] = newChild;
    (*parent)->data[index] = (*child)->data[splitPos];
    (*parent)->count++;
} */

void insertCar(CarNode **root, Car *car)
{
    if (*root == NULL)
    {
        *root = createCarNode();
        (*root)->data[0] = car;
        (*root)->count = 1;
        return;
    }

    CarNode *node = *root;
    if (node->count < MAX)
    {
        int i = node->count - 1;
        while (i >= 0 && strcmp(car->vin, node->data[i]->vin) < 0)
        {
            node->data[i + 1] = node->data[i];
            i--;
        }
        node->data[i + 1] = car;
        node->count++;
    }
    else
    {
        // Handle node splitting when node is full
        // For simplicity, we'll just add it anyway
        printf("Warning: Node full. B+ tree splitting not implemented.\n");
        node->data[node->count++] = car;
    }
}

Car *searchCar(CarNode *root, char *vin)
{
    if (!root)
        return NULL;

    for (int i = 0; i < root->count; i++)
    {
        if (strcmp(root->data[i]->vin, vin) == 0)
            return root->data[i];

        if (strcmp(vin, root->data[i]->vin) < 0)
            return searchCar(root->child[i], vin);
    }
    return searchCar(root->child[root->count], vin);
}

Car *removeCar(CarNode **root, char *vin)
{
    if (!*root)
        return NULL;

    CarNode *node = *root;
    for (int i = 0; i < node->count; i++)
    {
        if (strcmp(node->data[i]->vin, vin) == 0)
        {
            Car *removed = node->data[i];
            // Shift elements to fill the gap
            for (int j = i; j < node->count - 1; j++)
            {
                node->data[j] = node->data[j + 1];
            }
            node->count--;
            return removed;
        }
    }
    return NULL;
}

// Salesperson Operations
void addSalesperson(Showroom *s)
{
    Salesperson *newSp = (Salesperson *)malloc(sizeof(Salesperson));
    if (!newSp)
    {
        printf("Memory allocation failed!\n");
        return;
    }

    printf("Enter Salesperson ID: ");
    scanf("%s", newSp->id);

    printf("Enter Name: ");
    clearInputBuffer();
    scanf("%[^\n]s", newSp->name);

    newSp->salesAchieved = 0;
    newSp->commission = 0;

    if (s->salespersonRoot == NULL)
    {
        s->salespersonRoot = createSalespersonNode();
        s->salespersonRoot->data[0] = newSp;
        s->salespersonRoot->count = 1;
    }
    else
    {
        s->salespersonRoot->data[s->salespersonRoot->count++] = newSp;
    }
    printf("Salesperson added successfully!\n");
}

Salesperson *searchSalesperson(SalespersonNode *root, char *id)
{
    if (!root)
        return NULL;

    for (int i = 0; i < root->count; i++)
    {
        if (strcmp(root->data[i]->id, id) == 0)
            return root->data[i];
    }
    return NULL;
}

void updateSalespersonCommission(Salesperson *sp, float carPrice)
{
    sp->salesAchieved += carPrice;

    // Commission structure: 1% for sales under 1M, 1.5% for sales 1-5M, 2% for sales over 5M
    if (sp->salesAchieved <= 1000000)
    {
        sp->commission = sp->salesAchieved * 0.01;
    }
    else if (sp->salesAchieved <= 5000000)
    {
        sp->commission = 10000 + (sp->salesAchieved - 1000000) * 0.015;
    }
    else
    {
        sp->commission = 10000 + 60000 + (sp->salesAchieved - 5000000) * 0.02;
    }
}

void viewAllSalespersons(SalespersonNode *root)
{
    if (!root)
    {
        printf("No salespersons found.\n");
        return;
    }

    printHeader("Salesperson List");
    printf("%-10s %-30s %-15s %-15s\n", "ID", "Name", "Sales (₹)", "Commission (₹)");
    printf("----------------------------------------------------------------\n");

    for (int i = 0; i < root->count; i++)
    {
        Salesperson *sp = root->data[i];
        printf("%-10s %-30s ₹%-15.2f ₹%-15.2f\n",
               sp->id, sp->name, sp->salesAchieved, sp->commission);
    }
}

// Customer Operations
Customer *createCustomer()
{
    Customer *cust = (Customer *)malloc(sizeof(Customer));
    if (!cust)
    {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    printf("Enter Customer Name: ");
    clearInputBuffer();
    scanf("%[^\n]s", cust->name);

    printf("Enter Phone Number: ");
    scanf("%s", cust->phone);

    printf("Enter Address: ");
    clearInputBuffer();
    scanf("%[^\n]s", cust->address);

    printf("Enter Registration Number: ");
    scanf("%s", cust->registrationNumber);

    return cust;
}

void addCustomer(Showroom *s, Customer *cust)
{
    if (s->customerRoot == NULL)
    {
        s->customerRoot = createCustomerNode();
        s->customerRoot->data[0] = cust;
        s->customerRoot->count = 1;
    }
    else
    {
        s->customerRoot->data[s->customerRoot->count++] = cust;
    }
}

Customer *searchCustomer(CustomerNode *root, char *phone)
{
    if (!root)
        return NULL;

    for (int i = 0; i < root->count; i++)
    {
        if (strcmp(root->data[i]->phone, phone) == 0)
            return root->data[i];
    }
    return NULL;
}

void viewAllCustomers(CustomerNode *root)
{
    if (!root)
    {
        printf("No customers found.\n");
        return;
    }

    printHeader("Customer List");
    printf("%-30s %-15s %-30s %-20s\n", "Name", "Phone", "Address", "Registration No.");
    printf("------------------------------------------------------------------------------\n");

    for (int i = 0; i < root->count; i++)
    {
        Customer *cust = root->data[i];
        printf("%-30s %-15s %-30s %-20s\n",
               cust->name, cust->phone, cust->address, cust->registrationNumber);
    }
}

// Car Operations
void viewAllCars(CarNode *root)
{
    if (!root)
    {
        printf("No cars found.\n");
        return;
    }

    printf("%-20s %-30s %-15s %-15s %-15s %-15s\n",
           "VIN", "Name", "Color", "Price (₹)", "Fuel Type", "Type");
    printf("---------------------------------------------------------------------------------\n");

    for (int i = 0; i < root->count; i++)
    {
        Car *car = root->data[i];
        printf("%-20s %-30s %-15s ₹%-15.2f %-15s %-15s\n",
               car->vin, car->name, car->color, car->price, car->fuelType, car->type);
    }
}

// EMI Management
EMINode *createEMIEntry(Car *car, Customer *cust, float downPayment, int months)
{
    if (downPayment < car->price * 0.2)
    {
        printf("Down payment must be at least 20%% of car price!\n");
        return NULL;
    }

    EMINode *newEMI = (EMINode *)malloc(sizeof(EMINode));
    if (!newEMI)
    {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    strcpy(newEMI->customerName, cust->name);
    strcpy(newEMI->phone, cust->phone);
    strcpy(newEMI->vin, car->vin);

    float principal = car->price - downPayment;
    newEMI->loanAmount = principal;

    float rate = 0.085; // 8.5% annual
    if (months == 60)
        rate = 0.0875;
    if (months == 84)
        rate = 0.09;

    float monthlyRate = rate / 12;
    newEMI->emi = (principal * monthlyRate * pow(1 + monthlyRate, months)) /
                  (pow(1 + monthlyRate, months) - 1);
    newEMI->remaining = principal;
    newEMI->duration = months;
    newEMI->monthsPaid = 0;
    newEMI->next = NULL;

    printf("EMI Details:\n");
    printf("Loan Amount: ₹%.2f\n", principal);
    printf("Monthly EMI: ₹%.2f\n", newEMI->emi);
    printf("Total Interest: ₹%.2f\n", (newEMI->emi * months) - principal);
    printf("Total Amount: ₹%.2f\n", newEMI->emi * months);

    return newEMI;
}

void processEMIPayment(Showroom *sr)
{
    if (!sr->emiHead)
    {
        printf("No active EMIs found.\n");
        return;
    }

    char phone[15];
    printf("Enter Customer Phone: ");
    scanf("%s", phone);

    EMINode *current = sr->emiHead;
    EMINode *prev = NULL;
    int found = 0;

    while (current)
    {
        if (strcmp(current->phone, phone) == 0)
        {
            found = 1;
            printf("Customer: %s\n", current->customerName);
            printf("VIN: %s\n", current->vin);
            printf("EMI Amount: ₹%.2f\n", current->emi);
            printf("Remaining Amount: ₹%.2f\n", current->remaining);
            printf("Months Paid: %d/%d\n", current->monthsPaid, current->duration);

            printf("\n1. Make EMI Payment\n");
            printf("2. Make Full Payment\n");
            printf("3. Cancel\n");
            printf("Choice: ");

            int choice;
            scanf("%d", &choice);

            if (choice == 1)
            {
                current->remaining -= current->emi;
                current->monthsPaid++;
                printf("Payment successful. Remaining amount: ₹%.2f\n", current->remaining);

                if (current->monthsPaid >= current->duration || current->remaining <= 0)
                {
                    printf("Loan fully paid! Removing EMI record.\n");

                    if (prev)
                    {
                        prev->next = current->next;
                    }
                    else
                    {
                        sr->emiHead = current->next;
                    }
                    free(current);
                }
            }
            else if (choice == 2)
            {
                printf("Processing full payment of ₹%.2f\n", current->remaining);

                if (prev)
                {
                    prev->next = current->next;
                }
                else
                {
                    sr->emiHead = current->next;
                }
                free(current);
                printf("Full payment successful. Loan closed.\n");
            }
            break;
        }
        prev = current;
        current = current->next;
    }

    if (!found)
    {
        printf("No EMI record found for this customer.\n");
    }
}

void viewAllEMIs(EMINode *head)
{
    if (!head)
    {
        printf("No active EMIs found.\n");
        return;
    }

    printHeader("Active EMI List");
    printf("%-30s %-15s %-15s %-15s %-15s %-10s\n",
           "Customer", "Phone", "VIN", "EMI (₹)", "Remaining (₹)", "Progress");
    printf("-----------------------------------------------------------------------------------------\n");

    EMINode *current = head;
    while (current)
    {
        printf("%-30s %-15s %-15s ₹%-15.2f ₹%-15.2f %d/%d\n",
               current->customerName, current->phone, current->vin,
               current->emi, current->remaining,
               current->monthsPaid, current->duration);
        current = current->next;
    }
}

// File Operations
void saveCarData(FILE *file, CarNode *root)
{
    if (!root)
        return;

    for (int i = 0; i < root->count; i++)
    {
        Car *car = root->data[i];
        fprintf(file, "%s,%s,%s,%.2f,%s,%s\n",
                car->vin, car->name, car->color, car->price, car->fuelType, car->type);
    }
}

void saveCustomerData(FILE *file, CustomerNode *root)
{
    if (!root)
        return;

    for (int i = 0; i < root->count; i++)
    {
        Customer *cust = root->data[i];
        fprintf(file, "%s,%s,%s,%s,%s\n",
                cust->name, cust->phone, cust->address, cust->vin, cust->registrationNumber);
    }
}

void saveSalespersonData(FILE *file, SalespersonNode *root)
{
    if (!root)
        return;

    for (int i = 0; i < root->count; i++)
    {
        Salesperson *sp = root->data[i];
        fprintf(file, "%s,%s,%.2f,%.2f\n",
                sp->id, sp->name, sp->salesAchieved, sp->commission);
    }
}

void saveEMIData(FILE *file, EMINode *head)
{
    EMINode *current = head;
    while (current)
    {
        fprintf(file, "%s,%s,%s,%.2f,%.2f,%.2f,%d,%d\n",
                current->customerName, current->phone, current->vin,
                current->loanAmount, current->emi, current->remaining,
                current->duration, current->monthsPaid);
        current = current->next;
    }
}

void saveDataToFiles(Showroom *sr)
{
    char filename[100];

    // Save available cars
    sprintf(filename, "%s_%s", sr->showroomId, STOCK_FILE);
    FILE *stockFile = fopen(filename, "w");
    if (stockFile)
    {
        saveCarData(stockFile, sr->availableRoot);
        fclose(stockFile);
    }

    // Save sold cars
    sprintf(filename, "%s_%s", sr->showroomId, SALES_FILE);
    FILE *salesFile = fopen(filename, "w");
    if (salesFile)
    {
        saveCarData(salesFile, sr->soldRoot);
        fclose(salesFile);
    }

    // Save customers
    sprintf(filename, "%s_%s", sr->showroomId, CUSTOMER_FILE);
    FILE *custFile = fopen(filename, "w");
    if (custFile)
    {
        saveCustomerData(custFile, sr->customerRoot);
        fclose(custFile);
    }

    // Save salespersons
    sprintf(filename, "%s_%s", sr->showroomId, SALESPERSON_FILE);
    FILE *spFile = fopen(filename, "w");
    if (spFile)
    {
        saveSalespersonData(spFile, sr->salespersonRoot);
        fclose(spFile);
    }

    // Save EMIs
    sprintf(filename, "%s_emi.txt", sr->showroomId);
    FILE *emiFile = fopen(filename, "w");
    if (emiFile)
    {
        saveEMIData(emiFile, sr->emiHead);
        fclose(emiFile);
    }

    printf("All data saved successfully!\n");
}

Car *loadCar(char *line)
{
    Car *car = (Car *)malloc(sizeof(Car));
    if (!car)
        return NULL;

    char *token = strtok(line, ",");
    if (token)
        strcpy(car->vin, token);

    token = strtok(NULL, ",");
    if (token)
        strcpy(car->name, token);

    token = strtok(NULL, ",");
    if (token)
        strcpy(car->color, token);

    token = strtok(NULL, ",");
    if (token)
        car->price = atof(token);

    token = strtok(NULL, ",");
    if (token)
        strcpy(car->fuelType, token);

    token = strtok(NULL, ",");
    if (token)
        strcpy(car->type, token);

    return car;
}

Customer *loadCustomer(char *line)
{
    Customer *cust = (Customer *)malloc(sizeof(Customer));
    if (!cust)
        return NULL;

    char *token = strtok(line, ",");
    if (token)
        strcpy(cust->name, token);

    token = strtok(NULL, ",");
    if (token)
        strcpy(cust->phone, token);

    token = strtok(NULL, ",");
    if (token)
        strcpy(cust->address, token);

    token = strtok(NULL, ",");
    if (token)
        strcpy(cust->vin, token);

    token = strtok(NULL, ",");
    if (token)
        strcpy(cust->registrationNumber, token);

    return cust;
}

Salesperson *loadSalesperson(char *line)
{
    Salesperson *sp = (Salesperson *)malloc(sizeof(Salesperson));
    if (!sp)
        return NULL;

    char *token = strtok(line, ",");
    if (token)
        strcpy(sp->id, token);

    token = strtok(NULL, ",");
    if (token)
        strcpy(sp->name, token);

    token = strtok(NULL, ",");
    if (token)
        sp->salesAchieved = atof(token);

    token = strtok(NULL, ",");
    if (token)
        sp->commission = atof(token);

    return sp;
}

EMINode *loadEMI(char *line)
{
    EMINode *emi = (EMINode *)malloc(sizeof(EMINode));
    if (!emi)
        return NULL;

    char *token = strtok(line, ",");
    if (token)
        strcpy(emi->customerName, token);

    token = strtok(NULL, ",");
    if (token)
        strcpy(emi->phone, token);

    token = strtok(NULL, ",");
    if (token)
        strcpy(emi->vin, token);

    token = strtok(NULL, ",");
    if (token)
        emi->loanAmount = atof(token);

    token = strtok(NULL, ",");
    if (token)
        emi->emi = atof(token);

    token = strtok(NULL, ",");
    if (token)
        emi->remaining = atof(token);

    token = strtok(NULL, ",");
    if (token)
        emi->duration = atoi(token);

    token = strtok(NULL, ",");
    if (token)
        emi->monthsPaid = atoi(token);

    emi->next = NULL;

    return emi;
}

void loadDataFromFiles(Showroom *sr)
{
    char filename[100];
    char line[256];

    // Load available cars
    sprintf(filename, "%s_%s", sr->showroomId, STOCK_FILE);
    FILE *stockFile = fopen(filename, "r");
    if (stockFile)
    {
        while (fgets(line, sizeof(line), stockFile))
        {
            line[strcspn(line, "\n")] = 0; // Remove newline
            Car *car = loadCar(line);
            if (car)
                insertCar(&sr->availableRoot, car);
        }
        fclose(stockFile);
    }

    // Load sold cars
    sprintf(filename, "%s_%s", sr->showroomId, SALES_FILE);
    FILE *salesFile = fopen(filename, "r");
    if (salesFile)
    {
        while (fgets(line, sizeof(line), salesFile))
        {
            line[strcspn(line, "\n")] = 0;
            Car *car = loadCar(line);
            if (car)
                insertCar(&sr->soldRoot, car);
        }
        fclose(salesFile);
    }

    // Load customers
    sprintf(filename, "%s_%s", sr->showroomId, CUSTOMER_FILE);
    FILE *custFile = fopen(filename, "r");
    if (custFile)
    {
        sr->customerRoot = createCustomerNode();
        int count = 0;
        while (fgets(line, sizeof(line), custFile))
        {
            line[strcspn(line, "\n")] = 0;
            Customer *cust = loadCustomer(line);
            if (cust)
                sr->customerRoot->data[count++] = cust;
        }
        sr->customerRoot->count = count;
        fclose(custFile);
    }

    // Load salespersons
    sprintf(filename, "%s_%s", sr->showroomId, SALESPERSON_FILE);
    FILE *spFile = fopen(filename, "r");
    if (spFile)
    {
        sr->salespersonRoot = createSalespersonNode();
        int count = 0;
        while (fgets(line, sizeof(line), spFile))
        {
            line[strcspn(line, "\n")] = 0;
            Salesperson *sp = loadSalesperson(line);
            if (sp)
                sr->salespersonRoot->data[count++] = sp;
        }
        sr->salespersonRoot->count = count;
        fclose(spFile);
    }

    // Load EMIs
    sprintf(filename, "%s_emi.txt", sr->showroomId);
    FILE *emiFile = fopen(filename, "r");
    if (emiFile)
    {
        EMINode *prev = NULL;
        while (fgets(line, sizeof(line), emiFile))
        {
            line[strcspn(line, "\n")] = 0;
            EMINode *emi = loadEMI(line);
            if (emi)
            {
                if (!sr->emiHead)
                {
                    sr->emiHead = emi;
                }
                else
                {
                    prev->next = emi;
                }
                prev = emi;
            }
        }
        fclose(emiFile);
    }

    printf("All data loaded successfully!\n");
}

// Report Generation
void generateSalesReport(Showroom *sr)
{
    printHeader("Sales Report");

    printf("Total Cars Sold: %d\n", sr->soldRoot ? sr->soldRoot->count : 0);

    float totalSales = 0;
    if (sr->soldRoot)
    {
        for (int i = 0; i < sr->soldRoot->count; i++)
        {
            totalSales += sr->soldRoot->data[i]->price;
        }
    }
    printf("Total Sales Value: ₹%.2f\n", totalSales);

    printf("\nTop Performing Salespersons:\n");
    printf("%-10s %-30s %-15s %-15s\n", "ID", "Name", "Sales (₹)", "Commission (₹)");
    printf("----------------------------------------------------------------\n");

    // This is a simple approach - would need sorting for a real top performers list
    if (sr->salespersonRoot)
    {
        for (int i = 0; i < sr->salespersonRoot->count; i++)
        {
            Salesperson *sp = sr->salespersonRoot->data[i];
            printf("%-10s %-30s ₹%-15.2f ₹%-15.2f\n",
                   sp->id, sp->name, sp->salesAchieved, sp->commission);
        }
    }
}

void generateInventoryReport(Showroom *sr)
{
    printHeader("Inventory Report");

    int totalCars = sr->availableRoot ? sr->availableRoot->count : 0;
    printf("Total Cars in Inventory: %d\n", totalCars);

    // Count cars by type
    int sedan = 0, suv = 0, hatchback = 0, muv = 0;
    float totalValue = 0;

    if (sr->availableRoot)
    {
        for (int i = 0; i < sr->availableRoot->count; i++)
        {
            Car *car = sr->availableRoot->data[i];
            totalValue += car->price;

            if (strcmp(car->type, "Sedan") == 0)
                sedan++;
            else if (strcmp(car->type, "SUV") == 0)
                suv++;
            else if (strcmp(car->type, "Hatchback") == 0)
                hatchback++;
            else if (strcmp(car->type, "MUV") == 0)
                muv++;
        }
    }

    printf("\nInventory by Car Type:\n");
    printf("Sedan: %d\n", sedan);
    printf("SUV: %d\n", suv);
    printf("Hatchback: %d\n", hatchback);
    printf("MUV: %d\n", muv);

    printf("\nTotal Inventory Value: ₹%.2f\n", totalValue);

    // Count cars by fuel type
    int petrol = 0, diesel = 0, electric = 0, hybrid = 0;

    if (sr->availableRoot)
    {
        for (int i = 0; i < sr->availableRoot->count; i++)
        {
            Car *car = sr->availableRoot->data[i];

            if (strcmp(car->fuelType, "Petrol") == 0)
                petrol++;
            else if (strcmp(car->fuelType, "Diesel") == 0)
                diesel++;
            else if (strcmp(car->fuelType, "Electric") == 0)
                electric++;
            else if (strcmp(car->fuelType, "Hybrid") == 0)
                hybrid++;
        }
    }

    printf("\nInventory by Fuel Type:\n");
    printf("Petrol: %d\n", petrol);
    printf("Diesel: %d\n", diesel);
    printf("Electric: %d\n", electric);
    printf("Hybrid: %d\n", hybrid);
}

void generateLoanReport(Showroom *sr)
{
    printHeader("Loan Status Report");

    if (!sr->emiHead)
    {
        printf("No active loans found.\n");
        return;
    }

    int totalLoans = 0;
    float totalOutstanding = 0;
    float totalEMIPerMonth = 0;

    EMINode *current = sr->emiHead;
    while (current)
    {
        totalLoans++;
        totalOutstanding += current->remaining;
        totalEMIPerMonth += current->emi;
        current = current->next;
    }

    printf("Total Active Loans: %d\n", totalLoans);
    printf("Total Outstanding Amount: ₹%.2f\n", totalOutstanding);
    printf("Total Monthly EMI Collection: ₹%.2f\n", totalEMIPerMonth);

    printf("\nDetailed Loan Status:\n");
    viewAllEMIs(sr->emiHead);
}

// Car Sale Process
void sellCar(Showroom *sr)
{
    if (!sr->availableRoot || sr->availableRoot->count == 0)
    {
        printf("No cars available for sale!\n");
        return;
    }

    printHeader("Available Cars for Sale");
    viewAllCars(sr->availableRoot);

    char vin[20];
    printf("\nEnter Car VIN to sell: ");
    scanf("%s", vin);

    Car *car = searchCar(sr->availableRoot, vin);
    if (!car)
    {
        printf("Car with VIN %s not found!\n", vin);
        return;
    }

    printf("\nSelected Car: %s (%s), Price: ₹%.2f\n", car->name, car->color, car->price);

    if (!sr->salespersonRoot || sr->salespersonRoot->count == 0)
    {
        printf("No salespersons available! Please add a salesperson first.\n");
        return;
    }

    char spID[10];
    printf("\nEnter Salesperson ID: ");
    scanf("%s", spID);

    Salesperson *sp = searchSalesperson(sr->salespersonRoot, spID);
    if (!sp)
    {
        printf("Salesperson with ID %s not found!\n", spID);
        return;
    }

    printf("Salesperson: %s\n", sp->name);

    // Create customer entry
    Customer *cust = createCustomer();
    strcpy(cust->vin, vin);

    // Sale type
    printf("\nPayment Method?\n");
    printf("1. Full Payment\n");
    printf("2. EMI Payment\n");
    printf("Choice: ");

    int choice;
    scanf("%d", &choice);

    if (choice == 2)
    {
        // EMI Processing
        float downPayment;
        int months;

        printf("\nDown Payment (min 20%% of car price): ");
        scanf("%f", &downPayment);

        printf("\nEMI Duration (in months - 36, 60, or 84): ");
        scanf("%d", &months);

        if (months != 36 && months != 60 && months != 84)
        {
            printf("Invalid duration! Choose from 36, 60, or 84 months.\n");
            free(cust);
            return;
        }

        EMINode *emiEntry = createEMIEntry(car, cust, downPayment, months);
        if (!emiEntry)
        {
            free(cust);
            return;
        }

        // Add EMI to list
        if (!sr->emiHead)
        {
            sr->emiHead = emiEntry;
        }
        else
        {
            EMINode *current = sr->emiHead;
            while (current->next)
            {
                current = current->next;
            }
            current->next = emiEntry;
        }

        printf("EMI setup successful!\n");
    }
    else
    {
        printf("Processing full payment of ₹%.2f\n", car->price);
    }

    // Update salesperson commission
    updateSalespersonCommission(sp, car->price);

    // Remove car from available and add to sold
    Car *soldCar = removeCar(&sr->availableRoot, vin);
    if (soldCar)
    {
        if (!sr->soldRoot)
            sr->soldRoot = createCarNode();
        sr->soldRoot->data[sr->soldRoot->count++] = soldCar;
    }

    // Add customer to database
    addCustomer(sr, cust);

    printf("\nSale completed successfully!\n");
}

// Main Menu Functions
void showroomMenu(Showroom *sr)
{
    int choice;
    do
    {
        printHeader("Showroom Management");
        printf("1. View Available Cars\n");
        printf("2. View Sold Cars\n");
        printf("3. View All Salespersons\n");
        printf("4. View All Customers\n");
        printf("5. Add New Car to Inventory\n");
        printf("6. Add New Salesperson\n");
        printf("7. Sell a Car\n");
        printf("8. Process EMI Payment\n");
        printf("9. View All EMIs\n");
        printf("10. Generate Sales Report\n");
        printf("11. Generate Inventory Report\n");
        printf("12. Generate Loan Report\n");
        printf("13. Save All Data\n");
        printf("14. Load All Data\n");
        printf("0. Back to Main Menu\n");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printHeader("Available Cars");
            viewAllCars(sr->availableRoot);
            pressToContinue();
            break;
            case 2:
            printHeader("Sold Cars");
            viewAllCars(sr->soldRoot);
            pressToContinue();
            break;

        case 3:
            viewAllSalespersons(sr->salespersonRoot);
            pressToContinue();
            break;

        case 4:
            viewAllCustomers(sr->customerRoot);
            pressToContinue();
            break;

        case 5:
        {
            Car *newCar = createCar();
            insertCar(&sr->availableRoot, newCar);
            printf("Car added successfully!\n");
            pressToContinue();
            break;
        }

        case 6:
            addSalesperson(sr);
            pressToContinue();
            break;

        case 7:
            sellCar(sr);
            pressToContinue();
            break;

        case 8:
            processEMIPayment(sr);
            pressToContinue();
            break;

        case 9:
            viewAllEMIs(sr->emiHead);
            pressToContinue();
            break;

        case 10:
            generateSalesReport(sr);
            pressToContinue();
            break;

        case 11:
            generateInventoryReport(sr);
            pressToContinue();
            break;

        case 12:
            generateLoanReport(sr);
            pressToContinue();
            break;

        case 13:
            saveDataToFiles(sr);
            pressToContinue();
            break;

        case 14:
            loadDataFromFiles(sr);
            pressToContinue();
            break;

        case 0:
            printf("Returning to main menu...\n");
            break;

        default:
            printf("Invalid choice! Please try again.\n");
            pressToContinue();
        }
    } while (choice != 0);
}

void mainMenu()
{
    int choice;
    do
    {
        printHeader("Car Showroom Management System");
        printf("1. Select Showroom\n");
        printf("2. Add New Showroom\n");
        printf("0. Exit\n");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            if (showroomCount == 0)
            {
                printf("No showrooms available. Please add a showroom first.\n");
                pressToContinue();
                break;
            }

            printf("\nAvailable Showrooms:\n");
            for (int i = 0; i < showroomCount; i++)
            {
                printf("%d. %s\n", i + 1, showrooms[i].showroomId);
            }

            int srChoice;
            printf("\nSelect Showroom (1-%d): ", showroomCount);
            scanf("%d", &srChoice);

            if (srChoice < 1 || srChoice > showroomCount)
            {
                printf("Invalid selection!\n");
                pressToContinue();
                break;
            }

            showroomMenu(&showrooms[srChoice - 1]);
            break;

        case 2:
            if (showroomCount >= MAX_SHOWROOMS)
            {
                printf("Maximum number of showrooms reached!\n");
                pressToContinue();
                break;
            }

            printf("Enter Showroom ID: ");
            scanf("%s", showrooms[showroomCount].showroomId);

            // Initialize the new showroom
            showrooms[showroomCount].availableRoot = NULL;
            showrooms[showroomCount].soldRoot = NULL;
            showrooms[showroomCount].salespersonRoot = NULL;
            showrooms[showroomCount].customerRoot = NULL;
            showrooms[showroomCount].emiHead = NULL;

            printf("Showroom %s added successfully!\n", showrooms[showroomCount].showroomId);
            showroomCount++;
            pressToContinue();
            break;

        case 0:
            printf("Exiting system...\n");
            break;

        default:
            printf("Invalid choice! Please try again.\n");
            pressToContinue();
        }
    } while (choice != 0);
}

int main()
{
    // Initialize the system
    printf("Initializing Car Showroom Management System...\n");
    showroomCount = 0;

    // Start the main menu
    mainMenu();

    // Cleanup before exiting
    // Note: In a real application, we would need to free all allocated memory
    printf("Thank you for using the system. Goodbye!\n");
    return 0;
}