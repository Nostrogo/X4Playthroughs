#include <iostream>
#include <vector>
#include <string>
#include "X4PlaythroughsParameters.h"
#include "X4PlaytoughManager.h"
#include "X4PlaythroughsConsoleUI.h"
#include "X4PlaythroughsUI.h"
#include <windows.h>

void print_error_msg(std::string_view msg)
{
    std::cerr << msg << std::endl;
};

std::ostream& get_help_msg(std::ostream& out)
{
    out << "X4 Foundations Playthroughs by LLA"<< std::endl;
    out << "This program enables multiple save folders (ideally one per playthrough) for X4 foundations by Egosoft." << std::endl;
    out << "This way, each playthrough can benefit from all possible saves." << std::endl;
    out << "This software works on the OS level with folders. It is independent from your game and can be installed anywhere." << std::endl;
    out << "Please note that this does not modify the X4 Foundations game in any way, your saves will not be *modified*." << std::endl;
    out << "Please back up your X4 foundation save folder prior to using this software." << std::endl;
    out << "This software will replace the X4 foundation save folder by a link to the selected playthrough save folder." << std::endl;
    out << "Do not forget to deactivate Steam Cloud for the save files" << std::endl;
    out << "(unless you want steam cloud to overwrite all your playthroughs)" << std::endl;
    out << "This is done in the General tab of the game Properties on steam." << std::endl;
    out << "Features: " << std::endl;
    out << " - Edit configuration (X4 Foundation save folder location, X4 Foundation Playthroughs data folder, X4 Foundation executable, OS specificities) (Not implemented yet)" << std::endl;
    out << " - Create a news save folder and name it according to a playthrough." << std::endl;
    out << " - Delete a previously created save folder." << std::endl;
    out << " - Setting a save folder as the current X4 Foundation save folder." << std::endl;
    return out;
}

void print_help()
{
    get_help_msg(std::cout);
}

/**
 * @brief X4Playthroughs.exe main function: using OS sym links to redirect default X4 save folder to a dedicated save folder for the playthrough
 * 
 * @return int 0 if ok
 */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
//int main(int argc, char** argv)
//{
    // Welcome message
    std::cout << "X4 Foundations Playthroughs"<< std::endl << std::endl;

/**
    // Reading arguments
    std::vector<std::string> args {};
    for(size_t i = 0; i < argc; ++i)
    {
        //args.insert(iterator,std::string{argv[i]});
        args.push_back(std::string{argv[i]});
    }

    // Processing arguments
    if(args.size() > 1 && args[1].compare("-h") == 0)
    {
        print_help();
        return 0;
    }
    else if(args.size() > 1 && args[1].compare("-H") == 0)
    {
        print_help();
        return 0;
    }
    else if(args.size() > 1)
    {
        print_error_msg("Unrecognized parameters. Try X4Playthroughs.exe -h for help");
        return 1;
    }
**/

    // Get parameters
    X4PlaythroughsParameters parameters;

    if(parameters.are_parameter_completly_set())
    {
        std::cout << "X4 Foundations Playthroughs ready to start with the following parameters."<< std::endl;
        parameters.print();

        // Create manager
        X4PlaythroughManager manager(&parameters);

        // Start HMI
        if(parameters.get_x4playthroughs_hmi().compare(X4PlaythroughsParameters::X4_PLAYTHROUGHS_UI_GUI) == 0)
        {
            std::cout << "Showing windows..." << std::endl;
            // User selected GUI
            X4PlaythroughsUI gui(&manager, hInst, hPrevInst);
            gui.start_hmi();

        }
        else
        {
            // Default = console HMI
            X4PlaythroughsConsoleUI gui{&manager};
            gui.start_hmi();
        }
    }
    else
    {
        // Unable to proccess parameters are incomplete
        print_error_msg("Unrecognized configuration. Try X4Playthroughs.exe -h for help");
    }

    return 0;
}
