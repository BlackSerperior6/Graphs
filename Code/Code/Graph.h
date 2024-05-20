#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <queue>

#define PI 3.14159265 // Константа, выражающая собой число Pi

#include "SFML/Graphics.hpp"

using namespace std;
using namespace sf;

const int VertexRadius = 20; //Радиус вершины графа

const int startMinValue = 10000; //Недостижимый минимум

template<typename T>
struct GraphElement //Структура элемента графа
{
	CircleShape visualVertex; //Визуальная часть элемента графа
	
	T Data; //Данные
};

template <typename T>
class Graph
{
public:

	void DrawGraph(RenderWindow &window, vector<T> &choosenElements) //Метод рисования графа
	{

		for (int i = 0; i < VertexList.size(); i++)
			DrawVertex(window, VertexList[i], choosenElements); //Рисуем вершины

		for (int i = 0; i < AdjMatrix.size(); i++)
		{
			for (int j = 0; j < AdjMatrix.size(); j++)
				DrawEdge(window, VertexList[i], VertexList[j], AdjMatrix[i][j]); //Рисуем грани
		}
	}

	void TryRemoveVertexByCoordinates(Vector2i &coords) //Метод удаления вершины по координатам
	{
		T item;
		bool flag = false;

		for (int i = 0; i < VertexList.size() && !flag; i++) //Находим вершину по координатам
		{
			flag = VertexList[i].visualVertex.getGlobalBounds().contains(coords.x, coords.y);

			if (flag)
				item = VertexList[i].Data;
		}

		if (flag) //Удаляем, если нашли
			RemoveVertex(item);
	}

	void AddVertex(T vertex, Vector2i position) //Метод добавления вершины, принимает данные и позицию
	{
		int oldSize = VertexList.size();

		GraphElement<T> elm;
		
		//Задавания данных новому элементу графа
		elm.Data = vertex;
		elm.visualVertex.setRadius(VertexRadius);
		elm.visualVertex.setPosition(position.x - VertexRadius, position.y - VertexRadius);
		elm.visualVertex.setOutlineColor(Color::Black);
		elm.visualVertex.setFillColor(Color::White);
		elm.visualVertex.setOutlineThickness(2);

		//Добавление
		VertexList.push_back(elm);	
		
		int currentSize = VertexList.size();

		vector<vector<int>> Buffer = AdjMatrix;

		AdjMatrix.clear();

		//Создание колонци и строки для нового элемента графа
		AdjMatrix = vector<vector<int>>(currentSize, vector<int>(currentSize));

		for (int i = 0; i < oldSize; i++)
		{
			for (int j = 0; j < oldSize; j++)
				AdjMatrix[i][j] = Buffer[i][j];
		}
			
		for (int i = oldSize; i < currentSize; i++)
		{
			for (int j = oldSize; j < currentSize; j++)
				AdjMatrix[i][j] = 0;
		}
	}

	void TrySetNewChoosen(Vector2i coords, vector<T> &listChoosen) //По координатам пытаемся определить выбранный пользователем элемент
	{
		bool flag = false;

		T choosenData;

		for (int i = 0; i < VertexList.size() && !flag; i++) 
		{
			flag = VertexList[i].visualVertex.getGlobalBounds().contains(coords.x, coords.y);

			if (flag)
				choosenData = VertexList[i].Data;
		}
			

		if (!flag)
			return;

		flag = false;

		//Если нашли, то проверяем, не выбран ли уже элемент
		for (int i = 0; i < listChoosen.size() && !flag; i++)
			flag = listChoosen[i] == choosenData;

		if (flag)
			return;

		//Если не выбран, то выбираем
		listChoosen.push_back(choosenData);
	}

