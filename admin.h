#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/socket.h>
#include "structure.h"






/*-----------------------------ADD FACULTY---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

int addFaculty(int clientSocket) {
    struct faculty f;


    memset(f.login_id,0,sizeof(f.login_id));
    // Prompt and receive the Login Id
    send(clientSocket, "Enter the Login Id\n", strlen("Enter the Login Id\n"), 0);
    int bytesRead = recv(clientSocket, f.login_id, sizeof(f.login_id) - 1, 0);
    if (bytesRead <= 0) {
        perror("Error while receiving Login Id");
        return false;
    }
    f.login_id[bytesRead] = '\0';
    // Set the Password to "1111"

    memset(f.password,0,sizeof(f.password));
    strcpy(f.password, "1111");

     memset(f.name,0,sizeof(f.name));
    // Prompt and receive the Name
    send(clientSocket, "Enter Name\n", strlen("Enter Name\n"), 0);
    bytesRead = recv(clientSocket, f.name, sizeof(f.name) - 1, 0);
    if (bytesRead <= 0) {
        perror("Error while receiving Name");
        return false;
    }
    f.name[bytesRead] = '\0';


    
    memset(f.dept,0,sizeof(f.dept));
    // Prompt and receive the Department
    send(clientSocket, "Department Name\n", strlen("Department Name\n"), 0);
    bytesRead = recv(clientSocket, f.dept, sizeof(f.dept) - 1, 0);
    if (bytesRead <= 0) {
        perror("Error while receiving Department");
        return false;
    }
    f.dept[bytesRead] = '\0';


   

    
    memset(f.age,0,sizeof(f.age));
    // Prompt and receive the Age
    send(clientSocket, "Age\n", strlen("Age\n"), 0);
    bytesRead = recv(clientSocket, f.age, sizeof(f.age) - 1, 0);
    if (bytesRead <= 0) {
        perror("Error while receiving Age");
        return false;
    }
    f.age[bytesRead] = '\0';

    
        
    memset(f.email,0,sizeof(f.email));
    // Prompt and receive the Email
    send(clientSocket, "Email\n", strlen("Email\n"), 0);
    bytesRead = recv(clientSocket, f.email, sizeof(f.email) - 1, 0);
    if (bytesRead <= 0) {
        perror("Error while receiving Email");
        return false;
    }
    f.email[bytesRead] = '\0';

    // Open the file to enter this data in the database
    int fd = open("faculty.txt", O_WRONLY | O_APPEND | O_CREAT, 0666); // Open the file in append mode

    if (fd == -1) {
        perror("Error opening the file");
        return false;
    }

    // Use flock to apply a mandatory lock
    if (flock(fd, LOCK_EX) == -1) {
        perror("Error applying lock");
        close(fd);
        return false;
    }

    // Write the structure data to the file
    if (write(fd, &f, sizeof(struct faculty)) == -1) {
        perror("Error writing to the file");
        flock(fd, LOCK_UN); // Release the lock before closing the file
        close(fd);
        send(clientSocket, "Failed to add Faculty\n", strlen("Failed to add Faculty\n"), 0);

    }else{

        flock(fd, LOCK_UN); // Release the lock
        close(fd);          // Close the file
        send(clientSocket, "Faculty added successfully!!\n", strlen("Faculty added successfully!!\n"), 0);

    }
    
}




/*-----------------------------VIEW FACULTY------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


int viewFacultyDetails(int clientSocket) {

    char duff[1024];
    int fd = open("faculty.txt", O_RDONLY);
    if (fd == -1) {
        perror("Error opening the file");
        return false;
    }
    int found = 0;
    char loginID[10];

    struct faculty f;

    memset(loginID,0,sizeof(loginID));
    
    send(clientSocket, "Enter Login Id\n", strlen("Enter Login Id\n"), 0);
    int bytesRead = recv(clientSocket, loginID, sizeof(loginID) - 1, 0);
    if (bytesRead <= 0) {
        perror("Error while receiving Login Id");
        return false;
    }
    loginID[bytesRead] = '\0';
 
    // Search for the faculty with the matching login ID
    while (read(fd, &f, sizeof(struct faculty)) > 0) {
        if (strcmp(loginID, f.login_id) == 0) {
            
            found = 1;
            break;
        }
    }
    memset(duff,0,sizeof(duff));

    if(found){
        // construct detailed message
        sprintf(duff,"\n--Required Details of faculty--\nLogin ID:%s\nName:%s\nDept:%s\nAge:%s\nEmail:%s\n",f.login_id,f.name,f.dept,f.age,f.email);
        send(clientSocket, duff, strlen(duff),0);
        close(fd);
        
    }else{

        send(clientSocket, "faculty not found..\n", strlen("faculty not found..\n"), 0);
        close(fd);
        
    }
     
}



/*------------------------------ADD STUDENT------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

int addStudent(int clientSocket) {
    struct student s;
    
    memset(s.stud_id,0,sizeof(s.stud_id));

    // Prompt and receive the Login Id
    send(clientSocket, "Enter Login Id\n", strlen("Enter Login Id\n"), 0);
    int bytesRead = recv(clientSocket, s.stud_id, sizeof(s.stud_id) - 1, 0);
    if (bytesRead <= 0) {
        perror("Error while receiving Login Id");
        return false;
    }
    s.stud_id[bytesRead] = '\0';

    memset(s.password,0,sizeof(s.password));
    strcpy(s.password, "1111");

    memset(s.dept,0,sizeof(s.dept));
    // Prompt and receive the Department
    send(clientSocket, "Enter Department\n", strlen("Enter Department\n"), 0);
    bytesRead = recv(clientSocket, s.dept, sizeof(s.dept) - 1, 0);
    if (bytesRead <= 0) {
        perror("Error while receiving Department");
        return false;
    }
    s.dept[bytesRead] = '\0';


    memset(s.name,0,sizeof(s.name));
    // Prompt and receive the Name
    send(clientSocket, "Enter Student Name\n", strlen("Enter Student Name\n"), 0);
    bytesRead = recv(clientSocket, s.name, sizeof(s.name) - 1, 0);
    if (bytesRead <= 0) {
        perror("Error while receiving Name");
        return false;
    }
    s.name[bytesRead] = '\0';


     memset(s.age,0,sizeof(s.age));
    // Prompt and receive the Age
    send(clientSocket, "Age\n", strlen("Age\n"), 0);
    bytesRead = recv(clientSocket, s.age, sizeof(s.age) - 1, 0);
    if (bytesRead <= 0) {
        perror("Error while receiving Age");
        return false;
    }
    s.age[bytesRead] = '\0';


     memset(s.email,0,sizeof(s.email));

    // Prompt and receive the Email
    send(clientSocket, "Enter Email\n", strlen("Enter Email\n"), 0);
    bytesRead = recv(clientSocket, s.email, sizeof(s.email) - 1, 0);
    if (bytesRead <= 0) {
        perror("Error while receiving Email");
        return false;
    }
    s.email[bytesRead] = '\0';

    // Open the file to enter this data in the database
    int fd = open("student.txt", O_WRONLY | O_APPEND | O_CREAT, 0666); // Open the file in append mode

    if (fd == -1) {
        perror("Error opening the file");
        return false;
    }

    // Use flock to apply a mandatory lock
    if (flock(fd, LOCK_EX) == -1) {
        perror("Error applying lock");
        close(fd);
        return false;
    }

    // Write the structure data to the file
    if (write(fd, &s, sizeof(struct student)) == -1) {
        perror("Error writing to the file");
        flock(fd, LOCK_UN); // Release the lock before closing the file
        close(fd);
        send(clientSocket, "Failed to add Student\n", strlen("Failed to add Student\n"), 0);

    }else{

        flock(fd, LOCK_UN); // Release the lock
        close(fd);          // Close the file
        send(clientSocket, "Student adeed successfully\n", strlen("Student adeed successfully\n"), 0);

    }
    
}





/*-------------------------------VIEW STUDENT DETAILS----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


int viewStudentDetails(int clientSocket) {

    char buff[1024];
    int fd = open("student.txt", O_RDONLY);
    if (fd == -1) {
        perror("Error opening the file");
        return false;
    }
    int found = 0;
    char loginID[10];

    struct student s;

    memset(loginID,0,sizeof(loginID));
    
    send(clientSocket, "Enter LoginId\n", strlen("Enter LoginId\n"), 0);
    int bytesRead = recv(clientSocket, loginID, sizeof(loginID) - 1, 0);
    if (bytesRead <= 0) {
        perror("Error while receiving Login Id");
        return false;
    }
    loginID[bytesRead] = '\0';
 
    // Search for the student with the matching login ID
    while (read(fd, &s, sizeof(struct student)) > 0) {
        if (strcmp(loginID, s.stud_id) == 0) {
            // Print student details when found
            found = 1;
            break;
        }
    }
    memset(buff,0,sizeof(buff));

    if(found){
        // construct detailed message
        sprintf(buff,"\n--Required Details of student--\nLogin ID:%s\nName:%s\nDept:%s\nAge:%s\nEmail:%s\n",s.stud_id,s.name,s.dept,s.age,s.email);
        send(clientSocket, buff, strlen(buff),0);
        close(fd);
        
    }else{

        send(clientSocket, "Student not found..\n", strlen("Student not found..\n"), 0);
        close(fd);
        
    }
     
}









/*--------------------------------UPDATE STUDENT DETAILS----------------------------------------------------------------------------------------------------------------------------------------------------------------*/


