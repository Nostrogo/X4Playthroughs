#ifndef CONSOLE_GUI_H
#define CONSOLE_GUI_H

#include <iostream>
#include "X4PlaytoughManager.h"

class X4PlaythroughsConsoleUI
{
    public:
        enum ACTION { FAILT_INPUT, NEW, REFRESH, SELECT, ACTIVATE, DELETE, BACKUP, QUIT };
        static const int NO_PLAYTHROUGH_SELECTED{-1};
    protected:
        X4PlaythroughManager *manager{nullptr};

    protected:
        void print_manager(X4PlaythroughManager* manager)
        {
            std::cout << *manager;
        };

        void print_menu()
        {
            std::cout << std::endl << "X4Playthroughs Console UI menu : " << std::endl;
            std::cout << NEW << " - NEW playthrough" << std::endl;
            std::cout << REFRESH << " - REFRESH playthrough list from files" << std::endl;
            std::cout << SELECT << " - SELECT a playthrough" << std::endl;
            std::cout << ACTIVATE << " - ACTIVATE the selected playthrough" << std::endl;
            std::cout << DELETE << " - DELETE the selected playthrough" << std::endl;
            std::cout << BACKUP << " - BACKUP then delete your X4 save folder" << std::endl;
            std::cout << QUIT << " - QUIT" << std::endl;
            std::cout << "Input command : ";
        };

        int select_action()
        {
            int output;
            print_menu();
            std::cin >> output;
            if(std::cin.fail())
            {
                // User did not type an int
                output = FAILT_INPUT;
                std::cin.clear(); // Repairing the stream
                std::cin.ignore(std::numeric_limits<int>::max(),'\n'); // removing any bad values
            }
            return output;
        };

        int select_playthrough(X4PlaythroughManager *manager)
        {
            int output;
            print_manager(manager);
            std::cout << "Input playthrough number : ";
            std::cin >> output;
            if(std::cin.fail())
            {
                // User did not type an int
                output = FAILT_INPUT;
                std::cin.clear(); // Repairing the stream
                std::cin.ignore(std::numeric_limits<int>::max(),'\n'); // removing any bad values
            }
            return output;
        };

        std::string get_new_playthroug_name()
        {
            std::string output{""};
            std::cout << "Input New playthrough name (must be a valid directory name) : ";
            std::cin.clear(); // Repairing the stream
            std::cin.ignore(std::numeric_limits<int>::max(),'\n'); // removing any bad values
            std::getline(std::cin, output);
            return output;
        };

        bool request_confirmation(std::string message)
        {
            bool output = false;
            std::string yes_no{"No"};
            std::cout << message;
            std::cin.clear(); // Repairing the stream
            std::cin.ignore(std::numeric_limits<int>::max(),'\n'); // removing any bad values
            std::getline(std::cin, yes_no);
            // Very restrictive, input only y nothing else will work
            if(yes_no == "y")
                output = true;
            return output;
        }

        bool request_delete_confirmation()
        {
            return request_confirmation("This will delete the selected playthrough. Are you sure [y to confirm] :");
        };

        bool request_backup_confirmation()
        {
            return request_confirmation("This will backup your X4 foundation save folder to the X4 Playthroughs backup folder. Then it will delete the X4 Foundation save folder to allow playthrough substitution. Are you sure [y to confirm] :");
        };

        bool request_activate_confirmation()
        {
            return request_confirmation("This will activate the selected playthrough. It will alter your X4 Foundation save folder. Please back it up before proceeding. Proceed [y to confirm] ?");
        };

    public:
        X4PlaythroughsConsoleUI(X4PlaythroughManager *manager_param):manager(manager_param)
        {
            // Do nothing else
        }

