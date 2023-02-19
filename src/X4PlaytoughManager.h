#ifndef PLAYTHROUGH_MANAGER_H
#define PLAYTHROUGH_MANAGER_H

#include <vector>
#include "X4PlaythroughsParameters.h"
#include "X4Playthrough.h"
#include <string>

class X4PlaythroughManager
{

    friend std::ostream& operator<<(std::ostream& out, const X4PlaythroughManager& manager);

    protected:
        X4Playthrough* selected_playthrough{nullptr};   // Pointer to the currently selected playthrough
        X4Playthrough* active_playthrough{nullptr};     // Pointer to the currently active playthrough, nullptr if none is active
        std::vector<X4Playthrough> playthrough_list{};  // List of playthrough managed by the X4PlaythroughManager

        private:
            X4PlaythroughsParameters* parameters;

    public:
        // Getter
        std::vector<X4Playthrough>& get_playthrough_list();
        X4Playthrough* get_selected_playthrough();
        X4Playthrough* get_active_playthrough();
        // Setter
        void set_selected_playthrough(X4Playthrough* aPlaythroughparam);        // set the selected playthrough and check its files; must be in list
        
        void refresh_playthrough_list();                                                 // Empty playthrough list and rebuild it by browsing files according to parameters
        void add_new_playthrough_to_list(std::string aPlaythroughNameParam);            // Create a new playthrough and add it in the list; create new playthrough folders (no save yet)
        void delete_selected_playthrough();                                             // remove current playthrough from the list and delete it and its save folder and save files
        bool activate_selected_playthrough();                                           // activate the playthrough (remove link and recreate it) using cmd : system("a command here"); from <stdlib.h>
        bool is_valid_playthrough_name(std::string aNameParam);                         // Check if the name is valid for a folder name and if the playthrough does not already exists.
        bool backup_and_delete_x4_save();                                               // This function is to be used once with fresh X4Playthroughs install, it will backup X4 saves and delete the save folder in order to make room for the symlink

        // Lifecycle
        X4PlaythroughManager(X4PlaythroughsParameters* parametersParam);            // Playthrough manager constructor
        ~X4PlaythroughManager();                                                // Playthrough manager destructor
};

#endif