#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/socket.h>
//#include "structure.h"

/*---------------------------VIEW ALL COURSES-----------------------------------------------------------------------------------------------------------------------------------------------------*/


int viewAllCourses(int clientSocket) {
    int fd;
    struct course cr;

    // Open the file for reading using system call
    fd = open("course.txt", O_RDONLY);
    if (fd == -1) {
        perror("Error opening the file");
        return 1;
    }

    // Create a dynamic buffer to store course details
    char buffer[4096];  // Adjust the size as needed

    // Initialize the buffer
    memset(buffer, 0, sizeof(buffer));

    // Capture the course details in the buffer
    ssize_t bytes_read;
    while ((bytes_read = read(fd, &cr, sizeof(struct course)) > 0)) {
        // Check if the course is active (active field is "1")
        if (strcmp(cr.active, "1") == 0) {
            char course_str[512];  // Adjust the size as needed for a single course
            snprintf(course_str, sizeof(course_str), "Course ID: %s\n"
                                                    "Professor ID: %s\n"
                                                    "Course Name: %s\n"
                                                    "Department: %s\n"
                                                    "Credit: %s\n"
                                                    "Total Seats: %s\n"
                                                    "Available Seats: %s\n\n",
                     cr.course_id, cr.prof_id, cr.course_name,
                     cr.dept, cr.course_credit, cr.total_seats, cr.avail_seats);
            strcat(buffer, course_str);
        }
    }
    // Close the file using system call
    close(fd);

    // Now, 'buffer' contains details of active courses as a single string
    send(clientSocket, buffer, strlen(buffer), 0);

    return 0;
}


