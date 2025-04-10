#include <iostream>
#include <vector>
#include <unordered_map>
#include <list>
#include <algorithm>
#include <string>
#include <fstream>
#include <ctime>


using namespace std;


struct Task {
    int id;
    string description;
    string category;
    time_t due_date;
    bool completed;
    int priority; // 1-5 where 1 is highest priority

    Task(int _id, string _desc, string _cat, time_t _due, int _priority)
        : id(_id), description(_desc), category(_cat), due_date(_due),
        completed(false), priority(_priority) {}

    
    string toString() const {
        return "ID: " + to_string(id) +
            "\nDescription: " + description +
            "\nCategory: " + category +
            "\nDue Date: " + to_string(due_date) + 
            "\nPriority: " + to_string(priority) +
            "\nStatus: " + (completed ? "Completed" : "Pending") + "\n";
    }
};

class TodoList {
private:
  
    list<Task> tasks;

  
    unordered_map<int, list<Task>::iterator> id_map;
    unordered_map<string, vector<list<Task>::iterator>> category_map;

   
    vector<list<Task>::iterator> priority_queue;

    int next_id;

public:
    TodoList() : next_id(1) {}


    // Add a new task
    void addTask(const string& description, const string& category, time_t due_date, int priority) {
       
        if (priority < 1 || priority > 5) {
            cout << "Priority must be between 1 and 5. Setting to default (3)." << endl;
            priority = 3;
        }

   
        tasks.emplace_back(next_id, description, category, due_date, priority);
        auto task_it = prev(tasks.end());

        
        id_map[next_id] = task_it;

        category_map[category].push_back(task_it);

        priority_queue.push_back(task_it);

        // higher priority first, then by due date
        sort(priority_queue.begin(), priority_queue.end(),
            [](const list<Task>::iterator& a, const list<Task>::iterator& b) {
                if (a->priority != b->priority)
                    return a->priority < b->priority; 
                return a->due_date < b->due_date; 
            });

        cout << "Task added with ID: " << next_id << endl;
        next_id++;
    }


    // Remove a task by ID
    bool removeTask(int id) {
        auto it = id_map.find(id);
        if (it == id_map.end()) {
            cout << "Task not found!" << endl;
            return false;
        }

        auto task_it = it->second;

        // Remove from category map
        auto& category_tasks = category_map[task_it->category];
        category_tasks.erase(
            remove_if(category_tasks.begin(), category_tasks.end(),
                [task_it](const list<Task>::iterator& it) { return it == task_it; }),
            category_tasks.end());


        // Remove from priority queue
        priority_queue.erase(
            remove_if(priority_queue.begin(), priority_queue.end(),
                [task_it](const list<Task>::iterator& it) { return it == task_it; }),
            priority_queue.end());


        // Remove from list and id map
        tasks.erase(task_it);
        id_map.erase(id);

        cout << "Task with ID " << id << " removed successfully." << endl;
        return true;
    }


    // Mark a task as completed
    bool completeTask(int id) {
        auto it = id_map.find(id);
        if (it == id_map.end()) {
            cout << "Task not found!" << endl;
            return false;
        }

        it->second->completed = true;
        cout << "Task with ID " << id << " marked as completed." << endl;
        return true;
    }


    // View all tasks
    void viewAllTasks() const {
        if (tasks.empty()) {
            cout << "No tasks found." << endl;
            return;
        }

        cout << "=== ALL TASKS ===" << endl;
        for (const auto& task : tasks) {
            cout << task.toString() << endl;
        }
    }


    // View tasks by category
    void viewTasksByCategory(const string& category) const {
        auto it = category_map.find(category);
        if (it == category_map.end() || it->second.empty()) {
            cout << "No tasks found in category: " << category << endl;
            return;
        }

        cout << "=== TASKS IN CATEGORY '" << category << "' ===" << endl;
        for (const auto& task_it : it->second) {
            cout << task_it->toString() << endl;
        }
    }


    // View tasks by priority
    void viewTasksByPriority() const {
        if (priority_queue.empty()) {
            cout << "No tasks found." << endl;
            return;
        }

        cout << "=== TASKS BY PRIORITY ===" << endl;
        for (const auto& task_it : priority_queue) {
            cout << task_it->toString() << endl;
        }
    }


    // View pending tasks
    void viewPendingTasks() const {
        cout << "=== PENDING TASKS ===" << endl;
        bool found = false;

        for (const auto& task : tasks) {
            if (!task.completed) {
                cout << task.toString() << endl;
                found = true;
            }
        }

        if (!found) {
            cout << "No pending tasks found." << endl;
        }
    }

    // Save tasks to file
    bool saveToFile(const string& filename) const {
        ofstream file(filename);
        if (!file) {
            cout << "Error opening file for writing." << endl;
            return false;
        }

        for (const auto& task : tasks) {
            file << task.id << "|"
                << task.description << "|"
                << task.category << "|"
                << task.due_date << "|"
                << task.priority << "|"
                << task.completed << endl;
        }

        cout << "Tasks saved to " << filename << endl;
        return true;
    }

 
};

// Helper function to get a future date
time_t getFutureDate(int days_from_now) {
    time_t now = time(nullptr);
    return now + (days_from_now * 24 * 60 * 60);
}

int main() {
    TodoList todoList;
    int choice;

 
    todoList.addTask("Complete DSA assignment", "University", getFutureDate(7), 2);
    todoList.addTask("Buy avocado", "Personal", getFutureDate(1), 3);
    todoList.addTask("Pay phone bill", "Finance", getFutureDate(5), 1);
    todoList.addTask("Call Mom", "Personal", getFutureDate(2), 4);

    while (true) {
        cout << "\n=== TODO LIST MENU ===\n";
        cout << "1. Add Task\n";
        cout << "2. Remove Task\n";
        cout << "3. Mark Task as Completed\n";
        cout << "4. View All Tasks\n";
        cout << "5. View Tasks by Category\n";
        cout << "6. View Tasks by Priority\n";
        cout << "7. View Pending Tasks\n";
        cout << "8. Save Tasks to File\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Clear input buffer

        switch (choice) {
        case 1: {
            string description, category;
            int days, priority;

            cout << "Enter task description: ";
            getline(cin, description);

            cout << "Enter category: ";
            getline(cin, category);

            cout << "Enter days until due: ";
            cin >> days;

            cout << "Enter priority (1-5, where 1 is highest): ";
            cin >> priority;

            todoList.addTask(description, category, getFutureDate(days), priority);
            break;
        }
        case 2: {
            int id;
            cout << "Enter task ID to remove: ";
            cin >> id;
            todoList.removeTask(id);
            break;
        }
        case 3: {
            int id;
            cout << "Enter task ID to mark as completed: ";
            cin >> id;
            todoList.completeTask(id);
            break;
        }
        case 4:
            todoList.viewAllTasks();
            break;
        case 5: {
            string category;
            cout << "Enter category to view: ";
            getline(cin, category);
            todoList.viewTasksByCategory(category);
            break;
        }
        case 6:
            todoList.viewTasksByPriority();
            break;
        case 7:
            todoList.viewPendingTasks();
            break;
        case 8: {
            string filename;
            cout << "Enter filename to save tasks: ";
            getline(cin, filename);
            todoList.saveToFile(filename);
            break;
        }
       
        case 0:
            cout << "Exiting program. Goodbye!" << endl;
            return 0;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}
