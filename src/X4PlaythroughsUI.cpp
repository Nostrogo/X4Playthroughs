#include "X4PlaythroughsUI.h"
#include "X4PlaytoughManager.h"

#define FILE_MENU_EXIT 5000
#define HELP_MENU_HELP 6000
// ListView specifics
// ID ListView
#define IDC_LIST 4000
// Button handles
#define ACTIVATE_BUTTON 7000
#define NEW_BUTTON 7100
#define DELETE_BUTTON 7200
#define REFRESH_BUTTON 7300

// Singleton init
X4PlaythroughsUI* X4PlaythroughsUI::theUIInstance = NULL;

// X4PlaythroughsUI constructor
X4PlaythroughsUI::X4PlaythroughsUI(X4PlaythroughManager* manager_param, HINSTANCE hInst, HINSTANCE hPrevInst): manager(manager_param), hMainAppInst(hInst), hPrevAppInst(hPrevInst)
{
    // Singleton init
    X4PlaythroughsUI::theUIInstance = this;
}

// Launch/show GUI
void X4PlaythroughsUI::start_hmi()
{
    // Creating window class
    WNDCLASSW wc = {0}; // Struct representing a windows
    wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); // if not set cursor will not show on windows, IDC* = type of cursor
    wc.hInstance = hMainAppInst;
    wc.lpszClassName = L"myX4PlaythroughWindowClass"; // Identifier for the class. L is for the type of char string
    wc.lpfnWndProc = WindowProcedure; // Callback for the window

    // register the window class so it can be used
    if(!RegisterClassW(&wc))
        MessageBoxW(NULL, L"Cannot register Window class!", L"An error occured when registering the window class.", MB_OK);

    // Creating window
    CreateWindowW(L"myX4PlaythroughWindowClass", L"X4 Playthroughs", WS_OVERLAPPEDWINDOW | WS_VISIBLE,  CW_USEDEFAULT,  CW_USEDEFAULT, 
        X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_WIDTH, X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_HEIGHT, NULL, NULL, NULL, NULL);

    // Msg loop
    MSG msg = {0};
    // True until closed
    while( GetMessage(&msg, NULL, 0, 0) ) // UNIT NULL  should be 0
    {
        TranslateMessage (&msg);
        DispatchMessage(&msg); // Send the msg to the windows procedure function
    }
}