/*-----------------------------ENROLL A NEW COURSE----------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


void enroll(int clientSocket,char* auth) {
    
    char studID[10];
    char courseID[10];
    
    strcpy(studID,auth);

    struct enroll e;
    struct course course_record;

    memset(e.stud_id,0,sizeof(e.stud_id));
    memset(e.course_id[0],'\0',sizeof(e.course_id[0]));
    memset(e.course_id[1],'\0',sizeof(e.course_id[1]));


    int cfound=0, sfound=0;
    int p = 1;
    int canenroll = 0;
    int numEnrollments = 0;

    send(clientSocket, "Enter the Course ID\n", strlen("Enter the Course ID\n"), 0);
        int bytesRead = recv(clientSocket, courseID, sizeof(courseID) - 1, 0);
        if (bytesRead <= 0) {
            perror("Error while receiving Login Id");
            return ;
        }
        courseID[bytesRead] = '\0';

        
        // Open the courses.txt file for reading and writing
        int fd2 = open("course.txt", O_RDWR);
        if (fd2 == -1) {
            perror("Error opening the file");
            return;
        }

        while (read(fd2, &course_record, sizeof(struct course)) > 0) {
            if ((strcmp(courseID,course_record.course_id ) == 0) && (strcmp("1",course_record.active) == 0)) {
                 cfound = 1;
                 break;
             }  
        }     
        
        if(cfound){
            // Check if there are available seats
            int avail_seats = atoi(course_record.avail_seats);
            
            
            if (avail_seats > 0) {
            
            	 // Open the enrollment.txt to check if student already enrolled for any course or not
    		int fd1 = open("enrollment.txt", O_RDWR | O_CREAT, 0666); // Open the file in append mode

    		if (fd1 == -1) {
       			perror("Error opening the file");
       			return ;
    		}
    		
    		 while (read(fd1, &e, sizeof(struct enroll)) > 0) {
	       		 if (strcmp(studID, e.stud_id) == 0) {
		    		sfound = 1;
		    		p = 0;
		   		break;
	       		 }
   	 	}
            
        if(sfound){
            

            for (int i = 0; i < 2; i++) {
                if (strcmp(e.course_id[i], courseID) == 0) {
                   
                        send(clientSocket, "You are already enrolled in this course.\n", strlen("You are already enrolled in this course.\n"), 0);
                        return;
                    
                }
            }
        
       		for (int i = 0; i < 2; i++){
           		if (strcmp(e.course_id[i], "") != 0){
                	numEnrollments++;
            	}
        	}

        	if (numEnrollments < 2)
        	{
                 // Enroll the student (decrement available seats)
                 avail_seats--;

                memset(course_record.avail_seats,0,sizeof(course_record.avail_seats));
                snprintf(course_record.avail_seats, sizeof(course_record.avail_seats), "%d", avail_seats);

                // Seek back to the beginning of the course record
                lseek(fd2, -sizeof(struct course), SEEK_CUR);

                    // Write the updated course data back to the file
                if (write(fd2, &course_record, sizeof(struct course)) == -1) {
                    perror("Error writing to the file");
                    send(clientSocket, "Error Updating course.txt\n", strlen("Error Updating course.txt\n"), 0);

                    return;

                } else {
                
                    for (int i = 0; i < 2; i++) {
                         if (strcmp(e.course_id[i], "") == 0) {
                             strcpy(e.course_id[i], courseID);
                                break;
                        }
                    }

                        
                    lseek(fd1, -sizeof(struct enroll), SEEK_CUR);
                    if (write(fd1, &e, sizeof(struct enroll)) == -1) {
                        perror("Error writing to the file");
                            send(clientSocket, "Error Updating enrollment.txt\n", strlen("Error Updating enrollment.txt\n"), 0);
                            return;
                    }
                        
                        
                    send(clientSocket, "Enrollment successful\n", strlen("Enrollment successful\n"), 0);
                    memset(studID,0,sizeof(studID));
                    memset(courseID,0,sizeof(courseID));

                }
        			
        
            
       		}else{
       			
            	send(clientSocket, "Student have already enrolled in the maximum allowed courses\n", strlen("Student have already enrolled in the maximum allowed courses\n"), 0);
                return;

        	}
   		}else{

                memset(e.stud_id,0,sizeof(e.stud_id));
                memset(e.course_id[0],'\0',sizeof(e.course_id[0]));
                memset(e.course_id[1],'\0',sizeof(e.course_id[1]));

   		 
   		 	    strcpy(e.stud_id, studID);
        		strcpy(e.course_id[0],courseID);
                strcpy(e.course_id[1],"");



        		
                 // Seek back to the beginning of the course record
                   // lseek(fd2, -sizeof(struct course), SEEK_CUR);

        		if (write(fd1, &e, sizeof(struct enroll)) == -1) {
         			perror("Error writing to the file");
          		  	send(clientSocket, "Error adding student enrollment\n", strlen("Error adding student enrollment\n"), 0);
          		  	close(fd1);
           		 	return;
        		}else{
        		

        			 // Enroll the student (decrement available seats)
                		avail_seats--;
                        memset(course_record.avail_seats,0,sizeof(course_record.avail_seats));

                		snprintf(course_record.avail_seats, sizeof(course_record.avail_seats), "%d", avail_seats);

               			 // Seek back to the beginning of the course record
               			 lseek(fd2, -sizeof(struct course), SEEK_CUR);

               			 // Write the updated course data back to the file
                		if (write(fd2, &course_record, sizeof(struct course)) == -1) {
                   		 perror("Error writing to the file");
                   		 send(clientSocket, "Error Updating course.txt\n", strlen("Error Updating course.txt\n"), 0);

                    		return;

                		} else{
                	
                	        	send(clientSocket, "First Enrollment successful\n", strlen("First Enrollment successful\n"), 0);
                                memset(studID,0,sizeof(studID));
                                memset(courseID,0,sizeof(courseID));
	
                		}
        		
        		}
        		
        		
        		
        		

		}
            
            
    }else {
        send(clientSocket, "No available seats for this course\n", strlen("No available seats for this course\n"), 0);
    }
            
}else{
    send(clientSocket, "Course not found\n", strlen("Course not found\n"), 0);

}
   


   

   
    
}









/*-------------------------DROP A COURSE----------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void drop(int clientSocket, char* auth) {
    char studID[10];
    char courseID[10];

    strcpy(studID, auth);

    struct enroll e;
    struct course course_record;

    // Initialize the enroll structure
    memset(&e, 0, sizeof(struct enroll));

    int sfound = 0, cfound = 0;

    send(clientSocket, "Enter the Course ID to Drop\n", strlen("Enter the Course ID to Drop\n"), 0);
    int bytesRead = recv(clientSocket, courseID, sizeof(courseID) - 1, 0);
    if (bytesRead <= 0) {
        perror("Error while receiving Course ID");
        return;
    }
    courseID[bytesRead] = '\0';

    int fd1 = open("enrollment.txt", O_RDWR); // Open the file for read and write
    int fd2;

    if (fd1 == -1) {
        perror("Error opening the enrollment file");
        return;
    }

    while (read(fd1, &e, sizeof(struct enroll)) > 0) {
        if (strcmp(studID, e.stud_id) == 0) {
            for (int i = 0; i < 2; i++) {
                if (strcmp(e.course_id[i], courseID) == 0) {
                    // Remove the enrolled course
                    memset(e.course_id[i], 0, sizeof(e.course_id[i]));

                    lseek(fd1, -sizeof(struct enroll), SEEK_CUR);
                    if (write(fd1, &e, sizeof(struct enroll)) == -1) {
                        perror("Error writing to the enrollment file");
                        send(clientSocket, "Error Updating enrollment.txt\n", strlen("Error Updating enrollment.txt\n"), 0);
                        return;
                    }

                    sfound = 1;
                    break;
                }
            }
        }
    }

    if (sfound) {
        fd2 = open("course.txt", O_RDWR); // Open the course file for read and write

        if (fd2 == -1) {
            perror("Error opening the course file");
            close(fd1);
            return;
        }

        while (read(fd2, &course_record, sizeof(struct course)) > 0) {
            if (strcmp(courseID, course_record.course_id) == 0) {
                cfound = 1;
                break;
            }
        }

        if (cfound) {
            // Increment available seats
            int total_seats = atoi(course_record.total_seats);
            int avail_seats = atoi(course_record.avail_seats);

            if (avail_seats < total_seats) {
                avail_seats++;

                memset(course_record.avail_seats,0,sizeof(course_record.avail_seats));
                snprintf(course_record.avail_seats, sizeof(course_record.avail_seats), "%d", avail_seats);

                // Seek back to the beginning of the course record
                lseek(fd2, -sizeof(struct course), SEEK_CUR);

                // Write the updated course record back to the file
                if (write(fd2, &course_record, sizeof(struct course)) == -1) {
                    perror("Error writing to the course file");
                    close(fd1);
                    close(fd2);
                    return;
                } else {
                    // Send a "drop successful" message
                    send(clientSocket, "Course dropped successfully. Available seats updated.\n", strlen("Course dropped successfully. Available seats updated.\n"), 0);
                }
            } else {
            
                send(clientSocket, "Can't drop the course. No available seats.\n", strlen("Can't drop the course. No available seats.\n"), 0);
            }
        } else {
            send(clientSocket, "Course not found.\n", strlen("Course not found.\n"), 0);
        }
    } else {
        send(clientSocket, "Student has not enrolled for the respective course\n", strlen("Student has not enrolled for the respective course\n"), 0);
    }

    // Close the files
    close(fd1);
    close(fd2);
}




/*-----------------------------VIEW ENROLLED COURSES-----------------------------------------------------------------------------*/

