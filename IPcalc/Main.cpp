﻿#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<CommCtrl.h>
#include<cstdio>
#include<iostream>
#include"resource.h"

#define IDC_COLUMN_NETWORK_ADDRESS 2000
#define IDC_COLUMN_BROADCAST_ADDRESS 2001
#define IDC_COLUMN_NUMBER_OF_IP_ADDRESSES 2002
#define IDC_COLUMN_NUMBER_OF_HOSTS 2003


BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcSubnets(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LPSTR FormatIPaddress(CONST CHAR* sz_message, DWORD IPaddress);
LPSTR FormatMessageWithNumber(CONST CHAR* sz_message, DWORD number);
VOID InitLVCcolumn(LPLVCOLUMN column, LPSTR text, INT subitem);


INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, (DLGPROC)DlgProc, 0);
	return 0;
}
BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
		HWND hIPaddress = GetDlgItem(hwnd, IDC_IPADDRESS);
		HWND hIPmask = GetDlgItem(hwnd, IDC_IPMASK);
		HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
		DWORD dwIPaddress = 0;
		DWORD dwIPmask = 0;
		CHAR sz_prefix[3]{};
		CONST INT SIZE = 256;
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		//InitCommonControls();
		HWND hUpDown = GetDlgItem(hwnd, IDC_SPIN_PREFIX);
		SendMessage(hUpDown, UDM_SETRANGE, 0, MAKELPARAM(30,1));
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		SendMessage(hIPaddress, IPM_SETADDRESS, 0, 0xc0a864c8); //отображаем ip при старте
		//SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
		
	}
		break;
	case WM_COMMAND:
	{
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
				SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
				std::cout <<"IP-addres:\t" << dwIPaddress <<"\n";
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
				DWORD dwPrefix = atoi(sz_prefix);
				UINT dwIPmask = UINT_MAX;
				//TODO: 
				
				dwIPmask <<= (32 - dwPrefix);
				if(dwPrefix!=0)SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPmask);
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
			//std::cout << dwNetworkAddress << std::endl;
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
		case IDC_BUTTON_SUBNETS:
			DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_SUBNETS), hwnd, (DLGPROC)DlgProcSubnets, 0);
		{

		}
			break;
		case IDCANCEL: EndDialog(hwnd, 0);
			break;
		}
	}
		break;
		case WM_CLOSE:
		FreeConsole();
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
BOOL CALLBACK DlgProcSubnets(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hList = GetDlgItem(hwnd, IDC_LIST_SUBNETS);
	static LVCOLUMN lvcNetworkAddress;
	static LVCOLUMN lvcBroadcastAddress;
	static LVCOLUMN lvcNumberOfIPAddresses;
	static LVCOLUMN lvcNumberOfHosts;
	static LVITEM lvItem;
	InitLVCcolumn(&lvcNetworkAddress, (LPSTR)"Адрес сети", 0);
	InitLVCcolumn(&lvcBroadcastAddress, (LPSTR)"Широковещательный адрес", 1);
	InitLVCcolumn(&lvcNumberOfIPAddresses, (LPSTR)"Количество IP адресов", 2);
	InitLVCcolumn(&lvcNumberOfHosts, (LPSTR)"Количество узлов", 3);
	//////////////////////////////////////////////
	
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		hList = GetDlgItem(hwnd, IDC_LIST_SUBNETS);
		SendMessage(hList, LVM_INSERTCOLUMN, 0, (LPARAM)&lvcNetworkAddress);
		SendMessage(hList, LVM_INSERTCOLUMN, 1, (LPARAM)&lvcBroadcastAddress);
		SendMessage(hList, LVM_INSERTCOLUMN, 2, (LPARAM)&lvcNumberOfIPAddresses);
		SendMessage(hList, LVM_INSERTCOLUMN, 3, (LPARAM)&lvcNumberOfHosts);

		HWND hParent = GetParent(hwnd);
		HWND hIPaddress = GetDlgItem(hParent, IDC_IPADDRESS);
		HWND hPrefix = GetDlgItem(hParent, IDC_EDIT_PREFIX);
		DWORD dwIPaddress = 0;
		CHAR sz_prefix[16];
		CHAR sz_class[2];
		SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
		SendMessage(hPrefix, WM_GETTEXT, 16, (LPARAM)sz_prefix);
		DWORD dwPrefix = atoi(sz_prefix);	
		DWORD dwDefaultMask = 0;

		if (dwIPaddress && dwPrefix)
		{
			DWORD dwDefaultPrefix = 0;
			if (FIRST_IPADDRESS(dwIPaddress) < 128)
			{
				strcpy(sz_class, "A");
				dwDefaultPrefix = 8;
				dwDefaultMask = 0xFF000000;
			}
			else if (FIRST_IPADDRESS(dwIPaddress) < 192)
			{
				strcpy(sz_class, "B");
				dwDefaultPrefix = 16;
				dwDefaultMask = 0xFFFF0000;
			}
			else if (FIRST_IPADDRESS(dwIPaddress) < 224)
			{
				strcpy(sz_class, "C");
				dwDefaultPrefix = 24;
				dwDefaultMask = 0xFFFFFF00;
			}
			DWORD dwClassAddress = dwIPaddress & dwDefaultMask;
			DWORD dwDelta = dwPrefix - dwDefaultPrefix;
			DWORD dwNumberOfSubnets = 1;
			for (; dwDelta > 0;dwDelta--)dwNumberOfSubnets *= 2;
			if (dwNumberOfSubnets == 0)dwNumberOfSubnets = 0;
			std::cout << dwDelta << std::endl;
			std::cout << dwNumberOfSubnets << std::endl;

			CHAR sz_message[256]{};
			DWORD dwHostBits = 32 - dwPrefix;
			DWORD dwNetworkCapacity = pow(2, dwHostBits);
			sprintf(sz_message, "Сеть класса %s разделена на %i подсетей, по %i IP-адресов", sz_class, dwNumberOfSubnets, dwNetworkCapacity);
			SendMessage(GetDlgItem(hwnd, IDC_STATIC_NUMBER_OF_SUBNETS), WM_SETTEXT, 0, (LPARAM)sz_message);

			for (DWORD i=0, dwNetworkAddress = dwClassAddress; i<dwNumberOfSubnets; i++, dwNetworkAddress+=dwNetworkCapacity)
			{
				LPSTR szNetworkAddress = NULL;
				//_itoa(dwNetworkCapacity , sz_IP_address, 10);
				szNetworkAddress = FormatIPaddress("", dwNetworkAddress);
				//LVITEM lvItem;
				ZeroMemory(&lvItem, sizeof(lvItem));
				lvItem.pszText =szNetworkAddress;
				lvItem.mask = LVIF_TEXT | LVIF_STATE;
				lvItem.stateMask = 0;
				lvItem.iSubItem = 0;
				lvItem.state = 0;
				lvItem.iItem = i;

				SendMessage(hList, LVM_INSERTITEM, 0, (LPARAM)&lvItem);
			}
		}
	}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:EndDialog(hwnd, 0);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
	}
	return FALSE;
}

VOID InitLVCcolumn(LPLVCOLUMN column, LPSTR text, INT subitem)
{
	ZeroMemory(column, sizeof(LVCOLUMN));
	column->mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	column->cx = 100;
	column->pszText = text;
	column->iSubItem = subitem;
	column->fmt = LVCFMT_LEFT;
}

