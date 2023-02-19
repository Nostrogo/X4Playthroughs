#include "X4PlaythroughsParameters.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>

// STATIC CONST INIT
const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_UI_CONSOLE{"console"};
const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_UI_GUI{"gui"};
const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_UI_PARAM_NAME{"x4playthroughs.ui"};


const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_CONFIG_FILENAME{"X4Playthroughs.conf"};
const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_DATA_FOLDER_PATH{"./data"};
const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_BACKUP_FOLDER_PATH{"./backup"};
const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_RELATIVE_ROOT_PATH{"."};
const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_FOLDER_SEPARATOR{"/"};
const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_SAVE_FOLDER_NAME{"save"};

const std::string X4PlaythroughsParameters::X4_SAVE_FOLDER_PATH_PARAM_NAME{"x4.save"};
const std::string X4PlaythroughsParameters::X4_EXECUTABLE_PATH_PARAM_NAME{"x4.exe"};
const std::string X4PlaythroughsParameters::X4_LINK_CREATE_CMD_PARAM_NAME{"x4playthroughs.create.link.cmd"};
const std::string X4PlaythroughsParameters::X4_LINK_DELETE_CMD_PARAM_NAME{"x4playthroughs.delete.link.cmd"};

const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_LOG_CONFIGFILE{"X4Playthroughs - opening configuration file : "};
const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_LOG_CONFIGFILE_COMPLETE{"X4Playthroughs - configuration file has been parsed."};
const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_LOG_CONFIGFILE_UNKNOWN_PARAM{"X4Playthroughs - Unknown parameter name : "};
const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_LOG_TESTING_PARAM{"X4Playthroughs - testing parameter "};

const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_ERR_CONFIGFILE_OPENING{"X4Playthroughs - Error while opening configuration file : "};
const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_ERR_PARAMETER_EMPTY{"X4Playthroughs - Error: parameter is not set"};
const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_ERR_PARAMETER_DIRECTORY{"X4Playthroughs - Error: wrong parameter type - directory expected at "};
const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_ERR_PARAMETER_FILE{"X4Playthroughs - Error: wrong parameter type - file expected at "};
const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_ERR_PARAMETER_ACCESS{"X4Playthroughs - Error: cannot access parameter at "};

const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_SAVE_FOLDER_AT{"X4Playthroughs playthrough save folders will be stored at "};
const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_UI_IS{"X4 Foundations Playthroughs UI is "};

const std::string X4PlaythroughsParameters::WHITESPACES = " \t\n\r\f\v";
const char X4PlaythroughsParameters::X4_PLAYTHROUGHS_CONFIGFILE_COMMENT_CHAR{'#'};
const std::string X4PlaythroughsParameters::X4_PLAYTHROUGHS_CONFIGFILE_SEPARATOR{"="};
const std::string X4PlaythroughsParameters::ACTIVE_FLAG{"active"};

const int X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_WIDTH = 800;
const int X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_HEIGHT = 600;
const int X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_VERTICAL_SPACING = 5;
const int X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_HORIZONTAL_SPACING = 10;
const std::wstring X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_HOWTO = L"X4Playthroughs : a playthrough manager for X4 Foundations. \n\n\
The list on the left displays all available playthroughs.\n\
Only one playthrough can be active at once. Your X4 save folder is linked to this playthrough save directory.\n\
/!\\ WARNING : Depending on the command (check config file) to create the link you might need to run this program with Administrator privilege./!\\\n\n\
To activate a playthrough, select it on the list and click the Activate button.\n\n\
To create a playthrough, type in the name of the playthrough (less than 255 alphanumeric characters) \
in the field next to the *New playthrough* button, then click on said button.\n\n\
To delete a playthrough, select it on the list and click the Delete button. \n\
/!\\ WARNING : this will delete the playthrough base directory and all saves in it./!\\\n\n\
To refresh the list, click on the Refresh button. it may comes handy if a file/folder have been manually modified outside this program.";

/**
 * @brief Construct a new X4PlaythroughsParameters::X4PlaythroughsParameters object Parse the configuration file
 * 
 */