void viewEn(int clientSocket,char* auth){
    char studID[10];


    strcpy(studID,auth);
    struct enroll e;
    memset(e.stud_id,0,sizeof(e.stud_id));
    memset(e.course_id[0],'\0',sizeof(e.course_id[0]));
    memset(e.course_id[1],'\0',sizeof(e.course_id[1]));

    int fd1 = open("enrollment.txt", O_RDWR | O_CREAT, 0666); // Open the file in append mode

    if (fd1 == -1) {
        perror("Error opening the file");
        return ;
    }
    int found = 0;
    char enrolledCoursesBuffer[1024]; // Adjust the buffer size as needed
    char* bufferPtr = enrolledCoursesBuffer;
    
    while (read(fd1, &e, sizeof(struct enroll)) > 0) {
        if (strcmp(studID, e.stud_id) == 0) {
            found = 1;
            strcpy(bufferPtr, "Enrolled Courses:\n");
            bufferPtr += strlen("Enrolled Courses:\n");
            for (int i = 0; i < 2; i++) {
                if (strcmp(e.course_id[i], "") != 0) {
                    strcpy(bufferPtr, e.course_id[i]);
                    bufferPtr += strlen(e.course_id[i]);
                    strcpy(bufferPtr, "\n");
                    bufferPtr += 1;
                }
            }
            break;
        }
    }
    
    if (!found) {
        strcpy(bufferPtr, "Student not found or not enrolled in any courses.\n");
    }

    send(clientSocket, enrolledCoursesBuffer, strlen(enrolledCoursesBuffer), 0);
    
    close(fd1);
}









/*---------------------------UPDATE PASSWORD------------------------------------------------------------------------------------------------------------------------------------------*/



int updatePassword(int clientSocket, char* auth) {
    
    char buff[1024];
    int fd = open("student.txt", O_RDWR); // Open the file for both reading and writing
    if (fd == -1) {
        perror("Error opening the file");
        return false;
    }

    char studID[10];
    int found = 0;
    strcpy(studID,auth);
    char newPass[10];
    struct student s;

    

    

    // Search for the student with the matching student ID
    while (read(fd, &s, sizeof(struct student)) > 0) {
        if (strcmp(studID, s.stud_id) == 0) {
            found = 1;
            break;
        }
    }

    if (found) {
        // Send a prompt for the new password
        send(clientSocket, "Enter New Password: ", strlen("Enter New Password: "), 0);
        int bytesRead = recv(clientSocket, newPass, sizeof(newPass) - 1, 0);
        if (bytesRead <= 0) {
            perror("Error while receiving new password");
            close(fd); // Close the file
            return false;
        }
        newPass[bytesRead] = '\0';

        memset(s.password,0,sizeof(s.password));
        // Update the Student password
        strcpy(s.password, newPass);

        // Seek back to the beginning of the faculty record
        lseek(fd, -sizeof(struct student), SEEK_CUR);

        // Write the updated faculty data to the file
        if (write(fd, &s, sizeof(struct student)) == -1) {
            perror("Error writing to the file");
            send(clientSocket, "Failed to update Password.\n", strlen("Failed to update Password.\n"), 0);

            close(fd); // Close the file
            
        }
        close(fd); // Close the file
        send(clientSocket, "Password updated successfully.\n", strlen("Password updated successfully.\n"), 0);
       
    } else {
        send(clientSocket, "Faculty not found.\n", strlen("Faculty not found.\n"), 0);
        close(fd); // Close the file
        
    }
}