int updateStudDetails(int clientSocket){
    struct student my_student, temp;
    int fd = open("student.txt", O_RDWR, 0644); // Open in read-only mode
    struct flock lock;
    if (fd == -1)
    {
        perror("Error opening file");
        return 0;
    }
    bool found = false; // Initialize found to false
    char buffer[1024];  // Declare buffer for sending data
    send(clientSocket, "Enter Student ID for updation of data: ", strlen("Enter Student ID for updation of data: "), 0);
    int readResult = read(clientSocket, my_student.stud_id, sizeof(my_student.stud_id) - 1);

    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving student ID from server", strlen("Error receiving student ID from server"), 0);
        return 0;
    }
    my_student.stud_id[readResult] = '\0';

    // Reset the file pointer to the beginning of the file
    lseek(fd, 0, SEEK_SET);

    // Loop to search for the student in the file
    while (read(fd, &temp, sizeof(temp)) > 0)
    {
        if (strcmp(my_student.stud_id, temp.stud_id) == 0)
        { // Compare the student IDs
            found = true;
            
            lock.l_type = F_WRLCK;  // Write (exclusive) lock
            lock.l_whence = SEEK_SET;
            lock.l_start = lseek(fd, 0, SEEK_CUR) - sizeof(struct student); // Position lock at the current record
            lock.l_len = sizeof(struct student);

            if (fcntl(fd, F_SETLKW, &lock) == -1)
            {
                perror("Error locking file");
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                close(fd);
                return 0;
            }
            break;
        }
    }
    if (found)
    {

        // Construct the details message
        sprintf(buffer, "...................Student..................\n...................Original Details ..................\nName: %s\nAge: %s\nLogin ID: %s\nPassword: %s\nEmail Address: %s\n.............................................\n",
                temp.name, temp.age, temp.stud_id, temp.password,temp.email);

        // Send the details to the client
        send(clientSocket, buffer, strlen(buffer), 0);
        
        struct student up_stud;
        strcpy(up_stud.stud_id, temp.stud_id);

        send(clientSocket, "Enter Student Name to update: ", strlen("Enter Student Name to update: "), 0);
        readResult = read(clientSocket, up_stud.name, sizeof(up_stud.name) - 1);
        up_stud.name[readResult]='\0';

        send(clientSocket, "Enter password to update: ", strlen("Enter password to update: "), 0);
        readResult = read(clientSocket, up_stud.password, sizeof(up_stud.password) - 1);
        up_stud.password[readResult]='\0';

        send(clientSocket, "Enter Age to update: ", strlen("Enter Age to update: "), 0);
        readResult = read(clientSocket, up_stud.age, sizeof(up_stud.age) - 1);
        up_stud.age[readResult]='\0';

        send(clientSocket, "Enter Email Address to update: ", strlen("Enter Email Address to update: "), 0);
        readResult = read(clientSocket, up_stud.email, sizeof(up_stud.email) - 1);
        up_stud.email[readResult]='\0';

        lseek(fd,-sizeof(struct student),SEEK_CUR); //// Move the file pointer back to the beginning of the current record
        write(fd,&up_stud,sizeof(up_stud));// Overwrite the entire record with the updated data

        sprintf(buffer, "...................Student..................\n...................Updated Details ..................\nName: %s\nAge: %s\nLogin ID: %s\nPassword: %s\nEmail Address: %s\n.............................................\n",
                up_stud.name, up_stud.age, up_stud.stud_id, up_stud.password,up_stud.email);
        
        // Send the details to the client
        send(clientSocket, buffer, strlen(buffer), 0);
        //struct flock unlock;
        lock.l_type = F_UNLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = lock.l_start;
        lock.l_len = lock.l_len;
        fcntl(fd, F_SETLK, &lock);
        close(fd);

        send(clientSocket, "Student Updated Successfully\n", strlen("Student Updated Successfully\n"), 0);

    }
    else
    {
        send(clientSocket, "Student not found\n", strlen("Student not found\n"), 0);
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        close(fd);
        
    }
    close(fd);
     // Close the file after use
}



