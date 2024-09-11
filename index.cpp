#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

struct Item
{
    int id;
    string name;
    double price;
    int stock;

    Item() : id(0), name(""), price(0.0), stock(0) {}

    Item(int i, const string &n, double p, int s)
        : id(i), name(n), price(p), stock(s) {}
};

struct Order
{
    int order_id;
    vector<Item> items;
    vector<int> quantities;
    double total_price;
};

void saveOrderIdCounter(int order_id_counter);
int loadOrderIdCounter();
Order takeOrder(vector<Item> &inventory, int &order_id_counter);
void displayMenu(const vector<Item> &inventory);
Item *searchItemById(vector<Item> &inventory, int id);
void changeItemQuantity(vector<Item> &inventory);
void saveInventoryToFile(const vector<Item> &inventory);
vector<Item> loadInventoryFromFile();
void saveOrderToFile(const Order &order, int &order_id_counter);
vector<Order> loadOrdersFromFile();
void displayBill(const Order &order);
void viewOrderById(int order_id);

void displayMenu(const vector<Item> &inventory)
{
    cout << "\n========================= Available Items =========================\n";
    cout << setw(4) << "ID" << " | "
         << setw(20) << left << "Name" << " | "
         << setw(10) << "Price" << " | "
         << setw(10) << "Quantity" << " | " << endl;
    cout << "====================================================================\n";

    for (size_t i = 0; i < inventory.size(); ++i)
    {
        const Item &item = inventory[i];
        cout << setw(4) << item.id << " | "
             << setw(20) << left << item.name << " | "
             << setw(10) << fixed << setprecision(2) << item.price << " | "
             << setw(10) << item.stock << " | " << endl;
    }
    cout << "====================================================================\n";
}

Item *searchItemById(vector<Item> &inventory, int id)
{
    for (size_t i = 0; i < inventory.size(); ++i)
    {
        Item &item = inventory[i];
        if (item.id == id)
        {
            return &item;
        }
    }
    return NULL;
}

void changeItemQuantity(vector<Item> &inventory)
{
    int item_id, new_quantity;
    displayMenu(inventory);
    cout << "Enter the item ID to update quantity: ";
    cin >> item_id;

    Item *item = searchItemById(inventory, item_id);
    if (item != NULL)
    {
        cout << "Current quantity for " << item->name << " is " << item->stock << ".\n";
        cout << "Enter new quantity: ";
        cin >> new_quantity;

        if (new_quantity < 0)
        {
            cout << "Quantity cannot be negative.\n";
        }
        else
        {
            item->stock = new_quantity;
            cout << "Quantity updated to " << item->stock << " for " << item->name << ".\n";
        }
    }
    else
    {
        cout << "Invalid item ID!\n";
    }
}

Order takeOrder(vector<Item> &inventory, int &order_id_counter)
{
    Order order;
    order.order_id = order_id_counter;
    order.total_price = 0.0;

    int item_id, quantity;
    char addMore;

    do
    {
        displayMenu(inventory);
        cout << "Enter the item ID to add to your order: ";
        cin >> item_id;

        Item *item = searchItemById(inventory, item_id);
        if (item != NULL)
        {
            cout << "Enter quantity for " << item->name << ": ";
            cin >> quantity;

            if (quantity <= item->stock)
            {
                item->stock -= quantity;
                order.items.push_back(*item);
                order.quantities.push_back(quantity);
                order.total_price += item->price * quantity;
                cout << quantity << " x " << item->name << " added to the order.\n";
            }
            else
            {
                cout << "Sorry, insufficient stock for " << item->name << ".\n";
            }
        }
        else
        {
            cout << "Invalid item ID!\n";
        }

        cout << "Add another item? (y/n): ";
        cin >> addMore;

    } while (addMore == 'y' || addMore == 'Y');

    return order;
}

void saveOrderToFile(const Order &order)
{
    ofstream orderFile("orders.txt", ios::app);
    if (orderFile.is_open())
    {
        orderFile << "\n===== COFFEE SHOP BILL =====\n";
        orderFile << "Order ID: " << order.order_id << "\n";
        orderFile << "-----------------------------------------------\n";
        orderFile << left << setw(15) << "Item" << setw(10) << "Qty" << setw(10) << "Price" << setw(15) << "Total Price" << "\n";
        orderFile << "-----------------------------------------------\n";

        for (size_t i = 0; i < order.items.size(); ++i)
        {
            const Item &item = order.items[i];
            int quantity = order.quantities[i];
            double total_item_price = item.price * quantity;
            orderFile << left << setw(15) << item.name << setw(10) << quantity
                      << setw(10) << fixed << setprecision(2) << item.price
                      << setw(15) << fixed << setprecision(2) << total_item_price << "\n";
        }

        orderFile << "-----------------------------------------------\n";
        orderFile << "Total Price: $" << fixed << setprecision(2) << order.total_price << "\n";
        orderFile << "===== THANK YOU! COME AGAIN. =====\n\n";
        orderFile.close();
    }
}

