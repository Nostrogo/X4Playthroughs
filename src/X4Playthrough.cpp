#include "X4Playthrough.h"
#include "X4PlaythroughsParameters.h"

#include <filesystem>
#include <chrono>
#include <fstream>

/**
 * @brief name getter
 * 
 * @return std::string 
 */
std::string X4Playthrough::get_name()
{
    return name;
}

/**
 * @brief Return the lastest last write time of all the save file of this playthrough
 * 
 * @return std::filesystem::file_time_type 
 * @return std::filesystem::file_time_type::min() on error (no files...)
 */
std::filesystem::file_time_type X4Playthrough::get_latest_write_time()
{
    // Get the latest write time, update ptime
    return latest_write_time;
}


/**
 * @brief retrieve the lastest of the late_write_time from the save files of the playthrough
 * 
 * @return std::filesystem::file_time_type::min() if no save files
 */
void X4Playthrough::get_latest_write_time_from_files()
{
    latest_write_time = std::filesystem::file_time_type::min();
    std::filesystem::path save_folder(get_save_path());
    for(const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(save_folder))
    {
        std::error_code ec;
        ec.clear();
        std::filesystem::file_time_type atime = std::filesystem::last_write_time(entry, ec);
        if(ec)
            atime = std::filesystem::file_time_type::min();
        // Comparing atime with latest_write_time
        if(latest_write_time < atime)
            latest_write_time = atime;

    }
}

/**
 * @brief Create the folders {rootpath}/{name}/ and {rootpath}/{name}/save if they do not exists.
 * 
 * @return true if successfull
 * @return false otherwise
 */
bool X4Playthrough::create_playthrough_folders()
{
    bool output = true;
    std::filesystem::path basePath(basedir_path);
    std::filesystem::path savePath(save_path);
    bool created_basedir_path = false;
    bool created_save_path = false;
    if(std::filesystem::exists(basePath))
        created_basedir_path = true;
    else
        created_basedir_path = std::filesystem::create_directories(basePath);

    if(std::filesystem::exists(savePath))
        created_save_path = true;
    else
        created_save_path = std::filesystem::create_directory(savePath);
    
    output = created_basedir_path && created_save_path;
    
    return output;
}

std::string X4Playthrough::get_save_path()
{
    return save_path;
}
std::string X4Playthrough::get_basedir_path()
{
    return basedir_path;
}

/**
 * @brief Delete the folders and files.
 * 
 * @return true if successfull
 * @return false otherwise
 */
bool X4Playthrough::delete_playthrough_files()
{
    bool output = true;
    output = std::filesystem::remove_all(basedir_path);
    return output;
}

/**
 * @brief Check if the playthrough files are OK.
 * 
 * @return true if playthrough can be activated
 * @return false otherwise
 */
bool X4Playthrough::check_playthrough_files()
{
    bool output = true;
    // A PlayThrough can be activated if it got a save folder
    std::filesystem::path aSavePath(save_path);
    std::error_code ec;
    if(std::filesystem::exists(aSavePath) && std::filesystem::is_directory(aSavePath, ec))
        output = true;
    return output;
}

/**
 * @brief Construct a new X4Playthrough::X4Playthrough object
 * 
 * @param aNameParam  check parameter validity as a directory name before calling
 * @param root_path_param from parameters
 */
X4Playthrough::X4Playthrough(std::string aNameParam, std::string root_path_param)
{
    name = aNameParam;
    basedir_path = root_path_param + X4PlaythroughsParameters::X4_PLAYTHROUGHS_FOLDER_SEPARATOR + aNameParam;    // Build from name and root_path from parameter std::filesystem::path
    save_path = root_path_param + X4PlaythroughsParameters::X4_PLAYTHROUGHS_FOLDER_SEPARATOR + aNameParam + X4PlaythroughsParameters::X4_PLAYTHROUGHS_FOLDER_SEPARATOR + X4PlaythroughsParameters::X4_PLAYTHROUGHS_SAVE_FOLDER_NAME;    // Build from name and parameters std::filesystem::path
    create_playthrough_folders(); // do nothing if folders already exist
    get_latest_write_time_from_files();
}

X4Playthrough::~X4Playthrough()
{
    // Do nothing
}

std::ostream& operator<<(std::ostream& out, const X4Playthrough& playthrough)
{
    // Converting std::filesystem::file_time_type to std::time_t and std::tm.
    std::time_t cftime = std::chrono::system_clock::to_time_t(std::chrono::file_clock::to_sys(playthrough.latest_write_time));
    std::string last_write_date{""};
    // Local time may return null.
    std::tm *tmptr = std::localtime(&cftime);
    if(tmptr)
    {
        std::stringstream buffer;
        buffer << std::put_time(tmptr, "%c");
        last_write_date = buffer.str();
        //last_write_date = std::asctime(std::localtime(&cftime));
    }
    else
        last_write_date = "Unknown";

    out << "Playthrough [" << playthrough.name << " | LAST WRITE " << last_write_date << " | BASE DIR " << playthrough.basedir_path << " | SAVE PATH " << playthrough.save_path << "]" << std::endl;
    return out;
}