/*-------------------------------UPDATE FACULTY DETAILS--------------------------------------------------------------------------------------------------------------------------------------------------------------*/



int updateFacDetails(int clientSocket){
    struct faculty my_faculty, temp;
    int fd = open("faculty.txt", O_RDWR, 0644); // Open in read-only mode
    struct flock lock;
    if (fd == -1)
    {
        perror("Error opening file");
        return 0;
    }
    bool found = false; // Initialize found to false
    char buffer[1024];  // Declare buffer for sending data
    send(clientSocket, "Enter Professor ID for updation of data: ", strlen("Enter Professor ID for updation of data: "), 0);
    int readResult = read(clientSocket, my_faculty.login_id, sizeof(my_faculty.login_id) - 1);

    if (readResult <= 0)
    {
        send(clientSocket, "Error receiving student ID from server", strlen("Error receiving student ID from server"), 0);
        return 0;
    }
    my_faculty.login_id[readResult] = '\0';

    // Reset the file pointer to the beginning of the file
    lseek(fd, 0, SEEK_SET);

    // Loop to search for the student in the file
    while (read(fd, &temp, sizeof(temp)) > 0)
    {
        if (strcmp(my_faculty.login_id, temp.login_id) == 0)
        { // Compare the student IDs
            found = true;
            
            lock.l_type = F_WRLCK;  // Write (exclusive) lock
            lock.l_whence = SEEK_SET;
            lock.l_start = lseek(fd, 0, SEEK_CUR) - sizeof(struct faculty); // Position lock at the current record
            lock.l_len = sizeof(struct faculty);

            if (fcntl(fd, F_SETLKW, &lock) == -1)
            {
                perror("Error locking file");
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                close(fd);
                return 0;
            }
            break;
        }
    }
    if (found)
    {

        // Construct the details message
        sprintf(buffer, "...................Professor..................\n...................Original Details ..................\nLogin ID:%s\nName:%s\nDept:%s\nAge:%s\nPassword:%s\nEmail:%s\n.............................................\n",
                temp.login_id,temp.name,temp.dept,temp.age,temp.password,temp.email);

        // Send the details to the client
        send(clientSocket, buffer, strlen(buffer), 0);
        
        struct faculty up_fac;
        strcpy(up_fac.login_id, temp.login_id);

        send(clientSocket, "Enter Professor Name to update: ", strlen("Enter Professor Name to update: "), 0);
        readResult = read(clientSocket, up_fac.name, sizeof(up_fac.name) - 1);
        up_fac.name[readResult]='\0';

        send(clientSocket, "Enter password to update: ", strlen("Enter password to update: "), 0);
        readResult = read(clientSocket, up_fac.password, sizeof(up_fac.password) - 1);
        up_fac.password[readResult]='\0';

        send(clientSocket, "Enter Age to update: ", strlen("Enter Age to update: "), 0);
        readResult = read(clientSocket, up_fac.age, sizeof(up_fac.age) - 1);
        up_fac.age[readResult]='\0';

        send(clientSocket, "Enter Email Address to update: ", strlen("Enter Email Address to update: "), 0);
        readResult = read(clientSocket, up_fac.email, sizeof(up_fac.email) - 1);
        up_fac.email[readResult]='\0';

        send(clientSocket, "Enter Department to update: ", strlen("Enter Department to update: "), 0);
        readResult = read(clientSocket, up_fac.dept, sizeof(up_fac.dept) - 1);
        up_fac.dept[readResult]='\0';

        lseek(fd,-sizeof(struct student),SEEK_CUR); //// Move the file pointer back to the beginning of the current record
        write(fd,&up_fac,sizeof(up_fac));// Overwrite the entire record with the updated data

        sprintf(buffer, "...................Professor..................\n...................Updated Details ..................\nLogin ID:%s\nName:%s\nDept:%s\nAge:%s\nPassword:%s\nEmail:%s\n.............................................\n",
                up_fac.login_id,up_fac.name,up_fac.dept,up_fac.age,up_fac.password,up_fac.email);
        
        // Send the details to the client
        send(clientSocket, buffer, strlen(buffer), 0);
        //struct flock unlock;
        lock.l_type = F_UNLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = lock.l_start;
        lock.l_len = lock.l_len;
        fcntl(fd, F_SETLK, &lock);
        close(fd);

        send(clientSocket, "Student Updated Successfully\n", strlen("Student Updated Successfully\n"), 0);

    }
    else
    {
        send(clientSocket, "Student not found\n", strlen("Student not found\n"), 0);
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        close(fd);
        
    }
    close(fd);
     // Close the file after use
}