// WARNING Static method.
LRESULT CALLBACK X4PlaythroughsUI::WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch(msg)
    {
        case WM_CREATE:
            // when creating the window, msg send just once; create window content here
            theUIInstance->AddMenus(hWnd);
            theUIInstance->AddControls(hWnd);
            break;
        case WM_NOTIFY:
            theUIInstance->HandleWM_NOTIFY(wp, lp);
            break;
        case WM_COMMAND:
            // something was clicked : a menu ?
            switch (LOWORD(wp))
            {
            case IDC_LIST:
            {
                std::cout << "WM_COMMAND IDC_LIST" << std::endl; // Test
                if(HIWORD(wp) == LBN_SELCHANGE)
                    std::cout << "WM_COMMAND IDC_LIST LBN_SELCHANGE" << std::endl; // Test
            }
            break;
            case HELP_MENU_HELP:
                // Display Help modal window with help message
                // TODO
                break;
            case FILE_MENU_EXIT:
                // Exiting the application
                DestroyWindow(hWnd); // Next message will be WM_DESTROY
                break;
            case ACTIVATE_BUTTON:
                { // Needed because of type declaration visible below
                    // Activate the selected playthrough
                    int validatActivate = MessageBox(hWnd, "Are you sure ? X4 save will redirect toward selected playthrough save folder. In case of a first time activation, make sure your save folder is backuped...", "ACTIVATE command: Activating selected playthrough?", MB_OKCANCEL);
                    if(validatActivate == IDOK)
                    {
                        if(theUIInstance->manager->get_selected_playthrough())
                        {
                            bool result = theUIInstance->manager->activate_selected_playthrough();
                            if(!result)
                                MessageBox(hWnd, "ACTIVATION FAILED, check you save folder!", "ACTIVATE command", MB_ICONERROR);
                            //else
                            //    MessageBox(hWnd, "ACTIVATION SUCCESS !", "ACTIVATE command", MB_OK);
                        }
                        else
                        {
                            MessageBox(hWnd, "No selected playthrough. Command aborted.", "ACTIVATE command", MB_ICONEXCLAMATION);
                        }
                    }
                    else
                    {
                        MessageBox(hWnd, "Command aborted.", "ACTIVATE command", MB_ICONINFORMATION);
                    }
                }
                theUIInstance->RefreshList();
                break;
            case REFRESH_BUTTON:
                theUIInstance->RefreshList();
                break;
            case NEW_BUTTON:
                {
                    // Create a new playthrough
                    std::string aPlaythroughName{""};
                    char nameEdit[256];
                    GetWindowText(theUIInstance->hNewPlaythroughName, nameEdit, 256);
                    nameEdit[255] = '\0'; // Protection
                    aPlaythroughName = std::string(nameEdit);
                    // Trimming parameter name
                    aPlaythroughName.erase(aPlaythroughName.find_last_not_of(X4PlaythroughsParameters::WHITESPACES)+1); // Removing whitespaces in the end
                    aPlaythroughName.erase(0, aPlaythroughName.find_first_not_of(X4PlaythroughsParameters::WHITESPACES)); // Removing whitespaces in the begining

                    // Checking output validity
                    if(theUIInstance->manager->is_valid_playthrough_name(aPlaythroughName))
                        theUIInstance->manager->add_new_playthrough_to_list(aPlaythroughName);
                    else
                        MessageBox(hWnd, "Invalid playthrough name!", "NEW command", MB_ICONERROR);
                    theUIInstance->RefreshList();
                }
                break;
            case DELETE_BUTTON:
            {
                // Delete the selected playthrough
                if(theUIInstance->manager->get_selected_playthrough())
                {
                    // ask for confirmation:
                    int delete_confirmed = MessageBox(hWnd, "Are you sure ? Playthrough save directory and all its saves will be deleted. ", "DELETE command", MB_OKCANCEL);
                    if(delete_confirmed == IDOK)
                        theUIInstance->manager->delete_selected_playthrough();
                    else
                        MessageBox(hWnd, "Command aborted.", "DELETE command", MB_ICONINFORMATION);
                }
                else
                {
                    MessageBox(hWnd, "No selected playthrough. Command aborted.", "DELETE command", MB_ICONEXCLAMATION);
                }
                // Update the itemsd on the listview from the files
                theUIInstance->RefreshList();
            }
                break;
            default:
                //Do nothing
                break;
            }
            break;
        case WM_DESTROY:
            // Next GertMessage function will return false;
            PostQuitMessage(0);
            break;
        default:
            //return DefWindowProcW(hWnd, msg, wp, lp); // Dispatch the message to default handling
            break;
    }
    return DefWindowProcW(hWnd, msg, wp, lp); // Dispatch the message to default handling
}

// Add menu to the window
void X4PlaythroughsUI::AddMenus(HWND hWnd)
{
    // Create the menu
    HMENU hMenu = CreateMenu();
    // Sub-Menu
    HMENU hFileMenu = CreateMenu();
    HMENU hHelpMenu = CreateMenu();

    // Add to the file sub-menu
    AppendMenu(hFileMenu, MF_SEPARATOR, (UINT_PTR) NULL, NULL);
    AppendMenu(hFileMenu, MF_STRING, FILE_MENU_EXIT, "Exit");
    AppendMenu(hHelpMenu, MF_SEPARATOR, (UINT_PTR) NULL, NULL);
    AppendMenu(hHelpMenu, MF_STRING, HELP_MENU_HELP, "Help");

    // Add to the menu
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR) hFileMenu, "File"); // MF_POPUP: type of menu, then 3rd arg = sub menu ptr. 
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR) hHelpMenu, "Help"); // MF_STRING: type of menu, then 4th arg = text of menu. 3rd arg => ID for the menu in msg 
    // Add the menu to the window
    SetMenu(hWnd, hMenu);
}

