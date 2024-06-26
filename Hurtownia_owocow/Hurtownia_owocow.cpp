#include <iostream>
#include <random>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <iomanip>

#include "Person.h"
#include "Customer.h"
#include "Employee.h"
#include "Supplier.h"
#include "Database.h"
#include "Fruit.h"
#include "Storage.h"
#include "Order.h"
#include "Delivery.h"

Database* Database::database = nullptr;
Storage* Storage::storage = nullptr;
void enterData(std::string& login, std::string& password);
void logging(bool logged, int option, std::string& login, std::string& password, Database* database, Storage* storage);
bool customerCondition(std::string login, std::string password, bool logged, Database* database, Storage* storage);
bool employeeCondition(std::string login, std::string password, bool logged, Database* database, Storage* storage);
bool supplierCondition(std::string login, std::string password, bool logged, Database* database, Storage* storage);

int main()
{
    // Wczytanie bazy danych
    Database* database = Database::getInstance();
    database->readFolks();

    // Stworzenie magazynu
    Storage* storage = Storage::getInstance();
    storage->readStorage();

    // Interface logowania
    std::cout << "LOGOWANIE - WYBIERZ TYP KONTA\n";
    std::cout << "1: Klient\n";
    std::cout << "2: Pracownik\n";
    std::cout << "3: Dostawca\n";
    std::cout << "4: Zeby wyjsc z systemu\n";

    int option;
    std::cin >> option;
    bool logged = true;
    std::string login, password;

    // aplikacja
    logging(logged, option, login, password, database, storage);

    return 0;
}

void logging(bool logged, int option, std::string& login, std::string& password, Database* database, Storage* storage)
{
    int newAccount = 0;
    while (logged)
    {
        switch (option)
        {
            case 1:
            {
                std::cout << "Jeżeli chcesz zalożyć konto to wybierz 1, jesli nie 0\n";
                std::cin >> newAccount;

                if (newAccount == 1)
                {
                    enterData(login, password);
                    //addCustomerAccount(login, password);
                    database->addCustomer(login, password);
                    database->readFolks();

                }

                enterData(login, password);
                logged = customerCondition(login, password, logged, database, storage);
                break;
            }
            case 2:
            {
                enterData(login, password);
                logged = employeeCondition(login, password, logged, database, storage);
                break;
            }
            case 3:
            {
                enterData(login, password);
                logged = supplierCondition(login, password, logged, database, storage);
                break;

            }
            case 4:
            {
                logged = false;
                std::cout << "Zapraszamy ponownie :)" << std::endl;
                break;
            }
            default:
            {
                std::cout << "Nieprawidlowy wybor.\n";
                logged = false;
                break;
            }
        }
    }
}


void enterData(std::string& login, std::string& password)
{
    std::cout << "PODAJ DANE\n";
    std::cout << "Podaj login: ";
    std::cin >> login;
    std::cout << "\nPodaj hasło: ";
    std::cin >> password;
    std::cout << "\n";
}

