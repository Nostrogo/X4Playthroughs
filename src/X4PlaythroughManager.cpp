#include "X4PlaytoughManager.h"
#include "X4Playthrough.h"
#include <algorithm>
#include <fstream>

std::vector<X4Playthrough>& X4PlaythroughManager::get_playthrough_list()
{
    return playthrough_list;
}

X4Playthrough* X4PlaythroughManager::get_selected_playthrough()
{
    return selected_playthrough;
}


X4Playthrough* X4PlaythroughManager::get_active_playthrough()
{
    return active_playthrough;
}

// set the selected playthrough and check its files; must be in list
void X4PlaythroughManager::set_selected_playthrough(X4Playthrough* aPlaythroughparam)
{
    // Check if Playthrough is in the list ? not necessary
    selected_playthrough = aPlaythroughparam;
}

// Empty playthrough list and rebuild it by browsing files according to parameters
// also set the active playthrough
void X4PlaythroughManager::refresh_playthrough_list()
{
    std::cout << "X4Playthroughs manager refreshing playthrough list" << std::endl;
    // First empty the vector
    playthrough_list.clear();
    // Clear selected and activate playthrough
    selected_playthrough = nullptr;
    active_playthrough = nullptr;
    // Browse the elements in the data folder
    for(auto element_iterator = std::filesystem::directory_iterator(parameters->get_x4playthroughs_data_folder_path()); element_iterator != std::filesystem::directory_iterator(); ++element_iterator)
    {
        std::filesystem::path element = *element_iterator;
        std::filesystem::file_status status = element_iterator->symlink_status();
        // if the element is a file, it is ignored
        if(std::filesystem::is_regular_file(status))
        {
            std::cout << "file " << element << " is a file -> ignored" << std::endl;
        }
        else if(std::filesystem::is_directory(status))
        {
            // If the file is a directory and the folder "save" exist in it, then it a a playthrough
            std::filesystem::path playthrough_save_folder{element.string() + X4PlaythroughsParameters::X4_PLAYTHROUGHS_FOLDER_SEPARATOR + X4PlaythroughsParameters::X4_PLAYTHROUGHS_SAVE_FOLDER_NAME};
            if(std::filesystem::exists(playthrough_save_folder) && std::filesystem::is_directory(playthrough_save_folder))
            {
                // Create the playthrough from the file path and adding it to the vector
                std::string playthrough_name = element.filename().string();
                std::cout << "folder " << element << " is a valid playthrough folder (contain a save folder) -> creating playthrough " << playthrough_name << std::endl;
                playthrough_list.push_back(X4Playthrough(playthrough_name, parameters->get_x4playthroughs_data_folder_path()));
            }
            else
            {
                // Empty folder -> ignoring
                std::cout << "folder " << element << " does not contains save folder -> ignored" << std::endl;
            }
        }
    }

    // Compute the active playthrough
    // Check if the active flag is present
    for (X4Playthrough& play : playthrough_list)
    {
        std::cout << "X4Playthroughs testing '" <<  play.get_name() << "'" << std::endl;
        std::filesystem::path target(play.get_save_path());
        std::filesystem::path target_active = target / parameters->ACTIVE_FLAG;
        if(std::filesystem::exists(target_active))
        {
            // Play is the activated playthrough
            active_playthrough = &play;
        }
    }

}

bool X4PlaythroughManager::backup_and_delete_x4_save()
{
    bool output = false;

    std::filesystem::path save_folder(parameters->get_x4_save_folder_path());
    std::filesystem::file_status status = std::filesystem::symlink_status(save_folder);
    if(std::filesystem::is_directory(status)) // WARNING under Win 10 MSYS64/mingw64 a symlink will return true !!!
    {
        // Back it up !
        std::stringstream buffer;
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);
        buffer << "save" << std::put_time(&tm, "%Y%m%d%H%M%S"); // save[year][month][day][hour][minute][seconds]
        std::filesystem::path save_folder_backup = std::filesystem::path(parameters->get_x4playthroughs_backup_folder_path()) / buffer.str();
        save_folder_backup = std::filesystem::absolute(save_folder_backup.make_preferred());
        std::cout << "Backing up " << save_folder << " folder to " << save_folder_backup << std::endl;
        std::error_code ec;
        ec.clear();
        std::filesystem::copy(save_folder, save_folder_backup, std::filesystem::copy_options::recursive, ec); // ec is neglected if it fail
        if(ec)
        {
            // Could not backup!
            output = false;
        }
        else
        {
            output = true;
            // Then remove  the directory
            // if it has the activate flag it is a symlink
            std::filesystem::path old_active_flag =  save_folder / parameters->ACTIVE_FLAG;
            if(std::filesystem::exists(old_active_flag))
                output = std::filesystem::remove(save_folder);
            else
            {
                // else it is the regular directory
                std::uintmax_t nb_file_deleted = std::filesystem::remove_all(save_folder); // remove_all follow the symlink ! all saves are deletes
                output = output && (nb_file_deleted != 0);
            }
        }
    }
    else if(status.type() == std::filesystem::file_type::not_found)
    {
        // Does not exist
        // user should check it
        output = false;
    }
    else
    {
        // Does not exist
        // user should check it
        output = false;
    }

    return output;
}

