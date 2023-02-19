#ifndef PARAMETER_H
#define PARAMETER_H

#include <string>
#include <iostream>


class X4PlaythroughsParameters
{
public:
    /* data */
    static const std::string X4_PLAYTHROUGHS_UI_CONSOLE;            // x4playthroughs_hmi parameter in the configuration file corresponding to the console HMI
    static const std::string X4_PLAYTHROUGHS_UI_GUI;                // x4playthroughs_hmi parameter in the configuration file corresponding to the GUI HMI
    static const std::string X4_PLAYTHROUGHS_CONFIG_FILENAME;       // Name of the X4 playthroughs configuration file
    static const std::string X4_PLAYTHROUGHS_DATA_FOLDER_PATH;      // Relative path to the folder containing the configuration file and all the playthrough dedicated save folders
    static const std::string X4_PLAYTHROUGHS_BACKUP_FOLDER_PATH;    // Relative path to the folder containing the backup files
    static const std::string X4_SAVE_FOLDER_PATH_PARAM_NAME;        // Name used to parse the X4_save_folder_path parameter in the configuration file
    static const std::string X4_EXECUTABLE_PATH_PARAM_NAME;         // Name used to parse the X4_executable_path parameter in the configuration file
    static const std::string X4_LINK_CREATE_CMD_PARAM_NAME;         // Name used to parse the os_link_create_cmd parameter in the configuration file
    static const std::string X4_LINK_DELETE_CMD_PARAM_NAME;         // Name used to parse the os_link_delete_cmd parameter in the configuration file
    static const std::string X4_PLAYTHROUGHS_UI_PARAM_NAME;         // Name used to parse the x4playthroughs_hmi parameter in the configuration file
    static const std::string X4_PLAYTHROUGHS_RELATIVE_ROOT_PATH;    // Relative path to current directory : "." 
    static const std::string X4_PLAYTHROUGHS_FOLDER_SEPARATOR;      // Path separator : "/"
    static const std::string X4_PLAYTHROUGHS_SAVE_FOLDER_NAME;      // save
    static const std::string WHITESPACES;                           // Used for trimming the parameters from the configuration files
    static const std::string ACTIVE_FLAG;                           // Used to flag a directory as active. Because read_symlink on windows is not implemented...
    /* HMI parameters */
    static const int X4_PLAYTHROUGHS_HMI_WIDTH;                     // Width of the main window
    static const int X4_PLAYTHROUGHS_HMI_HEIGHT;                    // Width of the main window
    static const int X4_PLAYTHROUGHS_HMI_VERTICAL_SPACING;          // Vertical space between GUI components on the window
    static const int X4_PLAYTHROUGHS_HMI_HORIZONTAL_SPACING;        // Horizontal space between components on the window
    static const std::wstring X4_PLAYTHROUGHS_HMI_HOWTO;             // Help text on the HMI explaining how to use it.

private:
    // Print msg
    static const std::string X4_PLAYTHROUGHS_SAVE_FOLDER_AT;                    // Message indicating where the playthrough save folders will be at
    static const std::string X4_PLAYTHROUGHS_UI_IS;                             // Message indicating what HMI to use
    // Log msg
    static const std::string X4_PLAYTHROUGHS_LOG_CONFIGFILE;                    // Log message when opening the configuration file
    static const std::string X4_PLAYTHROUGHS_LOG_CONFIGFILE_COMPLETE;           // Log message when the configuration file has been read
    static const std::string X4_PLAYTHROUGHS_LOG_CONFIGFILE_UNKNOWN_PARAM;      // Log message when encountering an unknown parameter name
    static const std::string X4_PLAYTHROUGHS_LOG_TESTING_PARAM;                 // Log message when testing a parameter validity
    // Error msg
    static const std::string X4_PLAYTHROUGHS_ERR_CONFIGFILE_OPENING;            // Error message when opening the configuration file
    static const std::string X4_PLAYTHROUGHS_ERR_PARAMETER_EMPTY;               // Error message when a parameter is empty / not set
    static const std::string X4_PLAYTHROUGHS_ERR_PARAMETER_DIRECTORY;           // Error message when a parameter is not a directory
    static const std::string X4_PLAYTHROUGHS_ERR_PARAMETER_FILE;                // Error message when a parameter is not a file
    static const std::string X4_PLAYTHROUGHS_ERR_PARAMETER_ACCESS;              // Error message when a parameter is not accessible

    static const char X4_PLAYTHROUGHS_CONFIGFILE_COMMENT_CHAR;      // Char determining the line is a comment
    static const std::string X4_PLAYTHROUGHS_CONFIGFILE_SEPARATOR;  // 1 character string separing names and values in the configuration file

    std::string x4playthroughs_data_folder_path{""};                // Path to the X4playthroughs playthrough folder that will contain all the playthrough folders
    std::string x4playthroughs_backup_folder_path{""};              // Path to the X4playthroughs backup folder
    std::string x4_save_folder_path{""};                        // Path to the egosoft save folder - "C:\Users\Laurent\Documents\Egosoft\X4\1091595\save" on my local machine
    std::string x4_executable_path{""};                         // Path to the X4 Foundation executable - "C:\Program Files (x86)\Steam\steamapps\common\X4 Foundations\X4.exe" on my local machine
    std::string os_link_create_cmd{""};                         // OS dependant console command to create a hard link source being the playthrough dedicated save folder and destination being the X4 save folder
    std::string os_link_delete_cmd{""};                         // OS dependant consol command to erase the link (not the saves) in order to change it by the current playthrough dedicated folder link
    std::string x4playthroughs_hmi{""};                             // Parameter to choose the HMI, either console or GUI

public:
    /**
     * @brief Construct a new X4PlaythroughsParameters object load the parameters from configuration file
     * 
     */
    X4PlaythroughsParameters();
    ~X4PlaythroughsParameters();

    // Getters
    std::string get_x4playthroughs_data_folder_path()
    {
        return x4playthroughs_data_folder_path;
    };
    std::string get_x4playthroughs_backup_folder_path()
    {
        return x4playthroughs_backup_folder_path;
    };
    std::string get_x4_save_folder_path()
    {
        return x4_save_folder_path;
    };
    std::string get_x4_executable_path()
    {
        return x4_executable_path;
    };
    std::string get_os_link_create_cmd()
    {
        return os_link_create_cmd;
    };
    std::string get_os_link_delete_cmd()
    {
        return os_link_delete_cmd;
    };

    std::string get_x4playthroughs_hmi()
    {
        return x4playthroughs_hmi;
    };

    /**
     * @brief Check if all parameters have a value / are not empty strings
     * 
     * @return true if all parameter are set to a value
     * @return false otherwise
     */
    bool are_parameter_completly_set();
    void print();

protected:
    // Test each parameter for validity
    bool is_X4_save_folder_path_set();
    bool is_X4_executable_path_set();
    bool is_os_link_create_cmd_set();
    bool is_os_link_delete_cmd_set();
};



#endif