bool customerCondition(std::string login, std::string password, bool logged, Database* database, Storage* storage)
{
    // Logika dla klienta
    if (database->isCustomerExists(login, password))
    {
        int session = 1;
        while (session)
        {
            Customer klient = database->loginCustomer(login, password);
            logged = false;

            std::cout << "\n\n1. Zloz zamowienie\n";
            std::cout << "2. Zloz zwrot zamowienia\n";
            std::cout << "Inna opcja - wyjdz z systemu\n";
            int customerPrompt;
            std::cin >> customerPrompt;

            switch (customerPrompt)
            {
                case 1:
                {
                    std::cout << "DOSTĘPNE OWOCE (Nazwa, cena, ilosc):\n";
                    storage->showStorage();

                    Order newOrder;
                    newOrder.generateOrderID();
                    int choice = 0;
                    do
                    {
                        std::cout << "Podaj nazwe oraz ilosc owocu, ktory ma zostac dodany do zamowienia\n";
                        std::string name;
                        int amount;
                        std::cout << "Nazwa owocu: ";
                        std::cin >> name;

                        if (!storage->isInStorage(name))
                        {
                            std::cout << "Podany owoc nie istnieje\n";
                            continue;
                        }

                        std::cout << "Ilosc owocow (MAX - " << storage->getAmount(name) << "):";
                        std::cin >> amount;

                        newOrder.addToOrder(name, amount, storage);
                        storage->updateFruit(name, storage->getPrice(name), storage->getAmount(name) - amount);
                        std::cout << "\nKontynuowac zakupy? (1 - tak/0 - nie): ";
                        std::cin >> choice;

                        if (choice)
                        {
                            continue;
                        }
                        else
                        {
                            break;
                        }

                    } while (choice);
                    newOrder.showOrder();

                    std::cout << "WYBIERZ SPOSOB PLATNOSCI\n";
                    int payment;
                    std::cout << "1. - Platnosc przelewem\n";
                    std::cout << "2. - Platnosc karta\n";
                    std::cout << "Inne - Anuluj zamowienie\n";
                    std::cin >> payment;
                    switch (payment)
                    {
                        case 1:
                        {
                            std::cout << "Wybrano platnosc przelewem \n";
                            newOrder.addOrder("orders.txt", login);
                            break;
                        }
                        case 2:
                        {
                            std::cout << "Wybrano platnosc karta \n";
                            newOrder.addOrder("orders.txt", login);
                            break;
                        }
                        default:
                        {
                            std::cout << "Anulowanie zamowienia\n";
                            newOrder.cancelOrder(*storage);
                            break;
                        }
                    }
                    break;
                }
                case 2:
                {
                    std::cout << "ZWROT ZAMOWIENIA \n";
                    std::cout << "Podaj numer zamowienia: \n";
                    std::string orderId;
                    std::cin >> orderId;
                    Order oldOrder;
                    oldOrder.readOrder(login, orderId);
                    oldOrder.updateState(login, orderId, "Zwrot");
                    oldOrder.readOrder(login, orderId);
                    oldOrder.showOrder();

                    std::cout << "\nZamowienie " << orderId << " zostanie wkrótce zwrócone :)\n";
                    break;

                }
                default:
                {
                    logged = false;
                    session = 0;
                    break;
                }
            }
        }
    }
    else
    {
        std::cout << "Błędne dane, chcesz spróbować ponownie?\n";
        std::cout << "1 - tak, cokolwiek innego znaczy, że nie :(\n";
        bool opt = false;
        std::cin >> opt;

        if (opt == 1)
        {
            logged = true;
        }
        else
        {
            logged = false;
        }
    }

    return logged;
}

bool employeeCondition(std::string login, std::string password, bool logged, Database* database, Storage* storage)
{
    // Logika dla pracownika
    if (database->isEmployeeExists(login, password))
    {
        Employee pracownik = database->loginEmployee(login, password);
        std::cout << "System niedokończony\n";
        // Tu dodaj działania dla pracownika
        logged = false;
        
    }
    else
    {
        std::cout << "Błędne dane, chcesz spróbować ponownie?\n";
        std::cout << "1 - tak, cokolwiek innego znaczy, że nie :(\n";
        bool opt = false;
        std::cin >> opt;

        if (opt == 1)
        {
            logged = true;
        }
        else
        {
            logged = false;
        }
    }

    return logged;
}

bool supplierCondition(std::string login, std::string password, bool logged, Database* database, Storage* storage)
{
    // Logika dla dostawcy
    if (database->isSupplierExists(login, password))
    {
        Supplier dostawca = database->loginSupplier(login, password);
        std::cout << "System niedokończony\n";
        // Tu dodaj działania dla dostawcy
        logged = false;
    }
    else
    {
        std::cout << "Błędne dane, chcesz spróbować ponownie?\n";
        std::cout << "1 - tak, cokolwiek innego znaczy, że nie :(\n";
        bool opt = false;
        std::cin >> opt;

        if (opt == 1)
        {
            logged = true;
        }
        else
        {
            logged = false;
        }
    }

    return logged;
}