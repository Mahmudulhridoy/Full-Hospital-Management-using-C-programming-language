#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include <conio.h>

//Data Structures 
typedef struct
{
 int id;
 char name[50];
 char gender;
 char contact[15];
 char medicalHistory[500];
} Patient;

typedef struct
{
 int id;
 char name[50];
 char specialization[50];
 char availableTime[20];
 float consultancyFee;
 float profit;
} Doctor;

typedef struct {
 int id;
 int patientId;
 int doctorId;
 char date[20];
 char status[20];
} Appointment;

typedef struct {
 int id;
 int patientId;
 int doctorId;
 float amount;
 bool isPaid;
} Bill;

typedef struct
{
 char username[30];
 char password[30];
 char role[10];
 int userId;
} User;


//  Utility Functions 
void clearInputBuffer()
{
 int c;
 while ((c = getchar()) != '\n' && c != EOF);
}

int generateId()
{
 return rand() % 9000 + 1000;
}

void encryptDecrypt(char *str)
{
 int i;
 for (i = 0; str[i] != '\0'; i++)
 {
 str[i] ^= 0x55;
 }
}

void getPassword(char *password)
{
 int i = 0;
 char ch;
 while ((ch = _getch()) != 13)
 {
 if (ch == 8)
 {
 if (i > 0)
 {
 i--;
 printf("\b \b");
 }
 }
 else if (i < 29)
 {
 password[i++] = ch;
 printf("*");
 }
 }
 password[i] = '\0';
 printf("\n");
}

bool isValidDate(const char *date)
{
 if (strlen(date) != 16)
 return false;
 if (date[4] != '-' || date[7] != '-' || date[10] != ' ' || date[13] != ':')
 return false;
 return true;
}

bool isValidPhone(const char *phone)
{
 if (strlen(phone) != 10)
 return false;
 int i;
 for (i = 0; i < 10; i++)
 {
 if (!isdigit(phone[i]))
 return false;
 }
 return true;
}


// File Operations 
void saveToFile(const char *filename, void *data, size_t size)
{
 FILE *file = fopen(filename, "ab");
 if (!file)
 {
 perror("Error opening file");
 return;
 }
 fwrite(data, size, 1, file);
 fclose(file);
}

bool findUserById(const char *filename, int id, void *record, size_t size)
{
 FILE *file = fopen(filename, "rb");
 if (!file)
 return false;

 while (fread(record, size, 1, file))
 {
 if (((Patient *)record)->id == id || ((Doctor *)record)->id == id)
 {
 fclose(file);
 return true;
 }
 }
 fclose(file);
 return false;
}


//  Authentication 
void registerUser()
{
 User newUser;

 printf("Username: ");
 scanf("%29s", newUser.username);
 clearInputBuffer();

 printf("Password: ");
 getPassword(newUser.password);
 encryptDecrypt(newUser.password);

 printf("Role (admin/doctor/patient): ");
 scanf("%9s", newUser.role);
 clearInputBuffer();

 printf("Linked User ID: ");
 scanf("%d", &newUser.userId);
 clearInputBuffer();

 saveToFile("users.dat", &newUser, sizeof(User));
 printf("Registration successful.\n");
}

User *login()
{
 char username[30], password[30];

 printf("Username: ");
 scanf("%29s", username);
 clearInputBuffer();

 printf("Password: ");
 getPassword(password);

 FILE *file = fopen("users.dat", "rb");
 if (!file)
 {
 printf("No users registered yet.\n");
 return NULL;
 }

 static User user;

 while (fread(&user, sizeof(User), 1, file))
 {
 char decrypted[30];
 strcpy(decrypted, user.password);
 encryptDecrypt(decrypted);

 if (strcmp(user.username, username) == 0 &&
 strcmp(decrypted, password) == 0)
 {
 fclose(file);
 return &user;
 }
 }

 fclose(file);
 printf("Invalid credentials.\n");
 return NULL;
}


//  Patient Management 
void addPatient()
{
 Patient p;
 p.id = generateId();

 printf("Name: ");
 fgets(p.name, sizeof(p.name), stdin);
 p.name[strcspn(p.name, "\n")] = '\0';

 printf("Gender (M/F): ");
 p.gender = getchar();
 clearInputBuffer();

 do
 {
 printf("Contact (10 digits): ");
 fgets(p.contact, sizeof(p.contact), stdin);
 p.contact[strcspn(p.contact, "\n")] = '\0';
 } while (!isValidPhone(p.contact));

 printf("Medical History: ");
 fgets(p.medicalHistory, sizeof(p.medicalHistory), stdin);
 p.medicalHistory[strcspn(p.medicalHistory, "\n")] = '\0';

 saveToFile("patients.dat", &p, sizeof(Patient));
 printf("Patient added successfully! (ID: %d)\n", p.id);
}

