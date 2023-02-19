#ifndef PLAYTHROUGH_H
#define PLAYTHROUGH_H

#include <string>
#include <filesystem>

/**
 * @brief This class represent a playthrough. Each playthrough is associated with folders {rootpath}/{name}/ and {rootpath}/{name}/save/ that may or may not be linked with the X4 Foundation save folder and may or may not contain save files
 * 
 */
class X4Playthrough
{
    friend std::ostream& operator<<(std::ostream& out, const X4Playthrough& playthrough);

    protected:
        std::string name{""};   // Name of the playthrough; used as the folder name that will contain the save folder
        std::filesystem::file_time_type latest_write_time{std::filesystem::file_time_type::min()}; // Lastest write time on a save file => more likely on-demand c.f. get_latest_write_time
    private:
        std::string basedir_path;    // Build from name and root_path from parameter -> std::filesystem::path
        std::string save_path;      // Build from name and parameters -> std::filesystem::path

    public:
        // Getter
        std::string get_name();
        std::filesystem::file_time_type get_latest_write_time();    // Return the lastest last write time of all the save file of this playthrough; file_time_type::min() on error (no files...)
        std::string get_save_path();
        std::string get_basedir_path();

        bool create_playthrough_folders();  // Create the folders {rootpath}/{name}/ and {rootpath}/{name}/save. Return true if successfull, false otherwise
        bool delete_playthrough_files();  // Delete the folders and files. Return true if successfull, false otherwise
        bool check_playthrough_files();   // Check if the playthrough files are OK. Return true if playthrough can be activated, false otherwise
        void get_latest_write_time_from_files();    // Return the lastest last write time of all the save file of this playthrough; file_time_type::min() on error (no files...)

        // Lifecycle
        X4Playthrough(std::string aNameParam, std::string root_path_param);    // Playthrough constructor, must check name validity before ; directories are created if the do not exists
        ~X4Playthrough();   // Playthrough destructor
};

#endif
