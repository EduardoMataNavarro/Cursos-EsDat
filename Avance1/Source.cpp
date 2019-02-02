#include <Windows.h>
#include <Windowsx.h>
#include <iostream>
#include <fstream>
#include "resource.h"

using namespace std;

/* Agregar			+
   Eliminar			+
   Editar			+
   Ordenamiento		-
   Busqueda			+
   Segunda lista	-
 */
struct curso
{
	int id;
	char name[50];
	char description[100];
	char date[14];
	int duracion;
	int calificacionAprobatoria;
	int price;
	bool available;
	curso *prev = NULL;
	curso *next = NULL;
}*first = NULL, *last = NULL, aux{}, *infoNode = NULL, *infoNodeRel = NULL, *searchResNode = NULL;
struct relCursos
{
	int idParent;
	int idChild;
	relCursos*prev = NULL;
	relCursos*next = NULL;
}*firstRel = NULL, *lastRel = NULL, auxRel{};

TCHAR fileDir[MAX_PATH + 1];

HWND hWnd;
HINSTANCE hInstance;

bool emptyField, unsavedChanges;
char intBuffer[10], intBuffer2[10], dateBuffer[10];
char nameCurso[45];
int mbResult;
int lbRelCursel;

//Funciones de lista 
void addNode(curso info)//Agrega un nodo al final de la lista 
{
	curso *newNode = new curso;
	*newNode = info;
	if (first == NULL)
	{
		first = newNode;
		last = first;
	}
	else
	{
		last->next = newNode;
		newNode->prev = last;
		last = newNode;
	}
}
void addNode(int idParent, int idChild)
{
	relCursos *newNode = new relCursos;
	newNode->idParent = idParent;
	newNode->idChild = idChild;
	if (firstRel == NULL)
	{
		firstRel = newNode;
		lastRel = newNode;
	}
	else
	{
		lastRel->next = newNode;
		newNode->prev = lastRel;
		lastRel = newNode;
	}
}