// Check if the name is valid for a folder name and if the playthrough does not already exists.
bool X4PlaythroughManager::is_valid_playthrough_name(std::string aNameParam)
{
    bool output = true;
    std::filesystem::path playthrough_path(parameters->get_x4playthroughs_data_folder_path() + X4PlaythroughsParameters::X4_PLAYTHROUGHS_FOLDER_SEPARATOR + aNameParam);    // Build from name and root_path from parameter std::filesystem::path

    // First check if alphanum
    if(!std::ranges::all_of(aNameParam.begin(), aNameParam.end(), ::isalnum))
    {
        output = false;
        std::cout << std::endl << "Alpha numeric value only" << std::endl; 
    }

    // Then check if space
    //if(std::ranges::none_of(aNameParam.begin(), aNameParam.end(), ::isspace))
    // size_type
    if(aNameParam.find_first_of(X4PlaythroughsParameters::WHITESPACES) != aNameParam.npos)
    {
        output = false;
        std::cout << std::endl << "No space" << std::endl; 
    }

    // Then check if path exist
    if(std::filesystem::exists(playthrough_path))
    {
        output = false;
        std::cout << std::endl << "Non existing file only" << std::endl; 
    }

    return output;
}


// Create a new playthrough and add it in the list; create new playthrough folders (no save yet)
void X4PlaythroughManager::add_new_playthrough_to_list(std::string aPlaythroughNameParam)
{
    // Create a Playthrough
    X4Playthrough aPlayThrough(aPlaythroughNameParam, parameters->get_x4playthroughs_data_folder_path());
    // Add it to the list
    playthrough_list.push_back(aPlayThrough);
    // push_back does not append, if it go first then selected_playthrough points to invalid adress
    selected_playthrough = nullptr;
    active_playthrough = nullptr;
    refresh_playthrough_list();
}

// remove current playthrough from the list and delete it and its save folder and save files
void X4PlaythroughManager::delete_selected_playthrough()
{
    // Delete the files
    get_selected_playthrough()->delete_playthrough_files();
    // remove it from the list and rebuild the list. It will also reset the selected playthrough
    refresh_playthrough_list();
}