vector<Order> loadOrdersFromFile()
{
    vector<Order> orders;
    ifstream orderFile("orders.txt");
    if (orderFile.is_open())
    {
        string line;
        while (getline(orderFile, line))
        {
            if (line.find("Order ID:") != string::npos)
            {
                Order order;
                order.total_price = 0.0;

                stringstream ss(line.substr(line.find(":") + 1));
                ss >> order.order_id;

                getline(orderFile, line);

                getline(orderFile, line);

                while (getline(orderFile, line) && line.find("Total Price:") == string::npos)
                {
                    if (line.empty() || line.find("====") != string::npos || line.find("Order ID:") != string::npos)
                        continue;

                    istringstream iss(line);
                    string item_name;
                    int quantity;
                    double item_price, total_item_price;

                    iss >> item_name >> quantity >> item_price >> total_item_price;

                    if (!iss.fail() && quantity > 0)
                    {
                        Item item;
                        item.name = item_name;
                        item.price = item_price;

                        order.items.push_back(item);
                        order.quantities.push_back(quantity);
                        order.total_price += total_item_price;
                    }
                }

                orders.push_back(order);
            }
        }
        orderFile.close();
    }
    return orders;
}

void saveInventoryToFile(const vector<Item> &inventory)
{
    ofstream inventoryFile("inventory.txt");
    if (inventoryFile.is_open())
    {
        for (size_t i = 0; i < inventory.size(); ++i)
        {
            const Item &item = inventory[i];
            inventoryFile << item.id << " " << item.name << " " << item.price << " " << item.stock << "\n";
        }
        inventoryFile.close();
    }
}

vector<Item> loadInventoryFromFile()
{
    vector<Item> inventory;
    ifstream inventoryFile("inventory.txt");
    if (inventoryFile.is_open())
    {
        int id, stock;
        double price;
        string name;

        while (inventoryFile >> id >> name >> price >> stock)
        {
            inventory.push_back(Item(id, name, price, stock));
        }

        inventoryFile.close();
    }
    return inventory;
}

void saveOrderIdCounter(int order_id_counter)
{
    ofstream counterFile("order_id_counter.txt");
    if (counterFile.is_open())
    {
        counterFile << order_id_counter;
        counterFile.close();
    }
}

int loadOrderIdCounter()
{
    ifstream counterFile("order_id_counter.txt");
    int order_id_counter = 1;
    if (counterFile.is_open())
    {
        counterFile >> order_id_counter;
        counterFile.close();
    }
    return order_id_counter;
}

void displayBill(const Order &order)
{
    cout << "\n===== COFFEE SHOP BILL =====\n";
    cout << "Order ID: " << order.order_id << "\n";
    cout << "-----------------------------------------------\n";
    cout << left << setw(15) << "Item" << setw(10) << "Qty" << setw(10) << "Price" << setw(15) << "Total Price" << "\n";
    cout << "-----------------------------------------------\n";

    for (size_t i = 0; i < order.items.size(); ++i)
    {
        const Item &item = order.items[i];
        int quantity = order.quantities[i];
        double total_item_price = item.price * quantity;
        cout << left << setw(15) << item.name << setw(10) << quantity
             << setw(10) << fixed << setprecision(2) << item.price
             << setw(15) << fixed << setprecision(2) << total_item_price << "\n";
    }

    cout << "-----------------------------------------------\n";

    cout << "Total Price: $" << fixed << setprecision(2) << order.total_price << "\n";
    cout << "===== THANK YOU! COME AGAIN. =====\n";
}

void viewOrderById(int order_id)
{
    vector<Order> orders = loadOrdersFromFile();
    for (size_t i = 0; i < orders.size(); ++i)
    {
        const Order &order = orders[i];
        if (order.order_id == order_id)
        {
            displayBill(order);
            return;
        }
    }
    cout << "Order with ID " << order_id << " not found.\n";
}

int main()
{
    vector<Item> inventory = loadInventoryFromFile();
    int order_id_counter = loadOrderIdCounter();

    int choice;

    do
    {
        cout << "\n===== COFFEE SHOP MANAGEMENT SYSTEM =====\n";
        cout << "1. Take Order\n";
        cout << "2. Change Item Quantity\n";
        cout << "3. View Order by ID\n";
        cout << "4. Save and Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            Order order = takeOrder(inventory, order_id_counter);
            displayBill(order);
            saveOrderToFile(order);
            saveInventoryToFile(inventory);
            break;
        }
        case 2:
            changeItemQuantity(inventory);
            saveInventoryToFile(inventory);
            break;
        case 3:
        {
            int order_id;
            cout << "Enter order ID: ";
            cin >> order_id;
            viewOrderById(order_id);
            break;
        }
        case 4:
            saveInventoryToFile(inventory);
            saveOrderIdCounter(order_id_counter);
            cout << "Data saved successfully. Exiting...\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 4);

    return 0;
}