// Add control to the window
void X4PlaythroughsUI::AddControls(HWND hWnd)
{
    InitCommonControls();

    // ListView
    hList = CreateListView (hWnd);
    SendMessage(hList,LVM_SETTEXTBKCOLOR, 0,(LPARAM)CLR_NONE);
    // One click to select the whole line !
    SendMessage(hList,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT); // Set style
    // Columns
    InitListViewColumns(hList);
    // Items
    InsertListViewItems(hList);

    // positions
    RECT rcClient;                       // The parent window's client area.
    GetClientRect (hWnd, &rcClient);
    int windowWidth = (rcClient.right - rcClient.left);
    int windowHeight = (rcClient.bottom - rcClient.top);

    // Static for usage display
    int infoHeight = (windowHeight / 4) * 3;
    int right_col_X_pos = (X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_HORIZONTAL_SPACING * 2)+listWidth; // One H space after the ListView
    int info_Y_pos = (windowHeight - X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_VERTICAL_SPACING) - infoHeight;
    int infoWidth = listWidth - X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_HORIZONTAL_SPACING;
    CreateWindowW(L"Static", X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_HOWTO.c_str(), 
        WS_VISIBLE | WS_CHILD | WS_BORDER | SS_LEFT, right_col_X_pos, info_Y_pos, 
        infoWidth, infoHeight, hWnd, NULL, NULL, NULL);
    // Activate Button
    int buttonWidth = 125;
    int buttonHeight = ((windowHeight - infoHeight) / 4) - (X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_VERTICAL_SPACING * 2);
    CreateWindowW(L"Button", L"Activate", WS_VISIBLE | WS_CHILD, right_col_X_pos, X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_VERTICAL_SPACING, 
        buttonWidth, buttonHeight, hWnd, (HMENU)ACTIVATE_BUTTON, NULL, NULL);
    // Edit for new playthrough
    int edit_play_name_Y_pos = X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_VERTICAL_SPACING + buttonHeight + X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_VERTICAL_SPACING;
    int edit_play_name_width = (infoWidth - buttonWidth) - (X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_HORIZONTAL_SPACING * 2);
    hNewPlaythroughName = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, right_col_X_pos, edit_play_name_Y_pos, 
        edit_play_name_width, buttonHeight, hWnd, NULL, NULL, NULL);
    // New playthrough button
    int new_X_pos = right_col_X_pos+ edit_play_name_width + X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_HORIZONTAL_SPACING;
    CreateWindowW(L"Button", L"New playthrough", WS_VISIBLE | WS_CHILD, new_X_pos, edit_play_name_Y_pos, 
        buttonWidth, buttonHeight, hWnd, (HMENU)NEW_BUTTON, NULL, NULL);
    // Delete button
    int delete_Y_pos = edit_play_name_Y_pos + buttonHeight + X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_VERTICAL_SPACING;
    CreateWindowW(L"Button", L"Delete", WS_VISIBLE | WS_CHILD, right_col_X_pos, delete_Y_pos, buttonWidth, buttonHeight, hWnd, (HMENU)DELETE_BUTTON, NULL, NULL);
    // Refresh button
    int refresh_Y_pos = delete_Y_pos + buttonHeight + X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_VERTICAL_SPACING;
    CreateWindowW(L"Button", L"Refresh", WS_VISIBLE | WS_CHILD, right_col_X_pos, refresh_Y_pos, buttonWidth, buttonHeight, hWnd, (HMENU)REFRESH_BUTTON, NULL, NULL);
}

//==============From https://learn.microsoft.com/en-us/windows/win32/controls/create-a-list-view-control===================
// CreateListView: Creates a list-view control in report view.
// Returns the handle to the new control
// HWND  hWndParent: The handle to the control's parent window. 
HWND X4PlaythroughsUI::CreateListView (HWND hwndParent) 
{
    INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    // Size of the ListView
    RECT rcClient;                       // The parent window's client area.
    GetClientRect (hwndParent, &rcClient);
    listWidth = ((rcClient.right - rcClient.left) / 2) - X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_HORIZONTAL_SPACING; // Half the window space minus spaces
    listHeight = (rcClient.bottom - rcClient.top) - (X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_VERTICAL_SPACING *2); // Full size minus spacing top and down

    // Create the list-view window in report view without label editing enabled. 
    HWND hWndListView = CreateWindow(WC_LISTVIEW, 
                                     (LPCSTR) "",
                                     WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER | WS_TABSTOP,
                                     X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_HORIZONTAL_SPACING, X4PlaythroughsParameters::X4_PLAYTHROUGHS_HMI_VERTICAL_SPACING,
                                     listWidth,
                                     listHeight,
                                     hwndParent,
                                     (HMENU)IDC_LIST,
                                     hMainAppInst,
                                     NULL); 

    return (hWndListView);
}