// activate the playthrough (remove link and recreate it) using cmd : system("a command here"); from <stdlib.h>
bool X4PlaythroughManager::activate_selected_playthrough()
{
    bool output = false;
    bool continue_after_previous = false;
    // Get the path to the X4 Foundation save folder -> link
    std::filesystem::path link(parameters->get_x4_save_folder_path());
    // Get the path to the save folder of the playthrough -> target
    std::filesystem::path target(get_selected_playthrough()->get_save_path());

    std::cout << "X4PlaythroughManager::activate_selected_playthrough trying to '" << link  << "' -> '" << target << "'" << std::endl;

    // Remove the active flag from previously active playthrough
    if(active_playthrough)
    {
        std::filesystem::path old_active_flag = std::filesystem::path(active_playthrough->get_save_path()) / parameters->ACTIVE_FLAG;
        if(std::filesystem::exists(old_active_flag))
        {
            // Delete the flag
            std::cout << "X4PlaythroughManager::activate_selected_playthrough removing previously active flag '" << old_active_flag  << "'" << std::endl;
            std::filesystem::remove(old_active_flag);
            active_playthrough = nullptr; // Protection
        }
    }

    // Delete the previous link if it exist; if it is a directory, back it up
    // No need to test anymore, symlink is not implemented correctly
    // std::filesystem::file_status status = std::filesystem::symlink_status(link);
    if(!std::filesystem::exists(link))
    {
        // Does not exist
        std::cout << "X4 save folder '" << link  << "' does not exist, continuing" << std::endl;
        continue_after_previous = true;
    }
    else
    {
        // Removing the symbolic link
        std::cout << "Removing '" << link  << "'  X4 save folder or symlink..." << std::endl;
        continue_after_previous = std::filesystem::remove(link);
    }

    // Activating the playthrough by creating the link
    if(continue_after_previous)
    {
        std::cout << "Creating symbolic link " << link << " -> " << target << std::endl;
        // Create the symbolic link (is hardlink possible?)
        std::error_code ec;
        ec.clear();
        std::filesystem::create_directory_symlink(target, link, ec); // Return an error value 40 with message "Function not implemented" on Win10 MSYS64/mingw64
        // Check the link is correctly set
        if(ec)
        {
            std::cout << "Symbolic link " << link << " -> " << target << " creation went wrong..." << std::endl;
            std::cout << "Error value : " << ec.value() << std::endl;
            std::cout << "Error message : " << ec.message() << std::endl;
            output = false;
            // Maybe try win32API (winbase.h/windows.h - Kernel32.lib) with output = CreateSymbolicLinkA (link, target, SYMBOLIC_LINK_FLAG_DIRECTORY); // Admin privilleges still required!
            // Getting command from parameters
            std::string command = parameters->get_os_link_create_cmd();
            // Format the command replacing {Link} with link and {Target} with target
            std::string link_param = "{Link}";
            std::string target_param = "{Target}";
            std::size_t link_pos = command.find(link_param);
            command = command.substr(0, link_pos) + link.string() + command.substr(link_pos+link_param.size());
            std::size_t target_pos = command.find(target_param);
            std::string target_value = std::filesystem::absolute(target.make_preferred()).string(); // Need to transform into absolute path with proper separator or the command will failt
            command = command.substr(0, target_pos) + target_value + command.substr(target_pos+target_param.size());
            // Executing the command using the evil std::system
            std::cout << "Mitigation attempt : executing command " << command << std::endl;
            // Maybe should run ShellExecute(NULL, "runas", "mklink", " /D "{Link}" "{Target}", NULL, SW_SHOWNORMAL);
            int commandReturn = std::system(command.c_str());
            if(commandReturn != 0)
            {
                std::cout << "Mitigation attempt failed : command returned " << commandReturn << std::endl;
                output = false;
            }
            else
            {
                std::cout << "Mitigation attempt succeeded : command returned " << commandReturn << std::endl;
                output = true;
            }
        }
        else
        {
            std::cout << "Symbolic link " << link << " -> " << target << " creation went OK..." << std::endl;
            output = true;
        }
    }
    else
        output = false;

    if(output)
    {
        active_playthrough = selected_playthrough;
        // Creating the active flag
        std::filesystem::path target_active = target / parameters->ACTIVE_FLAG; // Append with separator
        std::ofstream active_flag(target_active.string(), std::ios::out);
        active_flag.flush();
        active_flag.close();
    }

    return output;
}


// Lifecycle
X4PlaythroughManager::X4PlaythroughManager(X4PlaythroughsParameters* parametersParam)
{
    std::cout << "X4Playthroughs manager initializing" << std::endl;
    parameters = parametersParam;
    refresh_playthrough_list();
}

X4PlaythroughManager::~X4PlaythroughManager()
{
    // Do nothing
}

std::ostream& operator<<(std::ostream& out, const X4PlaythroughManager& manager)
{
    out << "Playthrough manager" << std::endl;
    int counter_playthrough = 1; // Starting from 1 so in console GUI user could use this to select a playthrough
    for (const X4Playthrough& play : manager.playthrough_list)
    {
        out << " " << counter_playthrough << " -> " << play;
        ++counter_playthrough;
    }

    if(manager.selected_playthrough)
    {
        out << "SELECTED = " << *manager.selected_playthrough;
    }
    else
        out << "[No Selected Playthrough]" << std::endl;
    
    if(manager.active_playthrough)
    {
        out << "ACTIVE = " << *manager.active_playthrough;
    }
    else
        out << "[No active Playthrough]" << std::endl;

    return out;
}