void deleteNode(curso *node)
{
	if (node->prev == NULL && node->next == NULL)
		first = NULL;
	if (node->prev == NULL && node->next != NULL)
	{
		first = node->next;
		first->prev = NULL;
	}
	if (node->prev != NULL && node->next != NULL)
	{
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	if (node->next == NULL && node->prev != NULL)
	{
		last = node->prev;
		last->next = NULL;
	}
	delete node;
}
void deleteNode(relCursos *node)
{
	if (node->prev == NULL && node->next == NULL)
		first = NULL;
	if (node->prev == NULL && node->next != NULL)
	{
		firstRel = node->next;
		firstRel->prev = NULL;
	}
	if (node->prev != NULL && node->next != NULL)
	{
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	if (node->next == NULL && node->prev != NULL)
	{
		lastRel = node->prev;
		lastRel->next = NULL;
	}
	delete node;
}

bool existe(curso info)
{
	curso *aux = first;
	while (aux != NULL)
	{
		if (aux->id == info.id)
			return true;
		aux = aux->next;
	}
	return false;
}
//Funciones de busqueda 
	//Busqueda secuencial
curso *getCursoById(int id)
{
	bool found = false;
	curso *aux = first;
	while (aux != NULL)
	{
		if (aux->id == id)
			return aux;
		aux = aux->next;
	}
	return aux;
}//Busqueda secuencial
void fillRelListBox(int idPadre, HWND hListBox)//Carla los nombres de los cursos relacionados con el padre
{
	relCursos *auxRel = firstRel;
	curso *auxChild = NULL;
	while (auxRel != NULL)
	{
		if (idPadre == auxRel->idChild)
		{
			auxChild = getCursoById(auxRel->idChild);
			if (auxChild != NULL)
				SendMessage(GetDlgItem(hListBox, LB_CURSOSREQ), LB_ADDSTRING, 0, (LPARAM)auxChild->name);
		}
		auxRel = auxRel->next;
	}
}
curso *getNodeByCursel(int cursel)
{
	int index = 0;
	curso *aux = first;
	while ((aux != NULL) && (index != cursel))
	{
		aux = aux->next;
		index++;
	}
	return aux;
}
relCursos *getRelNodeByCursel(int cursel)
{
	int index = 0;
	relCursos *aux = firstRel;
	while ((aux != NULL) && (index != cursel))
	{
		aux = aux->next;
		index++;
	}
	return aux;
}

	//Busqueda binaria

int countNodes(curso *left, curso *right)
{
	int count = 0;
	curso *aux = left;
	while (aux <= right && aux != NULL)
	{
		count++;
		aux = aux->next;
	}
	return count;
}
curso *midNodeName(curso *left, curso *right, string name)
{
	if (left == right)
		if (right->name == name)
			return left;
		else 
			return NULL;
	else
	{
		curso *aux = left;
		int counter = 0;
		int limit = countNodes(left, right) / 2;
		while (counter != limit)
		{
			counter++;
			aux = aux->next;
		}
		return aux;
	}
}
curso *SearchById(int id)
{
	curso *aux = first;
		while (aux != NULL)
		{
			if (aux->id == id)
				return aux;
			aux = aux->next;
		}
		return 0;
}
curso *binarySearchByName(curso* left, curso* right, char name[])
{
	string str, str1;
	do
	{
		curso *actual = midNodeName(left, right, name);
		if (actual == NULL)
			return NULL;
		str = (actual->name);
		str1 = (name);
		if (str == str1)
			return actual;
		else if (str < str1)
			left = actual->next;
		else
			right = actual;
	} while (right == NULL || right->next != left);
	return NULL;
}

//Quicksort
	//Por nombre
void swapInfo(curso *node, curso *node2)//Pasa la información del nodo 1 al nodo 2 y del nodo 2 al nodo 1 
{
	curso auxiliar;
	auxiliar = *node;
	*node = *node2;
	*node2 = auxiliar;
}
curso *partition(curso *first, curso *last, int type)
{
	curso  *pivote = last;//se selecciona el último elemento como pivote
	curso  *temp1 = first;
	curso  *temp2 = first;//dos punteros temporales apuntando al primer elemento
	while (temp2 != last)
	{
		if (type == 0)
		{
			string str1 = pivote->name;
			string str2 = temp2->name;
			if (str1 > str2)
			{
				curso  *Aux = new curso;
				//todo esto es para cambiar nodos ahí solamente cambialo por lo que tenga tu estructura
				Aux->id = temp1->id;
				strcpy_s(Aux->name, temp1->name);
				strcpy_s(Aux->description, temp1->description);
				strcpy_s(Aux->date, temp1->date);
				Aux->duracion = temp1->duracion;
				Aux->price = temp1->price;
				Aux->calificacionAprobatoria = temp1->calificacionAprobatoria;
				Aux->available = temp1->available;

				temp1->id = temp2->id;
				strcpy_s(temp1->name, temp2->name);
				strcpy_s(temp1->description, temp2->description);
				strcpy_s(temp1->date, temp2->date);
				temp1->duracion = temp2->duracion;
				temp1->price = temp2->price;
				temp1->calificacionAprobatoria = temp2->calificacionAprobatoria;
				temp1->available = temp2->available;


				temp2->id = Aux->id;
				strcpy_s(temp2->name, Aux->name);
				strcpy_s(temp2->description, Aux->description);
				strcpy_s(temp2->date, Aux->date);
				temp2->duracion = Aux->duracion;
				temp2->price = Aux->price;
				temp2->calificacionAprobatoria = Aux->calificacionAprobatoria;
				temp2->available = Aux->available;

				temp1 = temp1->next;

			}
		}
		else if (type == 1)
		{
			int str1 = pivote->id;
			int str2 = temp2->id;
			if (str1 > str2)
			{
				curso  *Aux = new curso;
				//todo esto es para cambiar nodos ahí solamente cambialo por lo que tenga tu estructura
				Aux->id = temp1->id;
				strcpy_s(Aux->name, temp1->name);
				strcpy_s(Aux->description, temp1->description);
				strcpy_s(Aux->date, temp1->date);
				Aux->duracion = temp1->duracion;
				Aux->price = temp1->price;
				Aux->calificacionAprobatoria = temp1->calificacionAprobatoria;
				Aux->available = temp1->available;

				temp1->id = temp2->id;
				strcpy_s(temp1->name, temp2->name);
				strcpy_s(temp1->description, temp2->description);
				strcpy_s(temp1->date, temp2->date);
				temp1->duracion = temp2->duracion;
				temp1->price = temp2->price;
				temp1->calificacionAprobatoria = temp2->calificacionAprobatoria;
				temp1->available = temp2->available;


				temp2->id = Aux->id;
				strcpy_s(temp2->name, Aux->name);
				strcpy_s(temp2->description, Aux->description);
				strcpy_s(temp2->date, Aux->date);
				temp2->duracion = Aux->duracion;
				temp2->price = Aux->price;
				temp2->calificacionAprobatoria = Aux->calificacionAprobatoria;
				temp2->available = Aux->available;

				temp1 = temp1->next;

			}
		}
		temp2 = temp2->next;//sirve para iterar
	}
	curso *Aux = new curso;
	Aux->id = temp1->id;
	strcpy_s(Aux->name, temp1->name);
	strcpy_s(Aux->description, temp1->description);
	strcpy_s(Aux->date, temp1->date);
	Aux->duracion = temp1->duracion;
	Aux->price = temp1->price;
	Aux->calificacionAprobatoria = temp1->calificacionAprobatoria;
	Aux->available = temp1->available;

	temp1->id = pivote->id;
	strcpy_s(temp1->name, pivote->name);
	strcpy_s(temp1->description, pivote->description);
	strcpy_s(temp1->date, pivote->date);
	temp1->duracion = pivote->duracion;
	temp1->price = pivote->price;
	temp1->calificacionAprobatoria = pivote->calificacionAprobatoria;
	temp1->available = pivote->available;

	pivote->id = Aux->id;
	strcpy_s(pivote->name, Aux->name);
	strcpy_s(pivote->description, Aux->description);
	strcpy_s(pivote->date, Aux->date);
	pivote->duracion = Aux->duracion;
	pivote->price = Aux->price;
	pivote->calificacionAprobatoria = Aux->calificacionAprobatoria;
	pivote->available = Aux->available;
	return temp1;

}//0 por nombre, 1 por id
void quicksortByName(curso *first, curso *last)
{
	if (first != last && 
		first != NULL && 
		last != NULL  && 
		last != first->prev)
	{
		curso *pivot = partition(first, last, 0);
		quicksortByName(first, pivot->prev);
		quicksortByName(pivot->next, last);
	} 
}
	//Por ID
void quicksortById(curso *first, curso *last)
{
	if (first != last && first != NULL && last != NULL  && last != first->prev)
	{
		curso *pivot = partition(first, last, 1);
		quicksortById(first, pivot->prev);
		quicksortById(pivot->next, last);
	}
}

//Funciones de archivos
bool readFile()//Lee el archivo y carga la información a un nodo de la lista
{
	int count = 0, size = 0;
	fstream reader;
	reader.open(fileDir, ios::in || ios::binary);
	if (reader.is_open())
	{
		reader.seekg(0, reader.end);
		size = reader.tellg();
		reader.seekg(0, reader.beg);
		while (count < size)
		{
			reader.read((char*)&aux, sizeof(curso));
			addNode(aux);
			count += sizeof(curso);
		}
		reader.close();
		return true;
	}
	else
	{
		reader.close();
		reader.open(fileDir, ios::out || ios::trunc || ios::binary);
		reader.close();
	}
	return false;
}
bool readFileRel()//Lee el archivo y carga la información a un nodo de la lista
{
	int count = 0, size = 0;
	fstream reader;
	reader.open(fileDir, ios::in || ios::binary);
	if (reader.is_open())
	{
		reader.seekg(0, reader.end);
		size = reader.tellg();
		reader.seekg(0, reader.beg);
		while (count < size)
		{
			reader.read((char*)&auxRel, sizeof(relCursos));
			addNode(auxRel.idParent, auxRel.idChild);
			count += sizeof(relCursos);
		}
		reader.close();
		return true;
	}
	else
	{
		reader.close();
		reader.open(fileDir, ios::out || ios::trunc || ios::binary);
		reader.close();
	}
	return false;
}
bool saveFile() //Guarda la lista nodo por nodo
{
	ofstream writter;
	writter.open(fileDir, ios::out | ios::binary);
	curso *aux = first;
	if (writter.is_open())
	{
			while (aux != NULL)
			{
				writter.write((char*)aux, sizeof(*aux));
				aux = aux->next;
			}
			writter.close();
			return true;
	}
	writter.close();
	return false;
}
bool saveFileRel() //Guarda la lista nodo por nodo
{
	ofstream writter;
	writter.open(fileDir, ios::out | ios::binary);
	relCursos *aux = firstRel;
	if (writter.is_open())
	{
		while (aux != NULL)
		{
			writter.write((char*)aux, sizeof(*aux));
			aux = aux->next;
		}
		writter.close();
		return true;
	}
	writter.close();
	return false;
}


//Misc functions
void fillListBoxes(HWND hDlg)
{
	curso *aux = first;
	while (aux != NULL)
	{
		ZeroMemory(&intBuffer, sizeof(intBuffer));
		_itoa(aux->id, intBuffer, 10);
		SendMessage(GetDlgItem(hDlg, LB_ID), LB_ADDSTRING, 0, (LPARAM)intBuffer);
		SendMessage(GetDlgItem(hDlg, LB_NAME), LB_ADDSTRING, 0, (LPARAM)aux->name);
		SendMessage(GetDlgItem(hDlg, LB_DESCRIPCION), LB_ADDSTRING, 0, (LPARAM)aux->description);
		_itoa(aux->duracion, intBuffer, 10);
		SendMessage(GetDlgItem(hDlg, LB_DURACION), LB_ADDSTRING, 0, (LPARAM)intBuffer);
		SendMessage(GetDlgItem(hDlg, LB_CALAPROBATORIA), LB_ADDSTRING, 0, (LPARAM)(char*)&aux->calificacionAprobatoria);
		SendMessage(GetDlgItem(hDlg, LB_PRECIO), LB_ADDSTRING, 0, (LPARAM)(char*)&aux->price);
		SendMessage(GetDlgItem(hDlg, LB_FECHA), LB_ADDSTRING, 0, (LPARAM)aux->date);
		if (aux->available)
			SendMessage(GetDlgItem(hDlg, LB_AVAILABLE), LB_ADDSTRING, 0, (LPARAM)"Disponible");
		else

			SendMessage(GetDlgItem(hDlg, LB_AVAILABLE), LB_ADDSTRING, 0, (LPARAM)"No disponible");
		aux = aux->next;
	}
}
void fillRelListBoxByParent(int idPadre, HWND hListBox)//Carla los nombres de los cursos relacionados con el padre
{
	relCursos *auxRel = firstRel;
	curso *auxChild = NULL;
	while (auxRel != NULL)
	{
		if (idPadre == auxRel->idChild)
		{
			auxChild = getCursoById(auxRel->idChild);
			if (auxChild != NULL)
				SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)auxChild->name);
		}
		auxRel = auxRel->next;
	}
}
void fillNamesList(HWND hDlg)
{
	curso *aux = first;
	if (aux != NULL)
		while (aux != NULL)
		{
			SendMessage(hDlg, LB_ADDSTRING, 0, (LPARAM)aux->name);
			aux = aux->next;
		}
	else
	{
		SendMessage(GetDlgItem(hDlg, LB_CURSOSREQ), LB_ADDSTRING, 0, (LPARAM)"No hay cursos disponibles");
		EnableWindow(GetDlgItem(hDlg, LB_CURSOSREQ), false);
	}

}
void enableListBoxes(bool enable, HWND hDlg)
{
	EnableWindow(GetDlgItem(hDlg, LB_ID), enable);
	EnableWindow(GetDlgItem(hDlg, LB_NAME), enable);
	EnableWindow(GetDlgItem(hDlg, LB_DESCRIPCION), enable);
	EnableWindow(GetDlgItem(hDlg, LB_DURACION), enable);
	EnableWindow(GetDlgItem(hDlg, LB_CALAPROBATORIA), enable);
	EnableWindow(GetDlgItem(hDlg, LB_PRECIO), enable);
	EnableWindow(GetDlgItem(hDlg, LB_FECHA), enable);
	EnableWindow(GetDlgItem(hDlg, LB_AVAILABLE), enable);
}
void resetListBoxes(HWND hDlg)
{
	SendMessage(GetDlgItem(hDlg, LB_ID), LB_RESETCONTENT, 0, 0);
	SendMessage(GetDlgItem(hDlg, LB_NAME), LB_RESETCONTENT, 0, 0);
	SendMessage(GetDlgItem(hDlg, LB_DESCRIPCION), LB_RESETCONTENT, 0, 0);
	SendMessage(GetDlgItem(hDlg, LB_DURACION), LB_RESETCONTENT, 0, 0);
	SendMessage(GetDlgItem(hDlg, LB_CALAPROBATORIA), LB_RESETCONTENT, 0, 0);
	SendMessage(GetDlgItem(hDlg, LB_PRECIO), LB_RESETCONTENT, 0, 0);
	SendMessage(GetDlgItem(hDlg, LB_FECHA), LB_RESETCONTENT, 0, 0);
	SendMessage(GetDlgItem(hDlg, LB_AVAILABLE), LB_RESETCONTENT, 0, 0);
}
void clearListBoxes(HWND hDlg)
{
	SendMessage(GetDlgItem(hDlg, LB_ID), LB_ADDSTRING, 0, (LPARAM)"No hay datos");
	SendMessage(GetDlgItem(hDlg, LB_NAME), LB_ADDSTRING, 0, (LPARAM)"No hay datos");
	SendMessage(GetDlgItem(hDlg, LB_DESCRIPCION), LB_ADDSTRING, 0, (LPARAM)"No hay datos");
	SendMessage(GetDlgItem(hDlg, LB_DURACION), LB_ADDSTRING, 0, (LPARAM)"No hay datos");
	SendMessage(GetDlgItem(hDlg, LB_CALAPROBATORIA), LB_ADDSTRING, 0, (LPARAM)"No hay datos");
	SendMessage(GetDlgItem(hDlg, LB_PRECIO), LB_ADDSTRING, 0, (LPARAM)"No hay datos");
	SendMessage(GetDlgItem(hDlg, LB_FECHA), LB_ADDSTRING, 0, (LPARAM)"No hay datos");
	SendMessage(GetDlgItem(hDlg, LB_AVAILABLE), LB_ADDSTRING, 0, (LPARAM)"No hay datos");
}
void clearTextBoxes(HWND hDlg)
{
	SetWindowText(GetDlgItem(hDlg, TXT_ID_CURSO), "");
	SetWindowText(GetDlgItem(hDlg, TXT_NOMBRE_CURSO), "");
	SetWindowText(GetDlgItem(hDlg, TXT_DESCRIPCION_CURSO), "");
	SetWindowText(GetDlgItem(hDlg, TXT_DURACION_CURSO), "");
	SetWindowText(GetDlgItem(hDlg, TXT_CALIFICACION_APROBATORIA_CURSO), "");
	SetWindowText(GetDlgItem(hDlg, TXT_PRECIO_CURSO), "");
}
void disableTextBoxes(HWND hDlg, bool enable)
{
	EnableWindow(GetDlgItem(hDlg, TXT_ID_CURSO), enable);
	EnableWindow(GetDlgItem(hDlg, TXT_NOMBRE_CURSO), enable);
	EnableWindow(GetDlgItem(hDlg, TXT_DESCRIPCION_CURSO), enable);
	EnableWindow(GetDlgItem(hDlg, TXT_DURACION_CURSO), enable);
	EnableWindow(GetDlgItem(hDlg, TXT_CALIFICACION_APROBATORIA_CURSO), enable);
	EnableWindow(GetDlgItem(hDlg, TXT_PRECIO_CURSO), enable);
}
void fillCombo(HWND hDlg)
{
	curso *aux = first;
	while (aux)
	{
		SendMessage(hDlg, LB_ADDSTRING, 0, (LPARAM)aux->name);
		aux = aux->next;
	}
}