X4PlaythroughsParameters::X4PlaythroughsParameters()
{
    // Creating the path to the config file
    std::string config_file_path = X4_PLAYTHROUGHS_DATA_FOLDER_PATH + X4_PLAYTHROUGHS_FOLDER_SEPARATOR + X4_PLAYTHROUGHS_CONFIG_FILENAME; // Relative path linux style function on windows as well
    x4playthroughs_data_folder_path = X4_PLAYTHROUGHS_DATA_FOLDER_PATH; // Create the folders in data
    x4playthroughs_backup_folder_path = X4_PLAYTHROUGHS_BACKUP_FOLDER_PATH;

    std::clog << X4_PLAYTHROUGHS_LOG_CONFIGFILE << config_file_path << std::endl;

    // Opening the config file
    std::ifstream config_file_stream(config_file_path);

    // Reading the config file
    if (config_file_stream.is_open())
    {
        // File is open, reading line by line
        std::string line;
        while(std::getline(config_file_stream,line))
        {
            // Line trimming
            line.erase(line.find_last_not_of(WHITESPACES)+1); // Removing whitespaces in the end
            line.erase(0, line.find_first_not_of(WHITESPACES)); // Removing whitespaces in the begining

            // if the line is not empty or start with an # for comments
            if(!( line.empty() || line[0] == X4_PLAYTHROUGHS_CONFIGFILE_COMMENT_CHAR ))
            {
                // Reading the parameters
                // Separating parameter name from parameter value
                std::size_t delimiterPos = line.find(X4_PLAYTHROUGHS_CONFIGFILE_SEPARATOR);
                std::string name = line.substr(0, delimiterPos);
                std::string  value = line.substr(delimiterPos + 1);
                // Trimming parameter name
                name.erase(name.find_last_not_of(WHITESPACES)+1); // Removing whitespaces in the end
                name.erase(0, name.find_first_not_of(WHITESPACES)); // Removing whitespaces in the begining
                // Trimming parameter value
                value.erase(value.find_last_not_of(WHITESPACES)+1); // Removing whitespaces in the end
                value.erase(0, value.find_first_not_of(WHITESPACES)); // Removing whitespaces in the begining

                // Assigning value to parameters
                if(name.compare(X4_SAVE_FOLDER_PATH_PARAM_NAME) == 0)
                    x4_save_folder_path = value;
                else if(name.compare(X4_EXECUTABLE_PATH_PARAM_NAME) == 0)
                    x4_executable_path = value;
                else if(name.compare(X4_LINK_CREATE_CMD_PARAM_NAME) == 0)
                    os_link_create_cmd = value;
                else if(name.compare(X4_LINK_DELETE_CMD_PARAM_NAME) == 0)
                    os_link_delete_cmd = value;
                else if(name.compare(X4_PLAYTHROUGHS_UI_PARAM_NAME) == 0)
                    x4playthroughs_hmi = value;
                else
                {
                    // unknown parameter name
                    std::clog << X4_PLAYTHROUGHS_LOG_CONFIGFILE_UNKNOWN_PARAM << name << std::endl;
                }

            }
        }
        std::clog << X4_PLAYTHROUGHS_LOG_CONFIGFILE_COMPLETE << std::endl;
        // config_file_stream.close(); // invoked at destructor not needed
    }
    else
    {
        // Unable to open configuration file
        std::cerr << X4_PLAYTHROUGHS_ERR_CONFIGFILE_OPENING << std::endl;
    }

}

X4PlaythroughsParameters::~X4PlaythroughsParameters()
{
}

bool X4PlaythroughsParameters::are_parameter_completly_set()
{
    bool output = false;

    std::clog << std::endl;
    bool x4_save_folder_path_test = is_X4_save_folder_path_set();
    bool x4_executable_path_test = is_X4_executable_path_set();
    bool os_link_create_cmd_test = is_os_link_create_cmd_set();
    bool os_link_delete_cmd_test = is_os_link_delete_cmd_set();
    std::clog << std::endl;

    if(x4_save_folder_path_test && x4_executable_path_test && os_link_create_cmd_test && os_link_delete_cmd_test)
        output = true;

    return output;
}