	vector<string> FloydsAlgotrithm(T startVertex) 
	{
		vector<vector<int>> BufferMatrix = vector<vector<int>>
			(AdjMatrix.size(), vector<int>(AdjMatrix.size()));

		for (int i = 0; i < AdjMatrix.size(); i++)
		{
			for (int j = 0; j < AdjMatrix.size(); j++)
			{
				if (i == j)
					BufferMatrix[i][j] = 0;

				else if (AdjMatrix[i][j] == 0 || AdjMatrix[i][j] == -1)
					BufferMatrix[i][j] = startMinValue;

				else
					BufferMatrix[i][j] = AdjMatrix[i][j];
			}
		}

		vector<vector<int>> resultMatrix = vector<vector<int>>
			(AdjMatrix.size(), vector<int>(AdjMatrix.size()));

		for (int i = 0; i < AdjMatrix.size(); i++)
		{
			for (int j = 0; j < AdjMatrix.size(); j++)
			{
				if (AdjMatrix[i][j] == 0 || AdjMatrix[i][j] == -1)
					resultMatrix[i][j] = -1;
				else
					resultMatrix[i][j] = j;
			}
		}

		for (int i = 0; i < VertexList.size(); i++)
		{
			for (int u = 0; u < VertexList.size(); u++)
			{
				for (int v = 0; v < VertexList.size(); v++)
				{
					if (BufferMatrix[u][i] + BufferMatrix[i][v] < BufferMatrix[u][v]) 
					{
						BufferMatrix[u][v] = BufferMatrix[u][i] + BufferMatrix[i][v];

						resultMatrix[u][v] = resultMatrix[u][i];
					}
				}
			}
		}

		vector<string> result;

		for (int i = 0; i < VertexList.size(); i++) 
		{
			if (startVertex == VertexList[i].Data)
				continue;

			result.push_back(GetShortesPath(startVertex, VertexList[i].Data, 
				resultMatrix, BufferMatrix));
		}

		return result;
	}

	//Метод сдвига вершины
	void MoveVertex(T data, Vector2i position) 
	{
		GraphElement<T>* element = nullptr;

		//Находим элемент по заданым данным
		for (int i = 0; i < VertexList.size() && element == nullptr; i++)
		{
			if (VertexList[i].Data == data)
				element = &VertexList[i];
		}

		//Сдивгаем
		element->visualVertex.setPosition(position.x - VertexRadius, position.y - VertexRadius);
	}

	//Возвращает кол-во вершин
	int GetVertexCount()
	{
		return VertexList.size();
	}

	void AddEdge(T vertex1, T vertex2, int weight) //Метод добавления ребра в граф
	{
		if (weight <= 0)
			return;

		int vertex1Index = GetVertexIndex(vertex1);
		int vertex2Index = GetVertexIndex(vertex2);

		AdjMatrix[vertex1Index][vertex2Index] = weight;

		if (AdjMatrix[vertex2Index][vertex1Index] == 0)
			AdjMatrix[vertex2Index][vertex1Index] = -1;
	}

	void RemoveEdge(T vertex1, T vertex2) //Метод удаления из графа ребра 
	{
		int vertex1Index = GetVertexIndex(vertex1);
		int vertex2Index = GetVertexIndex(vertex2);

		if (AdjMatrix[vertex2Index][vertex1Index] != -1)
			AdjMatrix[vertex1Index][vertex2Index] = -1;
		else
		{
			AdjMatrix[vertex1Index][vertex2Index] = 0;
			AdjMatrix[vertex2Index][vertex1Index] = 0;
		}		
	}

	int GetWeight(T vertex1, T vertex2) //Метод получения веса ребра между двумя вершинами
	{
		int v1index = GetVertexIndex(vertex1);
		int v2index = GetVertexIndex(vertex2);

		return AdjMatrix[v1index][v2index];;
	}

	void Clear() //Метод очистки графа
	{
		VertexList.clear();
		AdjMatrix.clear();
	}

private:

	//Список элементов графа
	vector<GraphElement<T>> VertexList;

	//Матрица смежности
	vector<vector<int>> AdjMatrix;

	string GetShortesPath(T vertex1, T vertex2, vector<vector<int>>& SolutionMatrix,
		vector<vector<int>>& BufferMatrix)
	{
		string result;
		result = "From " + vertex1 + " to " + vertex2 + ": ";

		if (BufferMatrix[GetVertexIndex(vertex1)][GetVertexIndex(vertex2)] == startMinValue)
			result += "No path found";
		else
		{
			T temp = vertex1;

			while (temp != vertex2)
			{
				result += temp + "->";

				temp = VertexList[SolutionMatrix[GetVertexIndex(temp)]
					[GetVertexIndex(vertex2)]].Data;
			}

			result += vertex2;
		}

		return result;
	}