void viewPatients()
{
 FILE *file = fopen("patients.dat", "rb");
 if (!file)
 {
 printf("No patient records found.\n");
 return;
 }

 printf("\n%-5s %-20s %-6s %-15s %s\n", "ID", "Name", "Gender", "Contact", "Medical History");
 printf("\n");

 Patient p;
 while (fread(&p, sizeof(Patient), 1, file))
 {
 printf("%-5d %-20s %-6c %-15s %s\n",
 p.id, p.name, p.gender, p.contact, p.medicalHistory);
 }

 fclose(file);
}


//  Doctor Management
void addDoctor()
{
 Doctor d;
 d.id = generateId();

 printf("Name: ");
 fgets(d.name, sizeof(d.name), stdin);
 d.name[strcspn(d.name, "\n")] = '\0';

 printf("Specialization: ");
 fgets(d.specialization, sizeof(d.specialization), stdin);
 d.specialization[strcspn(d.specialization, "\n")] = '\0';

 printf("Available Time (HH:MM-HH:MM): ");
 fgets(d.availableTime, sizeof(d.availableTime), stdin);
 d.availableTime[strcspn(d.availableTime, "\n")] = '\0';

 printf("Consultancy Fee: ");
 scanf("%f", &d.consultancyFee);
 clearInputBuffer();

 d.profit = 0;

 saveToFile("doctors.dat", &d, sizeof(Doctor));
 printf("Doctor added successfully! (ID: %d)\n", d.id);
}

void viewDoctors()
{
 FILE *file = fopen("doctors.dat", "rb");
 if (!file)
 {
 printf("No doctor records found.\n");
 return;
 }

 printf("\n%-5s %-20s %-20s %-15s %-10s %-10s\n",
 "ID", "Name", "Specialization", "Available", "Fee", "Profit");
 printf("\n");

 Doctor d;
 while (fread(&d, sizeof(Doctor), 1, file))
 {
 printf("%-5d %-20s %-20s %-15s $%-9.2f $%-9.2f\n",
 d.id, d.name, d.specialization, d.availableTime,
 d.consultancyFee, d.profit);
 }

 fclose(file);
}


// Appointment System 
void bookAppointment()
{
 int patientId, doctorId;
 char date[20];

 printf("Patient ID: ");
 scanf("%d", &patientId);
 clearInputBuffer();

 Patient p;
 if (!findUserById("patients.dat", patientId, &p, sizeof(Patient)))
 {
 printf("Patient not found.\n");
 return;
 }

 printf("Doctor ID: ");
 scanf("%d", &doctorId);
 clearInputBuffer();

 Doctor d;
 if (!findUserById("doctors.dat", doctorId, &d, sizeof(Doctor)))
 {
 printf("Doctor not found.\n");
 return;
 }

 do
 {
 printf("Date (YYYY-MM-DD HH:MM): ");
 fgets(date, sizeof(date), stdin);
 date[strcspn(date, "\n")] = '\0';
 } while (!isValidDate(date));

 Appointment apt = {
 .id = generateId(),
 .patientId = patientId,
 .doctorId = doctorId,
 .status = "BOOKED"
 };

 strcpy(apt.date, date);

 saveToFile("appointments.dat", &apt, sizeof(Appointment));
 printf("Appointment booked successfully! (ID: %d)\n", apt.id);

 Bill bill = {
 .id = generateId(),
 .patientId = patientId,
 .doctorId = doctorId,
 .amount = d.consultancyFee,
 .isPaid = false
 };

 saveToFile("bills.dat", &bill, sizeof(Bill));
 printf("Bill generated: $%.2f\n", bill.amount);

 d.profit += bill.amount;

 FILE *file = fopen("doctors.dat", "rb+");
 if (file)
 {
 while (fread(&d, sizeof(Doctor), 1, file))
 {
 if (d.id == doctorId)
 {
 fseek(file, -sizeof(Doctor), SEEK_CUR);
 fwrite(&d, sizeof(Doctor), 1, file);
 break;
 }
 }
 fclose(file);
 }
}

void viewAppointments()
{
 FILE *file = fopen("appointments.dat", "rb");
 if (!file)
 {
 printf("No appointments found.\n");
 return;
 }

 printf("\n%-5s %-8s %-8s %-20s %-10s\n",
 "ID", "Patient", "Doctor", "Date", "Status");
 printf("\n");

 Appointment apt;
 while (fread(&apt, sizeof(Appointment), 1, file))
 {
 printf("%-5d %-8d %-8d %-20s %-10s\n",
 apt.id, apt.patientId, apt.doctorId,
 apt.date, apt.status);
 }

 fclose(file);
}