/*--------------------------------AUTHENTICATE ADMIN------------------------------------------------------------------------------------------------------------------------------------*/

char* admin_Authentication(int client_socket) {
    char username[100];
    char pass[100];

    // Send a prompt for the username
    const char *uname_msg = "\nEnter Username of Admin: ";
    send(client_socket, uname_msg, strlen(uname_msg), 0);

    // Receive the username from the client
    ssize_t bytesRead = recv(client_socket, username, sizeof(username), 0);
    if (bytesRead <= 0) {
        close(client_socket);
        return false;
    }
    if (username[bytesRead - 1] == '\n') {
        username[bytesRead - 1] = '\0';
    } else {
        username[bytesRead] = '\0';
    }

    // Send a prompt for the password
    const char *pswd_msg = "\nEnter password of the Admin: ";
    send(client_socket, pswd_msg, strlen(pswd_msg), 0);

    // Receive the password from the client
    bytesRead = recv(client_socket, pass, sizeof(pass), 0);
    if (bytesRead <= 0) {
        close(client_socket);
        return false;
    }
    if (pass[bytesRead - 1] == '\n') {
        pass[bytesRead - 1] = '\0';
    } else {
        pass[bytesRead] = '\0';
    }

    // Compare the received username and password with stored credentials
    if (strcmp(username, adminCred.username) == 0 && strcmp(pass, adminCred.pass) == 0) {
        char* loginIdCopy = (char*)malloc(strlen(username) + 1);
                if (loginIdCopy != NULL) {
                    strcpy(loginIdCopy,username);
                    return loginIdCopy;
                }
        // Authentication successful
        
    }
    return NULL;
}






