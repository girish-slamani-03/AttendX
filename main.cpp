#include <iostream>
#include <string>
#include <vector>
#include <iomanip>   //for formating
#include <fstream>   //for file
#include <algorithm> //for max()

using namespace std;

class Person
{
private:
    string name;

public:
    Person(string name)
    {
        this->name = name;
    }

    string getName()
    {
        return name;
    }
};

class Student : public Person
{
private:
    string rollNO;
    int attended;
    int totalClasses;

public:
    Student(string name, string rollNO, int present = 0, int total = 0) : Person(name)
    {
        this->rollNO = rollNO;
        this->attended = present;
        this->totalClasses = total;
    }

    string getRollNO()
    {
        return rollNO;
    }

    int getAttended()
    {
        return attended;
    }

    int getTotalClasses()
    {
        return totalClasses;
    }

    void setTotalClasses(int total)
    {
        totalClasses = total;
    }

    void setAttendanceCounts(int present, int total)
    {
        attended = present;
        totalClasses = total;
    }

    float getPercentage()
    {
        if (totalClasses == 0)
            return 0.0;

        return (100.0 * attended) / totalClasses; ;
    }

    void markAttendance(bool present)
    {
        if (present)
            attended++;
    }

    void displayReport()
    {
        cout << left << setw(15) << rollNO
             << setw(25) << getName()
             << right << setw(10) << attended
             << setw(10) << totalClasses
             << setw(14) << fixed << setprecision(1) << getPercentage() << "%" << endl;
    }
};

class AttendanceManager
{
private:
    vector<Student> students;
    int totalClasses = 0;

    const string studentFile = "students.txt";
    const string attendanceFile = "attendance.txt";

    void saveStudents()
    {
        ofstream file(studentFile);
        if (!file)
        {
            cout << "Error: Could not save students file." << endl;
            return;
        }

        // Format: ID | name
        for (Student student : students)
        {
            file << student.getRollNO() << " | " << student.getName() << endl;
        }
    }

    void saveAttendance()
    {
        ofstream file(attendanceFile);
        if (!file)
        {
            cout << "Error: Could not save attendance file." << endl;
            return;
        }

        // Format: ID | attended classes | total classes
        for (Student student : students)
        {
            file << student.getRollNO() << " | " << student.getAttended() << " | " << student.getTotalClasses() << endl;
        }
    }

    //Putting data from file to vector
    void loadStudents()
    {
        ifstream file(studentFile);

        if (!file)
        {
            cout << "File does not exist or cannot be opened!" << endl;
            return;
        }

        string line;

        while (getline(file, line))
        {
            int pos = line.find(" | ");

            if (pos == string::npos)    //Checking whether a substring exists, npos means “no position found”
                continue;

            string idText = line.substr(0, pos);
            string name = line.substr(pos + 3);

            Student s(name, idText);
            students.push_back(s);
        }
    }

    void loadAttendance()
    {
        ifstream file(attendanceFile);

        if (!file)
        {
            cout << "Attendance file does not exist or cannot be opened!" << endl;
            return;
        }

        string line;

        while (getline(file, line))
        {
            int pos1 = line.find(" | ");
            if (pos1 == string::npos)
                continue;

            int pos2 = line.find(" | ", pos1 + 3);
            if (pos2 == string::npos)
                continue;

            string idText = line.substr(0, pos1);
            string attendedText = line.substr(pos1 + 3, pos2 - pos1 - 3);
            string totalText = line.substr(pos2 + 3);

            int attended = stoi(attendedText);  //Converting int from string, stoi means "string to integer"
            int total = stoi(totalText);

            for (Student &student : students)
            {
                if (student.getRollNO() == idText)
                {
                    student.setAttendanceCounts(attended, total);
                    break;
                }
            }

            totalClasses = max(totalClasses, total);
        }
    }

public:
    AttendanceManager()
    {
        loadStudents();
        loadAttendance();
    }

    bool studentIdExists(string rollNO)
    {
        for (auto student : students)
        {
            if (student.getRollNO() == rollNO)
                return true;
        }
        return false;
    }