//  Billing System 
void viewBills()
{
 FILE *file = fopen("bills.dat", "rb");
 if (!file)
 {
 printf("No bills found.\n");
 return;
 }

 printf("\n%-5s %-8s %-8s %-10s %s\n",
 "ID", "Patient", "Doctor", "Amount", "Paid");
 printf("\n");

 Bill bill;
 while (fread(&bill, sizeof(Bill), 1, file))
 {
 printf("%-5d %-8d %-8d $%-9.2f %s\n",
 bill.id, bill.patientId, bill.doctorId,
 bill.amount,
 bill.isPaid ? "Yes" : "No");
 }

 fclose(file);
}


//  Reporting 
void generateMonthlyReport()
{
 time_t t = time(NULL);
 struct tm *tm = localtime(&t);

 char month[7];
 strftime(month, sizeof(month), "%Y-%m", tm);

 FILE *aptFile = fopen("appointments.dat", "rb");
 int totalApts = 0;
 Appointment apt;

 while (aptFile && fread(&apt, sizeof(Appointment), 1, aptFile))
 {
 if (strncmp(apt.date, month, 7) == 0)
 totalApts++;
 }

 if (aptFile) fclose(aptFile);

 FILE *billFile = fopen("bills.dat", "rb");
 float revenue = 0;
 Bill bill;

 while (billFile && fread(&bill, sizeof(Bill), 1, billFile))
 {
 if (bill.isPaid)
 revenue += bill.amount;
 }

 if (billFile) fclose(billFile);

 printf("\nMonthly Report (%s)\n", month);
 printf("\n");
 printf("Appointments: %d\n", totalApts);
 printf("Revenue: $%.2f\n", revenue);
}


//  Menu System 
void adminMenu()
{
 int choice;

 do
 {
 printf("\n=== ADMIN MENU ===\n");
 printf("1. Add Patient\n");
 printf("2. View Patients\n");
 printf("3. Add Doctor\n");
 printf("4. View Doctors\n");
 printf("5. View Appointments\n");
 printf("6. View Bills\n");
 printf("7. Generate Report\n");
 printf("8. Logout\n");

 printf("Choice: ");
 scanf("%d", &choice);
 clearInputBuffer();

 switch (choice)
 {
 case 1: addPatient(); break;
 case 2: viewPatients(); break;
 case 3: addDoctor(); break;
 case 4: viewDoctors(); break;
 case 5: viewAppointments(); break;
 case 6: viewBills(); break;
 case 7: generateMonthlyReport(); break;
 case 8: return;
 default: printf("Invalid choice.\n");
 }

 } while (true);
}

void doctorMenu(int doctorId)
{
 int choice;

 do
 {
 printf("\n=== DOCTOR MENU ===\n");
 printf("1. View Appointments\n");
 printf("2. View Earnings\n");
 printf("3. Logout\n");

 printf("Choice: ");
 scanf("%d", &choice);
 clearInputBuffer();

 switch (choice)
 {
 case 1: viewAppointments(); break;
 case 2:
 {
 Doctor d;
 if (findUserById("doctors.dat", doctorId, &d, sizeof(Doctor)))
 {
 printf("Your total earnings: $%.2f\n", d.profit);
 }
 break;
 }
 case 3: return;
 default: printf("Invalid choice.\n");
 }

 } while (true);
}

void patientMenu(int patientId)
{
 int choice;

 do
 {
 printf("\n=== PATIENT MENU ===\n");
 printf("1. Book Appointment\n");
 printf("2. View Appointments\n");
 printf("3. View Bills\n");
 printf("4. Logout\n");

 printf("Choice: ");
 scanf("%d", &choice);
 clearInputBuffer();

 switch (choice)
 {
 case 1: bookAppointment(); break;
 case 2: viewAppointments(); break;
 case 3: viewBills(); break;
 case 4: return;
 default: printf("Invalid choice.\n");
 }

 } while (true);
}


// Main Program 
int main()
{
 srand(time(0));
 int choice;

 do
 {
 printf("\n=== HOSPITAL MANAGEMENT SYSTEM ===\n");
 printf("1. Login\n");
 printf("2. Register\n");
 printf("3. Exit\n");

 printf("Choice: ");
 scanf("%d", &choice);
 clearInputBuffer();

 switch (choice)
 {
 case 1:
 {
 User *user = login();
 if (user)
 {
 if (strcmp(user->role, "admin") == 0)
 adminMenu();
 else if (strcmp(user->role, "doctor") == 0)
 doctorMenu(user->userId);
 else
 patientMenu(user->userId);
 }
 break;
 }
 case 2: registerUser(); break;
 case 3:
 printf("Exiting system...\n");
 exit(0);
 default:
 printf("Invalid choice.\n");
 }

 } while (true);
}