/*---------------------------------ADMIN FUNCTIONALITY------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


int admin_Fun(int client_socket) {
    // You can implement admin-specific functionality here
    // This function will be called after successful admin authentication
    // Add your code to handle admin tasks, menu options, etc.
    // For example, sending admin menu options and processing admin tasks
    printf("USE INTIAL PASSWORD AS sambit and ADMIN sambit \n");
    char admin_Menu[] = "\nAdmin Menu:\n1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Update Student Details\n6. Update Faculty Details\n7. Exit\nEnter Choice\n";

    while (1) {
        send(client_socket, admin_Menu, strlen(admin_Menu), 0);
        char choice_buffer[10];
        ssize_t bytesRead = recv(client_socket, choice_buffer, sizeof(choice_buffer), 0);

        if (bytesRead <= 0) {
            perror("Error receiving admin menu choice");
            close(client_socket);
            return false;
        }

        int choice = atoi(choice_buffer);

        // Handle admin menu options
        switch (choice) {
            case 1:
                // Add Student
                addStudent(client_socket); 
                break;

            case 2:
                // View student details
                viewStudentDetails(client_socket);
                continue;
                break;

            case 3:
                // Add Faculty
                addFaculty(client_socket);

                break;

            case 4:
                // View Faculty Details
                viewFacultyDetails(client_socket);

                break;

            case 5:
                //Update Students details
                updateStudDetails(client_socket);
                break;

            case 6:
                //Update Faculty Details
                updateFacDetails(client_socket);
                break;

            case 7:
                // Exit admin functionality
                return true;

            default:
                // Handle invalid options
                send(client_socket, "Invalid choice. Try again.\n", strlen("Invalid choice. Try again.\n"), 0);
                break;
        }
    }
    
}