//==============From https://learn.microsoft.com/en-us/windows/win32/controls/add-list-view-columns===================
// InitListViewColumns: Adds columns to a list-view control.
// hWndListView:        Handle to the list-view control. 
void X4PlaythroughsUI::InitListViewColumns(HWND hWndListView) 
{ 
    LVCOLUMN LvCol; // Make Column struct for ListView
    // To avoid annoying warning messages: ISO C++ forbids converting a string constant to 'LPSTR'
    char playthroughChar[] = "Playthrough";
    char lastWriteChar[] = "Last write";
    char baseDirChar[] = "Base dir";
    char saveDirChar[] = "Save dir";
    char activeChar[] = "Active";
    memset(&LvCol,0,sizeof(LvCol)); // Reset struct
    LvCol.mask=LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM; // Type of mask
    RECT rcClient;                       // The parent window's client area.
    GetClientRect (hWndListView, &rcClient);
    int defaultColWidth = (rcClient.right - rcClient.left) / 5; // One fifth
    LvCol.cx = defaultColWidth;
    // Inserting Columns
    LvCol.pszText = playthroughChar;                                // First Header
    LvCol.cx = defaultColWidth * 2;                                 // Twice the size
    SendMessage(hWndListView,LVM_INSERTCOLUMN,0,(LPARAM)&LvCol);    // Insert the column Playthrough
    LvCol.pszText = activeChar;                                     // Next column : Active
    LvCol.cx = defaultColWidth;                                     // Active col is default size
    SendMessage(hWndListView,LVM_INSERTCOLUMN,1,(LPARAM)&LvCol);    // insert column Active
    LvCol.pszText = lastWriteChar;                                  // Next column : Last write date
    LvCol.cx = defaultColWidth;                                     // Last Write is default size
    SendMessage(hWndListView,LVM_INSERTCOLUMN,2,(LPARAM)&LvCol);    // insert column Last write
    LvCol.pszText = saveDirChar;                                    // Next column save dir
    LvCol.cx = defaultColWidth / 2;                                 // SaveDir column is half sized
    SendMessage(hWndListView,LVM_INSERTCOLUMN,3,(LPARAM)&LvCol);    // insert column Save dir
    LvCol.pszText = baseDirChar;                                    // Next column : base dir
    LvCol.cx = defaultColWidth / 2;                                 // Base dir column is half sized
    SendMessage(hWndListView,LVM_INSERTCOLUMN,4,(LPARAM)&LvCol);    // insert column Base dir
    
}


//==============From https://learn.microsoft.com/en-us/windows/win32/controls/add-list-view-items-and-subitems===================
// InsertListViewItems: Inserts items into a list view. 
// hWndListView:        Handle to the list-view control.
// cItems:              Number of items to insert.
// Returns TRUE if successful, and FALSE otherwise.
void X4PlaythroughsUI::InsertListViewItems(HWND hWndListView)
{
    LVITEM LvItem;  // ListView Item struct
    memset(&LvItem,0,sizeof(LvItem)); // Reset Item Struct
    //  Setting properties Of Items:
    char buffer[256]; // WCHAR
    LvItem.mask=LVIF_TEXT;   // Text Style
    LvItem.cchTextMax = 256; // Max size of test
    LvItem.iItem = 0;          // init item nb

    // One item per playthrough
    std::vector<X4Playthrough> listofPlay = theUIInstance->manager->get_playthrough_list();
    for (X4Playthrough& play : listofPlay)
    {
        // Playthrough Name
        LvItem.iSubItem = 0;       // Put in first column
        strncpy(buffer, play.get_name().c_str(), 256);
        buffer[255] = '\0'; // Protection
        LvItem.pszText = buffer; // Text to display (can be from a char variable) (Items)
        SendMessage(hList,LVM_INSERTITEM,0,(LPARAM)&LvItem); // Send to the Listview

        // Active
        LvItem.iSubItem = 1;
        std::string active = "";
        if(theUIInstance->manager->get_active_playthrough() != nullptr)
            if(theUIInstance->manager->get_active_playthrough()->get_name().compare(play.get_name()) == 0)
                active = "active";
        strncpy(buffer, active.c_str(), 256);
        buffer[255] = '\0'; // Protection
        LvItem.pszText = buffer; // Text to display (can be from a char variable) (Items)
        SendMessage(hList,LVM_SETITEM,0,(LPARAM)&LvItem); // Enter text to SubItems

        // Last Write time
        LvItem.iSubItem = 2;
        // Converting std::filesystem::file_time_type to std::time_t and std::tm.
        std::time_t cftime = std::chrono::system_clock::to_time_t(std::chrono::file_clock::to_sys(play.get_latest_write_time()));
        std::string last_write_date{""};
        // Local time may return null.
        std::tm *tmptr = std::localtime(&cftime);
        if(tmptr)
        {
            std::stringstream buffer;
            buffer << std::put_time(tmptr, "%c");
            last_write_date = buffer.str();
        }
        else
            last_write_date = "Unknown";
        strncpy(buffer, last_write_date.c_str(), 256);
        buffer[255] = '\0'; // Protection
        LvItem.pszText = buffer; // Text to display (can be from a char variable) (Items)
        SendMessage(hList,LVM_SETITEM,0,(LPARAM)&LvItem); // Enter text to SubItems

        // Save dir
        LvItem.iSubItem = 3;
        strncpy(buffer, play.get_save_path().c_str(), 256);
        buffer[255] = '\0'; // Protection
        LvItem.pszText = buffer; // Text to display (can be from a char variable) (Items)
        SendMessage(hList,LVM_SETITEM,0,(LPARAM)&LvItem); // Enter text to SubItems

        // Base dir
        LvItem.iSubItem = 4;
        strncpy(buffer, play.get_basedir_path().c_str(), 256);
        buffer[255] = '\0'; // Protection
        LvItem.pszText = buffer; // Text to display (can be from a char variable) (Items)
        SendMessage(hList,LVM_SETITEM,0,(LPARAM)&LvItem); // Enter text to SubItems

        LvItem.iItem++; // Next item
    }

}