bool X4PlaythroughsParameters::is_X4_save_folder_path_set()
{
    bool output = false;
    std::clog << X4_PLAYTHROUGHS_LOG_TESTING_PARAM << X4_SAVE_FOLDER_PATH_PARAM_NAME << std::endl;
    if(x4_save_folder_path.empty())
    {
        // Parameter is an empty string, it was not set properly
        output = false;
        std::cerr << X4_PLAYTHROUGHS_ERR_PARAMETER_EMPTY << std::endl;
    }
    else
    {
        // Parameter is not empty, checking if it is a directory
        std::filesystem::path thePath(x4_save_folder_path);
        std::error_code ec;
        if(!std::filesystem::exists(thePath))
        {
            // No corresponding file
            // This may be due to a restart after the BACKUP (and delete) command
            output = true;
        }
        else if(std::filesystem::is_directory(thePath, ec))
        {
            // It is a directory
            // Checking if it end with save
            if(thePath.filename().string().compare(X4_PLAYTHROUGHS_SAVE_FOLDER_NAME) == 0)
                output = true;
            else
            {
                output = false; // It is a file
                std::cerr << X4_PLAYTHROUGHS_ERR_PARAMETER_DIRECTORY << x4_save_folder_path << std::endl;
            }
        }
        else
        {
            output = false; // It is a file
            std::cerr << X4_PLAYTHROUGHS_ERR_PARAMETER_DIRECTORY << x4_save_folder_path << std::endl;
        }
        if(ec)
        {
            output = false; // It is a file
            std::cerr << X4_PLAYTHROUGHS_ERR_PARAMETER_ACCESS << x4_save_folder_path << std::endl;
        }
    }

    return output;
}

bool X4PlaythroughsParameters::is_X4_executable_path_set()
{
    bool output = false;
    std::clog << X4_PLAYTHROUGHS_LOG_TESTING_PARAM << X4_EXECUTABLE_PATH_PARAM_NAME << std::endl;
    if(x4_executable_path.empty())
    {
        // Parameter is an empty string, it was not set properly
        output = false;
        std::cerr << X4_PLAYTHROUGHS_ERR_PARAMETER_EMPTY << std::endl;
    }
    else
    {
        // Parameter is not empty, checking if it is a directory
        std::filesystem::path thePath(x4_executable_path);
        std::error_code ec;
        if(std::filesystem::is_regular_file(thePath, ec))
        {
            // It is a file
            // Checking if it end with X4.exe ? linux ?
            // TODO
            output = true;
        }
        else
        {
            output = false; // It is a directory or other
            std::cerr << X4_PLAYTHROUGHS_ERR_PARAMETER_FILE << x4_executable_path << std::endl;
        }
        if(ec)
        {
            output = false; // It is a file
            std::cerr << X4_PLAYTHROUGHS_ERR_PARAMETER_ACCESS << x4_executable_path << std::endl;
        }
    }

    return output;
}

bool X4PlaythroughsParameters::is_os_link_create_cmd_set()
{
    bool output = false;
    std::clog << X4_PLAYTHROUGHS_LOG_TESTING_PARAM << X4_LINK_CREATE_CMD_PARAM_NAME << std::endl;
    if(os_link_create_cmd.empty())
    {
        // Parameter is an empty string, it was not set properly
        output = false;
        std::cerr << X4_PLAYTHROUGHS_ERR_PARAMETER_EMPTY << std::endl;
    }
    else
    {
        output = true;
    }
    return output;
}

bool X4PlaythroughsParameters::is_os_link_delete_cmd_set()
{
    bool output = false;
    std::clog << X4_PLAYTHROUGHS_LOG_TESTING_PARAM << X4_LINK_DELETE_CMD_PARAM_NAME << std::endl;
    if(os_link_delete_cmd.empty())
    {
        // Parameter is an empty string, it was not set properly
        output = false;
        std::cerr << X4_PLAYTHROUGHS_ERR_PARAMETER_EMPTY << std::endl;
    }
    else
    {
        output = true;
    }
    return output;
}

/**
 * @brief Print on console output all parameters
 * 
 */
void X4PlaythroughsParameters::print()
{
    std::cout << std::endl;
    std::cout << "X4Playthroughs parameters" << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << X4_SAVE_FOLDER_PATH_PARAM_NAME << X4_PLAYTHROUGHS_CONFIGFILE_SEPARATOR << x4_save_folder_path << std::endl;
    std::cout << X4_EXECUTABLE_PATH_PARAM_NAME << X4_PLAYTHROUGHS_CONFIGFILE_SEPARATOR << x4_executable_path << std::endl;
    std::cout << X4_LINK_CREATE_CMD_PARAM_NAME << X4_PLAYTHROUGHS_CONFIGFILE_SEPARATOR << os_link_create_cmd << std::endl;
    std::cout << X4_LINK_DELETE_CMD_PARAM_NAME << X4_PLAYTHROUGHS_CONFIGFILE_SEPARATOR << os_link_delete_cmd << std::endl;
    std::cout << X4_PLAYTHROUGHS_SAVE_FOLDER_AT << x4playthroughs_data_folder_path << std::endl;
    std::cout << X4_PLAYTHROUGHS_UI_IS << x4playthroughs_hmi << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << std::endl;
}