	//Метод печати матрицы смежности в консоль
	void PrintTable(vector<vector<int>> to_print)
	{
		//Получаем самый длинный элемент в таблице, включая элементы графа
		int greatestLenght = GetWidestLenght();

		for (int i = 0; i < greatestLenght; i++)
			cout << "_";

		cout << "|";

		for (int i = 0; i < VertexList.size(); i++)
		{
			PrintWithSpaces(VertexList[i].Data, greatestLenght);

			cout << "|";
		}

		PrintDivideLine(VertexList.size() * greatestLenght * 2);

		for (int i = 0; i < AdjMatrix.size(); i++)
		{
			PrintWithSpaces(VertexList[i].Data, greatestLenght);

			cout << "|";

			for (int j = 0; j < AdjMatrix.size(); j++)
			{
				if (to_print[i][j] < 0)
					PrintWithSpaces("m", greatestLenght);
				else
					PrintWithSpaces(to_string(to_print[i][j]), greatestLenght);

				cout << "|";
			}

			PrintDivideLine(VertexList.size() * greatestLenght * 2);
		}
	}

	void RemoveVertex(T vertex) //Метод удаления вершины из графа
	{
		int vIndex = GetVertexIndex(vertex);

		//Буфер вершин
		vector<GraphElement<T>> VertexBuffer;

		//Переносим в него все вершины, кроме удаляемой
		for (int i = 0; i < VertexList.size(); i++)
		{
			if (VertexList[i].Data != vertex)
				VertexBuffer.push_back(VertexList[i]);
		}

		int oldSize = VertexList.size();

		//Задаем основному списку новые элементы
		VertexList.clear();
		VertexList = VertexBuffer;

		int currentSize = VertexList.size();

		//Буфер ребер
		vector<vector<int>> EdgesBuffer = AdjMatrix;

		AdjMatrix.clear();

		AdjMatrix = vector<vector<int>>(currentSize, vector<int>(currentSize));

		int ColumnOffset = 0;

		//Переносим в матрицу основную матрицу нужные значения
		for (int i = 0; i < currentSize; i++)
		{
			if (i == vIndex)
				ColumnOffset++;

			int RowOffset = 0;

			for (int j = 0; j < currentSize; j++)
			{
				if (j == vIndex)
					RowOffset++;

				AdjMatrix[i][j] = EdgesBuffer[i + ColumnOffset][j + RowOffset];
			}
		}
	}

	//Метод рисования вершины
	void DrawVertex(RenderWindow &window, GraphElement<T> &Vertex, vector<T> &choosenElements) 
	{
		sf::Text text;
		Font font;

		font.loadFromFile("CyrilicOld.TTF");

		//Настройка текста
		text.setString(Vertex.Data);
		text.setFont(font);
		text.setFillColor(Color::Black);
		text.setOutlineColor(Color::White);
		text.setCharacterSize(VertexRadius);

		//Отцентровка текста
		FloatRect textRect = text.getLocalBounds();
		text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
		text.setPosition(Vertex.visualVertex.getPosition() + Vector2f(VertexRadius, VertexRadius));

		bool flag = false;

		for (int i = 0; i < choosenElements.size() && !flag; i++)
			flag = choosenElements[i] == Vertex.Data;

		Vertex.visualVertex.setOutlineColor(flag ? Color::Magenta : Color::Black);

		window.draw(Vertex.visualVertex);
		window.draw(text);
	}
	