    void addStudent()
    {
        string name, rollNO ;

        cout << "Enter students Roll NO: ";
        cin >> rollNO;

        if (studentIdExists(rollNO))
        {
            cout << "That Roll NO already exists." << endl;
            return;
        }

        cout << "Enter student name: ";
        getline(cin >> ws, name);   //ws removes leftover leading whitespace

        if (name.empty())
        {
            cout << "Name cannot be empty." << endl;
            return;
        }

        Student s(name, rollNO, 0, totalClasses);
        students.push_back(s);

        saveStudents();
        saveAttendance();

        cout << "Student added successfully." << endl;
    }

    void showStudents()
    {
        if (students.empty())
        {
            cout << "No students registered yet." << endl;
            return;
        }

        cout << endl << "--- Student List ---" << endl;
        cout << left << setw(15) << "Roll NO" << setw(25) << "Name" << endl;
        cout << "-----------------------------------" << endl;
        for (Student student : students)
        {
            cout << left << setw(15) << student.getRollNO() << setw(25) << student.getName() << endl;
        }
    }

    void markAttendance()
    {
        if (students.empty())
        {
            cout << "Add students before marking attendance." << endl;
            return;
        }

        cout << endl << "Enter P for Present or A for Absent." << endl;

        vector<bool> responses;

        for (auto student : students)
        {
            char mark;

            while (true)
            {
                cout << student.getName() << "(Roll NO " << student.getRollNO() << "): ";
                cin >> mark;

                if (mark == 'P' || mark == 'p' || mark == 'A' || mark == 'a')
                {
                    responses.push_back(mark == 'P' || mark == 'p');
                    break;
                }

                cin.ignore(10000, '\n');   // Clears the remaining characters from an invalid input line. (e.g., after reading 'x' from "xyz", discards "yz" and the newline)
                cout << "Please enter only P/p or A/a." << endl;
            }
        }
        totalClasses++;

        for (int i = 0; i < students.size(); i++)
        {
            students[i].markAttendance(responses[i]);
            students[i].setTotalClasses(totalClasses);
        }

        saveAttendance();
        cout << "Attendance saved successfully." << endl;
    }

    void showReport()
    {
        if (students.empty())
        {
            cout << "No students registered yet." << endl;
            return;
        }

        cout << endl
             << "--- Attendance Report ---" << endl;
        cout << left << setw(15) << "Roll NO"
             << setw(25) << "Name"
             << right << setw(10) << "Present"
             << setw(10) << "Total"
             << setw(15) << "Percentage" << endl;
        cout << string(75, '-') << endl;

        for (Student student : students)
        {
            student.displayReport();
        }
    }

    void deleteStudent()
    {
        if (students.empty())
        {
            cout << "No students available to delete." << endl;
            return;
        }

        string rollNO;
        cout << "Enter the student Roll NO to delete: ";
        cin >> rollNO;

        for (int i = 0; i < students.size(); i++)
        {
            if (students[i].getRollNO() == rollNO)
            {
                cout << "Student found: " << students[i].getName() << endl;

                cout << "Are you sure you want to delete this student? (Y/N): ";
                char confirm;
                cin >> confirm;

                if (confirm != 'Y' && confirm != 'y')
                {
                    cout << "Deletion cancelled." << endl;
                    return;
                }

                students.erase(students.begin() + i);

                saveStudents();
                saveAttendance();

                cout << "Student deleted successfully." << endl;
                return;
            }
        }

        cout << "Student rollNO not found." << endl;
    }
};

int main()
{
    AttendanceManager manager;
    int choice;

    while (true)
    {
        cout << endl
             << "========== AttendX ==========" << endl;
        cout << "1. Mark attendance" << endl;
        cout << "2. Add students" << endl;
        cout << "3. Show students" << endl;
        cout << "4. Attendance report" << endl;
        cout << "5. Delete student" << endl;
        cout << "0. Exit" << endl;
        cout << "Choose an option: ";

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Please enter a number." << endl;
            continue;
        }

        switch (choice)
        {
        case 1:
            manager.markAttendance();
            break;
        case 2:
            manager.addStudent();
            break;
        case 3:
            manager.showStudents();
            break;
        case 4:
            manager.showReport();
            break;
        case 5:
            manager.deleteStudent();
            break;
        case 0:
            cout << "Thank you for using AttendX." << endl;
            return 0;
        default:
            cout << "Invalid choice. Try again." << endl;
        }
    }
}