        void start_hmi()
        {
            bool continue_loop = true;
            if(!manager)
                continue_loop = false; // Protection in case manager was not correctly set
            int playthrough_number = NO_PLAYTHROUGH_SELECTED;
            std::string aPlaythroughName{""};
            while(continue_loop)
            {
                // Adding more space before printing manager
                std::cout << std::endl << std::endl;

                // Printing loaded playthrough
                print_manager(manager);

                int aUserAction = select_action();

                // Adding more space between command
                std::cout << std::endl << std::endl << std::endl << std::endl;

                switch (aUserAction)
                {
                    case NEW:
                        std::cout << std::endl << "X4Playthroughs Console UI NEW command..." << std::endl << std::endl;
                        aPlaythroughName = get_new_playthroug_name();
                        // Trimming parameter name
                        aPlaythroughName.erase(aPlaythroughName.find_last_not_of(X4PlaythroughsParameters::WHITESPACES)+1); // Removing whitespaces in the end
                        aPlaythroughName.erase(0, aPlaythroughName.find_first_not_of(X4PlaythroughsParameters::WHITESPACES)); // Removing whitespaces in the begining

                        // Checking output validity
                        if(manager->is_valid_playthrough_name(aPlaythroughName))
                            manager->add_new_playthrough_to_list(aPlaythroughName);
                        else
                            std::cout << std::endl << "X4Playthroughs Console UI NEW command : invalid playthrough name '" << aPlaythroughName << "'. Command aborted." << std::endl;

                        aPlaythroughName = "";
                        break;
                    case REFRESH:
                        std::cout << std::endl << "X4Playthroughs Console UI REFRESH command..." << std::endl << std::endl;
                        manager->refresh_playthrough_list();
                        break;
                    case SELECT:
                        std::cout << std::endl << "X4Playthroughs Console UI SELECT command..." << std::endl << std::endl;
                        playthrough_number = select_playthrough(manager);
                        if(playthrough_number != NO_PLAYTHROUGH_SELECTED)
                        {
                            X4Playthrough* selected_playthrough{nullptr};
                            playthrough_number -= 1; // User selection started from 1, vector starts at 0
                            if((playthrough_number >= 0) &&(playthrough_number < manager->get_playthrough_list().size()))
                            {
                                manager->set_selected_playthrough(&(manager->get_playthrough_list().at(playthrough_number)));
                            }
                            else
                            {
                                std::cout << std::endl << "X4Playthroughs Console UI SELECT command : invalid playthrough number. Command aborted." << std::endl;
                            }
                        }
                        break;
                    case ACTIVATE:
                        std::cout << std::endl << "X4Playthroughs Console UI ACTIVATE command" << std::endl << std::endl;
                        if(request_activate_confirmation())
                        {
                            if(manager->get_selected_playthrough())
                            {
                                bool result = manager->activate_selected_playthrough();
                                if(!result)
                                    std::cout << std::endl << "X4Playthroughs Console UI ACTIVATE command : ACTIVATION FAILED, check you save folder!" << std::endl;
                            }
                            else
                            {
                                std::cout << std::endl << "X4Playthroughs Console UI ACTIVATE command : No active playthrough. Command aborted." << std::endl;
                            }
                        }
                        else
                        {
                            std::cout << std::endl << "X4Playthroughs Console UI ACTIVATE command : Command aborted." << std::endl;
                        }
                        break;
                    case DELETE:
                        std::cout << std::endl << "X4Playthroughs Console UI DELETE command..." << std::endl << std::endl;
                        if(manager->get_selected_playthrough())
                        {
                            // ask for confirmation:
                            bool delete_confirmed = request_delete_confirmation();
                            if(delete_confirmed)
                                manager->delete_selected_playthrough();
                            else
                                std::cout << std::endl << "X4Playthroughs Console UI DELETE command : Command aborted." << std::endl;
                        }
                        else
                        {
                            std::cout << std::endl << "X4Playthroughs Console UI DELETE command : no selected playthrough Command aborted." << std::endl;
                        }
                        break;
                    case BACKUP:
                        std::cout << std::endl << "X4Playthroughs Console UI BACKUP command..." << std::endl << std::endl;
                        // ask for confirmation:
                        { // Needed for bool backup_confirmed initialization
                            bool backup_confirmed = request_backup_confirmation();
                            if(backup_confirmed)
                            {
                                bool result = false;
                                result = manager->backup_and_delete_x4_save();
                                if(result)
                                    std::cout << std::endl << "X4Playthroughs Console UI BACKUP command : Backup performed, save folder deleted" << std::endl;
                                else
                                    std::cout << std::endl << "X4Playthroughs Console UI BACKUP command : Error while processing, please check your X4 Foundation save folder" << std::endl;
                            }
                            else
                                std::cout << std::endl << "X4Playthroughs Console UI BACKUP command : Command aborted." << std::endl;
                        }
                        break;
                    case QUIT:
                        std::cout << std::endl << "X4Playthroughs Console UI QUIT command..." << std::endl << "Thank you for using this software." << std::endl << std::endl;
                        continue_loop = false;
                        break;
                    default:
                        std::cout << "X4Playthroughs Console UI: unknown commande" << std::endl;

                }
            }

        };

};

#endif
