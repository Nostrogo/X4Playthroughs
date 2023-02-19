#ifndef PLAYTHROUGHS_GUI_H
#define PLAYTHROUGHS_GUI_H

#include "X4PlaytoughManager.h"
#include <windows.h>
#include <commctrl.h> // For ListView

class X4PlaythroughsUI
{
    protected:
        X4PlaythroughManager* manager;

        // Instances
        HINSTANCE hMainAppInst;
        HINSTANCE hPrevAppInst;
        // handles
        HWND hList = NULL;  // List View identifier
        HWND hNewPlaythroughName = NULL; // Handle to the edit field for the name of the new playthrough
        // Size of the listview in the window (to avoid multiple computation)
        int listWidth;
        int listHeight;

    public:
        static X4PlaythroughsUI* theUIInstance;

    public:
        X4PlaythroughsUI(X4PlaythroughManager* manager_param, HINSTANCE hInst, HINSTANCE hPrevInst);
        void start_hmi();

    protected:
        // Callback defined as static so it can be used in a win32 API env. (No delegate)
        static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
        // Function for Controls
        void AddControls(HWND hWnd);
        void AddMenus(HWND hWnd);
        // ListView specifics
        HWND CreateListView (HWND hwndParent);
        void InitListViewColumns(HWND hWndListView);
        void InsertListViewItems(HWND hWndListView);
        void HandleWM_NOTIFY(WPARAM wp, LPARAM lParam);
        void RefreshList();

};

#endif