//==============From https://learn.microsoft.com/en-us/windows/win32/controls/add-list-view-items-and-subitems===================
// HandleWM_NOTIFY - Handles the LVN_GETDISPINFO notification code that is 
//         sent in a WM_NOTIFY to the list view parent window. The function 
//        provides display strings for list view items and subitems.
//
// lParam - The LPARAM parameter passed with the WM_NOTIFY message.
void X4PlaythroughsUI::HandleWM_NOTIFY(WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case IDC_LIST:
        {
            // Using NM Click and not LVN_ITEMCHANGED because LVN_ITEMCHANGED is send 3 times each time the selection change (deselect, focus and select)
            if(((LPNMHDR)lParam)->code == NM_CLICK)
            {
                int iSelect = 0;
                iSelect=SendMessage(theUIInstance->hList,LVM_GETNEXTITEM,-1,LVNI_FOCUSED); // Get the first item that has the LVIS_FOCUSED flag set from pos -1 (all)
                // Not LVNI_SELECTED flag because a click elsewhere on the ListView will deselect an item but not unfocus it ?
                
                if(iSelect==-1)
                {
                    // No Playthrough is selected
                    theUIInstance->manager->set_selected_playthrough(nullptr);
                }
                else
                {
                    // Set the selected playthrough name as the one corresponding to iSelect
                    LVITEM LvItem;  // ListView Item struct
                    char buffer[256]={0};
                    memset(&LvItem,0,sizeof(LvItem));
                    LvItem.mask=LVIF_TEXT;
                    LvItem.iSubItem=0; // We want the first column
                    LvItem.pszText=buffer; // This will receive the text
                    LvItem.cchTextMax=256;
                    LvItem.iItem=iSelect; // We want a particular item
                    SendMessage(hList,LVM_GETITEMTEXT, iSelect, (LPARAM)&LvItem);

                    // Get the playthrough from manager corresponding to this name
                    int playthrough_number = -1;
                    int index = 0;
                    std::vector<X4Playthrough> listofPlay = theUIInstance->manager->get_playthrough_list();
                    for (X4Playthrough& play : listofPlay)
                    {
                        if(play.get_name().compare(buffer) == 0)
                        {
                            // That is the one
                            playthrough_number = index;
                        }
                        index++;
                    }
                    // Set this play as the selected one !
                    if((playthrough_number >= 0) &&(playthrough_number < theUIInstance->manager->get_playthrough_list().size()))
                    {
                        theUIInstance->manager->set_selected_playthrough(&(theUIInstance->manager->get_playthrough_list().at(playthrough_number)));
                    }
                    std::cout << "Selecting " << " item nb " << iSelect << " whith name " << buffer << std::endl; // Test
                }
            } // End if NM_CLICK
        }
        break;
    default:
        break;
    }

    return;
}

void X4PlaythroughsUI::RefreshList()
{
    // Refresh (clear and reload) the listView
    // Clear the list view
    SendMessage(hList,LVM_DELETEALLITEMS,(WPARAM) NULL,(LPARAM)NULL);
    // Refresh manager
    manager->refresh_playthrough_list();
    // reload the list view
    InsertListViewItems(hList);
}