/*---------------------------STUDENT AUTHENTICATION-----------------------------------------------------------------------------------------------------------------------------------*/


char* student_Authentication(int client_socket) {
    char loginId[100];
    char pass[100];

    // Send a prompt for the username
    const char *uname_msg = "\nEnter StudentID of Student: ";
    send(client_socket, uname_msg, strlen(uname_msg), 0);

    // Receive the username from the client
    ssize_t bytesRead = recv(client_socket, loginId, sizeof(loginId), 0);
    if (bytesRead <= 0) {
        close(client_socket);
        return NULL;
    }
    if (loginId[bytesRead - 1] == '\n') {
        loginId[bytesRead - 1] = '\0';
    } else {
        loginId[bytesRead] = '\0';
    }

    // Send a prompt for the password
    const char *pswd_msg = "\nEnter password of the Student: ";
    send(client_socket, pswd_msg, strlen(pswd_msg), 0);

    // Receive the password from the client
    bytesRead = recv(client_socket, pass, sizeof(pass), 0);
    if (bytesRead <= 0) {
        close(client_socket);
        return NULL;
    }
    if (pass[bytesRead - 1] == '\n') {
        pass[bytesRead - 1] = '\0';
    } else {
        pass[bytesRead] = '\0';
    }

    struct student s;

    int fd = open("student.txt", O_RDONLY);
    if (fd == -1) {
        perror("Error opening the file");
        return NULL;
        
    }
    int found = 0;
    char* loginIdCopy = NULL;

    while (read(fd, &s, sizeof(struct student)) > 0) {
        if (strcmp(loginId, s.stud_id) == 0 && strcmp(pass, s.password) == 0) {
            // Allocate memory for the student's ID
            loginIdCopy = (char*)malloc(strlen(loginId) + 1);
            if (loginIdCopy != NULL) {
                strcpy(loginIdCopy, loginId);
            }
            found = 1;
            break;
        }
    }

    // Close the file
    close(fd);

    if (found) {
        return loginIdCopy; // Return the student ID
    } else {
        free(loginIdCopy); // Free the allocated memory
        return NULL; // Return NULL to indicate authentication failure
    }

    
        
    
}

/*-----------------------------STUDENT FUNCTIONALITY-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

int student_Fun(int client_socket,char* auth) {
    // You can implement admin-specific functionality here
    // This function will be called after successful admin authentication
    // Add your code to handle admin tasks, menu options, etc.
    // For example, sending admin menu options and processing admin tasks

    char student_Menu[] = "\nStudent Menu:\n1. View All Courses\n2. Enroll New Course\n3. Drop Course\n4. View Enrolled Course Details\n5. Change Password\n6. Exit\nEnter Choice\n";

    while (1) {
        send(client_socket, student_Menu, strlen(student_Menu), 0);
        char choice_buffer[10];
        ssize_t bytesRead = recv(client_socket, choice_buffer, sizeof(choice_buffer), 0);

        if (bytesRead <= 0) {
            perror("Error receiving admin menu choice");
            close(client_socket);
            return false;
        }

        int choice = atoi(choice_buffer);

        // Handle Faculty menu options
        switch (choice) {
            case 1:
                 // View all Courses
                viewAllCourses(client_socket);
                
                break;
        
            case 2:
                //Enroll New Course
                enroll(client_socket,auth); 
                break;

            case 3:
                //Drop Course
                drop(client_socket,auth);

                break;

            case 4:
                //View Enrolled Course Details
                viewEn(client_socket,auth);
                break;
            case 5:
                //Change Password
                updatePassword(client_socket,auth);

                break;
            case 6:
                // Exit admin functionality
                return true;

            default:
                // Handle invalid options
                send(client_socket, "Invalid choice. Try again.\n", strlen("Invalid choice. Try again.\n"), 0);
                break;
        }
    }
    
}