	//Метод рисования ребра
	void DrawEdge(RenderWindow& window, GraphElement<T> &Vertex1, GraphElement<T> &Vertex2, int weight)
	{
		if (weight == 0 || weight == -1)
			return;

		//Расчет точки соприкосновения с линией для первого элемента
		Vector2f boundaryForVertex1 =
			CalculateBoundaryPoint(Vertex2.visualVertex.getPosition(), Vertex1.visualVertex.getPosition(), 22)
			+ Vector2f(VertexRadius, VertexRadius);

		//Для второго
		Vector2f boundaryForVertex2 =
			CalculateBoundaryPoint(Vertex1.visualVertex.getPosition(), Vertex2.visualVertex.getPosition(), 22)
			+ Vector2f(VertexRadius, VertexRadius);

		VertexArray Edge(Lines, 2);

		//Рисуем линии
		Edge[0] = boundaryForVertex1;
		Edge[1] = boundaryForVertex2;

		Edge[0].color = Color::Black;
		Edge[1].color = Color::Black;

		//Расчитываем местопложение содержимого ребра
		Vector2f weightTextPosition = CalculateBoundaryPoint(boundaryForVertex1, boundaryForVertex2, 
			sqrt(pow(boundaryForVertex2.x - boundaryForVertex1.x, 2) + pow(boundaryForVertex2.y - boundaryForVertex1.y, 2)) 
			/ 4);

		Font font;

		//Загрузка шрифта
		font.loadFromFile("CyrilicOld.TTF");

		//Настройка текста
		sf::Text text;
		text.setFont(font);
		text.setString(to_string(weight));
		text.setCharacterSize(15);
		text.setFillColor(Color::Black);
		text.setOutlineColor(Color::White);

		FloatRect textRect = text.getLocalBounds();
		text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
		text.setPosition(weightTextPosition);

		//Стрелка ребра
		VertexArray triangleArrow(Triangles, 3);

		//Получение угла, под которым должна быть повернута стрелка
		double angle = GetAngle(boundaryForVertex2, boundaryForVertex1, Vector2f(boundaryForVertex2.x, boundaryForVertex2.y + 20));

		if (boundaryForVertex1.x > boundaryForVertex2.x)
			angle *= -1;

		Transform rotation;
		rotation.rotate(angle, boundaryForVertex2);

		//Точки B и C треугольника
		Vector2f pointB = Vector2f(boundaryForVertex2.x - 10, boundaryForVertex2.y + 12);
		Vector2f pointC = Vector2f(boundaryForVertex2.x + 10, boundaryForVertex2.y + 12);

		//Рисуем триугольную стрелку

		triangleArrow[0] = boundaryForVertex2;
		triangleArrow[1] = rotation.transformPoint(pointB);
		triangleArrow[2] = rotation.transformPoint(pointC);

		triangleArrow[0].color = Color::Black;
		triangleArrow[1].color = Color::Black;
		triangleArrow[2].color = Color::Black;

		window.draw(text);
		window.draw(triangleArrow);
		window.draw(Edge);
	}

	double GetAngle(Vector2f &A, Vector2f &B, Vector2f &C) //Метод рассчета угла
	{
		double AB = GetSideLenght(A, B); //Длина стороны AB

		double AC = GetSideLenght(A, C); //Длина стороны AC

		double BC = GetSideLenght(B, C); //Длина стороны BC

		return acos((AB * AB + AC * AC - BC * BC) / (2 * AB * AC)) * 180/PI; //Возвращаем угл
	}

	double GetSideLenght(Vector2f &point1, Vector2f &point2) //Метод нахождения длины между двумя точками
	{
		return sqrt(pow(point2.x - point1.x, 2) + pow(point2.y - point1.y, 2));
	}

	//Метод для рассчета точки соприкосновения между вершиной и ребром
	Vector2f CalculateBoundaryPoint(Vector2f point1, Vector2f point2, double minus)
	{
		double Lenght = GetSideLenght(point1, point2) - minus;

		double a = atan2(point2.y - point1.y, point2.x - point1.x);

		return Vector2f(point1.x + Lenght * cos(a), point1.y + Lenght * sin(a));
	}

	//Метод для получния индекса вершины
	int GetVertexIndex(T vertex)
	{
		int result = -1;

		for (int i = 0; i < VertexList.size() && result == -1; i++)
		{
			if (VertexList[i].Data == vertex)
				result = i;
		}

		return result;
	}

	//Метод для нахождения самого длинного элемента в матрице графа
	int GetWidestLenght()
	{
		int result = 0;

		for (int i = 0; i < VertexList.size(); i++)
		{
			string element = VertexList[i].Data;

			if (result < element.length())
				result = element.length();
		}

		for (int i = 0; i < VertexList.size(); i++)
		{
			for (int j = 0; j < VertexList.size(); j++)
			{
				string element = to_string(AdjMatrix[i][j]);

				if (result < element.length())
					result = element.length();
			}
		}

		return result;
	}

	//Метод печати элемент в консоль
	void PrintWithSpaces(string element, int maxLenght)
	{
		cout << element;

		for (int u = 0; u < maxLenght - element.length(); u++)
			cout << " ";
	}

	//Метод печати разделяющей линии
	void PrintDivideLine(int Lenght)
	{
		cout << endl;

		for (int i = 0; i < Lenght * 2; i++)
			cout << "-";

		cout << endl;
	}
};

