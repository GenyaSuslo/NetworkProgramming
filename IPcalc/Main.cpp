#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<CommCtrl.h>
#include<cstdio>
#include<iostream>
#include"resource1.h"


BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LPSTR FormatIPaddress(CONST CHAR* sz_message, DWORD IPaddress);
LPSTR FormatMessageWithNumber(CONST CHAR* sz_message, DWORD number);


INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, (DLGPROC)DlgProc, 0);
	return 0;
}
BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		//InitCommonControls();
		HWND hUpDown = GetDlgItem(hwnd, IDC_SPIN_PREFIX);
		SendMessage(hUpDown, UDM_SETRANGE, 0, MAKELPARAM(30,1));
		//AllocConsole();
		//freopen("CONOUT$", "w", stdout);
		//SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
		
	}
		break;
	case WM_COMMAND:
	{
		HWND hIPaddress = GetDlgItem(hwnd, IDC_IPADDRESS);
		HWND hIPmask = GetDlgItem(hwnd, IDC_IPMASK);
		HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
		DWORD dwIPaddress = 0;
		DWORD dwIPmask = 0;
		CHAR sz_prefix[3]{};
		CONST INT SIZE = 256;
		switch (LOWORD(wParam))
		{
		case IDC_IPADDRESS:
		{
			if (HIWORD(wParam) == EN_CHANGE)
			{
				SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
				BYTE first = FIRST_IPADDRESS(dwIPaddress);
				if (first < 128)dwIPmask = MAKEIPADDRESS(255, 0, 0, 0);
				else if (first < 192)dwIPmask = MAKEIPADDRESS(255, 255, 0, 0);
				else if (first < 224)dwIPmask = MAKEIPADDRESS(255, 255, 255, 0);
				SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPmask);
			}
		}
		break;
		case IDC_IPMASK:
		{
			if (HIWORD(wParam) == EN_CHANGE)
			{
				SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM) & dwIPmask);
				int i = 1;
				for (; dwIPmask <<= 1; i++);
				sprintf(sz_prefix, "%i", i);
				SendMessage(hEditPrefix, WM_SETTEXT, 0, (LPARAM)sz_prefix);

			}
		}
		break;
		case IDC_EDIT_PREFIX:
		{
			if (HIWORD(wParam) == EN_CHANGE)
			{
				SendMessage(hEditPrefix, WM_GETTEXT, 3, (LPARAM)sz_prefix);
				DWORD dwIPprefix = atoi(sz_prefix);
				UINT dwIPmask = UINT_MAX;
				//TODO: 
				
				dwIPmask <<= (32 - dwIPprefix);
				if(dwIPprefix!=0)SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPmask);
				//SendMessage(hIPmask, IPM_GETADDRESS, 0, /*(LPARAM)&*/dwIPmask);
				
			}
		}
		break;
		case IDOK:
		{
			CHAR sz_info[SIZE] = "Info:\n";//Здесь будет информация о сети"; //sz - String Zero (NULL - Terminated Line)
			//CHAR sz_network_address[SIZE]{};
			//CHAR sz_broadcast_adress[SIZE]{};
			CHAR sz_number_of_hosts[SIZE]{};
			
			SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
			SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
			DWORD dwNetworkAddress = dwIPaddress & dwIPmask;
			DWORD dwBroadcastAddres = dwNetworkAddress | ~dwIPmask;
			std::cout << dwNetworkAddress << std::endl;
			strcat(sz_info, FormatIPaddress("Адрес сети:\t\t", dwNetworkAddress));
			strcat(sz_info, FormatIPaddress("Широковещательный адрес:", dwBroadcastAddres));
			/*strcat(sz_info, "Количество IP-адресов:\t");
			strcat(sz_info, _itoa(dwBroadcastAddres - dwNetworkAddress+1, sz_number_of_hosts,10));
			strcat(sz_info, "\nКоличество узлов:\t");
			strcat(sz_info, _itoa(dwBroadcastAddres - dwNetworkAddress-1, sz_number_of_hosts,10));*/
			strcat(sz_info, FormatMessageWithNumber("Количество IP-адресов:\t", dwBroadcastAddres - dwNetworkAddress + 1));
			strcat(sz_info, FormatMessageWithNumber("Количество узлов:\t\t", dwBroadcastAddres - dwNetworkAddress - 1));


			HWND hInfo = GetDlgItem(hwnd, IDC_STATIC_INFO);
			SendMessage(hInfo, WM_SETTEXT, 0, (LPARAM)sz_info);
		}
		break;
		case IDCANCEL: EndDialog(hwnd, 0);
			break;
		}
	}
		break;
	case WM_CLOSE:
		//FreeConsole();
		EndDialog(hwnd, 0);
	}
	return FALSE;
}
LPSTR FormatIPaddress(CONST CHAR* sz_message, DWORD IPaddress)
{
	CHAR sz_bufer[256]{};
	sprintf
	(
		sz_bufer,
		"%s\t%i.%i.%i.%i;\n",
		sz_message,
		FIRST_IPADDRESS(IPaddress),
		SECOND_IPADDRESS(IPaddress),
		THIRD_IPADDRESS(IPaddress),
		FOURTH_IPADDRESS(IPaddress)
	);
		return sz_bufer;
}
LPSTR FormatMessageWithNumber(CONST CHAR* sz_message, DWORD number)
{
	CHAR sz_bufer[256]{};
	sprintf(sz_bufer,"%s%i;\n", sz_message, number);
	return sz_bufer;
}