LRESULT CALLBACK mainWindow(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK addCourse(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK viewCourseInfo(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevIns, LPSTR cmdLine, int cmdShow)
{
	hInstance = hInst;

	hWnd = CreateDialog(hInst, MAKEINTRESOURCE(FRM_MAINWINDOW), 0, (DLGPROC)mainWindow);
/*
	if(!RegisterHotKey(hWnd, 10, MOD_CONTROL | MOD_NOREPEAT, 0x53))
		MessageBox(hWnd, "No jala", "Aviso", MB_OK | MB_ICONINFORMATION);*/

	ShowWindow(hWnd, cmdShow);
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (mainWindow == 0 || !IsDialogMessage(hWnd, &msg))
		{
			/*if (msg.message == WM_HOTKEY)
					MessageBox(hWnd, "Sí jala", "Aviso", MB_OK | MB_ICONINFORMATION);*/
			if (!IsDialogMessage(hWnd, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK mainWindow(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	GetCurrentDirectory(MAX_PATH + 1, fileDir);
	strcat(fileDir, "\\cursos.bin");
	switch (msg)
	{
		case WM_INITDIALOG:
			SendMessage(GetDlgItem(hDlg, CB_SEARCHBY), CB_ADDSTRING, 0,(LPARAM)"ID");
			SendMessage(GetDlgItem(hDlg, CB_SEARCHBY), CB_ADDSTRING,0 ,(LPARAM)"Nombre");
			if (readFile())
				fillListBoxes(hDlg);
			else
			{
				clearListBoxes(hDlg);
				enableListBoxes(false, hDlg);
			}
		break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case ID_CURSOS_AGREGAR:
					DialogBox(hInstance, MAKEINTRESOURCE(FRM_CURSOS), hWnd, DLGPROC(addCourse));
					break;
				case ID_FILE_GUARDAR:
					saveFile();
					unsavedChanges = false;
				break;
				case ID_FILE_SALIR:
					mbResult= MessageBox(hWnd, "Está seguro que desea salir?", "Aviso", MB_YESNO);
					if (mbResult == IDYES)
						PostQuitMessage(0);
				break;
				case LB_ID:
					if (HIWORD(wParam) == LBN_DBLCLK)
					{
						int cursel = SendMessage(GetDlgItem(hWnd, LB_ID), LB_GETCURSEL, 0, 0);
						try
						{
							infoNode = getNodeByCursel(cursel);
							if (infoNode != NULL)
								DialogBox(hInstance, MAKEINTRESOURCE(FRM_CURSOS), hWnd, DLGPROC(viewCourseInfo));
						}
						catch (...)
						{
							MessageBox(hWnd, "No se pudo obtener información del elemento seleccionado", "Error", MB_OK | MB_ICONERROR);
						}
					}
				break;
				case BTN_SEARCH:
				{
					char buffer[50]{};
					switch (SendMessage(GetDlgItem(hDlg, CB_SEARCHBY), CB_GETCURSEL, 0, 0))
					{
					case 0:
					{
						quicksortById(first, last);
						GetWindowText(GetDlgItem(hDlg, TXT_SEARCHPARAM), buffer, 140);
						int searchId = atoi(buffer);
						infoNode = SearchById(searchId);
						if (infoNode != NULL)
							DialogBox(hInstance, MAKEINTRESOURCE(FRM_CURSOS), 0, (DLGPROC)viewCourseInfo);
					}
					break;
					case 1:
					{
						quicksortByName(first, last);
						GetWindowText(GetDlgItem(hDlg, TXT_SEARCHPARAM), buffer, 140);
						infoNode = binarySearchByName(first, last, buffer);
						if (infoNode != NULL)
							DialogBox(hInstance, MAKEINTRESOURCE(FRM_CURSOS), 0, (DLGPROC)viewCourseInfo);
					}
					break;
					}
				}
				break;
				case BTN_RESET:
					ShowWindow(GetDlgItem(hDlg, BTN_RESET), 0);
					SetWindowText(GetDlgItem(hDlg, TXT_SEARCHPARAM),"");
					resetListBoxes(hDlg);
					fillListBoxes(hDlg);
				break;
			}
		break;	
		case WM_ACTIVATE:
				if (first != NULL)
				{
					enableListBoxes(true, hWnd);
					resetListBoxes(hWnd);
					fillListBoxes(hWnd);
					EnableWindow(GetDlgItem(hDlg, BTN_SEARCH), true);
					EnableWindow(GetDlgItem(hDlg, CB_SEARCHBY), true);
					EnableWindow(GetDlgItem(hDlg, TXT_SEARCHPARAM), true);
				}
				else
				{
					enableListBoxes(false, hWnd);
					resetListBoxes(hWnd);
					clearListBoxes(hDlg);
					EnableWindow(GetDlgItem(hDlg, BTN_SEARCH), false);
					EnableWindow(GetDlgItem(hDlg, CB_SEARCHBY), false);
					EnableWindow(GetDlgItem(hDlg, TXT_SEARCHPARAM), false);
				}
		break;
		case WM_CLOSE:
			if (unsavedChanges)
				mbResult = MessageBox(hWnd, "Tiene cambios sin guardar \n¿Está seguro de que desea salir?", "Aviso", MB_YESNO | MB_ICONINFORMATION);
			else
				mbResult = MessageBox(hWnd, "Está seguro que desea salir?", "Aviso", MB_YESNO | MB_ICONINFORMATION);
			if (mbResult == IDYES)
				PostQuitMessage(0);
		break;
		case WM_DESTROY:
		break;
		
	}
	return false;
}
LRESULT CALLBACK addCourse(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		{
			if (first != NULL)
				fillNamesList(GetDlgItem(hDlg, LB_CURSOSEMP));
			else
			{
				EnableWindow(GetDlgItem(hDlg, LB_CURSOSEMP), false);
				SendMessage(GetDlgItem(hDlg, LB_CURSOSEMP), LB_ADDSTRING, 0, (LPARAM)"No hay cursos disponibles");
			}
			disableTextBoxes(hDlg, true);
			fillNamesList(hDlg);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			case BTN_SUBMIT:
			{
				if (GetWindowText(GetDlgItem(hDlg, TXT_ID_CURSO), intBuffer, 10) == 0) emptyField = true; else emptyField = false;
				if (GetWindowText(GetDlgItem(hDlg, TXT_NOMBRE_CURSO), aux.name, sizeof(aux.name)) == 0) emptyField = true;
				if (GetWindowText(GetDlgItem(hDlg, TXT_DESCRIPCION_CURSO), aux.description, sizeof(aux.description)) == 0) emptyField = true; else emptyField = false;
				if (GetWindowText(GetDlgItem(hDlg, TXT_DURACION_CURSO), intBuffer2, 10) == 0) emptyField = true; else emptyField = false;
				if (GetWindowText(GetDlgItem(hDlg, TXT_CALIFICACION_APROBATORIA_CURSO), (char*)&aux.calificacionAprobatoria, 4) == 0) emptyField = true; else emptyField = false;
				if (GetWindowText(GetDlgItem(hDlg, TXT_PRECIO_CURSO), (char*)&aux.price, 10) == 0) emptyField = true; else emptyField = false;
				GetWindowText(GetDlgItem(hDlg, DTP_DATE), aux.date, 12);

				if (SendMessage(GetDlgItem(hDlg, RB_DISPONIBILIDAD_CURSO), BM_GETSTATE, 0, 0) == BST_CHECKED)
					aux.available = true;
				else
					aux.available = false;
				if (!emptyField)
				{
					aux.id = atoi(intBuffer);
					if (existe(aux))
						MessageBox(hDlg, "El ID ingresado ya existe", "Aviso", MB_OK);
					else 
					{
						mbResult = MessageBox(hDlg, "Confirma que los datos están correctos?", "Aviso", MB_YESNOCANCEL | MB_ICONEXCLAMATION);
						if (mbResult == IDYES)
						{
							aux.duracion = atoi(intBuffer2);
							addNode(aux);
							unsavedChanges = true;
							if (first->next != NULL)
							{
								EnableWindow(GetDlgItem(hDlg, LB_CURSOSEMP), true);
								ShowWindow(GetDlgItem(hDlg, BTN_EMPARENTAR), 1);
								ShowWindow(GetDlgItem(hDlg, LBL_CURSONAME), 1);
							}
						}
					}
				}
				else
					MessageBox(hDlg, "No deje campos vacíos", "Aviso", MB_OK | MB_ICONHAND);
			}
			break;
			case LB_CURSOSEMP:
				if (HIWORD(wParam) == LBN_DBLCLK)
				{
					SendMessage(GetDlgItem(hDlg, LB_CURSOSEMP), LB_GETTEXT, SendMessage(GetDlgItem(hDlg, LB_CURSOSEMP), LB_GETCURSEL, 0, 0), (LPARAM)nameCurso);
				}
			break;
			case BTN_EMPARENTAR:
			{
				GetWindowText(GetDlgItem(hDlg, TXT_ID_CURSO), intBuffer, 10);
				int parentId = atoi(intBuffer);
				quicksortByName(first, last);
				curso *info = binarySearchByName(first, last, nameCurso);
				if (info != NULL)
					addNode(parentId, info->id);
			}
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, 1);
		break;
	case WM_DESTROY:
		break;

	}
	return false;
}
LRESULT CALLBACK viewCourseInfo(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
			if (infoNode != NULL)
			{
				_itoa(infoNode->id, intBuffer, 10);

				SetWindowText(hDlg, infoNode->name);
				SetWindowText(GetDlgItem(hDlg, TXT_ID_CURSO), intBuffer);
				SetWindowText(GetDlgItem(hDlg, TXT_NOMBRE_CURSO), infoNode->name);
				SetWindowText(GetDlgItem(hDlg, TXT_DESCRIPCION_CURSO), infoNode->description);
				_itoa(infoNode->duracion, intBuffer2, 10);
				SetWindowText(GetDlgItem(hDlg, TXT_DURACION_CURSO), intBuffer2);
				SetWindowText(GetDlgItem(hDlg, TXT_CALIFICACION_APROBATORIA_CURSO), (char*)&infoNode->calificacionAprobatoria);
				SetWindowText(GetDlgItem(hDlg, TXT_PRECIO_CURSO), (char*)&infoNode->price);
				SetWindowText(GetDlgItem(hDlg, DTP_DATE), infoNode->date);
				if (infoNode->available)
					SendMessage(GetDlgItem(hDlg, RB_DISPONIBILIDAD_CURSO), BM_SETCHECK, BST_CHECKED, 0);

				fillRelListBoxByParent(infoNode->id, GetDlgItem(hDlg, LB_CURSOSEMP));

				disableTextBoxes(hDlg, false);
				ShowWindow(GetDlgItem(hDlg, RB_EDIT_CURSO), 1);
				ShowWindow(GetDlgItem(hDlg, BTN_SUBMIT), 0);
			}
			else
				MessageBox(hDlg, "No se pudo cargar información", "Error", MB_OK | MB_ICONERROR);
		break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case RB_EDIT_CURSO:
					if (SendMessage(GetDlgItem(hDlg, RB_EDIT_CURSO), BM_GETCHECK, 0, 0) == BST_CHECKED)
					{
						ShowWindow(GetDlgItem(hDlg, BTN_SUBMIT), 1);
						ShowWindow(GetDlgItem(hDlg, BTN_DELETE), 1);
						EnableWindow(GetDlgItem(hDlg, TXT_NOMBRE_CURSO), true);
						EnableWindow(GetDlgItem(hDlg, TXT_DESCRIPCION_CURSO), true);
						EnableWindow(GetDlgItem(hDlg, TXT_DURACION_CURSO), true);
						EnableWindow(GetDlgItem(hDlg, TXT_CALIFICACION_APROBATORIA_CURSO), true);
						EnableWindow(GetDlgItem(hDlg, TXT_PRECIO_CURSO), true);
					}
					else
					{
						ShowWindow(GetDlgItem(hDlg, BTN_SUBMIT), 0);
						ShowWindow(GetDlgItem(hDlg, BTN_DELETE), 0);
						disableTextBoxes(hDlg, false);
					}
				break;
				case BTN_SUBMIT:
					if (GetWindowText(GetDlgItem(hDlg, TXT_NOMBRE_CURSO), aux.name, sizeof(aux.name)) == 0) emptyField = true;
					if (GetWindowText(GetDlgItem(hDlg, TXT_DESCRIPCION_CURSO), aux.description, sizeof(aux.description)) == 0) emptyField = true; else emptyField = false;
					if (GetWindowText(GetDlgItem(hDlg, TXT_DURACION_CURSO), intBuffer2, 10) == 0) emptyField = true; else emptyField = false;
					if (GetWindowText(GetDlgItem(hDlg, TXT_CALIFICACION_APROBATORIA_CURSO), (char*)&aux.calificacionAprobatoria, 4) == 0) emptyField = true; else emptyField = false;
					if (GetWindowText(GetDlgItem(hDlg, TXT_PRECIO_CURSO), (char*)&aux.price, 5) == 0) emptyField = true; else emptyField = false;
						GetWindowText(GetDlgItem(hDlg, DTP_DATE), aux.date, 12);
					if (SendMessage(GetDlgItem(hDlg, RB_DISPONIBILIDAD_CURSO), BM_GETSTATE, 0, 0) == BST_CHECKED)
						aux.available = true;
					else
						aux.available = false;
					if (!emptyField)
					{
						mbResult = MessageBox(hDlg, "Confirma que los datos están correctos?", "Aviso", MB_YESNOCANCEL | MB_ICONEXCLAMATION);
						if (mbResult == IDYES)
						{
							strcpy(infoNode->name, aux.name);
							strcpy(infoNode->description, aux.description);
							strcpy(infoNode->date, aux.date);
							infoNode->duracion = aux.duracion;
							infoNode->calificacionAprobatoria = aux.calificacionAprobatoria;
							infoNode->price = aux.price;
							infoNode->available = aux.available;
							infoNode->duracion = atoi(intBuffer2);
							unsavedChanges = true;
							clearTextBoxes(hDlg);
							EndDialog(hDlg, 1);
						}
					}
					else
						MessageBox(hDlg, "No deje campos vacíos", "Aviso", MB_OK | MB_ICONHAND);
				break;
				case BTN_DELETE:
					mbResult = MessageBox(hDlg, "Está seguro de que quiere eliminar este curso \nNo se podrán deshacer estos cambios", "Aviso importante", MB_YESNO | MB_ICONWARNING);
					if (infoNode != NULL && mbResult == IDYES)
					{
						deleteNode(infoNode);
						EndDialog(hDlg, 1);
					}
				break;
			}
			break;
		case WM_CLOSE:
			ZeroMemory(&infoNode, sizeof(&infoNode));
			EndDialog(hDlg, 1);
		break;
		case WM_DESTROY:
			break;
	}
	return false;
}
